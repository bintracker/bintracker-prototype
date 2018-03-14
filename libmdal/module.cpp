#include <iostream>
#include <string>

#include "mdal.h"

using namespace std;


mdModule::mdModule(const vector<string> &moduleLines, const mdConfig &config, bool &verbose) {

	rawDataBlock = nullptr;
	linecount = 0;

	try {

		MUSICASM.str(string());			//clear stream
		linecount = moduleLines.size();

		for (auto&& it : config.blockTypes) moduleBlocks.emplace_back(it.blockConfigID);
		moduleBlocks.shrink_to_fit();

		for (int i = 0; i < config.mdCmdCount; i++) config.mdCmdList[i].resetToDefault();


		if (verbose) cout << endl << "MODULE DATA\n===========" << endl;

		//parse global constants
		for (int i = 0; i < config.mdCmdCount; i++) {

			if (config.mdCmdList[i].mdCmdGlobalConst) {

				for (auto&& it: moduleLines) {

					if (!it.compare(0, config.mdCmdList[i].mdCmdName.size() + 1, config.mdCmdList[i].mdCmdName + "=")) {

						try {
							config.mdCmdList[i].setDefault(it.substr(config.mdCmdList[i].mdCmdName.size() + 1,
								string::npos));
						}
						catch(string &e) {
							throw (e + "in global constant definition of " + config.mdCmdList[i].mdCmdName);
						}

						break;
					}
				}
			}
		}

		unsigned blockStart = locateToken(string(":SEQUENCE"), moduleLines);
		unsigned blockEnd = getBlockEnd(blockStart, moduleLines);

		if (blockStart > blockEnd) throw (string("Sequence contains no patterns."));

		rawDataBlock = new string[blockEnd - blockStart];

		for (unsigned i = blockStart + 1; i <= blockEnd; i++) rawDataBlock[static_cast<int>(i - blockStart - 1)] = moduleLines[i];

		seq.read(rawDataBlock, blockEnd - blockStart, config);

		delete[] rawDataBlock;
		rawDataBlock = nullptr;


		//generate block references from sequence
		for (unsigned i = 0; i < config.trackSources.size(); i++) {

			unsigned ts = 0;
			for (; config.trackSources[i] != config.blockTypes[ts].blockConfigID; ts++) {};

			for (unsigned j = 0; j < seq.mdSequenceLength; j++)
				moduleBlocks.at(ts).addReference(seq.sequenceData[i][j], (j) == 0);
		}


		if (verbose) cout << seq << endl;
		MUSICASM << seq << endl;


		//add default references
		for (int i = 0; i < config.mdCmdCount; i++) {

			if (config.mdCmdList[i].isBlkReference) {

				for (auto&& it: moduleBlocks) {

					if (it.blockTypeID == config.mdCmdList[i].referenceBlkID)
						it.addReference(config.mdCmdList[i].mdCmdDefaultValString, false);
				}
			}
		}


		size_t blockType = 0;

		for (auto&& it : moduleBlocks) {

			size_t blockNr = 0;

			while (it.referenceCount) {

				blockStart = locateToken(":" + it.blocks.at(blockNr).blkName, moduleLines);
				blockEnd = getBlockEnd(blockStart, moduleLines);


				if (blockStart >= linecount - 1) throw ("Block \"" + it.blocks.at(blockNr).blkName + "\" is not defined.");
				if (blockStart >= blockEnd) throw ("Block \"" + it.blocks.at(blockNr).blkName + "\" contains no data");
				//TODO: does not reliably detect empty patterns.

				rawDataBlock = new string[blockEnd - blockStart];

				for (unsigned j = blockStart + 1; j <= blockEnd; j++) rawDataBlock[static_cast<int>(j - blockStart - 1)] = moduleLines[j];

				try {
					it.blocks.at(blockNr).read(rawDataBlock, static_cast<int>(blockEnd - blockStart), config,
						config.blockTypes.at(blockType), moduleBlocks);
				}
				catch(string &e) {
					throw ("In pattern \"" + it.blocks.at(blockNr).blkName + "\": " + e);
				}

				delete[] rawDataBlock;
				rawDataBlock = nullptr;


				MUSICASM << it.blocks.at(blockNr) << endl;
				if (verbose) cout << it.blocks.at(blockNr) << endl;

				blockNr++;
				it.referenceCount--;
			}

			blockType++;
		}

		return;
	}
	catch(string &e) {
		throw (e + "\nModule validation failed.");
	}
}


mdModule::~mdModule() {

	delete[] rawDataBlock;
}


unsigned mdModule::getBlockEnd(const unsigned &blockStart, const vector<string> &moduleLines) {

	size_t line;
	size_t pos = string::npos;

	for (line = blockStart + 1; line < linecount && pos == string::npos; line++) pos = moduleLines[line].find(':');

	if (line == linecount) return line - 1;
	return line - 2;
}

//TODO: throw error if token not found
unsigned mdModule::locateToken(const string &token, const vector<string> &moduleLines) {

	size_t line;

	for (line = 0; line < linecount; line++) {

		if (trimChars(moduleLines[line], " \t") == token) return line;
	}

	return line;
}


ostream& operator<<(ostream &os, const mdModule &mdf) {

	os << mdf.MUSICASM.str() << endl;
	return os;
}
