//=============================================================================
/**
 *  @file    Properties.h
 *
 *  ver 1.0.0 2004/06/03 Naven Exp
 *
 *  @author Naven 2004/06/03 created.
 */
//=============================================================================

#ifndef _PROPERTIES_H
#define _PROPERTIES_H

#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <fstream>
#include <strstream>
#include <vector>
#include "FastString.h"

#if defined(_WIN32) || defined(__WIN32__)

#if !defined (NAVEN_PRAGMA_ONCE)
# pragma once
#endif /* NAVEN_PRAGMA_ONCE */

#endif /* _WIN32 */


using namespace std;


/**
 * @class Properties_Base<LINESIZE>
 *
 * @brief This class provides a configure file reader.
 *
 * 配置文件读取工具，配置文件的设置形式类似Java的Properties_Base<LINESIZE>类。
 * 读取 name = value 这样设置数据，注释在每行第一个字符用“#”。
 * <p>
 * 使用实例，配置文件如下：
 * <p><blockquote><pre>
 *
 * # Test configure file: test.conf
 * testhost = 127.0.0.1
 * testport = 9008
 *
 * </pre></blockquote><p>
 * 测试程序如下：
 * <p><blockquote><pre>
 *
 *  #include <stdio.h>
 *  #include "Properties_Base<LINESIZE>.h"
 *  
 *  int main(int argc, char* argv[]) 
 *  {
 *      const char *filename = argc > 1 ? argv[1] : "test.conf";
 *      Properties props(filename); 
 *  
 *      for( int i = 0; i < props.size(); i ++ ) {
 *          printf("%s = %s %d %d %f %d\n", props.getName(i).c_str(), 
 *          props.getValue(i).c_str(), 
 *          props.getInt(props.getName(i)), 
 *          props.getLong(props.getName(i)), 
 *          props.getFloat(props.getName(i)),
 *          props.getBool(props.getName(i)));
 *      }
 *  
 *      printf("%s\n", props.get("testhost").c_str());
 *      printf("%s\n", props.get("testest", "not found").c_str());
 *  }    
 *
 * </pre></blockquote><p>
 * 执行结果如下：
 * <p><blockquote><pre>
 *
 * testhost = 127.0.0.1
 * testport = 9008
 * 127.0.0.1
 * not found
 *
 * </pre></blockquote><p>
 *
 * @author Naven
 */

template<size_t LINESIZE=4096>
class Properties_Base
{
public:
    Properties_Base<LINESIZE>();
    Properties_Base<LINESIZE>(istream &is); 
    Properties_Base<LINESIZE>(const string &filename); 
    Properties_Base<LINESIZE>(const char *filename); 

    int load(const string &filename);
    int load(const char *filename);
    unsigned long size(); 
    string filename();
    string get(const string &name, const string &default_value=""); 
    string get(const char *name, const char * default_value=""); 
    string getValue(const string &name, const string &default_value=""); 
    string getValue(const char *name, const char * default_value=""); 
    int getInt(const string &name, const int default_value=0); 
    int getInt(const char *name, const int default_value=0); 
    long getLong(const string &name, const long default_value=0); 
    long getLong(const char *name, const long default_value=0); 
    double getFloat(const string &name, const double default_value=0); 
    double getFloat(const char *name, const double default_value=0); 
    int getBool(const string &name, const int default_value=0); 
    int getBool(const char *name, const int default_value=0); 
    void remove(const string &name); 
    void remove(const char *name); 
    void set(const string &name, const string &value); 
    void set(const char *name, const char *value); 
    void add(const string &name, const string &value); 
    void add(const char *name, const char *value); 
    string getName(const int &i);
    string getValue(const int &i);
    void setName(const int &i, const string &name);
    void setName(const int &i, const char *name);
    void setValue(const int &i, const string &value);
    void setValue(const int &i, const char *value);
    void removeItem(const int &i);

    void save();
    void save(const string &filename); 
    void save(const char *filename); 

private:
    void init(istream &is); 

private:
    string _file; 
    vector<string> _vals, _elms; 
    unsigned long _size; 

}; 

typedef class Properties_Base<4096>     Properties; 


/**
 * Constructor, default.
 */
template<size_t LINESIZE> inline 
Properties_Base<LINESIZE>::Properties_Base<LINESIZE>()
{
    _size = 0;
}

/**
 * Constructor, with specified input io stream.
 *
 * @param is  input stream
 */
template<size_t LINESIZE> inline 
Properties_Base<LINESIZE>::Properties_Base<LINESIZE>(istream &is) 
{
    _size = 0;
    init(is); 
}

/**
 * Constructor, with specified configure file name, 
 * which includes full path and filename.
 *
 * @param filename  input file name
 */
template<size_t LINESIZE> inline 
Properties_Base<LINESIZE>::Properties_Base<LINESIZE>(const string &filename) 
{
    load(filename); 
}

/**
 * Constructor, with specified configure file name, 
 * which includes full path and filename.
 *
 * @param filename  input file name
 */
template<size_t LINESIZE> inline 
Properties_Base<LINESIZE>::Properties_Base<LINESIZE>(const char *filename) 
{
    load((string)filename); 
}

/**
 * Load specified configure file, 
 * which includes full path and filename.
 *
 * @param filename  input file name
 * @return load item count, -1 when open file failed
 */
template<size_t LINESIZE> int
Properties_Base<LINESIZE>::load(const string &filename) 
{
    _elms.clear(); _vals.clear(); 
    _size = 0; 
    if(filename.empty()) 
        return -1; 
    ifstream is(filename.c_str()); 
    _file = filename; 

    init(is); 
    return size(); 
}

/**
 * Load specified configure file, 
 * which includes full path and filename.
 *
 * @param filename  input file name
 * @return load item count, -1 when open file failed
 */
template<size_t LINESIZE> int
Properties_Base<LINESIZE>::load(const char *filename) 
{
    return load((string)filename); 
}

/**
 * Load configure file with specified input stream.
 *
 * @param is  input stream
 */
template<size_t LINESIZE> void 
Properties_Base<LINESIZE>::init(istream &is) 
{
    if(!is) return;
    char ln[LINESIZE+1]; 
    memset(ln, 0, sizeof(ln));
    _elms.clear(); _vals.clear(); 
    _size = 0; 
    while(is.getline(ln, LINESIZE)) { 
        FastString s(ln); 
        s.ltrim(); 
        int pos = s.indexOf('='); 
        if( s.charAt(0) != '#' && pos > 0 ) {
            FastString name, value; 
            s.substring(name, 0, pos); 
            s.substring(value, pos + 1); 
            name.trim(); 
            value.trim(); 
            if( !name.empty() && !value.empty() ) { 
                _elms.push_back((string)name.c_str()); 
                _vals.push_back((string)value.c_str());
                _size ++; 
            }
        }
        memset(ln, 0, sizeof(ln));
    }
}

/**
 * Get the items's count in the configure file.
 *
 * @return  configure items count
 */
template<size_t LINESIZE> inline unsigned long 
Properties_Base<LINESIZE>::size() 
{
    return _size;
}

/**
 * Get the configure filename.
 *
 * @return  configure file name with full path
 */
template<size_t LINESIZE> inline string
Properties_Base<LINESIZE>::filename()
{
    return _file; 
}

/**
 * Get the specified <code>index</code> item name.
 *
 * @return  configure item name
 */
template<size_t LINESIZE> string 
Properties_Base<LINESIZE>::getName(const int &i)
{
    if(i >=0 && i < (int)_elms.size()) 
        return _elms[i];
    return "";
}

/**
 * Get the specified <code>index</code> item's <code>value</code>.
 *
 * @return  configure item value
 */
template<size_t LINESIZE> string 
Properties_Base<LINESIZE>::getValue(const int &i)
{
    if(i >=0 && i < (int) _vals.size()) 
        return _vals[i];
    return "";
}

/**
 * Set the specified <code>index</code> item name.
 *
 * @param i     configure item index
 * @param name  configure item name
 */
template<size_t LINESIZE> void 
Properties_Base<LINESIZE>::setName(const int &i, const string &name)
{
    if( name == "" ) {
        removeItem(i); return; 
    }
    if(i >=0 && i < (int)_elms.size()) 
        _elms[i] = name;
}

/**
 * Set the specified <code>index</code> item name.
 *
 * @param i     configure item index
 * @param name  configure item name
 */
template<size_t LINESIZE> inline void 
Properties_Base<LINESIZE>::setName(const int &i, const char *name)
{
    setName(i, (string)name); 
}

/**
 * Set the specified <code>index</code> item's <code>value</code>.
 *
 * @param i     configure item index
 * @param value configure item value
 */
template<size_t LINESIZE> void 
Properties_Base<LINESIZE>::setValue(const int &i, const string &value)
{
    if(i >=0 && i < (int) _vals.size()) 
        _vals[i] = value;
}

/**
 * Set the specified <code>index</code> item's <code>value</code>.
 *
 * @param i     configure item index
 * @param value configure item value
 */
template<size_t LINESIZE> inline void 
Properties_Base<LINESIZE>::setValue(const int &i, const char *value)
{
    setValue(i, (string)value); 
}

/**
 * Remove the specified <code>index</code> item.
 *
 * @param i  configure item index
 */
template<size_t LINESIZE> void 
Properties_Base<LINESIZE>::removeItem(const int &i)
{
    if(i >=0 && i < (int) _elms.size() && i < (int) _vals.size()) {
        vector<string>::iterator elms_iter = _elms.begin() + i;
        _elms.erase(elms_iter); 
        vector<string>::iterator vals_iter = _vals.begin() + i;
        _vals.erase(vals_iter); 
        _size --; 
    }
}

/**
 * Get the specified <code>name</code> item's <code>value</code>.
 *
 * @param name  configure item name
 * @param default_value   default item value if not found
 * @return  configure item value
 */
template<size_t LINESIZE> string
Properties_Base<LINESIZE>::get(const string &name, const string &default_value) 
{
    if(name.empty()) 
        return default_value; 
/******************
    vector<string>::const_iterator elm_iter = _elms.begin(), elm_end = _elms.end();
    vector<string>::const_iterator val_iter = _vals.begin(), val_end = _elms.end();
    while(elm_iter != elm_end && val_iter != val_end) {
        if((*elm_iter) == name) 
            return *val_iter; 
        elm_iter ++; 
        val_iter ++; 
    }
********************/
    for( int i = 0; i < (int)_elms.size(); i ++ ) {
        if( getName(i) == name ) 
            return getValue(i); 
    }
    return default_value; 
}

/**
 * Get the specified <code>name</code> item's <code>value</code>.
 *
 * @param name  configure item name
 * @param default_value   default item value if not found
 * @return  configure item value
 */
template<size_t LINESIZE> inline string
Properties_Base<LINESIZE>::get(const char *name, const char *default_value) 
{
    return get((string)name, (string)default_value); 
}

/**
 * Get the specified <code>name</code> item's <code>value</code>.
 *
 * @param name  configure item name
 * @param default_value   default item value if not found
 * @return  configure item value
 */
template<size_t LINESIZE> inline string
Properties_Base<LINESIZE>::getValue(const string &name, const string &default_value) 
{
    return get(name, default_value); 
}

/**
 * Get the specified <code>name</code> item's <code>value</code>.
 *
 * @param name  configure item name
 * @param default_value   default item value if not found
 * @return  configure item value
 */
template<size_t LINESIZE> inline string
Properties_Base<LINESIZE>::getValue(const char *name, const char *default_value) 
{
    return getValue((string)name, (string)default_value); 
}

/**
 * Get the specified <code>name</code> item's <code>intValue</code>.
 *
 * @param name  configure item name
 * @param default_value   default item value if not found
 * @return  configure item value
 */
template<size_t LINESIZE> int
Properties_Base<LINESIZE>::getInt(const string &name, const int default_value) 
{
    FastString value(get(name, "").c_str()); 
    value.trim();
    if( value.empty() ) 
        return default_value; 
    else
        return atoi(value.c_str()); 
}

/**
 * Get the specified <code>name</code> item's <code>intValue</code>.
 *
 * @param name  configure item name
 * @param default_value   default item value if not found
 * @return  configure item value
 */
template<size_t LINESIZE> inline int
Properties_Base<LINESIZE>::getInt(const char *name, const int default_value) 
{
    return getInt((string)name, default_value); 
}

/**
 * Get the specified <code>name</code> item's <code>longValue</code>.
 *
 * @param name  configure item name
 * @param default_value   default item value if not found
 * @return  configure item value
 */
template<size_t LINESIZE> long
Properties_Base<LINESIZE>::getLong(const string &name, const long default_value) 
{
    FastString value(get(name, "").c_str()); 
    value.trim();
    if( value.empty() ) 
        return default_value; 
    else
        return atol(value.c_str()); 
}

/**
 * Get the specified <code>name</code> item's <code>longValue</code>.
 *
 * @param name  configure item name
 * @param default_value   default item value if not found
 * @return  configure item value
 */
template<size_t LINESIZE> inline long
Properties_Base<LINESIZE>::getLong(const char *name, const long default_value) 
{
    return getLong((string)name, default_value); 
}

/**
 * Get the specified <code>name</code> item's <code>floatValue</code>.
 *
 * @param name  configure item name
 * @param default_value   default item value if not found
 * @return  configure item value
 */
template<size_t LINESIZE> double
Properties_Base<LINESIZE>::getFloat(const string &name, const double default_value) 
{
    FastString value(get(name, "").c_str()); 
    value.trim();
    if( value.empty() ) 
        return default_value; 
    else
        return atof(value.c_str()); 
}

/**
 * Get the specified <code>name</code> item's <code>floatValue</code>.
 *
 * @param name  configure item name
 * @param default_value   default item value if not found
 * @return  configure item value
 */
template<size_t LINESIZE> inline double
Properties_Base<LINESIZE>::getFloat(const char *name, const double default_value) 
{
    return getFloat((string)name, default_value); 
}

/**
 * Get the specified <code>name</code> item's <code>boolValue</code>.
 *
 * @param name  configure item name
 * @param default_value   default item value if not found
 * @return  configure item value, TRUE=1 or FALSE=0
 */
template<size_t LINESIZE> int
Properties_Base<LINESIZE>::getBool(const string &name, const int default_value) 
{
    FastString value(get(name).c_str()); 
    value.trim();
    int defaultval = default_value == 0 ? 0 : 1; 
    if( value.empty() ) 
        return defaultval; 
    int bVal = atoi(value.c_str()); 
    if( value.equalsIgnoreCase("yes") || value.equalsIgnoreCase("true") || 
        bVal == 1 || bVal == -1 )
        return 1; 
    else if( value.equalsIgnoreCase("no") || value.equalsIgnoreCase("false") || 
        bVal == 0 )
        return 0; 
    else
        return defaultval; 
}

/**
 * Get the specified <code>name</code> item's <code>boolValue</code>.
 *
 * @param name  configure item name
 * @param default_value   default item value if not found
 * @return  configure item value, TRUE=1 or FALSE=0
 */
template<size_t LINESIZE> inline int
Properties_Base<LINESIZE>::getBool(const char *name, const int default_value) 
{
    return getBool((string)name, default_value); 
}

/**
 * Remove the specified <code>name</code> item.
 *
 * @param name  configure item name
 */
template<size_t LINESIZE> void
Properties_Base<LINESIZE>::remove(const string &name) 
{
    if(name.empty()) return;
/********************
    vector<string>::iterator elm_iter = _elms.begin(), elm_end = _elms.end();
    vector<string>::iterator val_iter = _vals.begin(), val_end = _elms.end();
    while(elm_iter != elm_end && val_iter != val_end) {
        if((*elm_iter) == name) {
            _elms.erase(elm_iter); 
            _vals.erase(val_iter); 
            _size --;
        } 
        elm_iter ++; 
        val_iter ++; 
    }
**********************/
    for( int i = 0; i < (int)_elms.size(); i ++ ) {
        if( getName(i) == name ) {
            removeItem(i); 
            i --; // recheck this again
        }
    }
}

/**
 * Remove the specified <code>name</code> item.
 *
 * @param name  configure item name
 */
template<size_t LINESIZE> inline void
Properties_Base<LINESIZE>::remove(const char *name) 
{
    remove((string)name); 
}

/**
 * Set new <code>value</code> to the specified <code>name</code> item.
 *
 * @param name  configure item name
 * @param value new item value
 */
template<size_t LINESIZE> void
Properties_Base<LINESIZE>::set(const string &name, const string &value) 
{
    if(name.empty()) return;
/******************
    vector<string>::iterator elm_iter = _elms.begin(), elm_end = _elms.end();
    vector<string>::iterator val_iter = _vals.begin(), val_end = _elms.end();
    while(elm_iter != elm_end && val_iter != val_end) {
        if((*elm_iter) == name) 
            *val_iter = value; 
        elm_iter ++; 
        val_iter ++; 
    }
*******************/
    int found = 0; 
    for( int i = 0; i < (int)_elms.size(); i ++ ) {
        if( getName(i) == name ) {
            setValue(i, value); 
            found = 1; 
        }
    }
    if( found == 0 ) 
        add(name, value); 
}

/**
 * Set new <code>value</code> to the specified <code>name</code> item.
 *
 * @param name  configure item name
 * @param value new item value
 */
template<size_t LINESIZE> inline void
Properties_Base<LINESIZE>::set(const char *name, const char *value) 
{
    set((string)name, (string)value); 
}

/**
 * Add new configure item <code>name/value</code>.
 *
 * @param name  new item name
 * @param value new item value
 */
template<size_t LINESIZE> void 
Properties_Base<LINESIZE>::add(const string &name, const string &value) 
{
    if(name.empty()) return; 
    _elms.push_back(name); _vals.push_back(value); 
    _size ++; 
}

/**
 * Add new configure item <code>name/value</code>.
 *
 * @param name  new item name
 * @param value new item value
 */
template<size_t LINESIZE> inline void 
Properties_Base<LINESIZE>::add(const char *name, const char *value) 
{
    add((string)name, (string)value); 
}

/**
 * Save the configure item's <code>name/value</code> to specified file.
 *
 * @param filename  configure file to store items
 */
template<size_t LINESIZE> void 
Properties_Base<LINESIZE>::save(const string &filename)
{
    if(filename.empty()) 
        return;
    ifstream is(_file.c_str()); // load original file
    if( !is ) is.open(filename.c_str()); // second try target file
    
    char ln[LINESIZE+1]; 
    ostrstream sbuf; string str, str2; int exist_flag = 0;
    vector<string> vec; 
    
    memset(ln, 0, sizeof(ln)); 

    if( is ) {
        while(is.getline(ln, LINESIZE)) { 
            str = ln;
            FastString s(ln); 
            s.ltrim(); 
            int pos = s.indexOf('='); 
            if( s.charAt(0) != '#' && pos > 0 ) {
                FastString name, value; 
                s.substring(name, 0, pos); 
                s.substring(value, pos + 1); 
                name.trim(); 
                value.trim(); 
                if( !name.empty() && !(str2=get(name.c_str())).empty()) {
                    exist_flag = 0;
                    for( int i = 0; i < (int)_elms.size(); i ++ ) {
                        if( getName(i) == name.c_str() ) {
                            exist_flag = 1; break; 
                        } 
                    }
                    if( exist_flag == 1 ) {
                        sbuf << name.c_str() << "=" << str2 << endl;
                        vec.push_back(name.c_str()); 
                    }
                }
            }
            else 
                sbuf << str << endl; 
            memset(ln, 0, sizeof(ln)); 
        }
        is.close(); 
    }

/***********************
    vector<string>::iterator elm_iter = _elms.begin(), elm_end = _elms.end();
    vector<string>::iterator val_iter = _vals.begin(), val_end = _elms.end();
    vector<string>::iterator vec_iter, vec_end = vec.end();
    while(elm_iter != elm_end && val_iter != val_end) {
        vec_iter = vec.begin(); exist_flag = false;
        while(vec_iter != vec_end) {
            if((*vec_iter) == (*elm_iter)) {
                exist_flag = true; break; 
            } 
            vec_iter ++; 
        } 
        if(exist_flag == false) 
            sbuf << *elm_iter << "=" << *val_iter << endl; 
        elm_iter ++; val_iter ++; 
    } 
    sbuf << ends;
***********************/
    
    for( int i = 0; i < (int)_elms.size() && i < (int)_vals.size(); i ++ ) {
        exist_flag = 0;
        for( int j = 0; j < (int)vec.size(); j ++ ) {
            if( getName(i) == vec[j] ) {
                exist_flag = 1; break; 
            } 
        } 
        if( exist_flag == 0 ) 
            sbuf << getName(i) << "=" << getValue(i) << endl; 
    } 
    sbuf << ends;
    
    ofstream os(filename.c_str()); 
    if(os) os << sbuf.str();
    os.close(); 
}

/**
 * Save the configure item's <code>name/value</code> to specified file.
 *
 * @param filename  configure file to store items
 */
template<size_t LINESIZE> inline void 
Properties_Base<LINESIZE>::save(const char *filename)
{
    save((string)filename); 
}

/**
 * Save the configure item's <code>name/value</code> to default file.
 *
 */
template<size_t LINESIZE> inline void 
Properties_Base<LINESIZE>::save()
{
    save(_file);
}


#endif

