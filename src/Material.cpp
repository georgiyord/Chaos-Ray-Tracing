#include "Material.hpp"
#include <stdexcept>
namespace RenderEngine {

[[nodiscard]] MaterialType getMaterialTypeFromString(const std::string &str) {
  if (str == "diffuse") {
    return MaterialType::DIFFUSE;
  } else if (str == "reflective") {
    return MaterialType::REFLECTIVE;
  } else if (str == "refractive") {
    return MaterialType::REFRACTIVE;
  } else if (str == "constant") {
    return MaterialType::CONSTANT;
  } else [[unlikely]] {
    throw std::runtime_error("Invalid Material Type enum string: " + str +
                             "! This should have been caught by the validator "
                             "but failed during the enum parser?");
  }
}

} // namespace RenderEngine