#ifndef COMMON_H
#define COMMON_H

#include <sstream>
using std::stringstream;

#include <string>
using std::string;

int my_str2int(const string& s);
string my_int2str(const int i);
int code_convert_ex(const char *from_code, const char *to_code, char *from, char *to);
string delete_special_str( string tmp_str, const char* special_str );
string get_text_from_html(const char* tmp_html);
string format_to_check(const string to_check,const string charset);

#endif /*COMMON_H*/
