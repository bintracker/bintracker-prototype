#include <string>
#include <cstring>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <random>

#include "helper_func.h"

using namespace std;

bool isNumber(const string &str) {

	if (str.empty()) return false;
	return (str.find_first_not_of("0123456789") == string::npos) ||
		(((str.substr(0,1) == "#") || (str.substr(0,1) == "$"))
		&& ((str.substr(1, string::npos)).find_first_not_of("0123456789abcdefABCDEF") == string::npos)
		&& !str.substr(1, string::npos).empty());
}

//TODO: use compare instead of find
long strToNum(string str) {

	if (str.find('$') != string::npos) {

		str.erase(0, 1);
		return stol(str, nullptr, 16);
	}
	return stol(str, nullptr, 10);
}


string numToStr(const long &num, const int &padding, const bool &hexFormat) {

	string restr;
	ostringstream convert;

	if (hexFormat) convert << hex << setfill('0') << setw(padding) << num;
	else convert << dec << setfill('0') << setw(padding) << num;

	return convert.str();
}

//generate random data via el cheapo xorshift* implementation
vector<unsigned> generate_random_data(unsigned amount) {

    vector<unsigned> randomData;

    unsigned long long state = random_device()();
    unsigned long long x;

    for (unsigned i = 0; i < amount; i++) {

        x = state;
        x ^= x >> 12;
        x ^= x << 25;
        x ^= x >> 27;
        state = x;
        x *= 0x2545F4914F6CDD1D;
        randomData.push_back(static_cast<unsigned>(x >> 32));
    }

    return randomData;
}


//MDAL helper funcs
string trimChars(const string& inputString, const char* chars) {

	string str = inputString;

	for (unsigned i = 0; i < strlen(chars); ++i) str.erase(remove(str.begin(), str.end(), chars[i]), str.end());

	return str;
}

//TODO: flag strings that start with or contain only numbers as invalid
int getType(const string& parameter) {

	//cout << "getType: " << param << endl;	//DEBUG ok

	if (parameter == "true" || parameter == "false") return MD_BOOL;
	if (parameter.find('"') != string::npos) return MD_STRING;
	if (parameter.find_first_of('$') != string::npos) {

		if (parameter.find_first_of('$') != 0) return MD_INVALID;
		string temp = trimChars(parameter, "$");
		if (temp.find_first_not_of("0123456789abcdefABCDEF") != string::npos) return MD_INVALID;
		return MD_HEX;
	}
	if (parameter.find_first_not_of("0123456789") != string::npos) return MD_INVALID;

	return MD_DEC;
}

string getArgument(const string& token, const vector<string> &moduleLines) {

	string tempstr;

	for (auto&& it: moduleLines) {

		size_t pos = it.find(token);
		if (pos != string::npos) tempstr = trimChars(it.substr(pos + token.size()), " =");
	}

	if (tempstr.empty()) throw ("No " + token + " statement found.");

	return tempstr;
}
