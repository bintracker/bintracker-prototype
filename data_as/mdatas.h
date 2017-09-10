#ifndef MDATAS__H__
#define MDATAS__H__

#include <unordered_map>

using namespace std;

class Data_Assembly {

public:
	char *binData;
	long binLength;
	static unordered_map<string, long> equates;
	unordered_map<string, long> symbols;

	Data_Assembly();
	~Data_Assembly();
	void init(const string &eqfile);
	void reset();
	void assemble(const vector<string> &asmLines_, const long &origin, const bool &littleEndian);
	long evalExpr(const string &expr);
    long solve(const long &argv1, const long &argv2, const string &op);

private:


};


#endif
