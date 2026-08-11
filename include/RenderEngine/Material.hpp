#ifndef RRENDERENGINE_MATERIAL_HPP
#define RRENDERENGINE_MATERIAL_HPP

#include <RenderEngine/Color.hpp>
#include <string>
namespace RenderEngine {
enum class MaterialType { DIFFUSE, REFLECTIVE, REFRACTIVE, CONSTANT };
[[nodiscard]] MaterialType
getMaterialTypeFromString(const std::string &str);

struct Material {
  MaterialType materialType_ = MaterialType::CONSTANT;
  std::string textureName;
  bool smoothShading_ = false;
  double ior_ = 1;
  
};
} // namespace RenderEngine

#endif // RRENDERENGINE_MATERIAL_HPP