// Split this into a Scene and Renderer class to both follow the diagrams from
// the presentation and to reduce the size of it

#include "RenderEngine/Renderer.hpp"
#include "RenderEngine/AccelerationTree.hpp"
#include "RenderEngine/Color.hpp"
#include "RenderEngine/ColorView.tpp"
#include "RenderEngine/Material.hpp"
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
#include <fstream>
#include <iostream>
#include <memory>
#include <stack>
#include <stdexcept>
#include <string>
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

Renderer::Renderer(const Scene &scene) noexcept
    : threadPool_{ThreadPool::getInstance()}, scene_{scene} {}

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

[[nodiscard]] bool intersectsFast(const Scene &scene, const Ray &ray,
                                  const size_t id_triangle,
                                  const double distance) noexcept {
  const auto [id_vertex1, id_vertex2, id_vertex3, id_mesh] =
      scene.triangles_[id_triangle];
  const vec3 &point1 = scene.vertices_[id_vertex1].position;
  const vec3 &point2 = scene.vertices_[id_vertex2].position;
  const vec3 &point3 = scene.vertices_[id_vertex3].position;
  const vec3 &normal = scene.triangleNormals_[id_triangle];
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

[[nodiscard]] double traceShadowRay(const Scene &scene, const vec3 &rayOrigin,
                                    const vec3 &surfaceNormal) {
  double finalLightReached = 0;
  for (const auto &light : scene.lights_) {
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
    std::vector<size_t> trianglIds;
    scene.accelerationTree_.intersects(ray, trianglIds);
    trianglIds.erase(std::unique(trianglIds.begin(), trianglIds.end()),
                     trianglIds.end());
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

[[nodiscard]] Color traceRay(const Scene &scene, const Ray &ray,
                             RenderMode debugRenderMode) {
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
  if (intersectResult.steps == std::numeric_limits<double>::infinity()) {
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
      return Color{(finalNormal.x_ + 1.) / 2, (finalNormal.y_ + 1.) / 2,
                   (finalNormal.z_ + 1.) / 2};
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
    return handleDiffuseMaterial(scene, intersectResult);
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

  return traceShadowRay(scene, offsetHitPoint, finalNormal) *
         getTextureColor(scene, intersectResult);
}

[[nodiscard]] Color
handleReflectiveMaterial(const Scene &scene,
                         const IntersectResult &intersectResult,
                         const Ray &previousRay) noexcept {
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
      traceRay(scene, reflectedRay, RenderMode::Default),
      getTextureColor(scene, intersectResult));
}

// Does not handle refractive and / or reflecive objects inside refractrive
// objects
[[nodiscard]] Color
handleRefractiveMaterial(const Scene &scene,
                         const IntersectResult &intersectResult,
                         const Ray &previousRay) noexcept {
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

    Color reflectionColor = traceRay(scene, reflectedRay, RenderMode::Default);
    Color refractionColor = traceRay(scene, refractedRay, RenderMode::Default);

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
    Color reflectionColor = traceRay(scene, reflectedRay, RenderMode::Default);

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

    Color refractionColor = traceRay(scene, refractedRay, RenderMode::Default);

    double fresnelFactor =
        .5 * std::pow(1. - dotProduct(previousRay.direction_, finalNormal), 5);

    return Color::elementWiseAddition(fresnelFactor * reflectionColor,
                                      (1 - fresnelFactor) * refractionColor);
  }
}

void renderBucket(const Scene &scene, const size_t bucket,
                  const size_t bucketCols, Color *const buffer,
                  RenderMode debugRenderMode, size_t rayMaxDepth) noexcept {
  const double resolutionWidth = static_cast<double>(scene.width_);
  const double resolutionHeight = static_cast<double>(scene.height_);
  size_t bucketOffsetX = (bucket % bucketCols) * scene.bucket_size_;
  size_t bucketOffsetY = (bucket / bucketCols) * scene.bucket_size_;
  for (size_t y = 0; y < scene.bucket_size_; ++y) {
    for (size_t x = 0; x < scene.bucket_size_; ++x) {
      double worldX = static_cast<double>(bucketOffsetX + x) + .5;
      double worldY = static_cast<double>(bucketOffsetY + y) + .5;

      worldX /= resolutionWidth;
      worldY /= resolutionHeight;

      worldX = worldX * 2 - 1;
      worldY = 1 - worldY * 2;

      worldX *= (resolutionWidth / resolutionHeight);
      vec3 direction{worldX, worldY, -1.0};
      direction = direction * scene.camera_.orientation();
      Ray ray{scene.camera_.position(), direction, rayMaxDepth};
      buffer[(bucketOffsetX + x) + (bucketOffsetY + y) * scene.width_] =
          traceRay(scene, ray, debugRenderMode);
    }
  }
}

void Renderer::takeSnapshot(const std::string &outFileName,
                            RenderMode debugRenderMode) const {
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
  std::string outputFileBuffer = "P3 " + std::to_string(scene_.width_) + " " +
                                 std::to_string(scene_.height_) + " 255 ";
  std::unique_ptr<Color[]> buffer(new Color[scene_.width_ * scene_.height_]);

  std::stack<size_t> bucketIdx;
  for (size_t i = 0; i < scene_.width_ * scene_.height_ / scene_.bucket_size_ /
                             scene_.bucket_size_;
       ++i) {
    threadPool_.addTask([this, &buffer, debugRenderMode, i]() {
      renderBucket(scene_, i, scene_.width_ / scene_.bucket_size_, buffer.get(),
                   debugRenderMode, rayMaxDepth_);
    });
  }
  threadPool_.startAndWait();

  double max_distance = 0.;
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
  for (size_t i = 0; i < scene_.width_ * scene_.height_; ++i) {
    outputFileBuffer += buffer[i].getU8View().toString() + " ";
  }
  const auto timerEnd = std::chrono::steady_clock::now();
  std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(timerEnd -
                                                                     timerStart)
            << '\n';
  std::ofstream image(outFileName, std::ios::trunc | std::ios::out);
  if (!image.is_open()) {
    throw std::runtime_error("Could not open " + outFileName + " for writing!");
  }
  image << outputFileBuffer;
}

void Renderer::overwriteMaxRayDepth(size_t value) noexcept {
  rayMaxDepth_ = value;
}
} // namespace RenderEngine