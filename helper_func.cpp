#include <string>
#include <cstring>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <random>

#include "helper_func.h"

using namespace std;

//TODO: should use $ instead of #
bool isNumber(const string &str) {

	if (str.size() == 0) return false;
	if ((str.find_first_not_of("0123456789") == string::npos) ||
		(((str.substr(0,1) == "#") || (str.substr(0,1) == "$"))
		&& ((str.substr(1, string::npos)).find_first_not_of("0123456789abcdefABCDEF") == string::npos)
		&& str.substr(1, string::npos).size())) return true;

	return false;
}

//TODO: use compare instead of find
long strToNum(string str) {

	if (str.find("$") != string::npos) {

		str.erase(0, 1);
		return stol(str, nullptr, 16);
	}
	else return stol(str, nullptr, 10);
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
int getType(const string& param) {

	//cout << "getType: " << param << endl;	//DEBUG ok

	if (param == "true" || param == "false") return BOOL;
	if (param.find('"') != string::npos) return STRING;
	if (param.find_first_of('$') != string::npos) {

		if (param.find_first_of('$') != 0) return INVALID;
		string temp = trimChars(param, "$");
		if (temp.find_first_not_of("0123456789abcdefABCDEF") != string::npos) return INVALID;
		else return HEX;
	}
	if (param.find_first_not_of("0123456789") != string::npos) return INVALID;

	return DEC;
}

string getArgument(string token, const vector<string> &moduleLines) {

	string tempstr = "";

	for (auto&& it: moduleLines) {

		size_t pos = it.find(token.data());
		if (pos != string::npos) tempstr = trimChars(it.substr(pos + token.size()), " =");
	}

	if (tempstr == "") throw ("No " + token + " statement found.");

	return tempstr;
}
