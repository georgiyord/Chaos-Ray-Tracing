// Split this into a Scene and Renderer class to both follow the diagrams from
// the presentation and to reduce the size of it

#include "RenderEngine/Renderer.hpp"
#include "RenderEngine/AccelerationTree.hpp"
#include "RenderEngine/Color.hpp"
#include "RenderEngine/Material.hpp"
#include "RenderEngine/Matrix3x3.hpp"
#include "RenderEngine/Scene.hpp"
#include "RenderEngine/Texture.hpp"
#include "RenderEngine/ThreadPool.hpp"
#include "RenderEngine/Triangle.hpp"
#include "RenderEngine/Vertex.hpp"
#include "RenderEngine/utils.hpp"
#include "RenderEngine/vec3.hpp"
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <memory>
#include <random>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

namespace RenderEngine {



Renderer::Renderer(const Scene &scene) noexcept
    : threadPool_{ThreadPool::getInstance()}, scene_{scene} {}

// Moller method with barycentric coordinates
[[nodiscard]] IntersectResult intersects(const Scene &scene, const Ray &ray,
                                         size_t id_triangle) noexcept {
  const auto [id_vertex1, id_vertex2, id_vertex3, id_mesh] =
      scene.triangles_[id_triangle];
  IntersectResult intersectResult;
  const vec3 &v1 = scene.vertices_[id_vertex1].position;
  const vec3 &v2 = scene.vertices_[id_vertex2].position;
  const vec3 &v3 = scene.vertices_[id_vertex3].position;
  const vec3 v1v2 = v2 - v1;
  const vec3 v1v3 = v3 - v1;
  const vec3 v1O = ray.origin_ - v1;
  const vec3 shared1 = crossProduct(v1v3, v1O);
  const vec3 shared2 = crossProduct(v1v2, ray.direction_);
  const float divisor = dotProduct(shared2, v1v3);

  if (std::abs(divisor) < 1e-6f) {
    intersectResult.steps = floatNaN;
    return intersectResult;
  }

  const float reciprocal = 1 / divisor;
  const float u = dotProduct(shared1, ray.direction_) * reciprocal;
  if (u < 0 || u > 1) {
    intersectResult.steps = floatNaN;
    return intersectResult;
  }
  const float v = dotProduct(shared2, v1O) * reciprocal;
  if (v < 0 || v > 1) {
    intersectResult.steps = floatNaN;
    return intersectResult;
  }
  const float w = 1 - u - v;
  if (w < 0 || w > 1) {
    intersectResult.steps = floatNaN;
    return intersectResult;
  }
  const float t = dotProduct(shared1, v1v2) * reciprocal;
  if (t < 0) {
    intersectResult.steps = floatNaN;
    return intersectResult;
  }

  intersectResult.steps = t;
  intersectResult.hitPoint = ray.origin_ + t * ray.direction_;
  intersectResult.id_triangle = id_triangle;
  intersectResult.id_mesh = scene.triangles_[id_triangle].id_mesh_;
  intersectResult.id_material =
      scene.meshes_[intersectResult.id_mesh].id_material_;
  return intersectResult;
}

[[nodiscard]] bool intersectsFast(const Scene &scene, const Ray &ray,
                                  const size_t id_triangle,
                                  const float distanceSquared) noexcept {
  const auto [id_vertex1, id_vertex2, id_vertex3, id_mesh] =
      scene.triangles_[id_triangle];
  const vec3 &v1 = scene.vertices_[id_vertex1].position;
  const vec3 &v2 = scene.vertices_[id_vertex2].position;
  const vec3 &v3 = scene.vertices_[id_vertex3].position;
  const vec3 v1v2 = v2 - v1;
  const vec3 v1v3 = v3 - v1;
  const vec3 shared2 = crossProduct(v1v2, ray.direction_);
  const float divisor = dotProduct(shared2, v1v3);

  if (std::abs(divisor) < 1e-6f) {
    return false;
  }

  const vec3 v1O = ray.origin_ - v1;
  const vec3 shared1 = crossProduct(v1v3, v1O);

  const float reciprocal = 1 / divisor;
  const float u = dotProduct(shared1, ray.direction_) * reciprocal;
  if (u < 0 || u > 1) {
    return false;
  }
  const float v = dotProduct(shared2, v1O) * reciprocal;
  if (v < 0 || v > 1) {
    return false;
  }
  const float w = 1 - u - v;
  if (w < 0 || w > 1) {
    return false;
  }
  const float t = dotProduct(shared1, v1v2) * reciprocal;
  if (t < 0 || t * t > distanceSquared) {
    return false;
  }

  return true;
}

[[nodiscard]] inline vec2
getBarycentricCoordinates(const vec3 &hitPoint, const vec3 &pos_vertex1,
                          const vec3 &pos_vertex2,
                          const vec3 &pos_vertex3) noexcept {
  const auto v1p = hitPoint - pos_vertex1;
  const auto v1v2 = pos_vertex2 - pos_vertex1;
  const auto v1v3 = pos_vertex3 - pos_vertex1;
  const auto triangleAreaDouble = crossProduct(v1v2, v1v3).length();
  const auto reciprocal_triangleAreaDouble = 1 / triangleAreaDouble;
  const auto u =
      crossProduct(v1p, v1v3).length() * reciprocal_triangleAreaDouble;
  const auto v =
      crossProduct(v1v2, v1p).length() * reciprocal_triangleAreaDouble;
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

[[nodiscard]] float traceShadowRay(const Scene &scene, const vec3 &rayOrigin,
                                   const vec3 &surfaceNormal) {
  float finalLightReached = 0;
  for (const auto &light : scene.lights_) {
    vec3 pointToLightSourceVec = light.position_ - rayOrigin;
    Ray ray{rayOrigin, pointToLightSourceVec};
    const auto cosineLawFactor =
        std::max(0.f, dotProduct(ray.direction_, surfaceNormal));
    auto tmpLight = light.intensity_ /
                    (4 * std::numbers::pi_v<float> *
                     pointToLightSourceVec.lengthSquared()) *
                    cosineLawFactor;
    bool shadowRayIntersection = false;
    IntersectResult intersectResult;
    std::vector<size_t> trianglIds;
    scene.accelerationTree_.intersects(ray, trianglIds);
    if (trianglIds.empty()) {
      finalLightReached += tmpLight;
      continue;
    }
    for (const size_t id : trianglIds) {
      // TODO: handle shadow ray through refractive material correctly
      if (scene
              .materials_[scene.meshes_[scene.triangles_[id].id_mesh_]
                              .id_material_]
              .materialType == MaterialType::REFRACTIVE)
        continue;
      shadowRayIntersection =
          intersectsFast(scene, ray, id, pointToLightSourceVec.lengthSquared());
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
    const float reciprocate = 1 / checkerTexture.square_size;
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
  const Color cold = {0, 0, .55f};
  const Color warm = {.3f, .3f, 0};
  const float warmFactor = .6f;
  const float coldFactor = .2f;
  const Color textureColor = getTextureColor(scene, intersectResult);
  const Color coldTint =
      Color::elementWiseAddition(cold, coldFactor * textureColor);
  const Color warmTint =
      Color::elementWiseAddition(warm, warmFactor * textureColor);
  float lightFactor = 0;
  vec3 finalNormal;
  if (scene.materials_[intersectResult.id_material].smoothShading) {
    finalNormal = interpolateNormal(scene, intersectResult);
  } else {
    finalNormal = scene.triangleNormals_[intersectResult.id_triangle];
  }
  for (const auto &light : scene.lights_) {
    lightFactor +=
        (1.f +
         dotProduct(finalNormal,
                    (light.position_ - intersectResult.hitPoint).normalise())) /
        2.f;
  }
  lightFactor /= static_cast<float>(scene.lights_.size());
  lightFactor = std::clamp(lightFactor, 0.f, 1.f);
  Color c = Color::elementWiseAddition(lightFactor * warmTint,
                                       (1 - lightFactor) * coldTint);
  return c;
}

[[nodiscard]] Color Renderer::traceRay(const Scene &scene, const Ray &ray,
                             RenderMode debugRenderMode) const {
  IntersectResult intersectResult;
  std::vector<size_t> trianglIds;
  scene.accelerationTree_.intersects(ray, trianglIds);
  trianglIds.erase(std::unique(trianglIds.begin(), trianglIds.end()),
                   trianglIds.end());
  if (trianglIds.empty()) {
    return scene.backgroundColor_;
  }
  for (const size_t id : trianglIds) {
    const IntersectResult newIntersectResult = intersects(scene, ray, id);
    if (newIntersectResult.steps < intersectResult.steps)
      intersectResult = newIntersectResult;
  }
  if (intersectResult.steps == std::numeric_limits<float>::infinity()) {
    return scene.backgroundColor_;
  }
  switch (debugRenderMode) {
  case RenderMode::NormalShade:
    return [intersectResult, &scene]() -> Color {
      vec3 finalNormal;
      if (scene.materials_[intersectResult.id_material].smoothShading) {
        finalNormal = interpolateNormal(scene, intersectResult);
      } else {
        finalNormal = scene.triangleNormals_[intersectResult.id_triangle];
      }
      return Color{(finalNormal.x_ + 1.f) / 2, (finalNormal.y_ + 1.f) / 2,
                   (finalNormal.z_ + 1.f) / 2};
    }();
  case RenderMode::DistanceShade:
    return {intersectResult.steps, 0, 0};
  case RenderMode::GoochShade:
    return goochShade(scene, intersectResult);
  case RenderMode::BarycentricShade:
    return [intersectResult, &scene]() -> Color {
      const auto [u, v] = getBarycentricCoordinates(scene, intersectResult);
      return Color{u, v, 0};
    }();
  case RenderMode::Default:
    break;
  default:
    throw std::runtime_error("Invalid Render Mode!");
  }
  switch (scene.materials_[intersectResult.id_material].materialType) {
  default:
    throw std::runtime_error("An invalid material type was given at " +
                             std::to_string(__LINE__) +
                             ". That should not happen!");
  case MaterialType::DIFFUSE:
    return handleDiffuseMaterial(intersectResult, ray.depthChances_, debugRenderMode);
  case MaterialType::CONSTANT:
    return getTextureColor(scene, intersectResult);
  case MaterialType::REFLECTIVE:
    return handleReflectiveMaterial(scene, intersectResult, ray);
  case MaterialType::REFRACTIVE:
    Color reflectionColor =
        handleReflectiveMaterial(scene, intersectResult, ray);
    Color refractionColor =
        handleRefractiveMaterial(scene, intersectResult, ray);

    vec3 finalNormal;
    if (scene.materials_[intersectResult.id_material].smoothShading) {
      finalNormal = interpolateNormal(scene, intersectResult);
    } else {
      finalNormal = scene.triangleNormals_[intersectResult.id_triangle];
    }
    float tmp = dotProduct(ray.direction_, finalNormal);
    if (tmp > 0.f)
      tmp = -tmp;

    const float &ior = scene.materials_[intersectResult.id_material].ior;
    const float reciprocate = 1.f / (ior + 1.f);
    const float refractionRelation = (1.f - ior) * (1.f - ior) * reciprocate * reciprocate;
    float fresnelFactor = refractionRelation + (1 - refractionRelation) * std::pow(1 - std::abs(dotProduct(finalNormal, ray.direction_)), 5.f);

    return Color::elementWiseAddition(fresnelFactor * reflectionColor,
                                      (1 - fresnelFactor) * refractionColor);
  }
}

[[nodiscard]] Color
Renderer::handleDiffuseMaterial(
                      const IntersectResult &intersectResult, size_t prevRayDepthChances, RenderMode debugRenderMode) const noexcept {
  if (prevRayDepthChances == 0) {
    return scene_.backgroundColor_;
  }
  const auto [steps, hitPoint, id_triangle, id_material, id_mesh] =
      intersectResult;
  // TODO: light can also be reflected from reflective material (or even
  // refractive) to the diffuse object. Maybe look into implementing virtual
  // lights for each reflective surface, by mirroring the position of the real
  // source
  vec3 finalNormal;
  if (scene_.materials_[id_material].smoothShading) {
    finalNormal = interpolateNormal(scene_, intersectResult);
  } else {
    finalNormal = scene_.triangleNormals_[id_triangle];
  }

  // global illumination

  // random applicances in the standard library are not thread safe
  thread_local std::random_device rd;
  thread_local std::mt19937 gen(rd());
  thread_local std::uniform_real_distribution<float> dis(-180.f, 180.f);

  auto offsetHitPoint = hitPoint + RENDERENGINE_SHADOW_BIAS * finalNormal;
  Color commumilativeColor =  traceShadowRay(scene_, offsetHitPoint, finalNormal) * getTextureColor(scene_, intersectResult);
  size_t successful = 1;

  for (size_t i = 0; i < n_diffuseReflectionsGI_; ++i) {
    const float theta = dis(gen);
    const float phi = dis(gen);
    const vec3 randomUnitVec{std::cos(phi) * std::sin(theta),
                             std::sin(phi) * std::sin(theta), std::cos(theta)};
    const vec3 hemisphereSampleRayDir = (finalNormal + randomUnitVec).normalise();
    const Ray ray{intersectResult.hitPoint + hemisphereSampleRayDir * RENDERENGINE_HITPOINT_BIAS, hemisphereSampleRayDir, prevRayDepthChances - 1};
    const Color result = traceRay(scene_, ray, debugRenderMode);
    if (result != scene_.backgroundColor_){
      commumilativeColor = Color::elementWiseAddition(commumilativeColor, result);
      ++successful;
    }
  }
  float reciprocal = 1.f / static_cast<float>(successful);
  commumilativeColor *= reciprocal;
  return commumilativeColor;
}

[[nodiscard]] Color
Renderer::handleReflectiveMaterial(const Scene &scene,
                         const IntersectResult &intersectResult,
                         const Ray &previousRay) const noexcept {
  if (previousRay.depthChances_ == 0) {
    return scene.backgroundColor_;
  }
  const auto [steps, hitPoint, id_triangle, id_material, id_mesh] =
      intersectResult;
  vec3 finalNormal;
  if (scene.materials_[id_material].smoothShading) {
    finalNormal = interpolateNormal(scene, intersectResult);
  } else {
    finalNormal = scene.triangleNormals_[id_triangle];
  }
  bool frontSide = dotProduct(previousRay.direction_, finalNormal) < .0f;
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
      traceRay(scene, reflectedRay, RenderMode::Default),
      getTextureColor(scene, intersectResult));
}

// Does not handle refractive and / or reflecive objects inside refractrive
// objects
[[nodiscard]] Color
Renderer::handleRefractiveMaterial(const Scene &scene,
                         const IntersectResult &intersectResult,
                         const Ray &previousRay) const noexcept {
  if (previousRay.depthChances_ == 0) {
    return scene.backgroundColor_;
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
    float cosI = -dotProduct(finalNormal, previousRay.direction_);
    float sinI = std::sqrt(1 - cosI * cosI);
    float sinR = sinI / ior;
    float cosR = std::sqrt(1 - sinR * sinR);
    vec3 A = (-1) * finalNormal * cosR;
    vec3 B = previousRay.direction_ + finalNormal * cosI;
    vec3 refractionDirection = A + B.normalise() * sinR;

    Ray refractedRay{hitPoint - RENDERENGINE_HITPOINT_BIAS * finalNormal,
                     refractionDirection, previousRay.depthChances_ - 1};

    Color reflectionColor = traceRay(scene, reflectedRay, RenderMode::Default);
    Color refractionColor = traceRay(scene, refractedRay, RenderMode::Default);

    //Shlick's aproximation
    const float reciprocate = 1.f / (ior + 1.f);
    const float refractionRelation = (ior - 1.f) * (ior - 1.f) * reciprocate * reciprocate;
    float fresnelFactor = refractionRelation + (1 - refractionRelation) * std::pow(1 + dotProduct(finalNormal, previousRay.direction_), 5.f);

    return Color::elementWiseAddition(fresnelFactor * reflectionColor,
                                      (1 - fresnelFactor) * refractionColor);
  } else {
    // ray hit from inside

    vec3 reflectionDirection =
        previousRay.direction_ -
        2 * dotProduct(previousRay.direction_, finalNormal) * finalNormal;
    Ray reflectedRay = {hitPoint - RENDERENGINE_HITPOINT_BIAS * finalNormal,
                        reflectionDirection, previousRay.depthChances_ - 1};
    Color reflectionColor = traceRay(scene, reflectedRay, RenderMode::Default);

    auto ior = scene.materials_[id_material].ior;
    float cosI = dotProduct(finalNormal, previousRay.direction_);
    float sinI = std::sqrt(1 - cosI * cosI);
    float sinR = sinI * ior;
    float cosR = std::sqrt(1 - sinR * sinR);
    vec3 A = finalNormal * cosR;
    vec3 B = previousRay.direction_ - finalNormal * cosI;
    vec3 refractionDirection = A + B.normalise() * sinR;
    if (sinI >= 1 / ior) {
      return reflectionColor;
    }

    Ray refractedRay{hitPoint + RENDERENGINE_HITPOINT_BIAS * finalNormal,
                     refractionDirection, previousRay.depthChances_ - 1};

    Color refractionColor = traceRay(scene, refractedRay, RenderMode::Default);

    const float reciprocate = 1.f / (ior + 1.f);
    const float refractionRelation = (1.f - ior) * (1.f - ior) * reciprocate * reciprocate;
    float fresnelFactor = refractionRelation + (1 - refractionRelation) * std::pow(1 - dotProduct(finalNormal, previousRay.direction_), 5.f);

    return Color::elementWiseAddition(fresnelFactor * reflectionColor,
                                      (1 - fresnelFactor) * refractionColor);
  }
}

void Renderer::renderBucket(const Scene &scene, const size_t bucket,
                  const size_t bucketCols, Color *const buffer,
                  RenderMode debugRenderMode, size_t rayMaxDepth,
                  size_t raySamplesPerPixelSquareSide) const noexcept {
  const float resolutionWidth = static_cast<float>(scene.width_);
  const float resolutionHeight = static_cast<float>(scene.height_);
  size_t bucketOffsetX = (bucket % bucketCols) * scene.bucket_size_;
  size_t bucketOffsetY = (bucket / bucketCols) * scene.bucket_size_;
  const float reciprocal =
      (1.f / (static_cast<float>(raySamplesPerPixelSquareSide) + 1.f));
  for (size_t y = 0; y < scene.bucket_size_; ++y) {
    for (size_t x = 0; x < scene.bucket_size_; ++x) {
      Color color;
      // with this method multiple ray samples could be fired towards empty
      // space without accomplishing anything
      // TODO: shoot a central main ray per pixel, if result is not the
      // backgroun color ( and/or some heuristics?), fire the additional samples
      for (size_t rx = 0; rx < raySamplesPerPixelSquareSide; ++rx) {
        for (size_t ry = 0; ry < raySamplesPerPixelSquareSide; ++ry) {
          float worldX = static_cast<float>(bucketOffsetX + x) +
                         reciprocal * static_cast<float_t>(1 + rx);
          float worldY = static_cast<float>(bucketOffsetY + y) +
                         reciprocal * static_cast<float_t>(1 + ry);

          worldX /= resolutionWidth;
          worldY /= resolutionHeight;

          worldX = worldX * 2 - 1;
          worldY = 1 - worldY * 2;

          worldX *= (resolutionWidth / resolutionHeight);
          vec3 direction{worldX, worldY, -1.0f};
          direction = direction * scene.camera_.orientation();
          Ray ray{scene.camera_.position(), direction, rayMaxDepth};

          color = Color::elementWiseAddition(
              color, traceRay(scene, ray, debugRenderMode));
        }
      }
      color.red() /= static_cast<float>(raySamplesPerPixelSquareSide *
                                        raySamplesPerPixelSquareSide);
      color.blue() /= static_cast<float>(raySamplesPerPixelSquareSide *
                                         raySamplesPerPixelSquareSide);
      color.green() /= static_cast<float>(raySamplesPerPixelSquareSide *
                                          raySamplesPerPixelSquareSide);
      buffer[(bucketOffsetX + x) + (bucketOffsetY + y) * scene.width_] = color;
    }
  }
}

std::unique_ptr<Color[]> Renderer::createColorBuffer() const {
  return std::unique_ptr<Color[]>{new Color[scene_.width_ * scene_.height_]};
}

// todo pass settings as a singular struct context object
std::chrono::milliseconds
Renderer::takeSnapshot(Color *const buffer, RenderMode debugRenderMode,
                       size_t raySamplesPerPixelSquareSide) const {
  if (raySamplesPerPixelSquareSide == 0) {
    throw std::runtime_error("Ray samples should be more than 0!");
  }
  const auto &width = scene_.width_;
  const auto &height = scene_.height_;
  const auto &bucket_size = scene_.bucket_size_;
  if (width % bucket_size != 0 || height % bucket_size != 0) {
    throw std::runtime_error("width and height values must be a multiple of "
                             "bucket_size(" +
                             std::to_string(bucket_size) +
                             ")! Padding is not supported.");
  }
  const auto timerStart = std::chrono::steady_clock::now();

  std::stack<size_t> bucketIdx;
  for (size_t i = 0; i < scene_.width_ * scene_.height_ / scene_.bucket_size_ /
                             scene_.bucket_size_;
       ++i) {
    threadPool_.addTask([this, buffer, debugRenderMode, i,
                         raySamplesPerPixelSquareSide]() {
      renderBucket(scene_, i, scene_.width_ / scene_.bucket_size_, buffer,
                   debugRenderMode, rayMaxDepth_, raySamplesPerPixelSquareSide);
    });
  }
  threadPool_.startAndWait();

  float max_distance = 0.f;
  // this is a stupid way of doing this
  // TODO: change traceRay to return information about distance and hitpoint
  // besides color
  if (debugRenderMode == RenderMode::DistanceShade) {
    for (size_t i = 0; i < scene_.width_ * scene_.height_; ++i) {
      max_distance = std::max(max_distance, buffer[i].red());
    }
    for (size_t i = 0; i < scene_.width_ * scene_.height_; ++i) {
      const auto val = buffer[i].red();
      buffer[i] = {val / max_distance, val / max_distance, val / max_distance};
    }
  }
  const auto timerEnd = std::chrono::steady_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(timerEnd -
                                                               timerStart);
}

void Renderer::overwriteMaxRayDepth(size_t value) noexcept {
  rayMaxDepth_ = value;
}
} // namespace RenderEngine