#include <iostream>
#include <utils.hpp>
#include <string>

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cout << "Usage: " << argv[0]
              << " <path to .crtscene file> [output file name]\n";
    return 1;
  }

  if (argc >= 3) {
    Scene::loadScene(std::string{argv[1]}).cameraTakeSnapshot(argv[2]);
  } else {
    Scene::loadScene(std::string{argv[1]}).cameraTakeSnapshot();
  }

  return 0;
}
