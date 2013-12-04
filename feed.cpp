#include "mime/MimeMessage.h"
#include "mime/String.h"
#include "fenci/CFenci.h"
#include "comm/Common.h"
#include "comm/CDataParse.h"
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

FastString get_file_content(const string filename);

#define FEED_SPAM 0
#define FEED_HAM 1
#define UN_FEED_SPAM 3
#define UN_FEED_HAM 4

int main(int argc,char* argv[])
{
	if (argc != 3)
	{
		cerr << "Usage: " << argv[0] << " <-s|-n|-S|-N> <email>" << endl;
		exit(-1);
	}

	int feed_type = 0;
	if (string(argv[1]) == "-s") feed_type = FEED_SPAM;
	else if (string(argv[1]) == "-n") feed_type = FEED_HAM;
	else if (string(argv[1]) == "-S") feed_type = UN_FEED_SPAM;
	else if (string(argv[1]) == "-N") feed_type = UN_FEED_HAM;
	else { cerr << "Usage: " << argv[0] << " <-s|-n|-S|-N> <email>" << endl; exit(-1); }

	FastString email_data = get_file_content(argv[2]);
        MimeMessage msg(email_data);

        FastString charset="";
	FastString plain = "";
	FastString html = "";
	FastString subject="";


	set<string> result;
	CFenci myFenci;
	myFenci.setDict("/usr/local/etc/dict_chs.utf8.xdb");
	myFenci.setRule("/usr/local/etc/rules.utf8.ini");
	myFenci.setCharset("utf-8");
	myFenci.setIgnoreSign();

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

	/* connect redis */
	CRedis myRedis("127.0.0.1",6379);
	const int CONNECT_TIMEOUT = 1;
	myRedis.setTimeout(CONNECT_TIMEOUT);
	if (false == myRedis.Connect())	
	{
		cerr << myRedis.getError() << endl;
		return -1;
	}

	/* feed spam */
	if (feed_type == FEED_SPAM)
	{
		for (set<string>::iterator it = result.begin(); it != result.end(); ++it)
		{
			string buffer = myRedis.Get(*it);
			if (buffer != "")
			{
				CDataParse myData(buffer," ");
				vector<string> ret = myData.getVector();
				ret[0] = my_int2str((my_str2int(ret[0]) + 1));
				myData.setVector(ret);
				if (false == myRedis.Set(*it,myData.getString()))
				{
					cerr << myRedis.getError() << endl;
					return -1;
				}
			}
			else
			{
				if (false == myRedis.Set(*it,"1 0"))
				{
					cerr << myRedis.getError() << endl;
					return -1;
				}
			}
		}
	}

	/* feed ham */
	if (feed_type == FEED_HAM)
	{
		for (set<string>::iterator it = result.begin(); it != result.end(); ++it)
		{
			string buffer = myRedis.Get(*it);
			if (buffer != "")
			{
				CDataParse myData(buffer," ");
				vector<string> ret = myData.getVector();
				ret[1] = my_int2str((my_str2int(ret[1]) + 1));
				myData.setVector(ret);
				if (false == myRedis.Set(*it,myData.getString()))
				{
					cerr << myRedis.getError() << endl;
					return -1;
				}
			}
			else
			{
				if (false == myRedis.Set(*it,"0 1"))
				{
					cerr << myRedis.getError() << endl;
					return -1;
				}
			}
		}
	}

	/* unfeed spam */
	if (feed_type == UN_FEED_SPAM)
	{
		for (set<string>::iterator it = result.begin(); it != result.end(); ++it)
		{
			string buffer = myRedis.Get(*it);
			if (buffer != "")
			{
				CDataParse myData(buffer," ");
				vector<string> ret = myData.getVector();
				if (my_str2int(ret[0]) > 0) ret[0] = my_int2str(my_str2int(ret[0]) - 1);
				myData.setVector(ret);
				if (false == myRedis.Set(*it,myData.getString()))
				{
					cerr << myRedis.getError() << endl;
					return -1;
				}
			}
		}
	}

	/* unfeed ham */
	if (feed_type == UN_FEED_HAM)
	{
		for (set<string>::iterator it = result.begin(); it != result.end(); ++it)
		{
			string buffer = myRedis.Get(*it);
			if (buffer != "")
			{
				CDataParse myData(buffer," ");
				vector<string> ret = myData.getVector();
				if (my_str2int(ret[1]) > 0) ret[1] = my_int2str(my_str2int(ret[1]) - 1);
				myData.setVector(ret);
				if (false == myRedis.Set(*it,myData.getString()))
				{
					cerr << myRedis.getError() << endl;
					return -1;
				}
			}
		}
	}

	/* close redis */
	myRedis.Close();

	exit(0);
}

FastString get_file_content(const string filename)
{
	FastString content("");
	struct stat statbuf;

	int infd = open(filename.c_str(),O_RDONLY);

	if( infd > 0 )
	{
	    fstat(infd, &statbuf);
	    int filesize = statbuf.st_size;
	    char buffer[4096] = {0};

	    content.resize(filesize+1);

	    int left = filesize;
	    while( left > 0 )
	    {
		int nread = 0;
		int readsize = left > (int) sizeof(buffer)-1 ? sizeof(buffer)-1 : left;

		memset(buffer, 0, sizeof(buffer));

		if( (nread = read(infd, buffer, readsize)) < 0 )
		{
		    if( errno == EINTR )
			continue;
		    else
			break;
		}
		else if( nread == 0 )
		    break;

		content.append(buffer, nread);
		left -= nread;
	    }
	    close(infd);
	}

	return content;
}
