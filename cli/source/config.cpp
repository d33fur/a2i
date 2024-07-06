#include "config.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <type_traits>
#include <sstream>

Config::Config() {
    
}

bool Config::execute() const {
  auto command = get<std::string>("command");

  if (command == "play") {
      executePlayFromAudiofile();
      return 1;
  } else if (command == "mic") {
      executePlayFromMic();
      return 1;
  }  else if (command == "config") {
      executeConfig();
      return 0;
  }  else {
      std::cout << "Unknown command" << std::endl;
      return 0;
  }
}

void Config::executePlayFromAudiofile() const {

  std::cout << "play" << std::endl;
}

void Config::executePlayFromMic() const {

  std::cout << "mic" << std::endl;
}

void Config::executeConfig() const {
  // субкомманды обрабатывать
  std::string filename = getConfigFilePath();
  saveToFile(filename, "default");
  std::cout << "Default configuration successfully saved in ~/.a2i/config.json" << std::endl;
}

bool Config::saveToFile(const std::string& filename, const std::string& configname) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    nlohmann::json outputJson;
    outputJson[configname] = configJson;

    outputJson[configname].erase("config");
    outputJson[configname].erase("command");
    outputJson[configname].erase("subcommand");

    file << outputJson.dump(4);
    return true;
}

std::string Config::getConfigFilePath() const {
    std::string homeDir = getenv("HOME");
    std::string configDir = homeDir + "/.a2i";
    mkdir(configDir.c_str(), 0755);
    return configDir + "/config.json";
}

bool Config::validate() const {
    auto frameSize = get<int>("f");
    auto windowFunction = get<int>("w");
    auto lineType = get<int>("l");
    auto graphMode = get<int>("g");
    auto numberOfPreviousFrames = get<int>("n");
    auto colormap = get<int>("grad");
    auto fillType = get<int>("fill");
    auto gradientCoefficient = get<int>("gc");
    auto volume = get<float>("v");

    if (frameSize < 512 || (frameSize & (frameSize - 1)) != 0 ||
        windowFunction < 0 || windowFunction > 9 ||
        lineType < 0 || lineType > 2 ||
        graphMode < 0 || graphMode > 1 ||
        numberOfPreviousFrames <= 0 ||
        colormap < 0 || colormap > 21 ||
        fillType < 0 || fillType > 2 ||
        gradientCoefficient < 0 || gradientCoefficient > 255 ||
        volume < 0.0f || volume > 1.0f) {
        return false;
    }
    return true;
}