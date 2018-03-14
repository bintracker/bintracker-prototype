#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <pugixml.hpp>
#include "mdal.h"

using namespace std;


const vector<string> mdConfig::reservedKeywords({"ANY", "ALL", "NONE", "CONFIG"});


mdConfig::mdConfig(): description(""), targetPlatform("generic"), seqLabel(";sequence") {

    cfgLines = nullptr;
    mdCmdList = nullptr;

    reset();
}

mdConfig::~mdConfig() {

    delete[] mdCmdList;
    delete[] cfgLines;
}


void mdConfig::reset() {

    description = "";
    targetPlatform = "generic";
    seqLabel = ";sequence";

    useSeqEnd = false;
    useSeqLoop = false;
    useSeqLoopPointer = false;
    useSamples = false;

    delete[] mdCmdList;
    delete[] cfgLines;
    cfgLines = nullptr;
    mdCmdList = nullptr;

    blockTypes.clear();
    trackSources.clear();

    seqMaxLength = 0;
    blockTypeCount = 0;
    linecount = 0;
    mdCmdCount = 0;
}

void mdConfig::init(const string &configname, bool &verbose) {

    reset();

    pugi::xml_document xml;
    pugi::xml_parse_result result = xml.load_file(configname.data(), pugi::parse_trim_pcdata|pugi::parse_wnorm_attribute);

    if (!result) {
        stringstream errormsg;
        errormsg << "XML parsing failed: " << result.description() << " @" << result.offset << ".";
        throw (errormsg.str());
    }

    try {

        if (verbose) cout << "configuration:\t\t" << configname << endl;

        pugi::xml_node mdalconfig = xml.child("mdalconfig");
        if (!mdalconfig) throw (string("Not a valid MDAL configuration."));

        string tempstr = mdalconfig.attribute("version").value();
        if (tempstr.empty()) throw (string("MDAL_VERSION not specified."));
        if (getType(tempstr) != MD_DEC) throw (string("MDAL_VERSION argument is not a decimal number."));
        if (stoi(tempstr, nullptr, 10) != MDALVERSION) throw ("Unsupported MDAL version " + tempstr + ".");
        if (verbose) cout << "MDAL version: \t\t" << stoi(tempstr, nullptr, 10) << endl;

        pugi::xml_node tempnode = mdalconfig.child("description");
        tempstr = tempnode.child_value();
        if (!tempstr.empty()) description = tempstr;
        if (verbose) cout << description << endl;

        tempnode = mdalconfig.child("global");
        tempstr = tempnode.attribute("target").value();
        if (!tempstr.empty()) targetPlatform = tempstr;
        if (verbose) cout << "target platform:\t" << targetPlatform << endl;

        tempstr = tempnode.attribute("word_directive").value();
        wordDirective = (tempstr.empty()) ? "dw" : tempstr;
        if (verbose) cout << "word directive:\t\t" << wordDirective << endl;

        tempstr = tempnode.attribute("byte_directive").value();
        byteDirective = (tempstr.empty()) ? "db" : tempstr;
        if (verbose) cout << "byte directive:\t\t" << byteDirective << endl;

        tempstr = tempnode.attribute("hex_prefix").value();
        hexPrefix = (tempstr.empty()) ? "$" : tempstr;
        if (verbose) cout << "hex prefix:\t\t" << hexPrefix << endl;

        if (verbose) cout << "\nSEQUENCE CONFIGURATION\n======================" << endl;

        tempnode = mdalconfig.child("sequence");
        if (!tempnode) throw (string("Missing sequence configuration"));

        tempstr = tempnode.attribute("label").value();
        if (!tempstr.empty()) seqLabel = tempstr;
        if (verbose) cout << "Sequence label:\t\t" << seqLabel << endl;

        tempstr = tempnode.attribute("end").value();
        if (!tempstr.empty()) {
            useSeqEnd = true;
            seqEndString = tempstr;
            if (verbose) cout << "Sequence end:\t\t" << seqEndString << endl;
        }

        tempstr = tempnode.attribute("max_length").value();
        if (!tempstr.empty()) {

            if (!isNumber(tempstr)) throw (string("<sequence>: max_length does not specify an integer value."));
            seqMaxLength = static_cast<unsigned>(strToNum(tempstr));
            if (verbose) cout << "Max. sequence length:\t" << seqMaxLength << endl;
        }

        for (tempnode = mdalconfig.child("sequence").child("track"); tempnode; tempnode = tempnode.next_sibling("track"))
            trackSources.emplace_back(string(tempnode.attribute("from").value()));
        if (trackSources.empty()) throw (string("<sequence>: no <track>s specified."));

        tempnode = mdalconfig.child("sequence").child("loop");
        if (tempnode) {
            tempstr = tempnode.attribute("type").value();
            if (tempstr.empty()) throw (string("<sequence><loop>: No loop type specified."));
            if (tempstr != "label" && tempstr != "pointer") throw ("<sequence><loop>: Invalid type \"" + tempstr + "\".");
            useSeqLoop = true;
            if (tempstr == "pointer") useSeqLoopPointer = true;
            if (verbose) cout << "Loop type:\t\t" << tempstr << endl;
            tempstr = tempnode.attribute("label").value();
            if (tempstr.empty()) throw (string("<sequence><loop>: No loop label specified."));
            seqLoopLabel = tempstr;
            if (verbose) cout << "Loop label:\t\t" << tempstr << endl;
        }


        pugi::xml_node commands = mdalconfig.child("commands");
        if (!commands) throw (string("No <commands> block found."));

        if (verbose) cout << endl << "USER COMMANDS\n=============" << endl;

        for (tempnode = commands.child("command"); tempnode; tempnode = tempnode.next_sibling("command")) mdCmdCount++;
        if (!mdCmdCount) throw (string("<commands>: No commands specified."));

        mdCmdList = new mdCommand[mdCmdCount];
        int cmdNr = 0;
        for (tempnode = commands.child("command"); tempnode; tempnode = tempnode.next_sibling("command")) {

            tempstr = tempnode.attribute("id").value();
            if (tempstr.empty()) throw (string("<command>: missing command id."));
            for (auto&& it: reservedKeywords) {
                if (tempstr == it) throw ("<command>: Reserved keyword \"" + tempstr + "\" used as command name.");
            }
            mdCmdList[cmdNr].mdCmdName = tempstr;

            tempstr = tempnode.attribute("size").value();
            if (tempstr.empty()) throw (string("<command>: size not specified."));
            if (tempstr == "bool") mdCmdList[cmdNr].mdCmdType = MD_BOOL;
            else if (tempstr == "byte") mdCmdList[cmdNr].mdCmdType = MD_BYTE;
            else if (tempstr == "word") mdCmdList[cmdNr].mdCmdType = MD_WORD;
            else throw ("<command>: unknown command type \"" + tempstr + "\".");

            tempstr = tempnode.attribute("default").value();
            if (tempstr.empty()) tempstr = "0";
            mdCmdList[cmdNr].setDefault(tempstr);

            //TODO could be attr of main command declaration
            pugi::xml_node param = tempnode.child("auto");
            if (param) {
                tempstr = param.attribute("value").value();
                if (tempstr.empty()) throw (string("<command>: missing auto value specification."));
                if (mdCmdList[cmdNr].mdCmdType == MD_BOOL && getType(tempstr) != MD_BOOL)
                    throw (string("<command>: non-boolean value specified as auto value for bool command."));
                mdCmdList[cmdNr].mdCmdAutoValString = tempstr;
                mdCmdList[cmdNr].mdCmdAuto = true;
            }

            param = tempnode.child("description");
            if (param) mdCmdList[cmdNr].description = param.child_value();
            param = tempnode.child("use_note_names");
            if (param) mdCmdList[cmdNr].useNoteNames = true;
            param = tempnode.child("allow_modifiers");
            if (param) mdCmdList[cmdNr].allowModifiers = true;
            param = tempnode.child("force_string");
            if (param) mdCmdList[cmdNr].mdCmdForceString = true;
            param = tempnode.child("force_int");
            if (param) mdCmdList[cmdNr].mdCmdForceInt = true;
            param = tempnode.child("force_repeat");
            if (param) mdCmdList[cmdNr].mdCmdForceRepeat = true;
            param = tempnode.child("use_last_set");
            if (param) mdCmdList[cmdNr].mdCmdUseLastSet = true;
            param = tempnode.child("global_const");
            if (param) mdCmdList[cmdNr].mdCmdGlobalConst = true;

            if (mdCmdList[cmdNr].mdCmdForceString && mdCmdList[cmdNr].mdCmdForceInt)
                throw (string("<command>: force_int and force_string are mutually exclusive."));

            param = tempnode.child("reference");
            if (param) {
                tempstr = param.attribute("to").value();
                if (tempstr.empty()) throw (string("<command>: reference used, but no block id given."));
                mdCmdList[cmdNr].isBlkReference = true;
                mdCmdList[cmdNr].referenceBlkID = tempstr;
            }

            param = tempnode.child("range");
            if (param && mdCmdList[cmdNr].mdCmdType != MD_BOOL) {
                mdCmdList[cmdNr].limitRange = true;
                tempstr = param.attribute("lower_limit").value();
                if (tempstr.empty()) throw (string("<command>: range used, but no lower limit specified."));
                if (!isNumber(tempstr)) throw (string("<command>: lower limit of range is not an integer."));
                mdCmdList[cmdNr].lowerRangeLimit = strToNum(tempstr);
                tempstr = param.attribute("upper_limit").value();
                if (tempstr.empty()) throw (string("<command>: range used, but no upper limit specified."));
                if (!isNumber(tempstr)) throw (string("<command>: upper limit of range is not an integer."));
                mdCmdList[cmdNr].upperRangeLimit = strToNum(tempstr);
            }

            param = tempnode.child("substitute");
            if (param) {
                mdCmdList[cmdNr].mdCmdForceSubstitution = true;

                for (; param; param = param.next_sibling("substitute")) {

                    string key = param.attribute("key").value();
                    if (key.empty()) throw (string("<command>: missing substitution key."));
                    tempstr = param.attribute("value").value();
                    if (tempstr.empty()) throw (string("<command>: missing substitution value."));
                    mdCmdList[cmdNr].substitutionList.insert(make_pair(key, tempstr));
                }
            }

            param = tempnode.child("default_substitute");
            if (param) {
                tempstr = param.attribute("from").value();
                if (tempstr.empty()) throw (string("<command>: default substitute used, but no source given."));

                for (int j = 0; j < mdCmdCount; j++) {
                    if (tempstr == mdCmdList[j].mdCmdName) {
                        mdCmdList[cmdNr].defaultSubstitute = &mdCmdList[j];
                        break;
                    }
                }

                if (mdCmdList[cmdNr].defaultSubstitute == nullptr)
                    throw ("<command>: default substitution command \"" + tempstr + "\" is undefined.");
            }

            if (verbose) {
                cout << mdCmdList[cmdNr].mdCmdName << ": " << mdCmdList[cmdNr].description << "\n";
                cout << "Type is ";

                if (mdCmdList[cmdNr].mdCmdType == MD_BYTE) cout << "BYTE";
                else if (mdCmdList[cmdNr].mdCmdType == MD_BOOL) cout << "BOOL";
                else cout << "WORD";

                cout << ", default is ";
                if (mdCmdList[cmdNr].defaultSubstitute == nullptr) cout << mdCmdList[cmdNr].mdCmdDefaultValString;
                else cout << "substituted by " << mdCmdList[cmdNr].defaultSubstitute->mdCmdName;
                if (mdCmdList[cmdNr].useNoteNames) cout << ", USE_NOTE_NAMES";
                if (mdCmdList[cmdNr].allowModifiers) cout << ", ALLOW_MODIFIERS";
                if (mdCmdList[cmdNr].mdCmdForceString) cout << ", FORCE_STRING";
                else if (mdCmdList[cmdNr].mdCmdForceInt) cout << ", FORCE_INT";
                if (mdCmdList[cmdNr].mdCmdForceRepeat) cout << ", FORCE_REPEAT";
                if (mdCmdList[cmdNr].mdCmdUseLastSet) cout << ", USE_LAST_SET";
                if (mdCmdList[cmdNr].mdCmdGlobalConst) cout << ", GLOBAL_CONST";
                if (mdCmdList[cmdNr].mdCmdAuto) cout << ", AUTO";
                if (mdCmdList[cmdNr].limitRange) cout << ", RANGE: " << mdCmdList[cmdNr].lowerRangeLimit << ".."
                                                          << mdCmdList[cmdNr].upperRangeLimit;
                if (mdCmdList[cmdNr].isBlkReference) cout << ", REFERENCE to " << mdCmdList[cmdNr].referenceBlkID;
                if (mdCmdList[cmdNr].mdCmdForceSubstitution) {

                    cout << ", FORCE_SUBSTITUTION: " << endl;
                    for (auto&& it: mdCmdList[cmdNr].substitutionList) cout << "\t" << it.first << " ==> " << it.second << endl;
                } else cout << endl;
            }

            cmdNr++;
        }

        if (verbose) cout << endl;

        //TODO merge counting into main parsing loop
        for (tempnode = mdalconfig.child("blocktype"); tempnode; tempnode = tempnode.next_sibling("blocktype")) blockTypeCount++;
        if (!blockTypeCount) throw (string("No blocktype configurations found."));
        if (verbose) cout << "BLOCKTYPE CONFIGURATIONS\n========================\nBlock types: \t\t" << blockTypeCount << endl;
        blockTypes.reserve(blockTypeCount);

        for (tempnode = mdalconfig.child("blocktype"); tempnode; tempnode = tempnode.next_sibling("blocktype")) {

            tempstr = tempnode.attribute("id").value();
            if (tempstr.empty()) throw (string("<blocktype>: missing block id."));
            blockTypes.emplace_back(tempstr);

            tempstr = tempnode.attribute("type").value();
            if (tempstr.empty()) blockTypes.back().baseType = GENERIC;
            else {
                if (tempstr == "pattern") blockTypes.back().baseType = PATTERN;
                else if (tempstr == "table") blockTypes.back().baseType = TABLE;
                else if (tempstr == "generic") blockTypes.back().baseType = GENERIC;
                else throw ("<blocktype>: " + blockTypes.back().blockConfigID + ": Unknown block base type " + tempstr);
            }

            if (verbose) {
                cout << "\nBLOCKTYPE " << blockTypes.back().blockConfigID << "\nBase type: \t\t";
                if (blockTypes.back().baseType == GENERIC) cout << "generic";
                else if (blockTypes.back().baseType == PATTERN) cout << "pattern";
                else cout << "table";
                cout << endl;
            }

            tempstr = tempnode.attribute("end").value();
            if (!tempstr.empty()) {
                blockTypes.back().useBlkEnd = true;
                blockTypes.back().blkEndString = tempstr;
                if (verbose) cout << "Block end:\t\t" << blockTypes.back().blkEndString << endl;
            }

            tempstr = tempnode.attribute("max_length").value();
            if (!tempstr.empty()) {
                if (!isNumber(tempstr)) throw (string("<blocktype>: argument for max_length is not a number."));
                blockTypes.back().blkMaxLength = strToNum(tempstr);
                if (verbose) cout << "Max. block length:\t" << blockTypes.back().blkMaxLength << endl;
            }

            tempstr = tempnode.attribute("label_prefix").value();
            if (!tempstr.empty()) {
                if (isNumber(tempstr)) throw (string("<blocktype>: argument for label_prefix is not a string."));
                blockTypes.back().blkLabelPrefix = tempstr;
            }
            if (verbose) cout << "Pattern label prefix:\t" << blockTypes.back().blkLabelPrefix << endl;

            pugi::xml_node argnode = tempnode.child("init_defaults");
            if (argnode) {
                blockTypes.back().initBlkDefaults = true;
                if (verbose) cout << "Initialize commands with default values at each block start" << endl;
            }

            for (argnode = tempnode.child("field"); argnode; argnode = argnode.next_sibling("field")) blockTypes.back().blkFieldCount++;
            if (!blockTypes.back().blkFieldCount) throw (string("<blocktype>: no output fields specified."));
            blockTypes.back().blkFieldList = new mdField[blockTypes.back().blkFieldCount];
            int fieldNr = 0;

            for (argnode = tempnode.child("field"); argnode; argnode = argnode.next_sibling("field")) {

                pugi::xml_node param;
                tempstr = argnode.attribute("size").value();
                if (tempstr.empty()) throw (string("<blocktype><field>: no field size specified."));
                if (tempstr == "word") blockTypes.back().blkFieldList[fieldNr].isWord = true;
                else if (tempstr != "byte") throw ("<blocktype><field>: invalid field size argument \"" + tempstr + "\".");
                else {
                    param = argnode.child("set_hi");
                    if (param) throw (string("<blocktype><field>: set_hi not allowed on byte-sized fields."));
                }

                //TODO inefficient
                param = argnode.child("set_hi");
                if (!param) {
                    param = argnode.child("set_lo");
                    if (!param) {
                        param = argnode.child("set");
                        if (!param) {
                            param = argnode.child("set_if");
                            if (!param) {
                                param = argnode.child("set_bits");
                                if (!param)
                                    throw (string("<blocktype><field>: field not set by any command."));
                            }
                        }
                    }
                }

                blockTypes.back().blkFieldList[fieldNr].requiredBy = new bool[mdCmdCount];
                blockTypes.back().blkFieldList[fieldNr].requiredWhenSet = new bool[mdCmdCount];
                for (int i = 0; i < mdCmdCount; i++) {
                    blockTypes.back().blkFieldList[fieldNr].requiredBy[i] = false;
                    blockTypes.back().blkFieldList[fieldNr].requiredWhenSet[i] = true;
                }

                param = argnode.child("required_seq_begin");
                if (param) blockTypes.back().blkFieldList[fieldNr].requiredSeqBegin = true;

                param = argnode.child("required_blk_begin");
                if (param) blockTypes.back().blkFieldList[fieldNr].requiredBlkBegin = true;

                param = argnode.child("required");
                if (param) {

                    tempstr = param.attribute("if").value();
                    if (tempstr.empty()) blockTypes.back().blkFieldList[fieldNr].requiredAlways = true;
                    else {
                        if (tempstr.find_first_not_of("()ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!+|") != string::npos)
                            throw ("<blocktype><field>: Invalid argument in <required if=\"" + tempstr + "\"");
                        if (tempstr.find('+') != string::npos) {
                            blockTypes.back().blkFieldList[fieldNr].requiredByAny = false;
                            if (tempstr.find('|') != string::npos)
                                throw ("<blocktype><field>: Use of both + and | in <required if=\"" + tempstr + "\"");
                        }
                        //handle global NOT
                        if (tempstr.size() > 1 && tempstr.substr(0,2) == "!(") {

                            for (int i = 0; i < mdCmdCount; i++)
                                blockTypes.back().blkFieldList[fieldNr].requiredWhenSet[i] = false;
                            tempstr.erase(0,2);
                            tempstr = trimChars(tempstr, ")");
                        }

                        if (tempstr.find('(') != string::npos)
                            throw ("<blocktype><field>: Expression too complex in <required if=\"" + tempstr + "\"");

                        if (tempstr == "none") {

                            for (int j = 0; j < mdCmdCount; j++) {

                                blockTypes.back().blkFieldList[fieldNr].requiredWhenSet[j] = false;
                                blockTypes.back().blkFieldList[fieldNr].requiredBy[j] = true;
                            }
                        } else if (tempstr == "all") {

                            blockTypes.back().blkFieldList[fieldNr].requiredByAny = false;
                            for (int j = 0; j < mdCmdCount; j++)
                                blockTypes.back().blkFieldList[fieldNr].requiredBy[j] = true;
                        } else if (tempstr == "any") {

                            for (int j = 0; j < mdCmdCount; j++)
                                blockTypes.back().blkFieldList[fieldNr].requiredBy[j] = true;
                        } else {

                            while (!tempstr.empty()) {

                                bool setNot = false;
                                string cmdString;

                                if (tempstr.substr(0,1) == "!") {
                                    setNot = true;
                                    tempstr.erase(0,1);
                                }

                                cmdString = tempstr.substr(0, tempstr.find_first_of("+|"));

                                cmdNr = getCmdNr(cmdString);
                                if (cmdNr == -1)
                                    throw ("<blocktype><field>: Unknown command \"" + cmdString
                                           + "\" found in required expression");

                                blockTypes.back().blkFieldList[fieldNr].requiredBy[cmdNr] = true;
                                if (setNot) blockTypes.back().blkFieldList[fieldNr].requiredWhenSet[cmdNr] = false;

                                if (tempstr != cmdString) tempstr.erase(0, tempstr.find_first_of("+|") + 1);	//or npos
                                else tempstr = "";
                            }
                        }
                    }
                }

                blockTypes.back().blkFieldList[fieldNr].useCmd = new bool[mdCmdCount];
                fill_n(blockTypes.back().blkFieldList[fieldNr].useCmd, mdCmdCount, false);

                param = argnode.child("set_hi");
                if (param) {
                    tempstr = param.attribute("from").value();
                    if (tempstr.empty()) throw (string("<blocktype><field><set_hi>: missing from= argument"));
                    cmdNr = getCmdNr(tempstr);
                    if (cmdNr == -1) throw ("<blocktype><field><set_hi>: Unknown command \"" + tempstr + "\" in from= argument");
                    if (mdCmdList[cmdNr].mdCmdType != MD_BYTE)
                        throw ("<blocktype><field><set_hi>: Command \"" + tempstr + "\" is not byte-sized");
                    blockTypes.back().blkFieldList[fieldNr].requiredBy[cmdNr] = true;
                    blockTypes.back().blkFieldList[fieldNr].setHiBy = cmdNr;
                    blockTypes.back().blkFieldList[fieldNr].useCmd[cmdNr] = true;
                }

                param = argnode.child("set_lo");
                if (param) {
                    tempstr = param.attribute("from").value();
                    if (tempstr.empty()) throw (string("<blocktype><field><set_lo>: missing from= argument"));
                    cmdNr = getCmdNr(tempstr);
                    if (cmdNr == -1) throw ("<blocktype><field><set_lo>: Unknown command \"" + tempstr + "\" in from= argument");
                    if (mdCmdList[cmdNr].mdCmdType != MD_BYTE)
                        throw ("<blocktype><field><set_lo>: Command \"" + tempstr + "\" is not byte-sized");
                    blockTypes.back().blkFieldList[fieldNr].requiredBy[cmdNr] = true;
                    blockTypes.back().blkFieldList[fieldNr].setLoBy = cmdNr;
                    blockTypes.back().blkFieldList[fieldNr].useCmd[cmdNr] = true;
                }

                param = argnode.child("set");
                if (param) {
                    tempstr = param.attribute("from").value();
                    if (tempstr.empty()) throw (string("<blocktype><field><set>: missing from= argument"));
                    cmdNr = getCmdNr(tempstr);
                    if (cmdNr == -1) throw ("<blocktype><field><set>: Unknown command \"" + tempstr + "\" in from= argument");
                    if (mdCmdList[cmdNr].mdCmdType == MD_BOOL)
                        throw ("<blocktype><field><set>: Command \"" + tempstr + "\" is boolean");
                    blockTypes.back().blkFieldList[fieldNr].requiredBy[cmdNr] = true;
                    blockTypes.back().blkFieldList[fieldNr].setBy = cmdNr;
                    blockTypes.back().blkFieldList[fieldNr].useCmd[cmdNr] = true;
                }

                for (param = argnode.child("set_bits"); param; param = param.next_sibling("set_bits"))
                    blockTypes.back().blkFieldList[fieldNr].setBitsCount++;
                if (blockTypes.back().blkFieldList[fieldNr].setBitsCount) {
                    blockTypes.back().blkFieldList[fieldNr].setBitsBy = new bool[mdCmdCount];
                    fill_n(blockTypes.back().blkFieldList[fieldNr].setBitsBy, mdCmdCount, false);
                    blockTypes.back().blkFieldList[fieldNr].setBitsMask = new int[mdCmdCount];
                    fill_n(blockTypes.back().blkFieldList[fieldNr].setBitsMask, mdCmdCount, 0);
                    blockTypes.back().blkFieldList[fieldNr].setBitsClear = new int[mdCmdCount];
                    fill_n(blockTypes.back().blkFieldList[fieldNr].setBitsClear, mdCmdCount, 0);
                }

                for (param = argnode.child("set_bits"); param; param = param.next_sibling("set_bits")) {

                    string from = param.attribute("from").value();
                    if (from.empty()) throw (string("<blocktype><field><set_bits>: missing from= argument"));
                    cmdNr = getCmdNr(from);
                    if (cmdNr == -1) throw ("<blocktype><field><set_bits>: unknown source command \"" + from);
                    if (mdCmdList[cmdNr].mdCmdType != MD_BOOL)
                        throw ("<blocktype><field><set_bits>: \"" + from + "\" is not boolean command");

                    string mask = param.attribute("value").value();
                    if (mask.empty()) throw (string("<blocktype><field><set_bits>: missing value= argument"));
                    if (!isNumber(mask)) throw (string("<blocktype><field><set_bits>: value argument is not a number"));

                    blockTypes.back().blkFieldList[fieldNr].setBitsMask[cmdNr] = strToNum(mask);
                    blockTypes.back().blkFieldList[fieldNr].requiredBy[cmdNr] = true;
                    blockTypes.back().blkFieldList[fieldNr].setBitsBy[cmdNr] = true;
                    blockTypes.back().blkFieldList[fieldNr].useCmd[cmdNr] = true;

                    tempstr = param.attribute("clear").value();
                    if (!tempstr.empty()) {
                        if (tempstr == "all") blockTypes.back().blkFieldList[fieldNr].setBitsClear[cmdNr] = CLEAR_ALL;
                        else if (tempstr == "hi") blockTypes.back().blkFieldList[fieldNr].setBitsClear[cmdNr] = CLEAR_HI;
                        else if (tempstr == "lo") blockTypes.back().blkFieldList[fieldNr].setBitsClear[cmdNr] = CLEAR_LO;
                        else throw (string("<blocktype><field><set_bits>: Invalid argument for clear="));
                    }
                }

                for (param = argnode.child("set_if"); param; param = param.next_sibling("set_if"))
                    blockTypes.back().blkFieldList[fieldNr].setIfCount++;
                if (blockTypes.back().blkFieldList[fieldNr].setIfCount) {
                    blockTypes.back().blkFieldList[fieldNr].setIfBy = new bool*[blockTypes.back().blkFieldList[fieldNr].setIfCount];
                    blockTypes.back().blkFieldList[fieldNr].setIfWhenSet = new bool*[blockTypes.back().blkFieldList[fieldNr].setIfCount];
                    blockTypes.back().blkFieldList[fieldNr].setIfByAny = new bool[blockTypes.back().blkFieldList[fieldNr].setIfCount];
                    fill_n(blockTypes.back().blkFieldList[fieldNr].setIfByAny, blockTypes.back().blkFieldList[fieldNr].setIfCount, true);
                    blockTypes.back().blkFieldList[fieldNr].setIfMask = new int[blockTypes.back().blkFieldList[fieldNr].setIfCount];
                    fill_n(blockTypes.back().blkFieldList[fieldNr].setIfMask, blockTypes.back().blkFieldList[fieldNr].setIfCount, 0);
                    blockTypes.back().blkFieldList[fieldNr].setIfClear = new int[blockTypes.back().blkFieldList[fieldNr].setIfCount];
                    fill_n(blockTypes.back().blkFieldList[fieldNr].setIfClear, blockTypes.back().blkFieldList[fieldNr].setIfCount, 0);
                    blockTypes.back().blkFieldList[fieldNr].setIfAlways = new bool[blockTypes.back().blkFieldList[fieldNr].setIfCount];
                    fill_n(blockTypes.back().blkFieldList[fieldNr].setIfAlways, blockTypes.back().blkFieldList[fieldNr].setIfCount, false);
                    for (int i = 0; i < blockTypes.back().blkFieldList[fieldNr].setIfCount; i++) {
                        blockTypes.back().blkFieldList[fieldNr].setIfBy[i] = new bool[mdCmdCount];
                        fill_n(blockTypes.back().blkFieldList[fieldNr].setIfBy[i], mdCmdCount, false);
                        blockTypes.back().blkFieldList[fieldNr].setIfWhenSet[i] = new bool[mdCmdCount];
                        fill_n(blockTypes.back().blkFieldList[fieldNr].setIfWhenSet[i], mdCmdCount, true);
                    }
                }
                int count = 0;
                for (param = argnode.child("set_if"); param; param = param.next_sibling("set_if")) {

                    string cond = param.attribute("if").value();
                    if (cond.find_first_not_of("()ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!+|$") != string::npos)
                        throw (string("<blocktype><field><set_if>: Invalid condition argument"));

                    string mask = param.attribute("value").value();
                    if (mask.empty()) throw (string("<blocktype><field><set_if>: missing value= argument"));
                    if (!isNumber(mask)) throw (string("<blocktype><field><set_if>: value argument is not a number"));
                    blockTypes.back().blkFieldList[fieldNr].setIfMask[count] = strToNum(mask);

                    tempstr = param.attribute("clear").value();
                    if (!tempstr.empty()) {
                        if (tempstr == "all") blockTypes.back().blkFieldList[fieldNr].setIfClear[count] = CLEAR_ALL;
                        else if (tempstr == "hi") blockTypes.back().blkFieldList[fieldNr].setIfClear[count] = CLEAR_HI;
                        else if (tempstr == "lo") blockTypes.back().blkFieldList[fieldNr].setIfClear[count] = CLEAR_LO;
                        else throw (string("<blocktype><field><set_if>: Invalid argument for clear="));
                    }

                    if (cond.find('+') != string::npos) {
                        blockTypes.back().blkFieldList[fieldNr].setIfByAny[count] = false;
                        if (cond.find('|') != string::npos)
                            throw (string("<blocktype><field><set_if>: Use of both + and | in condition"));
                    }

                    if (cond.empty()) blockTypes.back().blkFieldList[fieldNr].setIfAlways[count] = true;
                    else {
                        //handle global NOT
                        if (cond.size() > 1 && cond.substr(0,2) == "!(") {
                            for (int j = 0; j < mdCmdCount; j++)
                                blockTypes.back().blkFieldList[fieldNr].setIfWhenSet[count][j] = false;
                            cond.erase(0,2);
                            cond = trimChars(cond, ")");
                        }

                        if (cond.find('(') != string::npos)
                            throw (string("<blocktype><field><set_if>: expression too complex"));

                        if (cond == "none") {
                            for (int j = 0; j < mdCmdCount; j++) {
                                blockTypes.back().blkFieldList[fieldNr].setIfWhenSet[count][j] = false;
                                blockTypes.back().blkFieldList[fieldNr].setIfBy[count][j] = true;
                                blockTypes.back().blkFieldList[fieldNr].setIfByAny[count] = false;
                            }
                        } else if (cond == "all") {

                            blockTypes.back().blkFieldList[fieldNr].setIfByAny[count] = false;
                            for (int j = 0; j < mdCmdCount; j++)
                                blockTypes.back().blkFieldList[fieldNr].setIfBy[count][j] = true;
                        } else if (cond == "any") {
                            for (int j = 0; j < mdCmdCount; j++)
                                blockTypes.back().blkFieldList[fieldNr].setIfBy[count][j] = true;
                        } else {
                            while (!cond.empty()) {
                                bool setNot = false;
                                string cmdString;

                                if (cond.substr(0,1) == "!") {
                                    setNot = true;
                                    cond.erase(0,1);
                                }

                                cmdString = cond.substr(0, cond.find_first_of("+|"));

                                cmdNr = getCmdNr(cmdString);
                                if (cmdNr == -1)
                                    throw ("<blocktype><field><set_if>: Unknown command \"" + cmdString + "\" in condition");

                                blockTypes.back().blkFieldList[fieldNr].useCmd[cmdNr] = true;
                                blockTypes.back().blkFieldList[fieldNr].setIfBy[count][cmdNr] = true;
                                if (setNot) blockTypes.back().blkFieldList[fieldNr].setIfWhenSet[count][cmdNr] = false;

                                if (cond != cmdString) cond.erase(0, cond.find_first_of("+|") + 1);	//or npos
                                else cond = "";
                            }
                        }
                    }
                    count++;
                }

                if (verbose) {
                    cout << "Field " << fieldNr;
                    if (blockTypes.back().blkFieldList[fieldNr].requiredAlways) cout << " always required";
                    else {
                        cout << " required if ";
                        for (int j = 0; j < mdCmdCount; j++) {
                            if (blockTypes.back().blkFieldList[fieldNr].requiredBy[j]) {
                                bool lastentry = true;
                                for (int k = j + 1; k < mdCmdCount; k++)
                                    if (blockTypes.back().blkFieldList[fieldNr].requiredBy[k]) lastentry = false;
                                cout << mdCmdList[j].mdCmdName;
                                if (blockTypes.back().blkFieldList[fieldNr].requiredWhenSet[j]) cout << " is set ";
                                else cout << " is not set ";
                                if (blockTypes.back().blkFieldList[fieldNr].requiredByAny && !lastentry) cout << "or ";
                                else if (!blockTypes.back().blkFieldList[fieldNr].requiredByAny && !lastentry) cout << "and ";
                            }
                        }
                    }
                    cout << endl;
                }
                fieldNr++;
            }
        }


        for (int i = 0; i < mdCmdCount; i++) {
            if (mdCmdList[i].isBlkReference) {
                bool validRef = false;
                for (auto&& it : blockTypes) if (it.blockConfigID == mdCmdList[i].referenceBlkID) validRef = true;
                if (!validRef) throw("Block type referenced by command " + mdCmdList[i].mdCmdName + " does not exist.");
            }
        }


        //validate sequence track sources
        for (auto&& it: trackSources) {

            bool sourceFound = false;
            for (auto&& bt: blockTypes) {

                if (bt.blockConfigID == it) sourceFound = true;
            }

            if (!sourceFound) throw ("<sequence>: Track source \"" + it + "\" does not name an existing block type.");
        }

        return;
    } catch(string &e) {
        throw (configname + ": " + e + "\nConfig validation failed.");
    }
}


int mdConfig::getCmdNr(const string &cmdString) {

    for (int i = 0; i < mdCmdCount; i++) if (cmdString == mdCmdList[i].mdCmdName) return i;
    return -1;
}


//compare two commands to check if they are data compatible
bool mdConfig::isCompatible(const mdCommand &cmd1, const mdCommand &cmd2) {

    if (cmd1.mdCmdType != cmd2.mdCmdType) return false;
    if (cmd1.mdCmdAuto != cmd2.mdCmdAuto) return false;
    if (cmd1.useNoteNames != cmd2.useNoteNames) return false;
    if (cmd1.allowModifiers != cmd2.allowModifiers) return false;
    if (cmd1.limitRange != cmd2.limitRange) return false;
    if (cmd1.lowerRangeLimit != cmd2.lowerRangeLimit) return false;
    if (cmd1.upperRangeLimit != cmd2.upperRangeLimit) return false;
    if (cmd1.isBlkReference != cmd2.isBlkReference) return false;
    if (cmd1.referenceBlkID != cmd2.referenceBlkID) return false;
    if (cmd1.defaultSubstitute != cmd2.defaultSubstitute) return false;
    if (cmd1.mdCmdAutoValString != cmd2.mdCmdAutoValString) return false;
    if (cmd1.mdCmdForceString != cmd2.mdCmdForceString) return false;
    if (cmd1.mdCmdForceInt != cmd2.mdCmdForceInt) return false;
    if (cmd1.mdCmdForceSubstitution != cmd2.mdCmdForceSubstitution) return false;
    if (cmd1.substitutionList != cmd2.substitutionList) return false;
    if (cmd1.mdCmdForceRepeat != cmd2.mdCmdForceRepeat) return false;
//	if (cmd1.mdCmdUseLastSet != cmd2.mdCmdUseLastSet) return false;
    if (cmd1.mdCmdGlobalConst != cmd2.mdCmdGlobalConst) return false;

    return true;
}
