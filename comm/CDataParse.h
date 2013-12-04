#ifndef CDATAPARSE_H
#define CDATAPARSE_H

#include <string>
using std::string;

#include <vector>
using std::vector;

class CDataParse
{
	public:
		CDataParse(const char* split);
		CDataParse(const string& ss,const char* split);
		CDataParse(const vector<string>& vs,const char* split);
		~CDataParse();
	
		void setSplit(const char* split);

		void setString(const string& ss);
		const vector<string>& getVector();

		void setVector(const vector<string>& vs);
		const string& getString();


	private:
		string ss;
		vector<string> vs;
		const char* split;
	
};

#endif /*CDATAPARSE_H*/
