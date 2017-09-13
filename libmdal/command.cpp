#include <iostream>
#include <string>
#include <algorithm>

#include "mdal.h"

using namespace std;

mdCommand::mdCommand(): description(""), referenceBlkID(""), mdCmdDefaultValString(""), mdCmdAutoValString(""),
    mdCmdCurrentValString(""), mdCmdLastValString("") {

    mdCmdType = MD_BOOL;
    mdCmdGlobalConst = false;

    useNoteNames = false;
    allowModifiers = false;

    mdCmdForceSubstitution = false;
    mdCmdForceString = false;
    mdCmdForceInt = false;
    mdCmdForceRepeat = false;
    mdCmdUseLastSet = false;
    mdCmdAuto = false;

    isBlkReference = false;
    defaultSubstitute = nullptr;

    mdCmdIsSetNow = false;

    limitRange = false;
    lowerRangeLimit = 0;
    upperRangeLimit = 0;
}

mdCommand::~mdCommand() {}


void mdCommand::resetToDefault() {		//TODO: keep an eye on this to see if it really is useful

    if (!mdCmdGlobalConst) {

        mdCmdIsSetNow = false;
        mdCmdCurrentValString = "";
        mdCmdLastValString = getDefaultValString();

        if (mdCmdForceRepeat) {

            mdCmdIsSetNow = true;
            mdCmdCurrentValString = mdCmdLastValString;
        }
    }
}


void mdCommand::reset() {

    if (!mdCmdGlobalConst) {

        if ((mdCmdUseLastSet || mdCmdForceRepeat) && mdCmdIsSetNow) mdCmdLastValString = mdCmdCurrentValString;

        if (!mdCmdForceRepeat) {

            mdCmdIsSetNow = false;
            mdCmdCurrentValString = "";
        } else {

            mdCmdIsSetNow = true;
            if (mdCmdLastValString == "") mdCmdLastValString = getDefaultValString();
            mdCmdCurrentValString = mdCmdLastValString;
        }
    }
}


void mdCommand::set(const string &currentValString) {

    //TODO: if string is number, get value and check against BYTE/WORD range
    if (mdCmdGlobalConst) throw (string("Global constant redefined in block "));

    mdCmdIsSetNow = true;

    if (mdCmdAuto) {
        mdCmdCurrentValString = mdCmdAutoValString;
        return;
    }

    int paramType = getType(currentValString);

    if (mdCmdForceInt && (paramType != MD_DEC && paramType != MD_HEX))
        throw (string("String argument supplied for integer command "));
    //TODO: does not check for bool
    if (mdCmdForceString && isNumber(currentValString)) throw (string("Integer argument supplied for string command "));

    if (limitRange && isNumber(currentValString)) {

        if (strToNum(currentValString) < lowerRangeLimit || strToNum(currentValString) > upperRangeLimit)
            throw (string("Argument out of range for command "));
    }

    mdCmdCurrentValString = currentValString;

    if (mdCmdForceSubstitution) {

        if (substitutionList.count(currentValString)) mdCmdCurrentValString = substitutionList[currentValString];
        //TODO paramType == MD_STRING is unreliable, will pass bool but fail non-quote-enclosed strings
        else if (paramType == MD_STRING) throw ("\"" + currentValString + "\" is not a valid parameter for command ");
    }

    mdCmdLastValString = mdCmdCurrentValString;
}


string mdCommand::getValueString() {

    if (mdCmdIsSetNow) return mdCmdCurrentValString;
    if (mdCmdUseLastSet && mdCmdLastValString != "") return mdCmdLastValString;
    return getDefaultValString();
}


string mdCommand::getDefaultValString() {

    return (defaultSubstitute == nullptr) ? mdCmdDefaultValString : defaultSubstitute->mdCmdDefaultValString;
}


void mdCommand::setDefault(const string &param) {

    //TODO is substitution on this completed?
    if (param.find("SUBSTITUTE_FROM") != string::npos) return;

    int paramType = getType(param);

    //if (paramType == MD_INVALID) throw ("\"" + param + "\" is not a valid argument ");
    if (paramType == MD_BOOL && mdCmdType != MD_BOOL) throw (string("Non-Boolean default parameter specified for BOOL command"));

    mdCmdDefaultValString = trimChars(param, "\"");

    int paramVal = -1;

    if (paramType == MD_DEC) paramVal = static_cast<int>(stoul(trimChars(param, " "), nullptr, 10));
    else if (paramType == MD_HEX) paramVal = static_cast<int>(stoul(trimChars(param, " $"), nullptr, 16));


    if ((mdCmdType == MD_WORD && paramVal > 0xffff) || (mdCmdType == MD_BYTE && paramVal > 0xff))
        throw (string("Default value out of range "));
}
