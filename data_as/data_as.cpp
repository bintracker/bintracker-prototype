#include <fstream>
#include <iostream>
//#include <sstream>
#include <algorithm>
#include <vector>
//#include <string>

#include "../helper_func.h"
#include "mdatas.h"

//TODO: add a binsize limit check, abort assembly if max binsize exceeded
//TODO: clear equates when engine is changed
unordered_map<string, long> Data_Assembly::equates;


Data_Assembly::Data_Assembly() {

	binData = nullptr;
	binLength = 0;
}


void Data_Assembly::init(const string &eqfile) {

	ifstream EQFILE(eqfile.data());
	if (!EQFILE.is_open()) throw ("Could not open " + eqfile);

	string tempstr;

	equates.clear();
	while (getline(EQFILE, tempstr)) {

		tempstr.erase(remove_if(tempstr.begin(), tempstr.end(), [](char x){return std::isspace(x);}), tempstr.end());

		if (tempstr.find("equ") != string::npos) {

//			equates[tempstr.substr(0, tempstr.find("equ"))] = strToNum(tempstr.erase(0, tempstr.find("equ") + 3));
			//WORKAROUND because clang doesn't generate correct code for the short version
			string key = tempstr.substr(0, tempstr.find("equ"));
			equates[key] = strToNum(tempstr.erase(0, tempstr.find("equ") + 3));
		}
	}

	symbols.clear();
	symbols.insert(equates.begin(), equates.end());
}

void Data_Assembly::reset() {

	delete[] binData;
	binData = nullptr;
	binLength = 0;
}


Data_Assembly::~Data_Assembly() {

	delete[] binData;
}

//TODO: need proper full expression evaluation on the long run.
long Data_Assembly::evalExpr(const string &expr) {

	string op = "";
	string arg1 = trimChars(expr.substr(0, expr.find_first_of("+-/*%|&^")), "()");
	string arg2 = "";
	long argv1 = 0;
	long argv2 = 0;

	if (expr.find_first_of("+-/*%|&^") != string::npos) {

		op = expr.substr(expr.find_first_of("+-/*%|&^"), 1);
		arg2 = expr.substr(expr.find_first_of("+-/*%|&^") + 1, string::npos);
	}

	if (((arg1.find('$') != string::npos) && (arg1.find_first_not_of("$0123456789abcdef") == string::npos))
		|| ((arg1.find('$') == string::npos) && (arg1.find_first_not_of("0123456789") == string::npos))) argv1 = strToNum(arg1);
	else argv1 = symbols[arg1];

	if (arg2 != "") {
		if (((arg2.find('$') != string::npos) && (arg2.find_first_not_of("$0123456789abcdef") == string::npos))
			|| ((arg2.find('$') == string::npos) && (arg2.find_first_not_of("0123456789") == string::npos))) argv2 = strToNum(arg2);
		else argv2 = symbols[arg2];
	}

	if (op != "") return solve(argv1, argv2, op);
	else return argv1;
}


long Data_Assembly::solve(const long &argv1, const long &argv2, const string &op) {

	if (op == "+") return argv1 + argv2;
	else if (op == "-") return argv1 - argv2;
	else if (op == "|") return argv1 | argv2;
	else if (op == "*") return argv1 * argv2;
	else if (op == "/") return (argv2 == 0) ? argv1 : static_cast<long>(argv1 / argv2);
	else if (op == "&") return argv1 & argv2;
	else if (op == "^") return argv1 ^ argv2;
	return -1;
}


void Data_Assembly::assemble(const vector<string> &asmLines_, const long &origin, const bool &littleEndian) {

	reset();
	vector<string> asmLines = asmLines_;		//TODO: inefficient, try to not copy the vector

	symbols.clear();
	symbols.insert(equates.begin(), equates.end());

	long binfp = origin;

	//resolve symbols
	for (auto&& it: asmLines) {

		it.erase(remove_if(it.begin(), it.end(), [](char x){return std::isspace(x);}), it.end());

		if (it.find(";") != string::npos) it.erase(it.find(";"), string::npos);

		if (it != "") {

			string tempstr = it;

			if (tempstr.compare(0, 2, "db") == 0) binfp += (count(tempstr.begin(), tempstr.end(), ',') + 1);
			else if (tempstr.compare(0, 2, "dw") == 0) binfp += ((count(tempstr.begin(), tempstr.end(), ',') * 2) + 2);
			else if (tempstr.compare(0, 2, "dl") == 0) binfp += ((count(tempstr.begin(), tempstr.end(), ',') * 4) + 4);
			else if (tempstr.compare(0, 2, "ds") == 0) {

				tempstr.erase(0, 2);	//erase ds
				binfp += strToNum(tempstr.erase(tempstr.find(','), string::npos));
			}
			else if (tempstr.find("equ") != string::npos) {

				it = "";	//delete line so it's not evaluated again
				symbols[tempstr.substr(0, tempstr.find("equ"))] = strToNum(tempstr.erase(0, tempstr.find("equ") + 3));
			}
			else symbols[tempstr] = binfp;
		}
	}

//	for (auto& it: symbols) cout << hex << it.first << ": " << it.second << endl;

	binLength = binfp - origin;
	binData = new char[binLength];
	binfp = 0;

	//assemble
	for (auto&& it: asmLines) {

		if (it != "") {

			int argcount = count(it.begin(), it.end(), ',') + 1;

			if (it.compare(0, 2, "db") == 0) {

				it.erase(0,2);
				for (int j = 0; j < argcount; j++) {

					string tempexpr = it.substr(0, it.find(','));
					if ((j + 1) < argcount) it.erase(0, tempexpr.size() + 1);
					long val = evalExpr(tempexpr);
					binData[binfp] = static_cast<char>(val);
					binfp++;
				}
			}
			else if (it.compare(0, 2, "dw") == 0) {

				it.erase(0,2);
				for (int j = 0; j < argcount; j++) {

					string tempexpr = it.substr(0, it.find(','));
					if ((j + 1) < argcount) it.erase(0, tempexpr.size() + 1);
					long val = evalExpr(tempexpr);
					if (littleEndian) {
						binData[binfp] = static_cast<char>(val);
						binfp++;
						binData[binfp] = static_cast<char>(val >> 8);
					}
					else {
						binData[binfp] = static_cast<char>(val >> 8);
						binfp++;
						binData[binfp] = static_cast<char>(val);
					}
					binfp++;
				}
			}
			else if (it.compare(0, 2, "dl") == 0) {

				it.erase(0,2);
				for (int j = 0; j < argcount; j++) {

					string tempexpr = it.substr(0, it.find(','));
					if ((j + 1) < argcount) it.erase(0, tempexpr.size() + 1);
					long val = evalExpr(tempexpr);
					if (littleEndian) {
						binData[binfp] = static_cast<char>(val);
						binfp++;
						binData[binfp] = static_cast<char>(val >> 8);
						binfp++;
						binData[binfp] = static_cast<char>(val >> 16);
						binfp++;
						binData[binfp] = static_cast<char>(val >> 24);
					}
					else {
						binData[binfp] = static_cast<char>(val >> 24);
						binfp++;
						binData[binfp] = static_cast<char>(val >> 16);
						binfp++;
						binData[binfp] = static_cast<char>(val >> 8);
						binfp++;
						binData[binfp] = static_cast<char>(val);
					}
					binfp++;
				}
			}
			else if (it.compare(0, 2, "ds") == 0) {

				it.erase(0, 2);	//erase ds
				long length = strToNum(it.substr(0, it.find(',')));
				long val = 0;
				if (argcount) val = strToNum(it.substr(it.find(','), string::npos));

				for (long j = 0; j < length; j++) {

					binData[binfp] = static_cast<char>(val);
					binfp++;
				}
			}
		}
	}
}
