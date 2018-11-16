#include "CAntiSpamMail.h"

FastString get_file_content(const string filename);

const double SPAM_CUTOFF = 0.90;

//git test

int main(int argc,char* argv[])
{
	if (argc != 2)
	{
		cerr << "Usage: " << argv[0] << " <email>" << endl;		
		exit(-1);
	}

	/* get email data */
	FastString email_data = get_file_content(argv[1]);

	/* check */
	CAntiSpamMail  myAntispam;
	double spamicity = myAntispam.getSpamicity(email_data);
	cout << ((spamicity > SPAM_CUTOFF) ? "SPAM" : "HAM" ) << " " << spamicity << endl;

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
