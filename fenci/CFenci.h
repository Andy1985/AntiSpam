#ifndef _CFENCI_H_
#define _CFENCI_H_

#include <string>
using std::string;

#include <set>
using std::set;

#include <scws/scws.h>

class CFenci
{
public:
	CFenci();
	~CFenci();
	
	bool setDict(const string dictFile);
	bool setRule(const string ruleFile);
	bool setCharset(const string charset);
	bool setIgnoreSign();

	bool getFenciResult(const string strToParse,set<string>& result);
private:
	scws_t s;
};

#endif /*_CFENCI_H_*/
