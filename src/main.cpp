#include <charconv>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <string>
#include <utils.hpp>

struct ProgramSettings {
  std::string outPath = "scene.ppm";
  size_t renderWidth = 0;
  size_t renderHeight = 0;
};

enum class ArgumentType { NONE, OUTPUT, WIDTH, HEIGHT };

inline void printUsage(const char *binaryName,
                       std::ostream &ostream = std::cout) {
  ostream
      << "Usage: " << binaryName << " [options] <path to .crtscene file>\n"
      << "\n"
      << "Options:\n"
      << "\t--help :      print this menu"
      << "\t-o <path> :   path to the rendered image\n"
      << "\t-w <width> :  The render width. Default value is 0, meaning that "
         "the program will use the settings from the passed scene file.\n"
      << "\t-h <height> : The render height. Default value is 0, meaning that "
         "the program will use the settings from the passed scene file.\n"
      << std::endl;
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

[[noreturn]] inline void printNoDuplicateArgumentsErrorAndExit() {
  std::cerr << "Invalid arguments: Specified argument option more than once!"
            << std::endl;
  std::exit(1);
}
[[noreturn]] inline void printNumberTooBigAndExit() {
  std::cerr << "Invalid arguments: A given integer number was too big!"
            << std::endl;
  std::exit(1);
}

inline ProgramSettings processArgs(int argc, const char *const *argv) {
  ProgramSettings programSettings;
  u8 BitField_Flags = 0;
  constexpr u8 OUTPUT_FLAG = 1;
  constexpr u8 WIDTH_FLAG = 1 << 1;
  constexpr u8 HEIGHT_FLAG = 1 << 2;
  ArgumentType argumentType = ArgumentType::NONE;
  for (int i = 1; i < argc - 1; ++i) {
    std::string_view argument = argv[i];
    if ((argument[0] == '-' && argumentType != ArgumentType::NONE) ||
        (argument[0] != '-' && argumentType == ArgumentType::NONE)) {
      printUsageAndExit<EXIT_FAILURE>(argv[0]);
    }
    if (argumentType == ArgumentType::NONE) {
      switch (argument[1]) {
      case 'o':
        if ((BitField_Flags & OUTPUT_FLAG) != 0)
          printNoDuplicateArgumentsErrorAndExit();
        argumentType = ArgumentType::OUTPUT;
        BitField_Flags |= OUTPUT_FLAG;
        break;
      case 'w':
        if ((BitField_Flags & WIDTH_FLAG) != 0)
          printNoDuplicateArgumentsErrorAndExit();
        argumentType = ArgumentType::WIDTH;
        BitField_Flags |= WIDTH_FLAG;
        break;
      case 'h':
        if ((BitField_Flags & HEIGHT_FLAG) != 0)
          printNoDuplicateArgumentsErrorAndExit();
        argumentType = ArgumentType::HEIGHT;
        BitField_Flags |= HEIGHT_FLAG;
        break;
      case '-':
        if (argument == "--help") {
          printUsageAndExit<EXIT_SUCCESS>(argv[0]);
        }
        [[fallthrough]];
      default:
        printUsageAndExit<EXIT_FAILURE>(argv[0]);
      }
    } else {
      switch (argumentType) {
        std::from_chars_result parseRes;
      case ArgumentType::OUTPUT:
        programSettings.outPath = std::string{argument};
        break;
      case ArgumentType::WIDTH:
        parseRes = std::from_chars(argument.begin(), argument.end(),
                                   programSettings.renderWidth);
        if (parseRes.ec == std::errc::invalid_argument) {
          printUsageAndExit<EXIT_FAILURE>(argv[0]);
        }
        if (parseRes.ec == std::errc::result_out_of_range) {
          printNumberTooBigAndExit();
        }
        break;
      case ArgumentType::HEIGHT:
        parseRes = std::from_chars(argument.begin(), argument.end(),
                                   programSettings.renderHeight);
        if (parseRes.ec == std::errc::invalid_argument) {
          printUsageAndExit<EXIT_FAILURE>(argv[0]);
        }
        if (parseRes.ec == std::errc::result_out_of_range) {
          printNumberTooBigAndExit();
        }
        break;
      case ArgumentType::NONE:
      default:
        // ???
        std::abort();
      }
    }
  }
  return programSettings;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printUsageAndExit<EXIT_FAILURE>(argv[0]);
  }

  ProgramSettings programSettings = processArgs(argc, argv);
  auto scene = Scene::loadScene(argv[argc - 1]);
  if (programSettings.renderWidth != 0)
    scene.overwriteWidth(programSettings.renderWidth);
  if (programSettings.renderHeight != 0)
    scene.overwriteHeight(programSettings.renderHeight);
  scene.cameraTakeSnapshot(programSettings.outPath);

  return 0;
}
