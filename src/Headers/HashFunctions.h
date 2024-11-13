#pragma once

#include <string>


#include <vector>
#include <functional>



std::string computeHashSHA256(const std::string& input);
std::string computeHashMD5(const std::string& input);



std::function<std::string(const std::string&)> getHashFunction(int selectedValue);
