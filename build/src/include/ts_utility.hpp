// Author: Tylor Slay
// Description: These are functions I have build using lots of other resources to
// help with importing, export, and manipulating data. I have tried to follow
// Google's coding standard to make things easier to read and understand.
//
// Google C++ coding standard
// --------------------------
// https://google.github.io/styleguide/cppguide.html
//
// General rules:
// - Indents are two spaces. No tabs should be used anywhere.
// - Each line must be at most 80 characters long.
// - Comments can be // or /* but // is most commonly used.
// - Avoid Abbreviations
// - Type/Function Names: CamelCase
// - Variable Names: lowercase_underscores
// - Class Data Members: trailing_underscores_
// - Constant Names: kCamelCase
// - Enumerator/Macro Names: ALL_CAPS
//
// Note: Please reference http://en.cppreference.com/w/cpp for additional help

#ifndef TS_UTILITY_HPP_INCLUDED
#define TS_UTILITY_HPP_INCLUDED

// INCLUDE
#include <iostream>     // cin, cout
#include <fstream>      // ofstream, ifstream
#include <sstream>      // stringstream, istringstream
#include <string>       // getline, stoi, stod
#include <regex>        // regex_search
#include <map>          // find

// tylor slay utilities (tsu)
namespace tsu {

// map<section, map<property, value>>
// this is only for the INI file methods because it is huge
typedef std::map<std::string, std::map<std::string, std::string>> config_map;

// FileToString
// - this method reads the entire file into a single string
// - it is more efficient to pull the file into memory than parse lines in file
static std::string FileToString (const std::string& kFilename) {
    if (std::ifstream file{kFilename, std::ios::binary | std::ios::ate}) {
        auto buffer = file.tellg();
        std::string str(buffer, '\0');  // construct string to stream size
        file.seekg(0);
        if (file.read(&str[0], buffer)) {
            return str;
        } else {
            std::cout << "[ERROR] reading " << kFilename << std::endl;
            return NULL;
        }
    } else {
        std::cout << "[ERROR] " << kFilename << " not found\n";
        return NULL;
    }
} // end FileToString

// MapConfigFile
// - this method uses regular expressions to create a config map to
//   initialize class members or define program settings.
// - https://en.wikipedia.org/wiki/Regular_expression
static config_map MapConfigFile (const std::string& kFilename) {
    // read file into a string
    config_map file_map;
    std::string file_string, line, section, property, value;
    file_string = FileToString(kFilename);

    // create section and property regular expressions
    std::regex section_regex("^\\[(.*)\\]");
    std::regex property_regex("^(\\w+)=([^\\+]+(?!\\+{3}))");
    std::smatch match_regex;

    // split long string by each new line
    std::stringstream ss(file_string);
    while (std::getline(ss, line)) {
        // first look for section then property
        if (std::regex_search(line, match_regex, section_regex)) {
            section = match_regex[1];
        } else if (std::regex_search(line, match_regex, property_regex)){
            property = match_regex[1];
            value = match_regex[2];
            file_map[section][property] = value;
        }
    }
    return file_map;
} // end MapConfigFile

} // end namespace tsu

#endif // TS_UTILITY_HPP_INCLUDED
