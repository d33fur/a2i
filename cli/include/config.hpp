#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "nlohmann/json.hpp"
#include <string>
#include <opencv2/opencv.hpp>


class Config {
public:
    Config();
    bool execute() const;

    bool saveToFile(const std::string& filename) const;
    std::string getConfigFilePath() const;
    bool validate() const;

    template <typename T>
    T get(const std::string& key) const;

    template <typename T>
    void set(const std::string& key, const T& value);

    template <typename T>
    void push_back(const std::string& key, const T& value);

    nlohmann::json configJson;
};

template<typename T>
T Config::get(const std::string& key) const {
    if (!configJson.contains(key)) {
        throw std::runtime_error("Key not found: " + key);
    }

    if constexpr (std::is_same<T, cv::Scalar>::value) {
        std::string valueStr = configJson.at(key).get<std::string>();
        std::vector<int> values;
        std::stringstream ss(valueStr);
        std::string item;
        while (getline(ss, item, ',')) {
            values.push_back(std::stoi(item));
        }
        if (values.size() != 3) {
            throw std::runtime_error("Invalid format for cv::Scalar: " + valueStr);
        }
        return cv::Scalar(values[2], values[1], values[0]);
    } else {
        return configJson.at(key).get<T>();
    }
}

template <typename T>
void Config::set(const std::string& key, const T& value) {
    configJson[key] = value;
}

template <typename T>
void Config::push_back(const std::string& key, const T& value) {
    if (!configJson[key].is_array()) {
        configJson[key] = nlohmann::json::array();
    }
    configJson[key].push_back(value);
}

#endif // CONFIG_HPP
