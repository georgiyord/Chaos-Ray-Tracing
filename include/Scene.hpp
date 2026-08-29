#ifndef RENDERENGINE_SCENE_HPP
#define RENDERENGINE_SCENE_HPP

#include "AccelerationTree.hpp"
#include "Bitmap.hpp"
#include "Camera.hpp"
#include "Color.hpp"
#include "Light.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"
#include "Triangle.hpp"
#include "Vertex.hpp"
#include "vec2.hpp"

#include <string>
#include <vector>

namespace RenderEngine {
struct Scene {

  size_t width_;
  size_t height_;
  size_t bucket_size_;
  Color backgroundColor_;
  Camera camera_;
  std::vector<Light> lights_;
  std::vector<Bitmap> bitmaps_;
  std::vector<Texture> textures_;
  std::vector<Material> materials_;
  std::vector<Vertex> vertices_;
  std::vector<Triangle> triangles_;
  std::vector<vec3> triangleNormals_;
  std::vector<Mesh> meshes_;
  AccelerationTree accelerationTree_;

  Scene(size_t, size_t, size_t, Color &&, Camera &&, std::vector<Light> &&,
        std::vector<Bitmap> &&, std::vector<Texture> &&,
        std::vector<Material> &&, std::vector<Vertex> &&,
        std::vector<Triangle> &&, std::vector<vec3> &&, std::vector<Mesh> &&,
        AccelerationTree &&);

public:
  [[nodiscard]] static Scene loadScene(const std::string &filename);
  [[nodiscard]] Camera &camera() noexcept;

  void overwriteWidth(size_t width) noexcept;

  void overwriteHeight(size_t height) noexcept;

  void overwriteBackgroundColor(const Color &c) noexcept;

  [[nodiscard]] const Material &getMaterialFromId(size_t id) const noexcept;
};
} // namespace RenderEngine

#endif // RENDERENGINE_SCENE_HPP