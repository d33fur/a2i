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
        if (flags.contains(configName)) {
            auto defaultConfig = flags[configName];
            for (auto& item : defaultConfig.items()) {
                if (arguments["flags"].contains(item.key()) && arguments["flags"][item.key()]["dash"] == 1) {
                    c.set(item.key(), item.value());
                } else {
                    std::cout << "Unknown flag in default config: " << item.key() << std::endl;
                }
            }
        } else {
            std::cout << "Configuration '" << configName << "' not found. Loading default settings." << std::endl;
            loadDefault(c);
        }
    } else {
        std::cerr << "Failed to open config file: " << filename << std::endl;
        loadDefault(c);
    }
}


void Parser::loadDefault(Config& c) {
    for (auto& f : arguments["flags"].items()) {
      if(f.value()["dash"] == 1 && f.value().contains("default")) {
        c.set(f.key(), f.value()["default"]);
      }
    }
}

void Parser::parseArgs(int argc, char** argv, Config& c) {
    if (argc < 2) {
        throw std::runtime_error("No command provided");
    }

    std::string command = argv[1];
    int counter = 2;
    bool containsCommand = arguments["commands"].contains(command);
    bool containsSubcommands = false;
    bool needFlags = true;

    if (containsCommand) {
        containsSubcommands = arguments["commands"][command].contains("subcommands");
    }

    if (containsSubcommands) {
        if (argc < 3) {
            throw std::runtime_error("No subcommand provided");
        }
        std::string subcommand = argv[2];

        if (!arguments["commands"][command]["subcommands"].contains(subcommand)) {
            throw std::runtime_error("Unknown subcommand: " + subcommand);
        }

        int needValue = arguments["commands"][command]["subcommands"][subcommand].value("needValue", 0);
        counter++;
        for (int i = 3; i < 3 + needValue; ++i, ++counter) {
            c.push_back("subcommand_arguments", argv[i]);
        }
        c.set("command", command);
        c.set("subcommand", subcommand);

        needFlags = arguments["commands"][command]["subcommands"][subcommand].value("needFlags", 0);
    } 
    else if (containsCommand) {
        int needValue = arguments["commands"][command].value("needValue", 0);
        for (int i = 2; i < 2 + needValue; ++i, ++counter) {
            c.push_back("command_arguments", argv[i]);
        }
        c.set("command", command);

        needFlags = arguments["commands"][command].value("needFlags", 0);
    } 
    else if (command.substr(0, 2) == "--" && arguments["flags"].contains(command.substr(2, command.length()))) {
        std::string flag = command.substr(2, command.length());
        if (arguments["flags"].contains(flag) && arguments["flags"][flag].contains("dash") && arguments["flags"][flag]["dash"] == 2) {
            if (flag == "help") {
                printUsage();
                c.end = true;
                return;
            } else if (flag == "version") {
                printVersion();
                c.end = true;
                return;
            } else {
                throw std::runtime_error("Unknown flag --" + flag);
            }
        } else {
            throw std::runtime_error("Unknown flag --" + flag);
        }
    }
    else {
      std::string format = command.substr(command.length()-4, command.length());
      if(format == ".mp3" || format == ".wav") {
        c.set("command", "play");
        c.set("audiofile", command);
      }
      else {
        throw std::runtime_error("Unknown command " + command);
      }
    }

    for (int i = counter; i < argc && needFlags; ++i) {
        std::string arg = argv[i];

        if (arg[0] == '-' && arg[1] != '-') {
            size_t equalPos = arg.find('=');
            std::string flag = arg.substr(1, equalPos - 1);
            if (arguments["flags"].contains(flag)) {
                if (arguments["flags"][flag].contains("needValue") && arguments["flags"][flag]["needValue"]) {
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
        else {
            throw std::runtime_error("Unknown argument: " + arg);
        }
    }
    
    c.end = false;
}


void Parser::printUsage() const {  // сделать как на гите
    std::cout << "Usage: \n"
              << "  a2i <path>/<command> [options]\n\n"
              << "Commands:\n";
    for (const auto& cmd : arguments["commands"].items()) {
        if (cmd.value().contains("about")) {
            std::cout << "  " << cmd.key() << "                   " << cmd.value()["about"].get<std::string>() << "\n";
        } else {
            std::cout << "  " << cmd.key() << "                   (no description available)\n";
        }
    }

    std::cout << "\nOptions:\n";
    for (const auto& flag : arguments["flags"].items()) {
        std::cout << "  -" << flag.key() << "=" << (flag.value().contains("default") ? flag.value()["default"].dump() : "(no default)") 
                  << "               " << (flag.value().contains("about") ? flag.value()["about"].get<std::string>() : "(no description)") << "\n";
    }

    printControls();
}


void Parser::printControls() const {
    std::cout << "\nControls:\n";
    for (const auto& arg : arguments["controls"].items()) {
        if (arg.value().contains("about")) {
            std::cout << arg.key() << " " << arg.value()["about"].get<std::string>() << "\n";
        } else {
            std::cout << arg.key() << " (no description available)\n";
        }
    }
}


void Parser::printVersion() const {
    std::cout << "a2i version " << A2I_VERSION << std::endl;
}