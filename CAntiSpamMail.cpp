#include "CAntiSpamMail.h"

CAntiSpamMail::CAntiSpamMail()
{
	myRedis = CRedis("127.0.0.1",6379);
	myRedis.setTimeout(3);

	myFenci = CFenci();
	myFenci.setDict("/usr/local/etc/dict_chs.utf8.xdb");
	myFenci.setRule("/usr/local/etc/rules.utf8.ini");
	myFenci.setCharset("utf-8");
	myFenci.setIgnoreSign();
}

CAntiSpamMail::~CAntiSpamMail()
{
}

void CAntiSpamMail::setRedis(const string redisIp,const int redisPort,
		const int redisTimeout)
{
	myRedis.setIp(redisIp);
	myRedis.setPort(redisPort);
	myRedis.setTimeout(redisTimeout);
}

void CAntiSpamMail::setFenci(const string fenciDict,const string fenciRule,
		const string fenciCharset)
{
	myFenci.setDict(fenciDict);
	myFenci.setRule(fenciRule);
	myFenci.setCharset(fenciCharset);
}

double CAntiSpamMail::getSpamicity(FastString mailData)
{
    MimeMessage msg(mailData);

    FastString charset="";
	FastString plain = "";
	FastString html = "";
	FastString subject="";

	set<string> result;

	/* parse subject */
	msg.getSubject(subject,charset);
	myFenci.getFenciResult(format_to_check(subject.c_str(),charset.c_str()),result);

	/* parse plain */
    msg.getTextPlain(plain,charset);
	myFenci.getFenciResult(format_to_check(subject.c_str(),charset.c_str()),result);	

	/* parse html */
	msg.getTextHtml(html,charset);
	string html_to_check = get_text_from_html(html.c_str());
	myFenci.getFenciResult(format_to_check(html_to_check,charset.c_str()),result);

	/* get wordmap */
	myRedis.Connect();

	map<string,b_word_t> wordmap;
	getWords(result,wordmap);

	myRedis.Close();

	
	/* get fws */
	vector<double> fws;
	getFws(wordmap,fws);

	return spamicity(fws);
}

void CAntiSpamMail::getFws(map<string,b_word_t> wordmap,vector<double>& fws)
{
	double goodmsgs = 0.0;
	double badmsgs = 0.0;

	for (map<string,b_word_t>::iterator it = wordmap.begin(); it != wordmap.end(); ++it)
	{
		badmsgs += (it->second).bad;
		goodmsgs += (it->second).good;
	}

	fws.clear();
	for (map<string,b_word_t>::iterator it = wordmap.begin(); it != wordmap.end(); ++it)
	{
		fws.push_back(fw((it->second).bad,(it->second).good,badmsgs,goodmsgs));
	}
}

void CAntiSpamMail::getWords(const set<string>& result,map<string,b_word_t>& wordmap)
{
	for (set<string>::iterator it = result.begin(); it != result.end(); ++it)
	{
		string buffer = myRedis.Get(*it);
		if (buffer != "")
		{
			CDataParse myData(buffer," ");
			vector<string> ret = myData.getVector();
			b_word_t wordbuf = {0,0};
			wordbuf.bad = my_str2int(ret[0]);
			wordbuf.good = my_str2int(ret[1]);
			wordmap.insert(pair<string,b_word_t>(*it,wordbuf));
		}
		/*
		else
		{
			b_word_t wordbuf = {0,0};
			wordmap.insert(pair<string,b_word_t>(*it,wordbuf));
		}
		*/
	}
}
