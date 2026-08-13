#include "RenderEngine/Material.hpp"
#include "RenderEngine/Mesh.hpp"
#include "RenderEngine/Texture.hpp"
#include "RenderEngine/utils.hpp"
#include "RenderEngine/vec3.hpp"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/schema.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include <RenderEngine/Color.hpp>
#include <RenderEngine/ColorView.tpp>
#include <RenderEngine/Scene.hpp>
#include <RenderEngine/Table.tpp>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace RenderEngine {

Scene::Scene(Settings &&settings, Camera &&camera, std::vector<Light> &&lights,
             std::unordered_map<std::string, Texture> &&textures,
             std::vector<Material> &&materials, std::vector<Mesh> &&meshes)
    : settings_{std::move(settings)}, camera_{std::move(camera)},
      lights_{std::move(lights)}, textures_{std::move(textures)},
      materials_{std::move(materials)}, meshes_{std::move(meshes)} {}

[[nodiscard]] vec2 Scene::getBarycentricCoordinates(
    const vec3 &hitPoint,
    const std::array<const vec3 *, 3> &vertexPos) noexcept {
  const auto v0p = hitPoint - *vertexPos[0];
  const auto v0v1 = *vertexPos[1] - *vertexPos[0];
  const auto v0v2 = *vertexPos[2] - *vertexPos[0];
  const auto triangleArea = crossProduct(v0v1, v0v2).length();
  const auto u = crossProduct(v0p, v0v2).length() / triangleArea;
  const auto v = crossProduct(v0v1, v0p).length() / triangleArea;
  return {u, v};
}

[[nodiscard]] vec3 Scene::interpolateNormal(
    const vec3 &hitPoint, const std::array<const vec3 *, 3> &vertexPos,
    const std::array<const vec3 *, 3> &vertexNormals) noexcept {
  const auto [u, v] = getBarycentricCoordinates(hitPoint, vertexPos);
  return (u * *vertexNormals[1] + v * *vertexNormals[2] +
          (1 - u - v) * *vertexNormals[0])
      .normalise();
}

[[nodiscard]] double Scene::traceShadowRay(const vec3 &rayOrigin,
                                           const vec3 &surfaceNormal) const {
  double finalLightReached = 0;
  for (const auto &light : lights_) {
    vec3 pointToLightSourceVec = light.position() - rayOrigin;
    Ray ray{rayOrigin, pointToLightSourceVec};
    const auto cosineLawFactor =
        std::max(0., dotProduct(ray.direction_, surfaceNormal));
    auto tmpLight =
        light.intensity() /
        (4 * std::numbers::pi * pointToLightSourceVec.lengthSquared()) *
        cosineLawFactor;
    bool shadowRayIntersection = false;
    for (size_t j = 0; j < meshes_.size(); ++j) {
      if (materials_[meshes_[j].materialId()].materialType_ ==
          MaterialType::REFRACTIVE)
        continue;
      shadowRayIntersection =
          meshes_[j].intersectsFast(ray, pointToLightSourceVec.lengthSquared());
      if (shadowRayIntersection) {
        tmpLight = 0;
        break;
      }
    }
    finalLightReached += tmpLight;
  }
  return finalLightReached;
}

[[nodiscard]] Color
Scene::getTextureColor(const Mesh::IntersectResult &intersectResult) const {
  const Material &material = materials_[intersectResult.mesh->materialId()];
  const Texture &texture = textures_.at(material.textureName);
  if (texture.type == TextureType::ALBEDO) {
    return texture.asAlbedoTexture().albedo;
  } else if (texture.type == TextureType::EDGES) {
    const ConstEdgesTextureView &edgesTexture = texture.asEdgesTexture();
    const auto [u, v] = getBarycentricCoordinates(intersectResult.hitPoint,
                                                  intersectResult.vertexPos);
    if ((u < edgesTexture.edge_width) || (v < edgesTexture.edge_width) ||
        (1 - u - v < edgesTexture.edge_width))
      return edgesTexture.edge_color;
    else
      return edgesTexture.inner_color;
  } else if (texture.type == TextureType::CHECKER) {
    const ConstCheckerTextureView &checkerTexture = texture.asCheckerTexture();
    auto [u, v] = getBarycentricCoordinates(intersectResult.hitPoint,
                                            intersectResult.vertexPos);
    const vec3 interpolatedUV = u * *intersectResult.uvs[1] +
                                v * *intersectResult.uvs[2] +
                                (1 - u - v) * *intersectResult.uvs[0];
    u32 uSampled =
        static_cast<u32>(interpolatedUV.x_ / checkerTexture.square_size);
    u32 vSampled =
        static_cast<u32>(interpolatedUV.y_ / checkerTexture.square_size);
    if ((uSampled % 2 == 0 && vSampled % 2 == 0) ||
        (uSampled % 2 == 1 && vSampled % 2 == 1))
      return checkerTexture.color_A;
    else
      return checkerTexture.color_B;
  } else if (texture.type == TextureType::BITMAP) {
    const ConstBitmapTextureView &bitmapTexture = texture.asBitmapTexture();
    auto [u, v] = getBarycentricCoordinates(intersectResult.hitPoint,
                                            intersectResult.vertexPos);
    const vec3 interpolatedUV = u * *intersectResult.uvs[1] +
                                v * *intersectResult.uvs[2] +
                                (1 - u - v) * *intersectResult.uvs[0];

    return bitmapTexture.bitmap.getColor(interpolatedUV.x_,
                                         1 - interpolatedUV.y_);
  } else
    throw std::runtime_error(
        "Tried to render an object with invalid texture type! This error "
        "should have been caught earlier by the validator or parser!!!");
}

[[nodiscard]] Color
Scene::goochShade(const Mesh::IntersectResult &intersectResult) const noexcept {
  if (materials_[intersectResult.materialId].materialType_ ==
          MaterialType::REFLECTIVE ||
      materials_[intersectResult.materialId].materialType_ ==
          MaterialType::REFRACTIVE) {
    return Colors::White;
  }
  const Color cold = {0, 0, .55};
  const Color warm = {.3, .3, 0};
  const double warmFactor = .6;
  const double coldFactor = .2;
  const Color coldTint = Color::elementWiseAddition(
      cold, coldFactor * getTextureColor(intersectResult));
  const Color warmTint = Color::elementWiseAddition(
      warm, warmFactor * getTextureColor(intersectResult));
  double lightFactor = 0;
  vec3 finalNormal;
  if (materials_[intersectResult.materialId].smoothShading_) {
    finalNormal =
        interpolateNormal(intersectResult.hitPoint, intersectResult.vertexPos,
                          intersectResult.vertexNormals);
  } else {
    finalNormal = *intersectResult.triangleNormal;
  }
  for (const auto &light : lights_) {
    lightFactor +=
        (1. + dotProduct(
                  finalNormal,
                  (light.position() - intersectResult.hitPoint).normalise())) /
        2.;
  }
  lightFactor /= static_cast<double>(lights_.size());
  lightFactor = std::clamp(lightFactor, 0., 1.);
  Color c = Color::elementWiseAddition(lightFactor * warmTint,
                                       (1 - lightFactor) * coldTint);
  return c;
}

[[nodiscard]] Color Scene::traceRay(const Ray &ray,
                                    RenderMode debugRenderMode) const {
  Mesh::IntersectResult intersectResult;
  for (size_t j = 0; j < meshes_.size(); ++j) {
    const auto newIntersectResult = meshes_[j].intersects(ray);
    if (newIntersectResult.steps < intersectResult.steps)
      intersectResult = newIntersectResult;
  }
  if (intersectResult.steps == std::numeric_limits<double>::infinity()) {
    return settings_.backgroundColor;
  }
  switch (debugRenderMode) {
  case RenderMode::NormalShade:
    return [intersectResult, this]() -> Color {
      vec3 finalNormal;
      if (materials_[intersectResult.materialId].smoothShading_) {
        finalNormal = interpolateNormal(intersectResult.hitPoint,
                                        intersectResult.vertexPos,
                                        intersectResult.vertexNormals);
      } else {
        finalNormal = *intersectResult.triangleNormal;
      }
      return Color{(finalNormal.x_ + 1.) / 2, (finalNormal.y_ + 1.) / 2,
                   (finalNormal.z_ + 1.) / 2};
    }();
  case RenderMode::DistanceShade:
    return {intersectResult.steps, 0, 0};
  case RenderMode::GoochShade:
    return goochShade(intersectResult);
  case RenderMode::BarycentricShade:
    return [intersectResult]() -> Color {
      const auto [u, v] = getBarycentricCoordinates(intersectResult.hitPoint,
                                                    intersectResult.vertexPos);
      return Color{u, v, 0};
    }();
  case RenderMode::Default:
    break;
  default:
    throw std::runtime_error("Invalid Render Mode!");
  }
  switch (materials_[intersectResult.materialId].materialType_) {
  default:
    throw std::runtime_error("An invalid material type was given at " +
                             std::to_string(__LINE__) +
                             ". That should not happen!");
  case MaterialType::DIFFUSE:
    return handleDiffuseMaterial(intersectResult);
  case MaterialType::CONSTANT:
    return getTextureColor(intersectResult);
  case MaterialType::REFLECTIVE:
    return handleReflectiveMaterial(intersectResult, ray);
  case MaterialType::REFRACTIVE:
    Color reflectionColor = handleReflectiveMaterial(intersectResult, ray);
    Color refractionColor = handleRefractiveMaterial(intersectResult, ray);

    vec3 finalNormal;
    if (materials_[intersectResult.materialId].smoothShading_) {
      finalNormal =
          interpolateNormal(intersectResult.hitPoint, intersectResult.vertexPos,
                            intersectResult.vertexNormals);
    } else {
      finalNormal = *intersectResult.triangleNormal;
    }
    double tmp = dotProduct(ray.direction_, finalNormal);
    if (tmp > 0.)
      tmp = -tmp;

    double fresnelFactor = .5 * std::pow(1. + tmp, 5);

    return Color::elementWiseAddition(fresnelFactor * reflectionColor,
                                      (1 - fresnelFactor) * refractionColor);
  }
}

[[nodiscard]] Color Scene::handleDiffuseMaterial(
    const Mesh::IntersectResult &intersectResult) const noexcept {
  const auto [steps, hitPoint, triangleNormal, materialId, vertexPos,
              vertexNormals, uvs, mesh] = intersectResult;
  // TODO: light can also be reflected from reflective material (or even
  // refractive) to the diffuse object. Maybe look into implementing virtual
  // lights for each reflective reflective surface, by mirroring the position of
  // the real source
  vec3 finalNormal;
  if (materials_[materialId].smoothShading_) {
    finalNormal = interpolateNormal(hitPoint, vertexPos, vertexNormals);
  } else {
    finalNormal = *triangleNormal;
  }
  auto offsetHitPoint = hitPoint + RENDERENGINE_SHADOW_BIAS * finalNormal;

  return traceShadowRay(offsetHitPoint, finalNormal) *
         getTextureColor(intersectResult);
}

[[nodiscard]] Color
Scene::handleReflectiveMaterial(const Mesh::IntersectResult &intersectResult,
                                const Ray &previousRay) const noexcept {
  if (previousRay.depthChances_ == 0) {
    return settings_.backgroundColor;
  }
  const auto [steps, hitPoint, triangleNormal, materialId, vertexPos,
              vertexNormals, uvs, mesh] = intersectResult;
  vec3 finalNormal;
  if (materials_[materialId].smoothShading_) {
    finalNormal = interpolateNormal(hitPoint, vertexPos, vertexNormals);
  } else {
    finalNormal = *triangleNormal;
  }
  bool frontSide = dotProduct(previousRay.direction_, finalNormal) < .0;
  vec3 offsetHitPoint;
  if (frontSide) {
    offsetHitPoint = hitPoint + RENDERENGINE_HITPOINT_BIAS * finalNormal;
  } else {
    offsetHitPoint = hitPoint - RENDERENGINE_HITPOINT_BIAS * finalNormal;
  }
  vec3 direction =
      previousRay.direction_ -
      2 * dotProduct(previousRay.direction_, finalNormal) * finalNormal;
  Ray reflectedRay = {offsetHitPoint, direction, previousRay.depthChances_ - 1};

  return Color::elementWiseMultiplication(
      traceRay(reflectedRay, RenderMode::Default),
      getTextureColor(intersectResult));
}

// Does not handle refractive and / or reflecive objects inside refractrive
// objects
[[nodiscard]] Color
Scene::handleRefractiveMaterial(const Mesh::IntersectResult &intersectResult,
                                const Ray &previousRay) const noexcept {
  if (previousRay.depthChances_ == 0) {
    return settings_.backgroundColor;
  }
  const auto [steps, hitPoint, triangleNormal, materialId, vertexPos,
              vertexNormals, uvs, mesh] = intersectResult;

  vec3 finalNormal;
  if (materials_[materialId].smoothShading_) {
    finalNormal = interpolateNormal(hitPoint, vertexPos, vertexNormals);
  } else {
    finalNormal = *triangleNormal;
  }

  if (dotProduct(finalNormal, previousRay.direction_) < 0) {
    // ray hit from outside

    vec3 reflectionDirection =
        previousRay.direction_ -
        2 * dotProduct(previousRay.direction_, finalNormal) * finalNormal;
    Ray reflectedRay = {hitPoint + RENDERENGINE_HITPOINT_BIAS * finalNormal,
                        reflectionDirection, previousRay.depthChances_ - 1};

    auto ior = materials_[intersectResult.mesh->materialId()].ior_;
    double cosI = -dotProduct(finalNormal, previousRay.direction_);
    double sinI = std::sqrt(1 - cosI * cosI);
    double sinR = sinI / ior;
    double cosR = std::sqrt(1 - sinR * sinR);
    vec3 A = (-1) * finalNormal * cosR;
    vec3 B = previousRay.direction_ + finalNormal * cosI;
    vec3 refractionDirection = A + B.normalise() * sinR;
    assert(refractionDirection.x_ != doubleNaN);

    Ray refractedRay{hitPoint - RENDERENGINE_HITPOINT_BIAS * finalNormal,
                     refractionDirection, previousRay.depthChances_ - 1};

    Color reflectionColor = traceRay(reflectedRay, RenderMode::Default);
    Color refractionColor = traceRay(refractedRay, RenderMode::Default);

    double fresnelFactor =
        .5 * std::pow(1. + dotProduct(previousRay.direction_, finalNormal), 10);

    return Color::elementWiseAddition(fresnelFactor * reflectionColor,
                                      (1 - fresnelFactor) * refractionColor);
  } else {
    // ray hit from inside

    vec3 reflectionDirection =
        previousRay.direction_ -
        2 * dotProduct(previousRay.direction_, finalNormal) * finalNormal;
    Ray reflectedRay = {hitPoint - RENDERENGINE_HITPOINT_BIAS * finalNormal,
                        reflectionDirection, previousRay.depthChances_ - 1};
    Color reflectionColor = traceRay(reflectedRay, RenderMode::Default);

    auto ior = materials_[intersectResult.mesh->materialId()].ior_;
    double cosI = dotProduct(finalNormal, previousRay.direction_);
    double sinI = std::sqrt(1 - cosI * cosI);
    double sinR = sinI * ior;
    double cosR = std::sqrt(1 - sinR * sinR);
    vec3 A = finalNormal * cosR;
    vec3 B = previousRay.direction_ - finalNormal * cosI;
    vec3 refractionDirection = A + B.normalise() * sinR;
    assert(refractionDirection.x_ != doubleNaN);
    if (sinI >= 1 / ior) {
      return reflectionColor;
    }

    Ray refractedRay{hitPoint + RENDERENGINE_HITPOINT_BIAS * finalNormal,
                     refractionDirection, previousRay.depthChances_ - 1};

    Color refractionColor = traceRay(refractedRay, RenderMode::Default);

    double fresnelFactor =
        .5 * std::pow(1. - dotProduct(previousRay.direction_, finalNormal), 5);

    return Color::elementWiseAddition(fresnelFactor * reflectionColor,
                                      (1 - fresnelFactor) * refractionColor);
  }
}

[[nodiscard]] Scene Scene::loadScene(const std::string &sceneFile) {
  const std::filesystem::path sceneFilePath(sceneFile);
  if (!std::filesystem::exists(sceneFile)) {
    throw std::runtime_error("Scene file not found: " + sceneFile);
  }
  const std::filesystem::path sceneDir = sceneFilePath.parent_path();
  std::ifstream ifs(sceneFile, std::ios::binary);

  char fileBuffer[65536];
  const auto arrToColorObject = [](const rapidjson::Value &arr) {
    return Color{arr[0].GetDouble(), arr[1].GetDouble(), arr[2].GetDouble()};
  };
  const auto arrToVec3 = [](const rapidjson::Value &arr) {
    return vec3{arr[0].GetDouble(), arr[1].GetDouble(), arr[2].GetDouble()};
  };
  const auto arrToMatrix3x3 = [](const rapidjson::Value &arr) {
    return Matrix3x3{{
        arr[0].GetDouble(),
        arr[1].GetDouble(),
        arr[2].GetDouble(),
        arr[3].GetDouble(),
        arr[4].GetDouble(),
        arr[5].GetDouble(),
        arr[6].GetDouble(),
        arr[7].GetDouble(),
        arr[8].GetDouble(),
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

  // verify scene file correctness
  // matrix of camera and vertices indeces
  // albedo in materials that are not refractive
  Settings settings = {
      arrToColorObject(document["settings"]["background_color"]),
      {document["settings"]["image_settings"]["width"].GetUint64(),
       document["settings"]["image_settings"]["height"].GetUint64()}};

  Camera camera = {arrToVec3(document["camera"]["position"]),
                   arrToMatrix3x3(document["camera"]["matrix"])};

  std::unordered_map<std::string, Texture> textures;
  for (const auto &textureObject : document["textures"].GetArray()) {
    std::string textureName = textureObject["name"].GetString();
    textures[textureName] = {};
    Texture &texture = textures[textureName];
    texture.internalName = textureName;
    texture.type = getTextureTypeFromString(textureObject["type"].GetString());
    if (texture.type == TextureType::ALBEDO) {
      const AlbedoTextureView &albedoTexture = texture.asAlbedoTexture();
      albedoTexture.albedo =
          arrToColorObject(textureObject["albedo"].GetArray());
    } else if (texture.type == TextureType::EDGES) {
      const EdgesTextureView &edgesTexture = texture.asEdgesTexture();
      edgesTexture.edge_color =
          arrToColorObject(textureObject["edge_color"].GetArray());
      edgesTexture.inner_color =
          arrToColorObject(textureObject["inner_color"].GetArray());
      edgesTexture.edge_width = textureObject["edge_width"].GetDouble();
    } else if (texture.type == TextureType::CHECKER) {
      const CheckerTextureView &checkerTexture = texture.asCheckerTexture();
      checkerTexture.color_A =
          arrToColorObject(textureObject["color_A"].GetArray());
      checkerTexture.color_B =
          arrToColorObject(textureObject["color_B"].GetArray());
      checkerTexture.square_size = textureObject["square_size"].GetDouble();
    } else if (texture.type == TextureType::BITMAP) {
      BitmapTextureView bitmapTexture = texture.asBitmapTexture();
      std::string path =
          sceneDir.string() + textureObject["file_path"].GetString();
      bitmapTexture.bitmap = {path};
    } else
      throw std::runtime_error(
          "Not a valid texture type! This should have been caught by both the "
          "parser and the string->enum converter!!!");
  }

  std::vector<Material> materials;
  for (size_t i = 0; i < document["materials"].Size(); ++i) {
    const auto &material =
        document["materials"][static_cast<rapidjson::SizeType>(i)];
    materials.push_back({});
    Material &mat = materials.back();
    mat.materialType_ = getMaterialTypeFromString(material["type"].GetString());
    // assuming material's "albedo" property now always targets a texture and
    // color values are invalid
    if (mat.materialType_ == MaterialType::REFRACTIVE) {
      // Refractive materials may or may not have albedo values and ior?
      if (material.HasMember("albedo")) {
        std::string textureName = material["albedo"].GetString();
        if (!textures.contains(textureName)) {
          throw std::runtime_error(
              "Material ID " + std::to_string(i) + " references texture '" +
              textureName +
              "' but the texture is not defined in the scene file");
        }
        mat.textureName = textureName;
      }
      if (material.HasMember("ior"))
        mat.ior_ = material["ior"].GetDouble();
    } else {
      std::string textureName = material["albedo"].GetString();
      if (!textures.contains(textureName)) {
        throw std::runtime_error(
            "Material ID " + std::to_string(i) + " references texture '" +
            textureName + "' but the texture is not defined in the scene file");
      }
      mat.textureName = textureName;
    }
    mat.smoothShading_ = material["smooth_shading"].GetBool();
  }

  std::vector<Mesh> meshes;
  std::vector<vec3> vertices;
  std::vector<std::array<size_t, 3>> indices;
  std::vector<vec3> uvs;
  for (const auto &object : document["objects"].GetArray()) {
    const auto materialIndexMember = object["material_index"].GetUint64();
    const auto verticesMember = object["vertices"].GetArray();
    const auto trianglesMember = object["triangles"].GetArray();
    const auto uvsMember = object["uvs"].GetArray();
    if (verticesMember.Size() % 3 != 0) {
      throw std::runtime_error("Invalid crtscene file: root.objects.vertices "
                               "length is not a multiple of 3!");
    }
    for (auto vertexCoordinateItr = verticesMember.Begin();
         vertexCoordinateItr != verticesMember.End();) {
      const double x = vertexCoordinateItr++->GetDouble();
      const double y = vertexCoordinateItr++->GetDouble();
      const double z = vertexCoordinateItr++->GetDouble();
      vertices.emplace_back(x, y, z);
    }
    if (trianglesMember.Size() % 3 != 0) {
      throw std::runtime_error(
          "Invalid crtscene file: root.objects.triangles length is not a "
          "multiple of 3!");
    }
    for (auto triangleIndexItr = trianglesMember.Begin();
         triangleIndexItr != trianglesMember.End();) {
      const size_t x = triangleIndexItr++->GetUint64();
      const size_t y = triangleIndexItr++->GetUint64();
      const size_t z = triangleIndexItr++->GetUint64();
      indices.push_back({x, y, z});
    }
    if (uvsMember.Size() % 3 != 0) {
      throw std::runtime_error(
          "Invalid crtscene file: root.objects.uvs length is not a "
          "multiple of 3!");
    }
    for (auto uvsItr = uvsMember.Begin(); uvsItr != uvsMember.End();) {
      const double x = uvsItr++->GetDouble();
      const double y = uvsItr++->GetDouble();
      const double z = uvsItr++->GetDouble();
      uvs.push_back({x, y, z});
    }
    meshes.emplace_back(std::move(vertices), std::move(indices), std::move(uvs),
                        materialIndexMember);
    vertices.clear();
    indices.clear();
  }

  std::vector<Light> lights;
  if (document.HasMember("lights")) {
    for (const auto &light : document["lights"].GetArray()) {
      const auto positionMember = light["position"].GetArray();
      const auto intensityMember = light["intensity"].GetDouble();
      lights.emplace_back(intensityMember, vec3{positionMember[0].GetDouble(),
                                                positionMember[1].GetDouble(),
                                                positionMember[2].GetDouble()});
    }
  }

  Scene scene{std::move(settings), std::move(camera),    std::move(lights),
              std::move(textures), std::move(materials), std::move(meshes)};
  return scene;
}

[[nodiscard]] Camera &Scene::camera() noexcept { return camera_; }

void Scene::cameraTakeSnapshot(const std::string &outFileName,
                               RenderMode debugRenderMode) const {
  const auto timerStart = std::chrono::steady_clock::now();
  const double resolutionWidth =
      static_cast<double>(settings_.imageSettings.width);
  const double resolutionHeight =
      static_cast<double>(settings_.imageSettings.height);
  std::string outputFileBuffer =
      "P3 " + std::to_string(settings_.imageSettings.width) + " " +
      std::to_string(settings_.imageSettings.height) + " 255 ";
  std::unique_ptr<Color[]> buffer(new Color[settings_.imageSettings.width *
                                            settings_.imageSettings.height]);
  double max_distance = 0.;
  for (size_t y = 0; y < settings_.imageSettings.height; ++y) {
    for (size_t x = 0; x < settings_.imageSettings.width; ++x) {
      double worldX = static_cast<double>(x) + .5;
      double worldY = static_cast<double>(y) + .5;

      worldX /= resolutionWidth;
      worldY /= resolutionHeight;

      worldX = worldX * 2 - 1;
      worldY = 1 - worldY * 2;

      worldX *= (resolutionWidth / resolutionHeight);
      vec3 direction{worldX, worldY, -1.0};
      direction = direction * camera_.orientation();
      Ray ray{camera_.position(), direction, settings_.rayMaxDepth};
      buffer[x + y * settings_.imageSettings.width] =
          traceRay(ray, debugRenderMode);
    }
  }
  // this is a stupid way of doing this
  // TODO: change traceRay to return information about distance and hitpoint
  // besides color
  if (debugRenderMode == RenderMode::DistanceShade) {
    for (size_t i = 0;
         i < settings_.imageSettings.width * settings_.imageSettings.height;
         ++i) {
      max_distance = std::max(max_distance, buffer[i].red());
    }
    for (size_t i = 0;
         i < settings_.imageSettings.width * settings_.imageSettings.height;
         ++i) {
      const auto val = buffer[i].red();
      buffer[i] = {val / max_distance, val / max_distance, val / max_distance};
    }
  }
  for (size_t i = 0;
       i < settings_.imageSettings.width * settings_.imageSettings.height;
       ++i) {
    outputFileBuffer += buffer[i].getU8View().toString() + " ";
  }
  const auto timerEnd = std::chrono::steady_clock::now();
  std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(timerEnd - timerStart) << '\n';
  std::ofstream image(outFileName, std::ios::trunc | std::ios::out);
  if (!image.is_open()) {
    throw std::runtime_error("Could not open " + outFileName + " for writing!");
  }
  image << outputFileBuffer;
}

[[nodiscard]] Scene::Settings Scene::settings() const noexcept {
  return settings_;
}

void Scene::overwriteWidth(size_t width) noexcept {
  settings_.imageSettings.width = width;
}

void Scene::overwriteHeight(size_t height) noexcept {
  settings_.imageSettings.height = height;
}

void Scene::overwriteBackgroundColor(const Color &c) noexcept {
  settings_.backgroundColor = c;
}

[[nodiscard]] const Material &
Scene::getMaterialFromId(size_t id) const noexcept {
  return materials_[id];
}
} // namespace RenderEngine