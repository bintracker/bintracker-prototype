#ifndef BINTRACKER_HELPERS__H__
#define BINTRACKER_HELPERS__H__

#include <vector>
#include <string>

enum ParameterType {BOOL, BYTE, WORD, DEC, HEX, STRING, INVALID};

bool isNumber(const std::string &str);
long strToNum(std::string str);
std::string numToStr(const long &num, const int &padding, const bool &hexFormat);
std::vector<unsigned> generate_random_data(unsigned amount);

//from MDAL
std::string trimChars(const std::string& inputString, const char* chars);
int getType(const std::string& parameter);
std::string getArgument(std::string token, const std::vector<std::string> &moduleLines);


#endif
