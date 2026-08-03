#include <charconv>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <string>
#include <string_view>
#include <utils.hpp>

struct ProgramSettings {
  std::string outPath = "scene.ppm";
  size_t renderWidth = 0;
  size_t renderHeight = 0;
  std::string sceneFilePath;
};

inline void printUsage(const char *binaryName,
                       std::ostream &ostream = std::cout) {
  ostream
      << "Usage: " << binaryName << " [options] <path to .crtscene file>\n"
      << "\n"
      << "Options:\n"
      << "\t--help          Print this menu\n"
      << "\t-o <path>       Path to the rendered image\n"
      << "\t-w <width>      The render width. Default value is 0, meaning that "
         "the program will use the settings from the passed scene file.\n"
      << "\t-h <height>     The render height. Default value is 0, meaning "
         "that "
         "the program will use the settings from the passed scene file.\n"
      << '\n';
}

template <int returnValue>
[[noreturn]] inline void printUsageAndExit(const char *binaryName) {
  printUsage(binaryName, std::cerr);
  std::exit(returnValue);
}

template <>
[[noreturn]] inline void printUsageAndExit<0>(const char *binaryName) {
  printUsage(binaryName, std::cout);
  std::exit(0);
}

inline void printInvalidArgumentMessageAndExit(std::string_view message) {
  std::cerr << "Invalid arguments: " << message << "\n";
  std::exit(EXIT_FAILURE);
}

inline size_t parseNumber(std::string_view argument) {
  size_t out;
  const auto parseRes = std::from_chars(argument.begin(), argument.end(), out);
  if (parseRes.ec == std::errc::result_out_of_range) {
    printInvalidArgumentMessageAndExit("A given integer number was too big!");
  }
  if (parseRes.ec == std::errc::invalid_argument ||
      parseRes.ptr != argument.end()) {
    printInvalidArgumentMessageAndExit(
        "Option parameter is not an unsigned integer: '" +
        std::string{argument} + "'");
  }
  return out;
}

inline ProgramSettings processArgs(int argc, const char *const *argv) {
  ProgramSettings programSettings;
  u8 flags = 0;
  constexpr u8 OUTPUT_FLAG = 1;
  constexpr u8 WIDTH_FLAG = 1 << 1;
  constexpr u8 HEIGHT_FLAG = 1 << 2;
  constexpr u8 SCENE_FLAG = 1 << 3;

  for (int i = 1; i < argc; ++i) {
    const std::string_view argument = argv[i];

    if (argument == "--help") {
      printUsageAndExit<EXIT_SUCCESS>(argv[0]);
    }
    if (argument.empty() || argument[0] != '-') {
      if (flags & SCENE_FLAG) {
        printUsageAndExit<EXIT_FAILURE>(argv[0]);
      }
      flags |= SCENE_FLAG;
      programSettings.sceneFilePath = std::string{argument};
      continue;
    }

    const char option = argument.size() > 1 ? argument[1] : '\0';

    switch (option) {
    case 'o':
      if ((flags & OUTPUT_FLAG) != 0 || i == argc - 1) {
        printUsageAndExit<EXIT_FAILURE>(argv[0]);
      }
      flags |= OUTPUT_FLAG;
      programSettings.outPath = std::string{argv[++i]};
      break;
    case 'w':
      if ((flags & WIDTH_FLAG) != 0 || i == argc - 1) {
        printUsageAndExit<EXIT_FAILURE>(argv[0]);
      }
      flags |= WIDTH_FLAG;
      programSettings.renderWidth = parseNumber(argv[++i]);
      break;
    case 'h':
      if ((flags & HEIGHT_FLAG) != 0 || i == argc - 1) {
        printUsageAndExit<EXIT_FAILURE>(argv[0]);
      }
      flags |= HEIGHT_FLAG;
      programSettings.renderHeight = parseNumber(argv[++i]);
      break;
    default:
      printUsageAndExit<EXIT_FAILURE>(argv[0]);
    }
  }

  if ((flags & SCENE_FLAG) == 0) {
    printUsageAndExit<EXIT_FAILURE>(argv[0]);
  }
  return programSettings;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printUsageAndExit<EXIT_FAILURE>(argv[0]);
  }

  try {
    const ProgramSettings programSettings = processArgs(argc, argv);
    auto scene = Scene::loadScene(programSettings.sceneFilePath);
    if (programSettings.renderWidth != 0) {
      scene.overwriteWidth(programSettings.renderWidth);
    }
    if (programSettings.renderHeight != 0) {
      scene.overwriteHeight(programSettings.renderHeight);
    }
    scene.cameraTakeSnapshot(programSettings.outPath);
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }

  return 0;
}
