#include <iostream>
#include <algorithm>

#include "mdal.h"

using namespace std;


mdSequence::mdSequence(): sequenceString("") {}


void mdSequence::read(string *sequenceBlock, const unsigned &sequenceBlockLength, const mdConfig &config) {


	for (unsigned i = 0; i < sequenceBlockLength; i++) {

		string inputString = sequenceBlock[static_cast<int>(i)];
		inputString.erase(0, inputString.find_first_not_of(" \t"));
		inputString.erase(inputString.find_last_not_of(" \t")+1);

		if (inputString != "" && inputString != "[LOOP]") mdSequenceLength++;
	}


	if (!mdSequenceLength) throw (string("Sequence contains no patterns"));
	if (config.seqMaxLength && mdSequenceLength > config.seqMaxLength) throw (string("Maximum sequence length exceeded."));


	for (unsigned i = 0; i < config.trackSources.size(); i++) sequenceData.emplace_back(vector<string>());
	unsigned pos = 0;
	for (unsigned i = 0; i < sequenceBlockLength; i++) {

		string inputString = sequenceBlock[static_cast<int>(i)];
		inputString.erase(remove_if(inputString.begin(), inputString.end(), ::isspace), inputString.end());

		if (inputString == "[LOOP]") mdSequenceLoopPosition = pos;

		else if (inputString != "") {

			for (auto&& it: sequenceData) {

				size_t kpos = inputString.find_first_of(',');
				if (inputString == "") throw (string("Not enough tracks in sequence"));
				it.push_back(inputString.substr(0, kpos));
				if (kpos != string::npos) inputString.erase(0, kpos + 1);
			}
			pos++;
		}
	}

	if (mdSequenceLoopPosition >= mdSequenceLength) {

		cout << "Warning: [LOOP] position is invalid, falling back to default." << endl;
		mdSequenceLoopPosition = 0;
	}

	sequenceString = getSequenceString(config);

	return;
}


string mdSequence::getSequenceString(const mdConfig &config) {

	string seqString = config.seqLabel + "\n";

	for (unsigned i = 0; i < mdSequenceLength; i++) {

		if (i == mdSequenceLoopPosition && config.useSeqLoop) seqString += ("\n" + config.seqLoopLabel);

		seqString += ("\n\t" + config.wordDirective + " ");
		for (unsigned j = 0; j < config.trackSources.size(); j++) {

			string labelPrefix = "";
			for (auto&& it: config.blockTypes) {
				if (it.blockConfigID == config.trackSources[j]) {
					labelPrefix = it.blkLabelPrefix;
					break;
				}
			}

			seqString += (labelPrefix + sequenceData[j][i]);
			if (j + 1 < config.trackSources.size()) seqString += ",";
		}
	}

	seqString = seqString + "\n\t" + config.seqEndString;
	if (config.useSeqLoopPointer) seqString = seqString + "\n\t" + config.wordDirective + " " + config.seqLoopLabel;

	return seqString;
}


ostream& operator<<(ostream& os, const mdSequence &seq) {

	os << seq.sequenceString << endl << endl;
	return os;
}
