#include <iostream>
#include <utils.hpp>

int main(const int argc, const char **argv) {
  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " [path to .crtscene file]"
              << std::endl;
    return 1;
  }

  Scene::loadScene(std::string{argv[1]}).cameraTakeSnapshot();

  return 0;
}
