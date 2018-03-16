// Copyright 2017-2018 utz. See LICENSE for details.

#ifndef BINTRACKER_HELPERS__H__
#define BINTRACKER_HELPERS__H__

#include <vector>
#include <string>

enum ParameterType {MD_BOOL, MD_BYTE, MD_WORD, MD_DEC, MD_HEX, MD_STRING, MD_INVALID};

bool isNumber(const std::string& str);
int64_t strToNum(std::string str);
std::string numToStr(const int64_t& num, const int& padding, const bool& hexFormat);
std::vector<unsigned> generate_random_data(unsigned amount);

// from MDAL
std::string trimChars(const std::string& inputString, const char* chars);
int getType(const std::string& parameter);
std::string getArgument(const std::string& token, const std::vector<std::string>& moduleLines);


#endif  // BINTRACKER_HELPERS__H__
