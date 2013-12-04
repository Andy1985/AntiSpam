#include "Common.h"

#include <errno.h>

#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>

int my_str2int(const string& s)
{               
        stringstream ss;
        ss << s;
        int ii = 0;
        ss >> ii;

        return ii;
}   
    
string my_int2str(const int i)
{
        stringstream ss;
        ss << i;
        string s;
        ss >> s;
    
        return s;
}   

int code_convert_ex(const char *from_code, const char *to_code, char *from, char *to)
{
        iconv_t cd;
        char  *tptr, *fptr;
        size_t  ileft, oleft, ret,convert_len=0,i_len;
         tptr = fptr =NULL;
         if(from == NULL || to == NULL) return -1;
        cd = iconv_open(( const char * )to_code, ( const char * )from_code );
        if ( cd == ( iconv_t )-1 )
        {
                return -1;
        }
        i_len=ileft = strlen(from);
        fptr = from;
        for( ; ; )
        {
                tptr = to;
                oleft = strlen( from ) * 4 + 1;
                memset( to, 0x00, strlen( from ) * 4 + 1 );
#ifdef _SOLARIS_PLAT
                ret = iconv( cd, (const char**)&fptr, &ileft, &tptr, &oleft );
#else
                ret = iconv( cd, &fptr, &ileft, &tptr, &oleft );
#endif
                convert_len+=ret;
                if(ret==0)
                {
                        break;
                }
                else if(ret>0 && ileft==0 && convert_len<i_len)
                {
                        ( void )iconv_close( cd );
                        return -1;
                }
                if( errno == EINVAL )
                {
                        ( void )iconv_close( cd );
                        return 1;
                }
                else if(errno == E2BIG)
                {
                        continue;
                }
                else if(errno == EILSEQ)
                {
                        ( void )iconv_close( cd );
                        return 1;
                }
                else if(errno == EBADF)
                {
                        ( void )iconv_close( cd );
                        return -1;
                }
                else
                {
                        ( void )iconv_close( cd );
                        return -1;
                }
        }
        ( void )iconv_close( cd );
        return 0;
}

string delete_special_str( string tmp_str, const char* special_str )
{
   string str_ret;

   unsigned int pos1 = 0;
   unsigned int pos2 = 0;

   if( tmp_str.find(special_str, pos1) == string::npos ) return tmp_str;

   while( (pos2 = tmp_str.find(special_str, pos1)) != string::npos )
   {
	  if (!strcmp(special_str,"&nbsp;"))
	  {
		str_ret += " ";
      	str_ret += tmp_str.substr(pos1, pos2-pos1);
	  }
	  else
		str_ret += tmp_str.substr(pos1, pos2-pos1);

      pos1 = pos2 + strlen(special_str);
      if( pos1 >= tmp_str.length() ) break;
   }

   return str_ret + tmp_str.substr(pos1,string::npos);
}   

string get_text_from_html(const char* tmp_html)
{
   string str_html = tmp_html;
   string str_text;
   string tmp_str_text;

   unsigned int pos1 = 0;
   unsigned int pos2 = 0;

   pos1 = str_html.find("<",0);
   if (pos1 != string::npos) 
   {
      tmp_str_text = delete_special_str(str_html.substr(0,pos1),"&nbsp;");
      str_text = str_text + tmp_str_text;
   }

   while( (pos2 = str_html.find(">", pos1)) != string::npos )
   {
      pos1 = str_html.find("<", pos2);

      if( (pos1 != string::npos) && (pos1 > pos2+1) )
      {
         tmp_str_text = delete_special_str( str_html.substr(pos2+1, pos1-pos2-1), "&nbsp;" );
         str_text = str_text + tmp_str_text;
      }

      if( pos1 == string::npos ) break;
   }

   return str_text;

}

string format_to_check(const string to_check,const string charset)
{
	string ret("");
	char* to = (char*)malloc(sizeof(char)*(to_check.size()*4 + 1));
	memset(to,'\0',sizeof(char)*(to_check.size()*4 + 1));
	code_convert_ex(charset.c_str(),"utf-8",(char*)to_check.c_str(),to);
	if (to != NULL) ret = string(to);
	free(to);

	return ret;
}
