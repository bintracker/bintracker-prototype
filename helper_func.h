#ifndef BINTRACKER_HELPERS__H__
#define BINTRACKER_HELPERS__H__

#include <vector>
#include <string>

using namespace std;

enum ParameterType {BOOL, BYTE, WORD, DEC, HEX, STRING, INVALID};

bool isNumber(const string &str);
long strToNum(string str);
string numToStr(const long &num, const int &padding, const bool &hexFormat);

//from MDAL
string trimChars(const string& inputString, const char* chars);
int getType(const string& parameter);
string getArgument(string token, const vector<string> &moduleLines);


#endif
