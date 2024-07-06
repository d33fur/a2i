#include "parser.hpp"
#include "version.hpp"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>

Parser::Parser(int argc, char** argv, Config& c) {
    loadConfigFromFile(c, "default");
    parseArgs(argc, argv, c);
}

void Parser::loadConfigFromFile(Config& c, std::string configName) {
    auto filename = c.getConfigFilePath();
    std::ifstream configFile(filename);
    nlohmann::json flags;
    if (configFile.is_open()) {
        configFile >> flags;
        auto defaultConfig = flags[configName];
        for (auto& item : defaultConfig.items()) {
            c.set(item.key(), item.value());
        }
    } else {
        loadDefault(c);
    }
}

void Parser::loadDefault(Config& c) {
    for (auto& f : arguments["flags"].items()) {
      if(f.key() != "config")
        c.set(f.key(), f.value()["default"]);
    }
}

void Parser::parseArgs(int argc, char** argv, Config& c) {
    std::string command = argv[1];
    int counter = 2;
    if (arguments["commands"].contains(command)) {
        int needValue = arguments["commands"][command].value("needValue", 0);
        for(int i = 2; i < 2 + needValue; ++i, ++counter) {
            c.push_back("command_arguments", argv[i]);
        }
        c.set("command", command);
    } else {
        c.set("command", "play");
        c.set("audiofile", command);
    }

    for (int i = counter; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg[0] == '-') {
            size_t equalPos = arg.find('=');
            std::string flag = arg.substr(1, equalPos - 1);
            if (arguments["flags"].contains(flag)) {
                if (arguments["flags"][flag]["needValue"]) {
                    if (equalPos + 1 < arg.length()) {
                        std::string value = arg.substr(equalPos + 1);
                        c.set(flag, value);
                    } else {
                        throw std::runtime_error("Expected value for flag -" + flag);
                    }
                } else {
                    c.set(flag, true);
                }
            } else {
                throw std::runtime_error("Unknown flag -" + flag);
            }
        } 
    }
}

void Parser::printUsage() const {
    std::cout << "Usage: \n"
              << "  a2i <path>/<command> [options]\n\n"
              << "Commands:\n";
    for (auto& cmd : arguments["commands"].items()) {
        std::cout << "  " << cmd.key() << "                   " << cmd.value()["about"].get<std::string>() << "\n";
    }
    std::cout << "\nOptions:\n";
    for (auto& flag : arguments["flags"].items()) {
        std::cout << "  -" << flag.key() << "=" << flag.value()["default"].dump() << "               " << flag.value()["about"].get<std::string>() << "\n";
    }
    printControls();
}

void Parser::printControls() const {
    std::cout << "\nControls:\n";
    for (auto& arg : arguments["controls"].items()) {
        std::cout << arg.key() << " " << arg.value()["about"].get<std::string>() << "\n";
    }
}
