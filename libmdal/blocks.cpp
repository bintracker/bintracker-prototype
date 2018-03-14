#include <iostream>
#include <string>
#include <algorithm>
#include <utility>

#include "mdal.h"

using namespace std;

mdBlock::mdBlock(string name, bool seqStart): blkName(std::move(name)), blkLength(0), blkString("") {

	firstInSequence = seqStart;

	requestList = nullptr;
	lineCommands = nullptr;
	lineCmdVals = nullptr;
	lineCmdStrVals = nullptr;
}

mdBlock::mdBlock(const mdBlock &blk): blkName(blk.blkName), blkLength(blk.blkLength), blkString(blk.blkString) {

	firstInSequence = blk.firstInSequence;

	requestList = nullptr;
	lineCommands = nullptr;
	lineCmdVals = nullptr;
	lineCmdStrVals = nullptr;
}


mdBlock::~mdBlock() {

	for (int i = 0; i < blkLength; i++) {

		delete[] lineCommands[i];
		delete[] lineCmdVals[i];
		delete[] lineCmdStrVals[i];
	}

	delete[] lineCommands;
	delete[] lineCmdVals;
	delete[] lineCmdStrVals;

	delete[] requestList;


}


void mdBlock::read(const string *rawData, const int blockLength, const mdConfig &config, const mdBlockConfig &blkConfig, vector<mdBlockList> &moduleBlocks) {

	blkString = blkConfig.blkLabelPrefix + blkName + "\n";

	for (int i = 0; i < blockLength; i++) {

		string temp = trimChars(rawData[i], " ()\t");
		if (temp != "") blkLength++;
	}

	if (blkConfig.blkMaxLength && blkLength > blkConfig.blkMaxLength) throw (string("Maximum block length exceeded."));


	lineCommands = new bool*[blkLength];
	lineCmdVals = new int*[blkLength];
	lineCmdStrVals = new string*[blkLength];


	for (int row = 0; row < blkLength; row++) {

		lineCommands[row] = new bool[config.mdCmdCount];
		fill_n(lineCommands[row], config.mdCmdCount, false);

		lineCmdVals[row] = new int[config.mdCmdCount];
		fill_n(lineCmdVals[row], config.mdCmdCount, -1);

		lineCmdStrVals[row] = new string[config.mdCmdCount];
		fill_n(lineCmdStrVals[row], config.mdCmdCount, "");
	}


	int row = 0;

	for (int i = 0; i < blockLength; i++) {

//		string rowStr = trimChars(rawData[i], " ()\t");
		string tempstr = rawData[i];
		string rowStr = "";
		int quotes = count(tempstr.begin(), tempstr.end(), '\"');
		if (!quotes) rowStr = trimChars(tempstr, "()\t ");
		else {
			if (quotes & 1) throw ("Invalid argument in " + tempstr);
			for (int j = 0; j < (quotes/2); j++) {
				size_t pos = tempstr.find_first_of('\"');
				rowStr += trimChars(tempstr.substr(0, pos + 1), "()\t ");
				tempstr.erase(0, pos + 1);
				pos = tempstr.find_first_of('\"');
				rowStr += tempstr.substr(0, pos + 1);
				tempstr.erase(0, pos + 1);
			}
			if (tempstr != "") rowStr += trimChars(tempstr, "()\t ");
		}

		bool validData = false;

		while (rowStr != "") {

			validData = true;

			if (rowStr == ".") rowStr = "";
			else {

				if (count(begin(rowStr), end(rowStr), ',') > count(begin(rowStr), end(rowStr), '='))
					throw ("Syntax error in module, line " + rawData[i]);

				string temp;

				if (rowStr.find_first_of(",=") == string::npos) temp = rowStr;
				else temp = rowStr.substr(0, rowStr.find_first_of(",="));

				int cmdNr = -1;


				for (int j = 0; j < config.mdCmdCount && cmdNr == -1; j++) {

					if (config.mdCmdList[j].mdCmdName == temp) cmdNr = j;
				}


				if (cmdNr == -1) throw ("Unknown command \"" + temp + "\" found in " + rawData[i]);


				lineCommands[row][cmdNr] = true;

				rowStr.erase(0, temp.size() + 1);


				if (rowStr.find_first_of(',') != string::npos) {
					temp = rowStr.substr(0, rowStr.find_first_of(','));
					rowStr.erase(0, temp.size() + 1);
				}
				else {
					temp = rowStr;
					rowStr = "";
				}


				if (!config.mdCmdList[cmdNr].mdCmdAuto) lineCmdStrVals[row][cmdNr] = temp;
			}
		}

		if (validData) row++;
	}



	for (row = 0; row < blkLength; row++) {

		for (int cmd = 0; cmd < config.mdCmdCount; cmd++) {

			//TODO: this is probably not intended behaviour, but left in for now so mdalc produces same results as before
			if (row == 0 && (blkConfig.initBlkDefaults || blkConfig.baseType != PATTERN)) config.mdCmdList[cmd].resetToDefault();
			else config.mdCmdList[cmd].reset();


			if (lineCommands[row][cmd]) {

				if (config.mdCmdList[cmd].isBlkReference) {	//if cmd is reference

					for (auto&& it : moduleBlocks) {

						if (it.blockTypeID == config.mdCmdList[cmd].referenceBlkID) it.addReference(lineCmdStrVals[row][cmd], false);
					}
				}

				config.mdCmdList[cmd].set(lineCmdStrVals[row][cmd]);
			}
		}


		bool lastFieldIsWord = blkConfig.blkFieldList[0].isWord;
		bool firstSet = false;

		requestList = new bool[config.mdCmdCount];
		fill_n(requestList, config.mdCmdCount, false);



		for (int field = 0; field < blkConfig.blkFieldCount; field++) {

//			bool seqBegin = (patternNumber == 0) ? true : false;
			blkConfig.blkFieldList[field].getRequests(requestList, config, row, firstInSequence);

		}


		for (int field = 0; field < blkConfig.blkFieldCount; field++) {

			//cout << "row " << row << " field " << field << endl;	//DEBUG

			//bool seqBegin = (patternNumber == 0) ? true : false;

			string fieldString = blkConfig.blkFieldList[field].getFieldString(requestList, config);		//TODO: ...


			if (fieldString != "") {

				if (!firstSet || lastFieldIsWord != blkConfig.blkFieldList[field].isWord) {

					firstSet = true;
					lastFieldIsWord = blkConfig.blkFieldList[field].isWord;

					blkString += "\n\t";

					if (lastFieldIsWord) blkString += config.wordDirective + " ";
					else blkString += config.byteDirective + " ";
				}
				else blkString += ", ";
			}

			blkString += fieldString;
		}

		delete[] requestList;
		requestList = nullptr;
 	}

	if (blkConfig.useBlkEnd) blkString += "\n\t" + blkConfig.blkEndString;
	blkString += "\n";

}


ostream& operator<<(ostream& os, const mdBlock &blk) {

	os << blk.blkString << endl << endl;

	return os;
}




mdBlockList::mdBlockList(string blockTypeIdentifier): blockTypeID(std::move(blockTypeIdentifier)), referenceCount(0) {}

mdBlockList::mdBlockList(const mdBlockList &lst): blockTypeID(lst.blockTypeID), referenceCount(lst.referenceCount) {}


void mdBlockList::addReference(const string &title, bool seqStart) {

	if (uniqueReferences.insert(title).second == true) {

		blocks.emplace_back(title, seqStart);
		referenceCount++;
	}
}


mdBlockConfig::mdBlockConfig(const string &id): blockConfigID(id), baseType(GENERIC), useBlkEnd(false),
						blkEndString(""), initBlkDefaults(false),
						blkLabelPrefix("mdb_" + id + "_"), blkFieldCount(0), blkMaxLength(0) {

	blkFieldList = nullptr;
}

mdBlockConfig::mdBlockConfig(const mdBlockConfig &cfg): blockConfigID(cfg.blockConfigID), baseType(cfg.baseType),
								useBlkEnd(cfg.useBlkEnd), blkEndString(cfg.blkEndString),
								initBlkDefaults(cfg.initBlkDefaults), blkLabelPrefix(cfg.blkLabelPrefix),
								blkFieldCount(cfg.blkFieldCount), blkMaxLength(cfg.blkMaxLength) {

	blkFieldList = nullptr;
}

mdBlockConfig::~mdBlockConfig() {

	delete[] blkFieldList;
}
