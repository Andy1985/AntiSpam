#include "CFenci.h"

#include <string>
using std::string;

#include <set>
using std::set;

#include <iostream>
using std::cout;
using std::endl;

#include <cstdlib>


int main(int argc,char* argv[])
{
	const string str = "我是一个中国人,我会C++语言,我也有很多T恤衣服";
	const string charset = "utf-8";
	set<string> result;

	CFenci myFenci;
	myFenci.setDict("/usr/local/etc/dict_chs.utf8.xdb");
	myFenci.setRule("/usr/local/etc/rules.utf8.ini");
	myFenci.setCharset("utf8");
	myFenci.setIgnoreSign();
	myFenci.getFenciResult(str,result);

	for (set<string>::iterator it = result.begin(); it != result.end(); ++it)
	{
		cout << *it << " ";
	}

	const string str2 = "中华人民共和国";
	result.clear();
	myFenci.getFenciResult(str2,result);

	for (set<string>::iterator it = result.begin(); it != result.end(); ++it)
	{
		cout << *it << " ";
	}

	cout << endl;

	exit(0);
}
