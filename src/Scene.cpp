// Split this into a Scene and Renderer class to both follow the diagrams from
// the presentation and to reduce the size of it

#include "RenderEngine/Scene.hpp"
#include "RenderEngine/AABB.hpp"
#include "RenderEngine/AccelerationTree.hpp"
#include "RenderEngine/Bitmap.hpp"
#include "RenderEngine/Color.hpp"
#include "RenderEngine/ColorView.tpp"
#include "RenderEngine/Material.hpp"
#include "RenderEngine/Mesh.hpp"
#include "RenderEngine/Texture.hpp"
#include "RenderEngine/Triangle.hpp"
#include "RenderEngine/Vertex.hpp"
#include "RenderEngine/utils.hpp"
#include "RenderEngine/vec3.hpp"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/schema.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include <chrono>
#include <cmath>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace RenderEngine {

struct IntersectResult {
  double steps = doubleInf;
  vec3 hitPoint;
  size_t id_triangle;
  size_t id_material;
  size_t id_mesh;
};

Color handleDiffuseMaterial(const Scene &, const IntersectResult &) noexcept;
Color handleReflectiveMaterial(const Scene &, const IntersectResult &,
                               const Ray &) noexcept;
Color handleRefractiveMaterial(const Scene &, const IntersectResult &,
                               const Ray &) noexcept;

Scene::Scene(Settings &&settings, Camera &&camera, std::vector<Light> &&lights,
             std::vector<Bitmap> &&bitmaps, std::vector<Texture> &&textures,
             std::vector<Material> &&materials, std::vector<Vertex> &&vertices,
             std::vector<Triangle> &&triangles,
             std::vector<vec3> &&triangleNormals, std::vector<Mesh> &&meshes,
             AccelerationTree &&accelerationTree)
    : settings_{std::move(settings)}, camera_{std::move(camera)},
      lights_{std::move(lights)}, bitmaps_{std::move(bitmaps)},
      textures_{std::move(textures)}, materials_{std::move(materials)},
      vertices_{std::move(vertices)}, triangles_{std::move(triangles)},
      triangleNormals_{std::move(triangleNormals)}, meshes_{std::move(meshes)},
      accelerationTree_{std::move(accelerationTree)} {}

[[nodiscard]] IntersectResult intersects(const Scene &scene, const Ray &ray,
                                         size_t id_triangle) noexcept {
  const auto [id_vertex1, id_vertex2, id_vertex3, id_mesh] =
      scene.triangles_[id_triangle];
  const vec3 &point1 = scene.vertices_[id_vertex1].position;
  const vec3 &point2 = scene.vertices_[id_vertex2].position;
  const vec3 &point3 = scene.vertices_[id_vertex3].position;
  const vec3 &normal = scene.triangleNormals_[id_triangle];
  const auto rayStep = dotProduct(ray.direction_, normal);
  IntersectResult intersectResult;
  if (rayStep == 0) {
    intersectResult.steps = doubleNaN;
    return intersectResult;
  }
  const auto planeDistance = dotProduct(point1 - ray.origin_, normal);
  double tSteps = planeDistance / rayStep;

  if (tSteps < 0) {
    intersectResult.steps = doubleNaN;
    return intersectResult;
  }
  vec3 pointPlaneIntersection = ray.origin_ + tSteps * ray.direction_;

  if (dotProduct(normal, crossProduct(point2 - point1,
                                      pointPlaneIntersection - point1)) <
      -RENDERENGINE_HITPOINT_BIAS) {
    intersectResult.steps = doubleNaN;
    return intersectResult;
  }
  if (dotProduct(normal, crossProduct(point3 - point2,
                                      pointPlaneIntersection - point2)) <
      -RENDERENGINE_HITPOINT_BIAS) {
    intersectResult.steps = doubleNaN;
    return intersectResult;
  }
  if (dotProduct(normal, crossProduct(point1 - point3,
                                      pointPlaneIntersection - point3)) <
      -RENDERENGINE_HITPOINT_BIAS) {
    intersectResult.steps = doubleNaN;
    return intersectResult;
  }
  intersectResult.steps = tSteps;
  intersectResult.hitPoint = ray.origin_ + tSteps * ray.direction_;
  intersectResult.id_triangle = id_triangle;
  intersectResult.id_mesh = scene.triangles_[id_triangle].id_mesh_;
  intersectResult.id_material =
      scene.meshes_[intersectResult.id_mesh].id_material_;
  return intersectResult;
}

[[nodiscard]] bool Scene::intersectsFast(const Ray &ray,
                                         const size_t id_triangle,
                                         const double distance) const noexcept {
  const auto [id_vertex1, id_vertex2, id_vertex3, id_mesh] =
      triangles_[id_triangle];
  const vec3 &point1 = vertices_[id_vertex1].position;
  const vec3 &point2 = vertices_[id_vertex2].position;
  const vec3 &point3 = vertices_[id_vertex3].position;
  const vec3 &normal = triangleNormals_[id_triangle];
  const auto rayStep = dotProduct(ray.direction_, normal);
  if (rayStep == 0)
    return false;
  const auto planeDistance = dotProduct(point1 - ray.origin_, normal);
  double tSteps = planeDistance / rayStep;

  if (tSteps < 0)
    return false;
  vec3 pointPlaneIntersection = ray.origin_ + tSteps * ray.direction_;

  if (dotProduct(normal, crossProduct(point2 - point1,
                                      pointPlaneIntersection - point1)) <
      -RENDERENGINE_HITPOINT_BIAS)
    return false;
  if (dotProduct(normal, crossProduct(point3 - point2,
                                      pointPlaneIntersection - point2)) <
      -RENDERENGINE_HITPOINT_BIAS)
    return false;
  if (dotProduct(normal, crossProduct(point1 - point3,
                                      pointPlaneIntersection - point3)) <
      -RENDERENGINE_HITPOINT_BIAS)
    return false;

  if (tSteps > distance)
    return false;
  return true;
}

[[nodiscard]] inline vec2
getBarycentricCoordinates(const vec3 &hitPoint, const vec3 &pos_vertex1,
                          const vec3 &pos_vertex2,
                          const vec3 &pos_vertex3) noexcept {
  const auto v1p = hitPoint - pos_vertex1;
  const auto v1v2 = pos_vertex2 - pos_vertex1;
  const auto v1v3 = pos_vertex3 - pos_vertex1;
  const auto triangleArea = crossProduct(v1v2, v1v3).length();
  const auto reciprocalArea = 1 / triangleArea;
  const auto u = crossProduct(v1p, v1v3).length() * reciprocalArea;
  const auto v = crossProduct(v1v2, v1p).length() * reciprocalArea;
  return {u, v};
}

[[nodiscard]] inline vec2
getBarycentricCoordinates(const Scene &scene,
                          const IntersectResult &intersectResult) noexcept {
  const Triangle &triangle = scene.triangles_[intersectResult.id_triangle];
  const Vertex &vertex1 = scene.vertices_[triangle.id_vertex1_];
  const Vertex &vertex2 = scene.vertices_[triangle.id_vertex2_];
  const Vertex &vertex3 = scene.vertices_[triangle.id_vertex3_];
  return getBarycentricCoordinates(intersectResult.hitPoint, vertex1.position,
                                   vertex2.position, vertex3.position);
}

[[nodiscard]] inline vec3
interpolateNormal(const vec3 &hitPoint, const vec3 &pos_vertex1,
                  const vec3 &pos_vertex2, const vec3 &pos_vertex3,
                  const vec3 &normal_vertex1, const vec3 &normal_vertex2,
                  const vec3 &normal_vertex3) noexcept {
  const auto [u, v] = getBarycentricCoordinates(hitPoint, pos_vertex1,
                                                pos_vertex2, pos_vertex3);
  return (u * normal_vertex2 + v * normal_vertex3 +
          (1 - u - v) * normal_vertex1)
      .normalise();
}

[[nodiscard]] inline vec3
interpolateNormal(const Scene &scene,
                  const IntersectResult &intersectResult) noexcept {
  const Triangle &triangle = scene.triangles_[intersectResult.id_triangle];
  const Vertex &vertex1 = scene.vertices_[triangle.id_vertex1_];
  const Vertex &vertex2 = scene.vertices_[triangle.id_vertex2_];
  const Vertex &vertex3 = scene.vertices_[triangle.id_vertex3_];
  return interpolateNormal(intersectResult.hitPoint, vertex1.position,
                           vertex2.position, vertex3.position, vertex1.normal,
                           vertex2.normal, vertex3.normal);
}

[[nodiscard]] double Scene::traceShadowRay(const vec3 &rayOrigin,
                                           const vec3 &surfaceNormal) const {
  double finalLightReached = 0;
  for (const auto &light : lights_) {
    vec3 pointToLightSourceVec = light.position_ - rayOrigin;
    Ray ray{rayOrigin, pointToLightSourceVec};
    const auto cosineLawFactor =
        std::max(0., dotProduct(ray.direction_, surfaceNormal));
    auto tmpLight =
        light.intensity_ /
        (4 * std::numbers::pi * pointToLightSourceVec.lengthSquared()) *
        cosineLawFactor;
    bool shadowRayIntersection = false;
    IntersectResult intersectResult;
    const std::vector<size_t> trianglIds = accelerationTree_.intersects(ray);
    if (trianglIds.empty()) {
      finalLightReached += tmpLight;
      continue;
    }
    for (const size_t id : trianglIds) {
      // TODO: handle shadow ray through refractive material correctly
      if (materials_[meshes_[triangles_[id].id_mesh_].id_material_]
              .materialType == MaterialType::REFRACTIVE)
        continue;
      shadowRayIntersection =
          intersectsFast(ray, id, pointToLightSourceVec.lengthSquared());
      if (shadowRayIntersection) {
        tmpLight = 0;
        break;
      }
    }
    finalLightReached += tmpLight;
  }
  return finalLightReached;
}

[[nodiscard]] Color getTextureColor(const Scene &scene,
                                    const IntersectResult &intersectResult) {
  const Material &material = scene.materials_[intersectResult.id_material];
  const Texture &texture = scene.textures_[material.textureId];
  if (texture.type == TextureType::ALBEDO) {
    return texture.asAlbedoTexture().albedo;
  } else if (texture.type == TextureType::EDGES) {
    const EdgesTextureView &edgesTexture = texture.asEdgesTexture();
    const auto [u, v] = getBarycentricCoordinates(
        intersectResult.hitPoint,
        scene
            .vertices_[scene.triangles_[intersectResult.id_triangle]
                           .id_vertex1_]
            .position,
        scene
            .vertices_[scene.triangles_[intersectResult.id_triangle]
                           .id_vertex2_]
            .position,
        scene
            .vertices_[scene.triangles_[intersectResult.id_triangle]
                           .id_vertex3_]
            .position);
    if ((u < edgesTexture.edge_width) || (v < edgesTexture.edge_width) ||
        (1 - u - v < edgesTexture.edge_width))
      return edgesTexture.edge_color;
    else
      return edgesTexture.inner_color;
  } else if (texture.type == TextureType::CHECKER) {
    const CheckerTextureView &checkerTexture = texture.asCheckerTexture();
    const Triangle &triangle = scene.triangles_[intersectResult.id_triangle];
    const Vertex &vertex1 = scene.vertices_[triangle.id_vertex1_];
    const Vertex &vertex2 = scene.vertices_[triangle.id_vertex2_];
    const Vertex &vertex3 = scene.vertices_[triangle.id_vertex3_];
    auto [u, v] =
        getBarycentricCoordinates(intersectResult.hitPoint, vertex1.position,
                                  vertex2.position, vertex3.position);
    const vec3 interpolatedUV =
        u * vertex2.uv + v * vertex3.uv + (1 - u - v) * vertex1.uv;
    const double reciprocate = 1 / checkerTexture.square_size;
    size_t uSampled = static_cast<size_t>(interpolatedUV.x_ * reciprocate);
    size_t vSampled = static_cast<size_t>(interpolatedUV.y_ * reciprocate);
    if ((uSampled % 2 == 0 && vSampled % 2 == 0) ||
        (uSampled % 2 == 1 && vSampled % 2 == 1))
      return checkerTexture.color_A;
    else
      return checkerTexture.color_B;
  } else if (texture.type == TextureType::BITMAP) {
    const BitmapTextureView &bitmapTexture = texture.asBitmapTexture();
    const Triangle &triangle = scene.triangles_[intersectResult.id_triangle];
    const Vertex &vertex1 = scene.vertices_[triangle.id_vertex1_];
    const Vertex &vertex2 = scene.vertices_[triangle.id_vertex2_];
    const Vertex &vertex3 = scene.vertices_[triangle.id_vertex3_];
    auto [u, v] =
        getBarycentricCoordinates(intersectResult.hitPoint, vertex1.position,
                                  vertex2.position, vertex3.position);
    const vec3 interpolatedUV =
        u * vertex2.uv + v * vertex3.uv + (1 - u - v) * vertex1.uv;

    return scene.bitmaps_[bitmapTexture.id_bitmap].getColor(
        interpolatedUV.x_, 1 - interpolatedUV.y_);
  } else
    throw std::runtime_error(
        "Tried to render an object with invalid texture type! This error "
        "should have been caught earlier by the validator or parser!!!");
}

[[nodiscard]] Color
goochShade(const Scene &scene,
           const IntersectResult &intersectResult) noexcept {
  if (scene.materials_[intersectResult.id_material].materialType ==
          MaterialType::REFLECTIVE ||
      scene.materials_[intersectResult.id_material].materialType ==
          MaterialType::REFRACTIVE) {
    return Colors::White;
  }
  const Color cold = {0, 0, .55};
  const Color warm = {.3, .3, 0};
  const double warmFactor = .6;
  const double coldFactor = .2;
  const Color coldTint = Color::elementWiseAddition(
      cold, coldFactor * getTextureColor(scene, intersectResult));
  const Color warmTint = Color::elementWiseAddition(
      warm, warmFactor * getTextureColor(scene, intersectResult));
  double lightFactor = 0;
  vec3 finalNormal;
  if (scene.materials_[intersectResult.id_material].smoothShading) {
    finalNormal = interpolateNormal(scene, intersectResult);
  } else {
    finalNormal = scene.triangleNormals_[intersectResult.id_triangle];
  }
  for (const auto &light : scene.lights_) {
    lightFactor +=
        (1. +
         dotProduct(finalNormal,
                    (light.position_ - intersectResult.hitPoint).normalise())) /
        2.;
  }
  lightFactor /= static_cast<double>(scene.lights_.size());
  lightFactor = std::clamp(lightFactor, 0., 1.);
  Color c = Color::elementWiseAddition(lightFactor * warmTint,
                                       (1 - lightFactor) * coldTint);
  return c;
}

[[nodiscard]] Color Scene::traceRay(const Ray &ray,
                                    RenderMode debugRenderMode) const {
  IntersectResult intersectResult;
  const std::vector<size_t> trianglIds = accelerationTree_.intersects(ray);
  if (trianglIds.empty()) {
    return settings_.backgroundColor;
  }
  for (const size_t id : trianglIds) {
    const IntersectResult newIntersectResult = intersects(*this, ray, id);
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
      if (materials_[intersectResult.id_material].smoothShading) {
        finalNormal = interpolateNormal(*this, intersectResult);
      } else {
        finalNormal = triangleNormals_[intersectResult.id_triangle];
      }
      return Color{(finalNormal.x_ + 1.) / 2, (finalNormal.y_ + 1.) / 2,
                   (finalNormal.z_ + 1.) / 2};
    }();
  case RenderMode::DistanceShade:
    return {intersectResult.steps, 0, 0};
  case RenderMode::GoochShade:
    return goochShade(*this, intersectResult);
  case RenderMode::BarycentricShade:
    return [intersectResult, this]() -> Color {
      const auto [u, v] = getBarycentricCoordinates(*this, intersectResult);
      return Color{u, v, 0};
    }();
  case RenderMode::Default:
    break;
  default:
    throw std::runtime_error("Invalid Render Mode!");
  }
  switch (materials_[intersectResult.id_material].materialType) {
  default:
    throw std::runtime_error("An invalid material type was given at " +
                             std::to_string(__LINE__) +
                             ". That should not happen!");
  case MaterialType::DIFFUSE:
    return handleDiffuseMaterial(*this, intersectResult);
  case MaterialType::CONSTANT:
    return getTextureColor(*this, intersectResult);
  case MaterialType::REFLECTIVE:
    return handleReflectiveMaterial(*this, intersectResult, ray);
  case MaterialType::REFRACTIVE:
    Color reflectionColor =
        handleReflectiveMaterial(*this, intersectResult, ray);
    Color refractionColor =
        handleRefractiveMaterial(*this, intersectResult, ray);

    vec3 finalNormal;
    if (materials_[intersectResult.id_material].smoothShading) {
      finalNormal = interpolateNormal(*this, intersectResult);
    } else {
      finalNormal = triangleNormals_[intersectResult.id_triangle];
    }
    double tmp = dotProduct(ray.direction_, finalNormal);
    if (tmp > 0.)
      tmp = -tmp;

    double fresnelFactor = .5 * std::pow(1. + tmp, 5);

    return Color::elementWiseAddition(fresnelFactor * reflectionColor,
                                      (1 - fresnelFactor) * refractionColor);
  }
}

[[nodiscard]] Color
handleDiffuseMaterial(const Scene &scene,
                      const IntersectResult &intersectResult) noexcept {
  const auto [steps, hitPoint, id_triangle, id_material, id_mesh] =
      intersectResult;
  // TODO: light can also be reflected from reflective material (or even
  // refractive) to the diffuse object. Maybe look into implementing virtual
  // lights for each reflective reflective surface, by mirroring the position of
  // the real source
  vec3 finalNormal;
  if (scene.materials_[id_material].smoothShading) {
    finalNormal = interpolateNormal(scene, intersectResult);
  } else {
    finalNormal = scene.triangleNormals_[id_triangle];
  }
  auto offsetHitPoint = hitPoint + RENDERENGINE_SHADOW_BIAS * finalNormal;

  return scene.traceShadowRay(offsetHitPoint, finalNormal) *
         getTextureColor(scene, intersectResult);
}

[[nodiscard]] Color
handleReflectiveMaterial(const Scene &scene,
                         const IntersectResult &intersectResult,
                         const Ray &previousRay) noexcept {
  if (previousRay.depthChances_ == 0) {
    return scene.settings_.backgroundColor;
  }
  const auto [steps, hitPoint, id_triangle, id_material, id_mesh] =
      intersectResult;
  vec3 finalNormal;
  if (scene.materials_[id_material].smoothShading) {
    finalNormal = interpolateNormal(scene, intersectResult);
  } else {
    finalNormal = scene.triangleNormals_[id_triangle];
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
      scene.traceRay(reflectedRay, RenderMode::Default),
      getTextureColor(scene, intersectResult));
}

// Does not handle refractive and / or reflecive objects inside refractrive
// objects
[[nodiscard]] Color
handleRefractiveMaterial(const Scene &scene,
                         const IntersectResult &intersectResult,
                         const Ray &previousRay) noexcept {
  if (previousRay.depthChances_ == 0) {
    return scene.settings_.backgroundColor;
  }
  const auto [steps, hitPoint, id_triangle, id_material, id_mesh] =
      intersectResult;

  vec3 finalNormal;
  if (scene.materials_[id_material].smoothShading) {
    finalNormal = interpolateNormal(scene, intersectResult);
  } else {
    finalNormal = scene.triangleNormals_[id_triangle];
  }

  if (dotProduct(finalNormal, previousRay.direction_) < 0) {
    // ray hit from outside

    vec3 reflectionDirection =
        previousRay.direction_ -
        2 * dotProduct(previousRay.direction_, finalNormal) * finalNormal;
    Ray reflectedRay = {hitPoint + RENDERENGINE_HITPOINT_BIAS * finalNormal,
                        reflectionDirection, previousRay.depthChances_ - 1};

    auto ior = scene.materials_[id_material].ior;
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

    Color reflectionColor = scene.traceRay(reflectedRay, RenderMode::Default);
    Color refractionColor = scene.traceRay(refractedRay, RenderMode::Default);

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
    Color reflectionColor = scene.traceRay(reflectedRay, RenderMode::Default);

    auto ior = scene.materials_[id_material].ior;
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

    Color refractionColor = scene.traceRay(refractedRay, RenderMode::Default);

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

  // verify scene file correctness outside of schema validation
  // matrix of camera and vertices indeces
  // albedo in materials that are not refractive
  Settings settings = {
      arrToColorObject(document["settings"]["background_color"]),
      {document["settings"]["image_settings"]["width"].GetUint64(),
       document["settings"]["image_settings"]["height"].GetUint64()}};

  Camera camera = {arrToVec3(document["camera"]["position"]),
                   arrToMatrix3x3(document["camera"]["matrix"])};

  std::vector<Light> lights;
  lights.reserve(document["lights"].GetArray().Size());
  std::vector<Bitmap> bitmaps;
  std::unordered_map<std::string_view, size_t> texturesIdsTable;
  std::vector<Texture> textures;
  textures.reserve(document["textures"].GetArray().Size());
  std::vector<Material> materials;
  textures.reserve(document["materials"].GetArray().Size());
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
          double{}, -1);
      break;
    case TextureType::EDGES:
      textures.emplace_back(
          TextureType::EDGES,
          arrToColorObject(textureObject["inner_color"].GetArray()),
          arrToColorObject(textureObject["edge_color"].GetArray()),
          textureObject["edge_width"].GetDouble(), -1);
      break;
    case TextureType::CHECKER:
      textures.emplace_back(
          TextureType::CHECKER,
          arrToColorObject(textureObject["color_A"].GetArray()),
          arrToColorObject(textureObject["color_B"].GetArray()),
          textureObject["square_size"].GetDouble(), -1);
      break;
    case TextureType::BITMAP:
      bitmaps.emplace_back(
          sceneDir.string() +
          std::string_view{textureObject["file_path"].GetString(),
                           textureObject["file_path"].GetStringLength()});
      textures.emplace_back(TextureType::BITMAP, Color{}, Color{}, double{},
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
                             material["smooth_shading"].GetBool(), double{});
      break;
    case MaterialType::REFLECTIVE:
      materials.emplace_back(MaterialType::REFLECTIVE,
                             texturesIdsTable.at(std::string_view{
                                 material["albedo"].GetString(),
                                 material["albedo"].GetStringLength()}),
                             material["smooth_shading"].GetBool(), double{});
      break;
    case MaterialType::REFRACTIVE:
      materials.emplace_back(MaterialType::REFRACTIVE,
                             texturesIdsTable.at(std::string_view{
                                 material["albedo"].GetString(),
                                 material["albedo"].GetStringLength()}),
                             material["smooth_shading"].GetBool(),
                             material["ior"].GetDouble());
      break;
    case MaterialType::CONSTANT:
      materials.emplace_back(MaterialType::CONSTANT,
                             texturesIdsTable.at(std::string_view{
                                 material["albedo"].GetString(),
                                 material["albedo"].GetStringLength()}),
                             material["smooth_shading"].GetBool(), double{});
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
      const double x = vertexCoordinateItr++->GetDouble();
      const double y = vertexCoordinateItr++->GetDouble();
      const double z = vertexCoordinateItr++->GetDouble();
      vertices.emplace_back(vec3{x, y, z}, vec3::zero(), vec3::zero());
    }
    for (rapidjson::SizeType i = 0; i < uvsMember.Size();) {
      const double x = uvsMember[i++].GetDouble();
      const double y = uvsMember[i++].GetDouble();
      const double z = uvsMember[i++].GetDouble();
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
      const auto intensityMember = light["intensity"].GetDouble();
      lights.emplace_back(intensityMember, vec3{positionMember[0].GetDouble(),
                                                positionMember[1].GetDouble(),
                                                positionMember[2].GetDouble()});
    }
  }

  double minX, minY, minZ, maxX, maxY, maxZ;
  minX = minY = minZ = doubleInf;
  maxX = maxY = maxZ = -doubleInf;
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

  Scene scene{std::move(settings),        std::move(camera),
              std::move(lights),          std::move(bitmaps),
              std::move(textures),        std::move(materials),
              std::move(vertices),        std::move(triangles),
              std::move(triangleNormals), std::move(meshes),
              std::move(accelerationTree)};
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