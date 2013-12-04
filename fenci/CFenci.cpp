#include "CFenci.h"


#include <iostream>
using std::cerr;
using std::endl;
using std::cout;

CFenci::CFenci()
{
	/* create the scws engine */
	if (!(s = scws_new()))
	{
    		cerr << "ERROR: cann't init the scws!" << endl;
		exit(-1);
	}
}

CFenci::~CFenci()
{
	if (!s) scws_free(s);
}

bool CFenci::setDict(const string dictFile)
{
	/* set dict */
  	scws_set_dict(s, dictFile.c_str(), SCWS_XDICT_XDB);
	
	return true;
}

bool CFenci::setRule(const string ruleFile)
{
	/* set rule */
  	scws_set_rule(s, ruleFile.c_str());

	return true;
}

bool CFenci::setCharset(const string charset)
{
	/* set charset */
	scws_set_charset(s, charset.c_str());

	return true;
}

bool CFenci::setIgnoreSign()
{
	/* set ignore sign */
	scws_set_ignore(s,1);

	return true;	
}

bool CFenci::getFenciResult(const string strToParse,set<string>& result)
{
	scws_res_t res, cur;

	const char* str = strToParse.c_str();
	int str_len = strToParse.size();

	/* fork*/
	scws_t sf = scws_fork(s);

	/* set text to parse */
	scws_send_text(sf, str, str_len);

	/* loop get words */
	while ((cur = res = scws_get_result(sf)) != NULL)		
	{													
		while (cur != NULL)										
		{		
			if (cur->len != 1 || ((*(str + cur->off) != '\n')	
				&& (*(str + cur->off) != '\r')))
			{
				string buff(str + cur->off,cur->len);
				result.insert(buff);
			}
			cur = cur->next;									
		}												

		scws_free_result(res);									
	}

	scws_free(sf);
	return true;
}
