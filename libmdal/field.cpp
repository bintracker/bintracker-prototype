#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

#include "mdal.h"

using namespace std;


mdField::mdField() {

	isWord = false;
	isRequiredNow = false;

	requiredSeqBegin = false;
	requiredBlkBegin = false;
	requiredAlways = false;
	requiredBy = nullptr;
	requiredWhenSet = nullptr;

	requiredByAny = true;

	setIfCount = 0;
	setIfAlways = nullptr;
	setIfBy = nullptr;
	setIfWhenSet = nullptr;
	setIfByAny = nullptr;
	setIfMask = nullptr;
	setIfClear = nullptr;

	setBitsCount = 0;
	setBitsBy = nullptr;
	setBitsMask = nullptr;
	setBitsClear = nullptr;

	setBy = -1;
	setHiBy = -1;
	setLoBy = -1;

	useCmd = nullptr;
}



mdField::~mdField() {

	for (int i = 0; i < setIfCount; i++) {

		delete[] setIfBy[i];
		delete[] setIfWhenSet[i];
	}

	delete[] useCmd;

	delete[] setBitsBy;
	delete[] setBitsMask;
	delete[] setBitsClear;

	delete[] requiredBy;
	delete[] requiredWhenSet;
	delete[] setIfBy;
	delete[] setIfWhenSet;
	delete[] setIfByAny;
	delete[] setIfMask;
	delete[] setIfClear;
	delete[] setIfAlways;
}


void mdField::getRequests(bool *requestList, const mdConfig &config, const int &row, bool seqBegin) {

	isRequiredNow = false;

	if ((requiredSeqBegin && seqBegin && row == 0) || (requiredBlkBegin && row == 0) || requiredAlways
 		|| checkCondition(requiredBy, requiredWhenSet, requiredByAny, config)) isRequiredNow = true;
	if (setBy != -1 && (!config.mdCmdList[setBy].mdCmdCurrentValString.empty())) isRequiredNow = true;
	if (setHiBy != -1 && (!config.mdCmdList[setHiBy].mdCmdCurrentValString.empty())) isRequiredNow = true;
	if (setLoBy != -1 && (!config.mdCmdList[setLoBy].mdCmdCurrentValString.empty())) isRequiredNow = true;

 	for (int i = 0; i < config.mdCmdCount; i++) {

		if (setBitsCount && setBitsBy[i] && !config.mdCmdList[i].mdCmdCurrentValString.empty()) {

			isRequiredNow = true;
			requestList[i] = true;
 		}
 	}

	if (!isRequiredNow) return;

	if (setBy != -1) requestList[setBy] = true;
	if (setHiBy != -1) requestList[setHiBy] = true;
	if (setLoBy != -1) requestList[setLoBy] = true;

}


string mdField::getFieldString(bool *requestList, const mdConfig &config) {

	if (!isRequiredNow) return string("");

	stringstream fstr;
	string argstr1;
	long arg2 = 0;
//	bool hiWasSet = false;

	if (setBy != -1) {

		argstr1 = config.mdCmdList[setBy].getValueString();

		if (isNumber(argstr1)) {
			arg2 = strToNum(argstr1);
			argstr1 = "";
		}

		if (config.mdCmdList[setBy].isBlkReference && !argstr1.empty()) {

			string prefix;

			for (auto&& it: config.blockTypes) {

				if (config.mdCmdList[setBy].referenceBlkID == it.blockConfigID) {

					prefix = it.blkLabelPrefix;
					break;
				}
			}

			argstr1 = prefix + argstr1;
		}
	}

	if (setHiBy != -1) {

		//TODO check type and use arg2 whenever possible
		string arg = config.mdCmdList[setHiBy].getValueString();
		if (isNumber(arg)) arg2 = strToNum(arg) * 256;
		else argstr1 = arg + "*256";
//		hiWasSet = true;
	}

	if (setLoBy != -1) {

		//TODO check type and use arg2 whenever possible
		string arg = config.mdCmdList[setLoBy].getValueString();

		if (isNumber(arg)) {

			arg2 |= strToNum(arg);
		}
		else {
			if (!argstr1.empty()) argstr1 += "+";
			argstr1 += arg;
		}
	}


	bool clearedBySetBits = false;
	bool clearedHiBySetBits = false;
	bool clearedLoBySetBits = false;

	for (int i = 0; i < config.mdCmdCount && setBitsCount; i++) {

		if (setBitsBy[i]) {

			//TODO check type and use arg2 whenever possible
			if (config.mdCmdList[i].getValueString() == "true") {

				if (setBitsClear[i] == CLEAR_ALL) {

					argstr1 = "";
					arg2 = 0;
					clearedBySetBits = true;
				}
				else if (setBitsClear[i] == CLEAR_HI) {

					arg2 &= 0xff;
					clearedHiBySetBits = true;
				}
				else if (setBitsClear[i] == CLEAR_LO) {

					arg2 &= 0xff00;
					clearedLoBySetBits = true;
				}

				arg2 |= setBitsMask[i];
			}
		}
	}


	//TODO: still requires a solution for mixed string/int results

	for (int i = 0; i < setIfCount && !clearedBySetBits; i++) {

		//TODO: this needs a new function because we should check against requestList!!!
		//TODO check type and use arg2 whenever possible
		if (checkSetifCondition(setIfBy[i], setIfWhenSet[i], setIfByAny[i], config, requestList) || setIfAlways[i]) {

			if (setIfClear[i] == CLEAR_ALL) {
				arg2 = 0;
				argstr1 = "";
			}
			else if (setIfClear[i] == CLEAR_HI) arg2 &= 0xff;
			else if (setIfClear[i] == CLEAR_LO) arg2 &= 0xff00;

			if (clearedLoBySetBits) arg2 |= (setIfMask[i] & 0xff00);
			else if (clearedHiBySetBits) arg2 |= (setIfMask[i] & 0xff);
			else arg2 |= setIfMask[i];
		}
	}


	if (arg2 && !argstr1.empty()) {

		if (isNumber(argstr1)) {
			arg2 |= strToNum(argstr1);
			argstr1 = "";
		}
		else argstr1 = "(" + argstr1 + ")|";
	}
	fstr << argstr1;
	if (arg2) fstr << hex << config.hexPrefix << arg2;

	return (fstr.str().empty()) ? string("0") : fstr.str();
}



bool mdField::checkSetifCondition(const bool *by, const bool *whenSet, bool &byAny, const mdConfig &config,
    const bool *requestList) {

	if (byAny) {
		for (int cmd = 0; cmd < config.mdCmdCount; cmd++) {
			if (by[cmd] && requestList[cmd] == whenSet[cmd]) return true;
		}
		return false;
	}

    for (int cmd = 0; cmd < config.mdCmdCount; cmd++) {
        if (by[cmd] && requestList[cmd] != whenSet[cmd]) return false;
    }

	return true;
}


bool mdField::checkCondition(const bool *by, const bool *whenSet, bool &byAny, const mdConfig &config) {

	if (byAny) {
		for (int cmd = 0; cmd < config.mdCmdCount; cmd++) {
			if (by[cmd] && config.mdCmdList[cmd].mdCmdIsSetNow == whenSet[cmd]) return true;
		}
		return false;
	}

    for (int cmd = 0; cmd < config.mdCmdCount; cmd++) {
        if (by[cmd] && config.mdCmdList[cmd].mdCmdIsSetNow != whenSet[cmd]) return false;
    }

	return true;
}
