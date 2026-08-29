// Split this into a Scene and Renderer class to both follow the diagrams from
// the presentation and to reduce the size of it

#include "Scene.hpp"
#include "AABB.hpp"
#include "AccelerationTree.hpp"
#include "Bitmap.hpp"
#include "Color.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"
#include "Triangle.hpp"
#include "Vertex.hpp"
#include "utils.hpp"
#include "vec3.hpp"

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/schema.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace RenderEngine {

Scene::Scene(size_t width, size_t height, size_t bucket_size,
             Color &&backgroundColor, Camera &&camera,
             std::vector<Light> &&lights, std::vector<Bitmap> &&bitmaps,
             std::vector<Texture> &&textures, std::vector<Material> &&materials,
             std::vector<Vertex> &&vertices, std::vector<Triangle> &&triangles,
             std::vector<vec3> &&triangleNormals, std::vector<Mesh> &&meshes,
             AccelerationTree &&accelerationTree)
    : width_{width}, height_{height}, bucket_size_{bucket_size},
      backgroundColor_{std::move(backgroundColor)}, camera_{std::move(camera)},
      lights_{std::move(lights)}, bitmaps_{std::move(bitmaps)},
      textures_{std::move(textures)}, materials_{std::move(materials)},
      vertices_{std::move(vertices)}, triangles_{std::move(triangles)},
      triangleNormals_{std::move(triangleNormals)}, meshes_{std::move(meshes)},
      accelerationTree_{std::move(accelerationTree)} {}

[[nodiscard]] Scene Scene::loadScene(const std::string &sceneFile) {
  const std::filesystem::path sceneFilePath(sceneFile);
  if (!std::filesystem::exists(sceneFile)) {
    throw std::runtime_error("Scene file not found: " + sceneFile);
  }
  const std::filesystem::path sceneDir = sceneFilePath.parent_path();
  std::ifstream ifs(sceneFile, std::ios::binary);

  char fileBuffer[65536];
  const auto arrToColorObject = [](const rapidjson::Value &arr) {
    return Color{arr[0].GetFloat(), arr[1].GetFloat(), arr[2].GetFloat()};
  };
  const auto arrToVec3 = [](const rapidjson::Value &arr) {
    return vec3{arr[0].GetFloat(), arr[1].GetFloat(), arr[2].GetFloat()};
  };
  const auto arrToMatrix3x3 = [](const rapidjson::Value &arr) {
    return Matrix3x3{{
        arr[0].GetFloat(),
        arr[1].GetFloat(),
        arr[2].GetFloat(),
        arr[3].GetFloat(),
        arr[4].GetFloat(),
        arr[5].GetFloat(),
        arr[6].GetFloat(),
        arr[7].GetFloat(),
        arr[8].GetFloat(),
    }};
  };

  constexpr char schemaJson[] = {
#embed "../schema.json"
  };
  rapidjson::Document sd;
  sd.Parse(static_cast<const char *>(schemaJson), sizeof(schemaJson));
  rapidjson::SchemaDocument schema(sd);

  rapidjson::IStreamWrapper is(ifs, fileBuffer, sizeof(fileBuffer));
  rapidjson::Document document;
  document.ParseStream(is);
  if (document.HasParseError()) {
    throw std::runtime_error(
        "Failed to parse the scene file " + sceneFile + ": " +
        rapidjson::GetParseError_En(document.GetParseError()));
  }
  if (!document.IsObject()) {
    throw std::runtime_error(
        "Invalid crtscene file: root is not a JSON object");
  }

  rapidjson::SchemaValidator validator(schema);
  if (!document.Accept(validator)) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    validator.GetError().Accept(writer);
    throw std::runtime_error("Failed to validate the scene file " + sceneFile +
                             ": " + buffer.GetString());
  }

  // verify scene file correctness outside of schema validation
  // matrix of camera and vertices indeces
  // albedo in materials that are not refractive

  u64 bucket_size =
      document["settings"]["image_settings"]["bucket_size"].GetUint64();
  u64 width = document["settings"]["image_settings"]["width"].GetUint64();
  u64 height = document["settings"]["image_settings"]["height"].GetUint64();
  Color backgroundColor =
      arrToColorObject(document["settings"]["background_color"]);

  Camera camera = {arrToVec3(document["camera"]["position"]),
                   arrToMatrix3x3(document["camera"]["matrix"])};

  std::vector<Light> lights;
  lights.reserve(document["lights"].GetArray().Size());
  std::vector<Bitmap> bitmaps;
  std::unordered_map<std::string_view, size_t> texturesIdsTable;
  std::vector<Texture> textures;
  textures.reserve(document["textures"].GetArray().Size());
  std::vector<Material> materials;
  materials.reserve(document["materials"].GetArray().Size());
  std::vector<Vertex> vertices;
  std::vector<Triangle> triangles;
  std::vector<vec3> triangleNormals;
  std::vector<Mesh> meshes;
  meshes.reserve(document["objects"].GetArray().Size());
  for (const auto &textureObject : document["textures"].GetArray()) {
    std::string_view textureName{textureObject["name"].GetString(),
                                 textureObject["name"].GetStringLength()};
    texturesIdsTable[textureName] = textures.size();

    switch (getTextureTypeFromString(textureObject["type"].GetString())) {
    case TextureType::ALBEDO:
      textures.emplace_back(
          TextureType::ALBEDO,
          arrToColorObject(textureObject["albedo"].GetArray()), Color{},
          float{}, -1);
      break;
    case TextureType::EDGES:
      textures.emplace_back(
          TextureType::EDGES,
          arrToColorObject(textureObject["inner_color"].GetArray()),
          arrToColorObject(textureObject["edge_color"].GetArray()),
          textureObject["edge_width"].GetFloat(), -1);
      break;
    case TextureType::CHECKER:
      textures.emplace_back(
          TextureType::CHECKER,
          arrToColorObject(textureObject["color_A"].GetArray()),
          arrToColorObject(textureObject["color_B"].GetArray()),
          textureObject["square_size"].GetFloat(), -1);
      break;
    case TextureType::BITMAP:
      bitmaps.emplace_back(
          sceneDir.string() +
          std::string_view{textureObject["file_path"].GetString(),
                           textureObject["file_path"].GetStringLength()});
      textures.emplace_back(TextureType::BITMAP, Color{}, Color{}, float{},
                            bitmaps.size() - 1);
      break;
    default:
      std::unreachable();
    }
  }

  for (size_t i = 0; i < document["materials"].Size(); ++i) {
    const auto &material =
        document["materials"][static_cast<rapidjson::SizeType>(i)];
    switch (getMaterialTypeFromString(material["type"].GetString())) {
    case MaterialType::DIFFUSE:
      materials.emplace_back(MaterialType::DIFFUSE,
                             texturesIdsTable.at(std::string_view{
                                 material["albedo"].GetString(),
                                 material["albedo"].GetStringLength()}),
                             material["smooth_shading"].GetBool(), float{});
      break;
    case MaterialType::REFLECTIVE:
      materials.emplace_back(MaterialType::REFLECTIVE,
                             texturesIdsTable.at(std::string_view{
                                 material["albedo"].GetString(),
                                 material["albedo"].GetStringLength()}),
                             material["smooth_shading"].GetBool(), float{});
      break;
    case MaterialType::REFRACTIVE:
      materials.emplace_back(MaterialType::REFRACTIVE,
                             texturesIdsTable.at(std::string_view{
                                 material["albedo"].GetString(),
                                 material["albedo"].GetStringLength()}),
                             material["smooth_shading"].GetBool(),
                             material["ior"].GetFloat());
      break;
    case MaterialType::CONSTANT:
      materials.emplace_back(MaterialType::CONSTANT,
                             texturesIdsTable.at(std::string_view{
                                 material["albedo"].GetString(),
                                 material["albedo"].GetStringLength()}),
                             material["smooth_shading"].GetBool(), float{});
      break;
    default:
      std::unreachable();
    }
  }
  for (const auto &object : document["objects"].GetArray()) {
    const auto verticesMember = object["vertices"].GetArray();
    const auto uvsMember = object["uvs"].GetArray();
    const auto trianglesMember = object["triangles"].GetArray();
    if (verticesMember.Size() % 3 != 0) {
      throw std::runtime_error("Invalid crtscene file: root.objects.vertices "
                               "length is not a multiple of 3!");
    }
    if (uvsMember.Size() % 3 != 0) {
      throw std::runtime_error("Invalid crtscene file: root.objects.uvs "
                               "length is not a multiple of 3!");
    }
    if (trianglesMember.Size() % 3 != 0) {
      throw std::runtime_error("Invalid crtscene file: root.objects.triangles "
                               "length is not a multiple of 3!");
    }
    size_t verticesOffset = vertices.size();
    for (auto vertexCoordinateItr = verticesMember.Begin();
         vertexCoordinateItr != verticesMember.End();) {
      const float x = vertexCoordinateItr++->GetFloat();
      const float y = vertexCoordinateItr++->GetFloat();
      const float z = vertexCoordinateItr++->GetFloat();
      vertices.emplace_back(vec3{x, y, z}, vec3::zero(), vec3::zero());
    }
    for (rapidjson::SizeType i = 0; i < uvsMember.Size();) {
      const float x = uvsMember[i++].GetFloat();
      const float y = uvsMember[i++].GetFloat();
      const float z = uvsMember[i++].GetFloat();
      vertices[verticesOffset + i / 3 - 1].uv = {x, y, z};
    }
    size_t trianglesOffset = triangles.size();
    for (auto triangleIndexItr = trianglesMember.Begin();
         triangleIndexItr != trianglesMember.End();) {
      const size_t x = triangleIndexItr++->GetUint64();
      const size_t y = triangleIndexItr++->GetUint64();
      const size_t z = triangleIndexItr++->GetUint64();
      triangles.emplace_back(x + verticesOffset, y + verticesOffset,
                             z + verticesOffset, meshes.size());

      const vec3 edge1 = vertices[y + verticesOffset].position -
                         vertices[x + verticesOffset].position;
      const vec3 edge2 = vertices[z + verticesOffset].position -
                         vertices[x + verticesOffset].position;
      triangleNormals.push_back(crossProduct(edge1, edge2).normalise());
      vertices[x + verticesOffset].normal += triangleNormals.back();
      vertices[y + verticesOffset].normal += triangleNormals.back();
      vertices[z + verticesOffset].normal += triangleNormals.back();
    }
    for (Vertex &vertex : vertices) {
      vertex.normal.normalise();
    }
    meshes.emplace_back(trianglesOffset, trianglesMember.Size(),
                        object["material_index"].GetUint64());
  }

  if (document.HasMember("lights")) {
    for (const auto &light : document["lights"].GetArray()) {
      const auto positionMember = light["position"].GetArray();
      const auto intensityMember = light["intensity"].GetFloat();
      lights.emplace_back(intensityMember, vec3{positionMember[0].GetFloat(),
                                                positionMember[1].GetFloat(),
                                                positionMember[2].GetFloat()});
    }
  }

  float minX, minY, minZ, maxX, maxY, maxZ;
  minX = minY = minZ = floatInf;
  maxX = maxY = maxZ = -floatInf;
  for (const Vertex &vertex : vertices) {
    minX = std::min(minX, vertex.position.x_);
    minY = std::min(minY, vertex.position.y_);
    minZ = std::min(minZ, vertex.position.z_);
    maxX = std::max(maxX, vertex.position.x_);
    maxY = std::max(maxY, vertex.position.y_);
    maxZ = std::max(maxZ, vertex.position.z_);
  }

  AABB sceneBox{{minX, minY, minZ}, {maxX, maxY, maxZ}};
  AccelerationTree accelerationTree{vertices, triangles, sceneBox};

  Scene scene{width,
              height,
              bucket_size,
              std::move(backgroundColor),
              std::move(camera),
              std::move(lights),
              std::move(bitmaps),
              std::move(textures),
              std::move(materials),
              std::move(vertices),
              std::move(triangles),
              std::move(triangleNormals),
              std::move(meshes),
              std::move(accelerationTree)};
  return scene;
}

[[nodiscard]] Camera &Scene::camera() noexcept { return camera_; }

void Scene::overwriteWidth(size_t width) noexcept {
  width_ = width;
}

void Scene::overwriteHeight(size_t height) noexcept {
  height_ = height;
}

void Scene::overwriteBackgroundColor(const Color &c) noexcept {
  backgroundColor_ = c;
}

[[nodiscard]] const Material &
Scene::getMaterialFromId(size_t id) const noexcept {
  return materials_[id];
}
} // namespace RenderEngine