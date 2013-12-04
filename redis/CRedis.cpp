#include "CRedis.h"

#define REDIS_FREE(x) { if (x != NULL) {redisFree(x); x = NULL;} }
#define REPLY_FREE(x) { if (x != NULL) {freeReplyObject(x); x = NULL;} }

CRedis::CRedis(const string hostname_,const int port_) : hostname(hostname_),port(port_)
{
	c = NULL;
	reply = NULL;
	errorMsg = "";
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
}

CRedis::CRedis()
{
	c = NULL;
	reply = NULL;
	errorMsg = "";
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
}

CRedis::~CRedis()
{
	REDIS_FREE(c);
	REPLY_FREE(reply);
}

void CRedis::setIp(const string ip)
{
	hostname = ip;
}

void CRedis::setPort(const int port_)
{
	port = port_;
}

void CRedis::setTimeout(const int sec,const int microsec)
{
	timeout.tv_sec = sec;
	timeout.tv_usec = microsec;
}

bool CRedis::Connect()
{
	c = redisConnectWithTimeout(hostname.c_str(), port, timeout);
	
	if (c == NULL || c->err) 
	{
		if (c)
		{
			errorMsg = string(c->errstr);
		}
		else
		{
			errorMsg = "Connection error: can't allocate redis context";
		}

		REDIS_FREE(c);
		REPLY_FREE(reply);
		return false;
	}

	return true;
}

void CRedis::Close()
{
	REDIS_FREE(c);
	REPLY_FREE(reply);
}

string CRedis::getError()
{
	return errorMsg;
}

bool CRedis::Set(const string key,const string value)
{
	reply = (redisReply*)redisCommand(c,"SET %s %s",key.c_str(),value.c_str());
	errorMsg = c->errstr ? string(c->errstr) : string("");
	REPLY_FREE(reply);
	return c->err ? false : true;
}

string CRedis::Get(const string key)
{
	string ret("");
    	reply = (redisReply*)redisCommand(c,"GET %s",key.c_str());
	ret = reply->str ? reply->str : "";
	REPLY_FREE(reply);
	return ret;
}

bool CRedis::Delete(const string key)
{
	reply = (redisReply*)redisCommand(c,"DEL %s",key.c_str());
	errorMsg = c->errstr ? string(c->errstr) : string("");
	REPLY_FREE(reply);
	return c->err ? false : true;
}
