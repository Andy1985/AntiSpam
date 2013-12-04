#ifndef CANTISPAMMAIL_H
#define CANTISPAMMAIL_H

#include "mime/MimeMessage.h"
#include "mime/String.h"
#include "fenci/CFenci.h"
#include "comm/Common.h"
#include "comm/CDataParse.h"
#include "bayes/bayes.h"
#include "CRedis.h"
#include <map>
#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include <set>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>

using namespace std;
using namespace fast;

typedef struct b_word_t_
{
	int bad;
	int good;
} b_word_t;

class CAntiSpamMail
{
	public:
		CAntiSpamMail();
		~CAntiSpamMail();

		void setRedis(const string redisIp,const int redisPort,
			const int redisTimeout = 3);
		void setFenci(const string fenciDict,const string fenciRule,
			const string fenciCharset = "UTF-8");

		double getSpamicity(FastString mailData);
		
	private:
		CRedis myRedis;
		CFenci myFenci;

		void getFws(map<string,b_word_t> wordmap,vector<double>& fws);
		void getWords(const set<string>& result,map<string,b_word_t>& wordmap);
};

#endif /*CANTISPAMMAIL_H*/
