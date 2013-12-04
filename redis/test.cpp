#include "CRedis.h"

#include <iostream>
using std::cerr;
using std::cout;
using std::endl;

int main(int argc,char* argv[])
{
	const int CONNECT_TIMEOUT = 1;

	// init object
	CRedis myRedis("127.0.0.1",6379);

	// set connect timeout
	myRedis.setTimeout(CONNECT_TIMEOUT);

	// connect
	if (false == myRedis.Connect())	
	{
		cerr << myRedis.getError() << endl;
		return -1;
	}

	// set 
	if (false == myRedis.Set("foo","1 0"))
	{
		cerr << myRedis.getError() << endl;
		return -1;
	}

	// get
	cout << myRedis.Get("foo") << endl;

	// delete
	if (false == myRedis.Delete("foo"))
	{
		cerr << myRedis.getError() << endl;
		return -1;
	}

	// close
	myRedis.Close();

	return 0;
}
