#include <sstream>
using std::stringstream;

#include "CDataParse.h"

CDataParse::CDataParse(const char* split)
{
	ss.clear();
	vs.clear();
	this->split = split;
}

CDataParse::CDataParse(const string& ss_,const char* split)
{
	ss.clear();
	ss = string(ss_);
	this->split = split;
}

CDataParse::CDataParse(const vector<string>& vs_,const char* split) : vs(vs_)
{
	this->split = split;
}


CDataParse::~CDataParse()
{

}

void CDataParse::setString(const string& ss_)
{
	ss.clear();
	ss = string(ss_);
}

const vector<string>& CDataParse::getVector()
{
	string::size_type begin = 0;
	string::size_type end = ss.find(split,0);
	if (end == 0) 
	{
		begin += strlen(split);
		end = ss.find(split,begin);
	}
	
	while(end != string::npos)
	{
		vs.push_back(ss.substr(begin,end-begin));
		begin = end + strlen(split);
		if (begin != string::npos)
			end = ss.find(split,begin);
	}

	if (begin != string::npos && begin != ss.size())
	{
		vs.push_back(ss.substr(begin,end - begin));
	}

	return vs;
}

void CDataParse::setVector(const vector<string>& vs_)
{
	vs.clear();
	for (vector<string>::const_iterator i = vs_.begin(); i != vs_.end(); ++i)
	{
		vs.push_back(*i);
	}
}

const string& CDataParse::getString()
{
	ss.clear();
	for (vector<string>::iterator i = vs.begin(); i != vs.end(); ++i)
	{
		ss += *i;
		if (i != vs.end() - 1) ss += split;
	}

	return ss;
}

void CDataParse::setSplit(const char* split)
{
	this->split = split;
}
