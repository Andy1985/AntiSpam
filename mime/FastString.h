//=============================================================================
/**
 *  @file    FastString.h
 *
 *  ver 1.0.0 2004/06/03 Naven Exp, for Fast Common Framework.
 *
 *  @author Naven (Navy Chen) 2004/06/03 created.
 */
//=============================================================================

#ifndef _FAST_COMM_FASTSTRING_H
#define _FAST_COMM_FASTSTRING_H

#if defined(_WIN32) || defined(__WIN32__)

#if !defined (NAVEN_PRAGMA_ONCE)
# pragma once
#endif /* NAVEN_PRAGMA_ONCE */

#endif /* _WIN32 */


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FastBase.h"



_FAST_BEGIN_NAMESPACE


// Forward declarations.

template <typename CHAR, SIZET BUFSIZE = 256> 
class FastString_Base; 

typedef class FastString_Base<char, 256>        FastString; 
typedef class FastString_Base<char, 1024>       LongString; 
typedef class FastString_Base<char, 32>         ShortString; 
typedef class FastString_Base<char, 16>         SmallString; 

typedef class FastString_Base<char, 1024>       FastString1024; 
typedef class FastString_Base<char, 512>        FastString512; 
typedef class FastString_Base<char, 256>        FastString256; 
typedef class FastString_Base<char, 128>        FastString128; 
typedef class FastString_Base<char, 64>         FastString64; 
typedef class FastString_Base<char, 32>         FastString32; 
typedef class FastString_Base<char, 16>         FastString16; 
typedef class FastString_Base<char, 8>          FastString8; 

typedef FastString      String; 
typedef FastString1024  String1024; 
typedef FastString512   String512; 
typedef FastString256   String256; 
typedef FastString128   String128; 
typedef FastString64    String64; 
typedef FastString32    String32; 
typedef FastString16    String16; 
typedef FastString8     String8; 



//================class FastString define=================

/**
 * @class FastString
 *
 * @brief This class provides a wrapper facade for C strings.
 *
 * This class uses an included engine to allocate memory, 
 * that stored one line string buffer in chars arrary and bigger
 * string in malloc buffer.
 * NOTE: if an instance of this class is constructed from or
 * assigned an empty string (with first element of '\0'), then it
 * is not allocated new space.  Instead, its internal
 * representation is set equal to a global empty string.
 * CAUTION: in cases when FastString is constructed from a
 * provided buffer with the release parameter set to 0,
 * FastString is not guaranteed to be '\0' terminated.
 *
 * @author Naven
 */

template <typename CHAR, SIZET BUFSIZE> 
class FastString_Base
{
protected:
    CHAR m_psBufLine[BUFSIZE];
    CHAR *m_psBuf;
    CHAR *m_psRep;
    SIZET m_nLen;
    SIZET m_nBufLen;

private:
    void init(); 
    void allocate(const SSIZET size, int reallocate = 0); 
    
    template<typename T> 
    FastString_Base& append_num(const char *fmt, T n) 
    {
        char sbuf[32] = {0}; 
#if defined(_WIN32) || defined(__WIN32__) 
        int len = _snprintf(sbuf, sizeof(sbuf), fmt, n); 
#else // UNIX 
        int len = snprintf(sbuf, sizeof(sbuf), fmt, n); 
#endif
        if( len > 0 ) append(sbuf, len); 
        return *this; 
    }

    template<typename T> 
    FastString_Base& append_num2(const char *fmt, T n, int len, char c)
    {
        char sbuf[32] = {0}; 
#if defined(_WIN32) || defined(__WIN32__) 
        int numlen = _snprintf(sbuf, sizeof(sbuf), fmt, n); 
#else // UNIX 
        int numlen = snprintf(sbuf, sizeof(sbuf), fmt, n); 
#endif
        
        if( len > 0 ) 
        {
            if( numlen >= 0 && numlen < len ) 
            {
                if( !( c > 0x20 && c <= 0x7E) ) 
                    c = ' '; 
                append(c, len - numlen); 
            }
        }
    
        if( numlen > 0 ) append(sbuf, numlen); 
        
        return *this;
    }


public:
    // Traits 
    typedef CHAR char_type; 

    FastString_Base<CHAR, BUFSIZE>();
    FastString_Base<CHAR, BUFSIZE>(const FastString_Base<CHAR, BUFSIZE> &s);
    FastString_Base<CHAR, BUFSIZE>(const CHAR *str);
    FastString_Base<CHAR, BUFSIZE>(const CHAR *str, const SSIZET len);
    FastString_Base<CHAR, BUFSIZE>(const SSIZET len, const CHAR c = '\0');
    virtual ~FastString_Base<CHAR, BUFSIZE>();

    /**
     * Why cannot? I don't know. Naven 20050221
     */
    template<typename CHAR2, SIZET BUFSIZE2> 
    FastString_Base(const FastString_Base<CHAR2, BUFSIZE2> &s) 
     : m_psBufLine(), 
       m_psBuf(NULL),
       m_psRep(NULL),
       m_nLen(0),
       m_nBufLen(0)
    {
        init(); 
        this->set(s.c_str(), s.length()); 
    }

    /**
     * Used to Automatic converte FastString to ShortString and so on.
     */
    template<typename CHAR2, SIZET BUFSIZE2> 
    FastString_Base& operator = (const FastString_Base<CHAR2, BUFSIZE2> &s) 
    {
        if( (void *)this != (void *)&s ) 
            this->set(s.c_str(), s.length()); 
        
        return *this;
    }

    /**
     * Why cannot? I don't know. Naven 20050221
     */
#if defined(_WIN32) || defined(__WIN32__)
    template<typename CHAR2, SIZET BUFSIZE2> 
    operator const FastString_Base<CHAR2, BUFSIZE2> () 
    {
        return FastString_Base<CHAR2, BUFSIZE2>(this->c_str(), this->length());
    }
#endif 

    void set(const CHAR *str, int len); 
    void set(const CHAR *str); 
    FastString_Base<CHAR, BUFSIZE> substring(const SSIZET offset, int length = -1) const;
    void substring(FastString_Base<CHAR, BUFSIZE> &s, const SSIZET offset, int length = -1) const;
    FastString_Base<CHAR, BUFSIZE>& append(const CHAR *str, SSIZET len); 
    BOOL startsWith(const CHAR *prefix, SSIZET len = 0, SSIZET offset = 0, int ignorecase = 0) const; 
    BOOL equals(const CHAR *str, SSIZET len = 0, int ignorecase = 0) const; 
    FastString_Base<CHAR, BUFSIZE>& toLowerCase(); 
    FastString_Base<CHAR, BUFSIZE>& toUpperCase(); 
    FastString_Base<CHAR, BUFSIZE>& replace(
                const FastString_Base<CHAR, BUFSIZE> &name, 
                const FastString_Base<CHAR, BUFSIZE> &value, 
                const int ignorecase = 0); 
    FastString_Base<CHAR, BUFSIZE>& replaces(
                FastString_Base<CHAR, BUFSIZE> *pNames, 
                FastString_Base<CHAR, BUFSIZE> *pValues, 
                int count, const int ignorecase = 0); 
    FastString_Base<CHAR, BUFSIZE>& replaceToken(
                FastString_Base<CHAR, BUFSIZE> &token, 
                FastString_Base<CHAR, BUFSIZE> &value); 
    FastString_Base<CHAR, BUFSIZE>& replaceToken(
                const CHAR *token, 
                const CHAR *value); 
    void getChars(int srcBegin, int srcEnd, char *dst, int dstBegin); 
    CHAR *detach();
    CHAR *clone();
    void setLength(SSIZET len); 
    void resize(SSIZET len, CHAR c); 
    void resize(SSIZET len); 
    void release(); 
    void clear(); 

    SIZET length() const; 
    SIZET size() const; 
    SIZET capacity() const; 
    const CHAR *c_str() const;
    CHAR *rep() const;
    CHAR charAt(const SSIZET i) const;
    
    /**
     * Why cannot? I don't know. Naven 20050221
     */
    //CHAR operator [] (SIZET i) const;
    //operator CHAR * () const; 
    operator const CHAR * () const; 
    //operator void * () const; 
    //operator const void * () const; 
    
    BOOL operator == (const FastString_Base<CHAR, BUFSIZE> &s) const; 
    BOOL operator != (const FastString_Base<CHAR, BUFSIZE> &s) const; 
    void swap(FastString_Base<CHAR, BUFSIZE>& s); 
    FastString_Base<CHAR, BUFSIZE>& replaceIgnoreCase(
                const FastString_Base<CHAR, BUFSIZE> &name, 
                const FastString_Base<CHAR, BUFSIZE> &value); 
    FastString_Base<CHAR, BUFSIZE>& replacesIgnoreCase(
                FastString_Base<CHAR, BUFSIZE> *pNames, 
                FastString_Base<CHAR, BUFSIZE> *pValues, 
                int count); 
    FastString_Base<CHAR, BUFSIZE> substr(const SSIZET offset, int length = -1) const;
    FastString_Base<CHAR, BUFSIZE> &operator = (const FastString_Base<CHAR, BUFSIZE> &s);
    FastString_Base<CHAR, BUFSIZE> &operator = (const CHAR *str);
    FastString_Base<CHAR, BUFSIZE>& append(const FastString_Base<CHAR, BUFSIZE> &s); 
    FastString_Base<CHAR, BUFSIZE>& append(const CHAR *str); 
    FastString_Base<CHAR, BUFSIZE>& append(const CHAR c); 
    FastString_Base<CHAR, BUFSIZE>& append(const CHAR c, int len); 
    
    FastString_Base<CHAR, BUFSIZE>& append2(const int n); 
    FastString_Base<CHAR, BUFSIZE>& append2(const int n, int len, char c = ' '); 
    FastString_Base<CHAR, BUFSIZE>& append2(const unsigned int n); 
    FastString_Base<CHAR, BUFSIZE>& append2(const unsigned int n, int len, char c = ' '); 
    FastString_Base<CHAR, BUFSIZE>& append2(const float f); 
    FastString_Base<CHAR, BUFSIZE>& append2(const float f, int len, char c = ' '); 
    
    FastString_Base<CHAR, BUFSIZE>& append(const long n) 
        { return append_num("%d", n); } 
    FastString_Base<CHAR, BUFSIZE>& append(const unsigned long n) 
        { return append_num("%u", n); } 
    FastString_Base<CHAR, BUFSIZE>& append(const long long n) 
        { return append_num("%ld", n); } 
    FastString_Base<CHAR, BUFSIZE>& append(const unsigned long long n) 
        { return append_num("%lu", n); } 
    FastString_Base<CHAR, BUFSIZE>& append(const double n) 
        { return append_num("%f", n); } 
    FastString_Base<CHAR, BUFSIZE>& append(const long double n) 
        { return append_num("%e", n); } 
    FastString_Base<CHAR, BUFSIZE>& append(const void *n) 
        { return append_num("0x%08X", n); } 
    
    FastString_Base<CHAR, BUFSIZE>& append(const bool n) 
        { return append(static_cast<long>(n)); } 
    FastString_Base<CHAR, BUFSIZE>& append(const short n) 
        { return append(static_cast<long>(n)); } 
    FastString_Base<CHAR, BUFSIZE>& append(const unsigned short n) 
        { return append(static_cast<unsigned long>(n)); } 
    FastString_Base<CHAR, BUFSIZE>& append(const int n) 
        { return append(static_cast<long>(n)); } 
    FastString_Base<CHAR, BUFSIZE>& append(const unsigned int n) 
        { return append(static_cast<unsigned long>(n)); } 
    FastString_Base<CHAR, BUFSIZE>& append(const float n) 
        { return append(static_cast<double>(n)); } 
    
    FastString_Base<CHAR, BUFSIZE>& append(const long n, int len, char c = ' ') 
        { return append_num2("%d", n, len, c); } 
    FastString_Base<CHAR, BUFSIZE>& append(const unsigned long n, int len, char c = ' ') 
        { return append_num2("%u", n, len, c); } 
    FastString_Base<CHAR, BUFSIZE>& append(const long long n, int len, char c = ' ') 
        { return append_num2("%ld", n, len, c); } 
    FastString_Base<CHAR, BUFSIZE>& append(const unsigned long long n, int len, char c = ' ') 
        { return append_num2("%lu", n, len, c); } 
    FastString_Base<CHAR, BUFSIZE>& append(const double n, int len, char c = ' ') 
        { return append_num2("%f", n, len, c); } 
    FastString_Base<CHAR, BUFSIZE>& append(const long double n, int len, char c = ' ') 
        { return append_num2("%e", n, len, c); } 
    FastString_Base<CHAR, BUFSIZE>& append(const void *n, int len, char c) 
        { return append_num2("0x%08X", n, len, c); } 
    
    FastString_Base<CHAR, BUFSIZE>& append(const bool n, int len, char c) 
        { return append(static_cast<long>(n), len, c); } 
    FastString_Base<CHAR, BUFSIZE>& append(const short n, int len, char c = ' ') 
        { return append(static_cast<long>(n), len, c); } 
    FastString_Base<CHAR, BUFSIZE>& append(const unsigned short n, int len, char c = ' ') 
        { return append(static_cast<unsigned long>(n), len, c); } 
    FastString_Base<CHAR, BUFSIZE>& append(const int n, int len, char c = ' ') 
        { return append(static_cast<long>(n), len, c); } 
    FastString_Base<CHAR, BUFSIZE>& append(const unsigned int n, int len, char c = ' ') 
        { return append(static_cast<unsigned long>(n), len, c); } 
    FastString_Base<CHAR, BUFSIZE>& append(const float n, int len, char c = ' ') 
        { return append(static_cast<double>(n), len, c); } 
    
    BOOL endsWith(const CHAR *suffix, SSIZET len = 0) const; 
    BOOL endsWithIgnoreCase(const CHAR *suffix, SSIZET len = 0) const; 
    BOOL startsWithIgnoreCase(const CHAR *prefix, SSIZET len = 0, SSIZET offset = 0) const; 
    BOOL equalsIgnoreCase(const CHAR *str, SSIZET len = 0) const; 
    BOOL equals(const FastString_Base<CHAR, BUFSIZE> &s) const; 
    BOOL equalsIgnoreCase(const FastString_Base<CHAR, BUFSIZE> &s) const; 
    int getline(int pos, FastString_Base<CHAR, BUFSIZE> &s, const CHAR *sep = NULL); 
    BOOL empty() const; 

    FastString_Base<CHAR, BUFSIZE>& trimRight(SSIZET offset); 
    FastString_Base<CHAR, BUFSIZE>& trimLeft(SSIZET offset); 
    FastString_Base<CHAR, BUFSIZE>& removeChar(const CHAR c); 
    FastString_Base<CHAR, BUFSIZE>& removeChars(const CHAR *chrs); 
    FastString_Base<CHAR, BUFSIZE>& replaceChars(const CHAR *chrs, const CHAR chr); 
    FastString_Base<CHAR, BUFSIZE>& rtrimChars(const CHAR *chrs); 
    FastString_Base<CHAR, BUFSIZE>& ltrimChars(const CHAR *chrs); 
    FastString_Base<CHAR, BUFSIZE>& trimChars(const CHAR *chrs); 
    FastString_Base<CHAR, BUFSIZE>& trimRight(); 
    FastString_Base<CHAR, BUFSIZE>& trimLeft(); 
    FastString_Base<CHAR, BUFSIZE>& rtrim(); 
    FastString_Base<CHAR, BUFSIZE>& ltrim(); 
    FastString_Base<CHAR, BUFSIZE>& trim(); 

    SIZET extractValueBehind(
                    FastString_Base<CHAR, BUFSIZE> &name, 
                    FastString_Base<CHAR, BUFSIZE> &value, 
                    FastString_Base<CHAR, BUFSIZE> &token, 
                    int index = 0, int ignorecase = 0);
    SIZET extractValueBehind(
                    const CHAR *name, FastString_Base<CHAR, BUFSIZE> &value, 
                    const CHAR *token, 
                    int index = 0);
    SIZET extractValueBehind(
                    const CHAR *name, FastString_Base<CHAR, BUFSIZE> &value, 
                    int index = 0);
    SIZET extractValueBehindIgnoreCase(
                    FastString_Base<CHAR, BUFSIZE> &name, 
                    FastString_Base<CHAR, BUFSIZE> &value, 
                    FastString_Base<CHAR, BUFSIZE> &token, 
                    int index = 0);
    SIZET extractValueBehindIgnoreCase(
                    const CHAR *name, FastString_Base<CHAR, BUFSIZE> &value, 
                    const CHAR *token, 
                    int index = 0);
    SIZET extractValueBehindIgnoreCase(
                    const CHAR *name, FastString_Base<CHAR, BUFSIZE> &value, 
                    int index = 0);
    SIZET extractValueByName(
                    const CHAR *substr, 
                    const CHAR *div1, const CHAR *div2, 
                    FastString_Base<CHAR, BUFSIZE> &value, 
                    const CHAR *vdiv, 
                    const int count = 1);
    SIZET extractIntegerByName(
                    const CHAR *substr, 
                    long *value, 
                    const long defval = 0); 
    SIZET extractValueByName(
                    const CHAR *substr, 
                    FastString_Base<CHAR, BUFSIZE> &value, 
                    const int count = 1); 
    SIZET extractValueByIndex(
                    FastString_Base<CHAR, BUFSIZE> &value, 
                    const CHAR tag, 
                    const int index); 
    int indexOf(    CHAR target, SSIZET fromIndex = 0); 
    int indexOf(    const CHAR *target, SSIZET targetCount,
                    SSIZET fromIndex); 
    int indexOf(    const CHAR *target,
                    SSIZET fromIndex = 0); 
    int indexOf(    FastString_Base<CHAR, BUFSIZE> &target,
                    SSIZET fromIndex = 0); 
    int indexOfIgnoreCase(CHAR target, SSIZET fromIndex = 0); 
    int indexOfIgnoreCase(
                    const CHAR *target, SSIZET targetCount,
                    SSIZET fromIndex); 
    int indexOfIgnoreCase(
                    const CHAR *target, 
                    SSIZET fromIndex = 0); 
    int indexOfIgnoreCase(
                    FastString_Base<CHAR, BUFSIZE> &target, 
                    SSIZET fromIndex = 0); 
    int lastIndexOf(CHAR target, SSIZET fromIndex = 0); 
    int lastIndexOf(const CHAR *target, SSIZET targetCount,
                    SSIZET fromIndex); 
    int lastIndexOf(const CHAR *target, 
                    SSIZET fromIndex = 0); 
    int lastIndexOf(FastString_Base<CHAR, BUFSIZE> &target, 
                    SSIZET fromIndex = 0); 
    int lastIndexOfIgnoreCase(CHAR target, SSIZET fromIndex = 0); 
    int lastIndexOfIgnoreCase(
                    const CHAR *target, SSIZET targetCount,
                    SSIZET fromIndex); 
    int lastIndexOfIgnoreCase(
                    const CHAR *target, 
                    SSIZET fromIndex = 0); 
    int lastIndexOfIgnoreCase(
                    FastString_Base<CHAR, BUFSIZE> &target, 
                    SSIZET fromIndex = 0); 
    int countOf(    CHAR target, SSIZET fromIndex = 0); 
    int countOf(    const CHAR *target, SSIZET targetCount,
                    SSIZET fromIndex, 
                    int ignorecase = 0); 
    int countOf(    const CHAR *target,
                    SSIZET fromIndex = 0); 
    int countOf(    FastString_Base<CHAR, BUFSIZE> &target,
                    SSIZET fromIndex = 0); 
    int countOfIgnoreCase(CHAR target, SSIZET fromIndex = 0); 
    int countOfIgnoreCase(
                    const CHAR *target, SSIZET targetCount,
                    SSIZET fromIndex); 
    int countOfIgnoreCase(
                    const CHAR *target, 
                    SSIZET fromIndex = 0); 
    int countOfIgnoreCase(
                    FastString_Base<CHAR, BUFSIZE> &target, 
                    SSIZET fromIndex = 0); 
    int indexOfAny( const CHAR *target, SSIZET targetCount,
                    SSIZET fromIndex); 
    int indexOfAny( const CHAR *target,
                    SSIZET fromIndex = 0); 
    int indexOfAny( FastString_Base<CHAR, BUFSIZE> &target,
                    SSIZET fromIndex = 0); 

    template<typename CHAR2, SIZET BUFSIZE2> 
    FastString_Base& append(const FastString_Base<CHAR2, BUFSIZE2> &s) 
    {
        return append(s.c_str(), s.length()); 
    }

    template<typename CHAR2, SIZET BUFSIZE2> 
    BOOL endsWith(const FastString_Base<CHAR2, BUFSIZE2> &suffix, SSIZET len = 0) const 
    {
        return endsWith(suffix.c_str(), len); 
    }
    
    template<typename CHAR2, SIZET BUFSIZE2> 
    BOOL endsWithIgnoreCase(const FastString_Base<CHAR2, BUFSIZE2> &suffix, SSIZET len = 0) const 
    {
        return endsWithIgnoreCase(suffix.c_str(), len); 
    }
    
    template<typename CHAR2, SIZET BUFSIZE2> 
    BOOL startsWith(const FastString_Base<CHAR2, BUFSIZE2> &suffix, SSIZET len = 0) const 
    {
        return startsWith(suffix.c_str(), len); 
    }
    
    template<typename CHAR2, SIZET BUFSIZE2> 
    BOOL startsWithIgnoreCase(const FastString_Base<CHAR2, BUFSIZE2> &prefix, 
                              SSIZET len = 0, SSIZET offset = 0) const 
    {
        return startsWithIgnoreCase(prefix.c_str(), len, offset); 
    }
    
    template<typename CHAR2, SIZET BUFSIZE2> 
    BOOL equalsIgnoreCase(const FastString_Base<CHAR2, BUFSIZE2> &str, SSIZET len = 0) const 
    {
        return equalsIgnoreCase(str.c_str(), len); 
    }
    
    template<typename CHAR2, SIZET BUFSIZE2> 
    BOOL equals(const FastString_Base<CHAR2, BUFSIZE2> &s) const 
    {
        return equals(s.c_str(), s.length()); 
    }

    template<typename CHAR2, SIZET BUFSIZE2> 
    int indexOf(const FastString_Base<CHAR2, BUFSIZE2> &target, 
                SSIZET fromIndex = 0) 
    {
        return indexOf(target.c_str(), target.length(), fromIndex); 
    }

    template<typename CHAR2, SIZET BUFSIZE2> 
    int indexOfIgnoreCase(const FastString_Base<CHAR2, BUFSIZE2> &target, 
                          SSIZET fromIndex = 0) 
    {
        return indexOfIgnoreCase(target.c_str(), target.length(), fromIndex); 
    }

    template<typename CHAR2, SIZET BUFSIZE2> 
    int lastIndexOf(const FastString_Base<CHAR2, BUFSIZE2> &target, 
                    SSIZET fromIndex = 0) 
    {
        return lastIndexOf(target.c_str(), target.length(), fromIndex); 
    }

    template<typename CHAR2, SIZET BUFSIZE2> 
    int lastIndexOfIgnoreCase(const FastString_Base<CHAR2, BUFSIZE2> &target, 
                              SSIZET fromIndex = 0) 
    {
        return lastIndexOfIgnoreCase(target.c_str(), target.length(), fromIndex); 
    }

    template<typename CHAR2, SIZET BUFSIZE2> 
    int countOf(const FastString_Base<CHAR2, BUFSIZE2> &target, 
                SSIZET fromIndex = 0) 
    {
        return countOf(target.c_str(), target.length(), fromIndex); 
    }

    template<typename CHAR2, SIZET BUFSIZE2> 
    int countOfIgnoreCase(const FastString_Base<CHAR2, BUFSIZE2> &target, 
                          SSIZET fromIndex = 0) 
    {
        return countOfIgnoreCase(target.c_str(), target.length(), fromIndex); 
    }

    template<typename CHAR2, SIZET BUFSIZE2> 
    int indexOfAny(const FastString_Base<CHAR2, BUFSIZE2> &target, 
                   SSIZET fromIndex = 0) 
    {
        return indexOfAny(target.c_str(), target.length(), fromIndex); 
    }

    template<typename CHAR2, SIZET BUFSIZE2> 
    FastString_Base& removeChars(const FastString_Base<CHAR2, BUFSIZE2> &chrs) 
    {
        return removeChars(chrs.c_str()); 
    }
    
    template<typename CHAR2, SIZET BUFSIZE2> 
    FastString_Base& replaceChars(const FastString_Base<CHAR2, BUFSIZE2> &chrs, 
                                  const CHAR chr) 
    {
        return replaceChars(chrs.c_str(), chr); 
    }
    
    template<typename CHAR2, SIZET BUFSIZE2> 
    FastString_Base& rtrimChars(const FastString_Base<CHAR2, BUFSIZE2> &chrs) 
    {
        return rtrimChars(chrs.c_str()); 
    }
    
    template<typename CHAR2, SIZET BUFSIZE2> 
    FastString_Base& ltrimChars(const FastString_Base<CHAR2, BUFSIZE2> &chrs) 
    {
        return ltrimChars(chrs.c_str()); 
    }
    
    template<typename CHAR2, SIZET BUFSIZE2> 
    FastString_Base& trimChars(const FastString_Base<CHAR2, BUFSIZE2> &chrs) 
    {
        return trimChars(chrs.c_str()); 
    }


    int compareTo(const CHAR *str, int len, int offset1, int offset2); 
    
    template<typename CHAR2, SIZET BUFSIZE2> 
    int compareTo(const FastString_Base<CHAR2, BUFSIZE2> &chrs, int offset1 = 0, int offset2 = 0) 
    {
        return compareTo(chrs.c_str(), (int)chrs.length(), offset1, offset2); 
    }


private:
    static CHAR* removechars(CHAR *s, const CHAR *chrs); 
    static CHAR* replacechars(CHAR *s, const CHAR *chrs, const CHAR chr); 
    static CHAR* rtrimchars(CHAR *s, const CHAR *chrs); 
    static CHAR* ltrimchars(CHAR *s, const CHAR *chrs); 
    static CHAR* trimchars(CHAR *s, const CHAR *chrs); 
    static CHAR* rtrim(CHAR *s); 
    static CHAR* ltrim(CHAR *s); 
    static CHAR* trim(CHAR *s); 

public: 
    static CHAR* nextline(CHAR *sbuf, const int size, 
                    CHAR **pptr, CHAR **ppchr, CHAR *chr); 
    static SIZET extractValueByName(
                    const CHAR *str, const CHAR *substr, 
                    const CHAR *div1, const CHAR *div2, 
                    CHAR *buf, const SIZET size, 
                    const CHAR *vdiv, 
                    const int count = 1);
    static SIZET extractIntegerByName(
                    const CHAR *str, const CHAR *substr, 
                    long *value, 
                    const long defval = 0); 
    static SIZET extractValueByName(
                    const CHAR *str, const CHAR *substr, 
                    CHAR *value, const SSIZET size, 
                    const int count=1); 
    static SIZET extractValueByIndex(
                    const CHAR *pstr, const SSIZET len, 
                    CHAR *value, const SSIZET valsize, 
                    const CHAR tag, 
                    const int index); 
    static int findIndexOf(
                    const CHAR *source, SSIZET sourceCount, 
                    const CHAR *target, SSIZET targetCount,
                    SSIZET fromIndex = 0, 
                    const int ignorecase = 0); 
    static int findIndexOf(
                    const CHAR *source, const CHAR *target,
                    SSIZET fromIndex = 0); 
    static int findIndexOfIgnoreCase(
                    const CHAR *source, SSIZET sourceCount, 
                    const CHAR *target, SSIZET targetCount,
                    SSIZET fromIndex = 0); 
    static int findIndexOfIgnoreCase(
                    const CHAR *source, const CHAR *target, 
                    SSIZET fromIndex = 0); 
    static int findLastIndexOf(
                    const CHAR *source, SSIZET sourceCount, 
                    const CHAR *target, SSIZET targetCount,
                    SSIZET fromIndex = 0, 
                    const int ignorecase = 0); 
    static int findLastIndexOf(
                    const CHAR *source, const CHAR *target, 
                    SSIZET fromIndex = 0); 
    static int findLastIndexOfIgnoreCase(
                    const CHAR *source, SSIZET sourceCount, 
                    const CHAR *target, SSIZET targetCount,
                    SSIZET fromIndex = 0); 
    static int findLastIndexOfIgnoreCase(
                    const CHAR *source, const CHAR *target, 
                    SSIZET fromIndex = 0); 

};


//=======FastString member functions===============

/**
 * Allocate memory buffer to store string. If the size < 1024, 
 * it will not malloc and use the m_psBuffLine[1025] to increase
 * performance, and if reallocate it reserve the original string. 
 *
 * @param size  new buffer size
 * @param reallocate  default 0 allocate new buffer or 
 *      else realloc memory and reserve data
 */
template <typename CHAR, SIZET BUFSIZE> 
void FastString_Base<CHAR, BUFSIZE>::allocate(const SSIZET size, int reallocate) 
{
    CHAR *ptr; 
    
    if( size < 0 ) 
        return; 

    if( size >= (SSIZET)this->m_nBufLen ) 
    {
        if( this->m_psBuf ) 
        {
            this->m_nBufLen += sizeof(this->m_psBufLine) * 2;
            if( size >= (SSIZET)this->m_nBufLen ) 
                this->m_nBufLen = size + 1; 

            // 上调至 8 的倍数 
            SIZET alloc_size = _FAST::fast_round_up(this->m_nBufLen * sizeof(CHAR)); 

            this->m_psBuf = (CHAR *) ::realloc(this->m_psBuf, alloc_size); 
        }
        else
        {
            this->m_nBufLen = sizeof(this->m_psBufLine) * 2 + 1;
            if( size >= (SSIZET)this->m_nBufLen ) 
                this->m_nBufLen = size + 1;

            // 上调至 8 的倍数 
            SIZET alloc_size = _FAST::fast_round_up(this->m_nBufLen * sizeof(CHAR)); 

            this->m_psBuf = (CHAR *) ::malloc(alloc_size); 

            if( this->m_nLen > 0 && reallocate ) 
                memcpy(this->m_psBuf, this->m_psRep, this->m_nLen * sizeof(CHAR)); 

            this->m_psBufLine[0] = '\0'; 
        }
        this->m_psRep = this->m_psBuf; 
    }
    else
    {
        if( size >= (SSIZET)sizeof(this->m_psBufLine) ) 
        {
            this->m_nBufLen = _FAST::fast_round_up(size + 1);

            // 上调至 8 的倍数 
            SIZET alloc_size = _FAST::fast_round_up(this->m_nBufLen * sizeof(CHAR));

            ptr = (CHAR *) ::malloc(alloc_size); 

            this->m_nLen = (SSIZET)this->m_nLen <= size ? this->m_nLen : size; 

            if( this->m_nLen > 0 && reallocate ) 
                memcpy(ptr, this->m_psRep, this->m_nLen * sizeof(CHAR)); 

            if( this->m_psBuf ) 
            {
                free(this->m_psBuf); 
                this->m_psBuf = NULL;
            }

            this->m_psRep = this->m_psBuf = ptr; 
            ptr = NULL; 

            this->m_psBufLine[0] = '\0'; 
        }
        else if( this->m_psRep != this->m_psBufLine ) 
        {
            this->m_nLen = (SSIZET)this->m_nLen <= size ? this->m_nLen : size; 

            if( this->m_nLen > 0 && reallocate ) 
                memcpy(this->m_psBufLine, this->m_psRep, this->m_nLen * sizeof(CHAR)); 

            if( this->m_psBuf ) 
            {
                free(this->m_psBuf); 
                this->m_psBuf = NULL;
            }

            this->m_psRep = this->m_psBufLine; 
            this->m_nBufLen = sizeof(this->m_psBufLine); 
        }
        else 
        {
            this->m_nLen = (SSIZET)this->m_nLen <= size ? this->m_nLen : size; 
        }
    }

    if( !this->m_psRep ) 
    {
        if( this->m_psBuf ) 
        {
            free(this->m_psBuf); 
            this->m_psBuf = NULL;
        }
        this->m_psRep = this->m_psBufLine; 
        this->m_nBufLen = sizeof(this->m_psBufLine); 
    }

    if( !reallocate ) 
        this->m_nLen = 0; 
    this->m_psRep[this->m_nLen] = '\0'; 
}

/**
 * Copy @a len bytes of @a s (will zero terminate the result).
 *
 * @param str  Non-zero terminated input string
 * @param len  Length of input string 'str'
 */
template <typename CHAR, SIZET BUFSIZE> 
void FastString_Base<CHAR, BUFSIZE>::set(const CHAR *str, int len)
{
    if( str && len > 0 )
    {
        // allocate memory if needed.
        this->allocate(len);

        // only copy buffer length data.
        this->m_nLen = len <= (int) this->m_nBufLen ? len : this->m_nBufLen; 

        memcpy(this->m_psRep, str, this->m_nLen * sizeof(CHAR));
        this->m_psRep[this->m_nLen] = '\0';
    }
    else 
    {
        if( !this->m_psRep ) 
        {
            if( this->m_psBuf ) 
            {
                free(this->m_psBuf); 
                this->m_psBuf = NULL;
            }
            this->m_psRep = this->m_psBufLine; 
            this->m_nBufLen = sizeof(this->m_psBufLine); 
        }
        this->m_nLen = 0; 
        this->m_psRep[this->m_nLen] = '\0';
    }
}

/**
 * Return a substring given an offset and length, if length == 0
 * use rest of str.  Return empty substring if offset or
 * offset/length are invalid.
 *
 * @param offset  Index of first desired character of the substring.
 * @param length  How many characters to return starting at the offset.
 * @return The string containing the desired substring
 */
template <typename CHAR, SIZET BUFSIZE>  
inline FastString_Base<CHAR, BUFSIZE> 
FastString_Base<CHAR, BUFSIZE>::substring(const SSIZET offset, int length) const
{
    FastString_Base<CHAR, BUFSIZE> result;
    this->substring(result, offset, length); 
    return result; 
}

/**
 * Return a substring given an offset and length, if length == 0
 * use rest of str.  Return empty substring if offset or
 * offset/length are invalid.
 *
 * @param s       The string containing the desired substring
 * @param offset  Index of first desired character of the substring.
 * @param length  How many characters to return starting at the offset.
 */
template <typename CHAR, SIZET BUFSIZE>  
void FastString_Base<CHAR, BUFSIZE>::substring(
                    FastString_Base<CHAR, BUFSIZE> &s, 
                    const SSIZET offset, int length) const
{
    SIZET count = length;
    
    s.clear(); 

    // case 1. empty string
    if( this->m_nLen <= 0 )
        return;
    
    // case 2. start pos past our end == empty string.
    if( offset < 0 || offset >= (SSIZET)this->m_nLen )
        return;

    // case 3. length past our end or no length == empty string.
    if( length == 0 || length < -1 )
        return;

    // Get all remaining bytes.
    if( length == -1 || count > (this->m_nLen - offset) )
        count = this->m_nLen - offset;

    s.append(&this->m_psRep[offset], count);
}

/**
 * Appends the string to this string buffer. 
 * This string allocate memory by itself to store string.
 *
 * @param str  a string.
 * @param len  the string length.
 * @return  a reference to this <code>FastString</code>.
 */
template <typename CHAR, SIZET BUFSIZE> 
FastString_Base<CHAR, BUFSIZE>& 
FastString_Base<CHAR, BUFSIZE>::append(const CHAR *str, SSIZET len)
{
    if( str && len > 0 )
    {
        SIZET new_buf_len = this->m_nLen + len + 1;

        // case 1. No memory allocation needed.
        if( this->m_nBufLen >= new_buf_len )
        {

        }
        // case 2. Memory reallocation is needed
        else
        {
            this->allocate(new_buf_len, 1); 

            if( this->m_nBufLen < new_buf_len ) 
                return *this; 
        }

        // Copy in data from new string.
        memcpy(this->m_psRep + this->m_nLen,
                    str,
                    len * sizeof(CHAR) );

        this->m_nLen += len;
        this->m_psRep[this->m_nLen] = '\0';
    }

    return *this;
}

/**
 * Appends <code>len</code> number of CHAR to this string buffer. 
 * This string allocate memory by itself to store string.
 *
 * @param c     Input char.
 * @param len   number of char to append
 * @return  a reference to this <code>FastString</code>.
 */
template <typename CHAR, SIZET BUFSIZE> 
FastString_Base<CHAR, BUFSIZE>& 
FastString_Base<CHAR, BUFSIZE>::append(const CHAR c, int len)
{
    if( len > 0 )
    {
        SIZET new_buf_len = this->m_nLen + len + 1;

        // case 1. No memory allocation needed.
        if( this->m_nBufLen >= new_buf_len )
        {

        }
        // case 2. Memory reallocation is needed
        else
        {
            this->allocate(new_buf_len, 1); 

            if( this->m_nBufLen < new_buf_len ) 
                return *this; 
        }

        // Set number of c in data from new string.
        ::memset(this->m_psRep + this->m_nLen,
                    c,
                    len * sizeof(CHAR) );

        this->m_nLen += len;
        this->m_psRep[this->m_nLen] = '\0';
    }

    return *this;
}

/**
 * This method is designed for high-performance. Please use with
 * care ;-) Current FastString will detach with the buffer, 
 * You must free the buffer outside.
 *
 * @return  The chars point to buffer
 */
template <typename CHAR, SIZET BUFSIZE> 
CHAR * FastString_Base<CHAR, BUFSIZE>::detach() 
{
    CHAR *rep = this->m_psRep; 

    if( this->m_nLen <= 0 ) 
        return NULL; 

    if( this->m_psRep == this->m_psBufLine ) 
    {
        // 上调至 8 的倍数
        SIZET alloc_size = _FAST::fast_round_up((this->m_nLen + 1) * sizeof(CHAR)); 
        rep = (CHAR *) ::malloc(alloc_size); 

        memcpy(rep, this->m_psRep, this->m_nLen * sizeof(CHAR)); 
        rep[this->m_nLen] = '\0'; 

        if( this->m_psBuf ) 
            free(this->m_psBuf); 
    }

    this->m_psBuf = NULL;
    this->m_psRep = this->m_psBufLine; 
    this->m_nBufLen = sizeof(this->m_psBufLine); 
    
    this->m_nLen = 0; 
    this->m_psRep[this->m_nLen] = '\0'; 

    return rep; 
}

/**
 * This method is designed for high-performance. Please use with
 * care ;-) Current FastString will copy data to new buffer, 
 * You must free the buffer outside.
 *
 * @return  The chars point to copy buffer
 */
template <typename CHAR, SIZET BUFSIZE> 
CHAR * FastString_Base<CHAR, BUFSIZE>::clone() 
{
    CHAR *rep = NULL; 

    if( this->m_nLen <= 0 ) 
        return NULL; 

    if( this->m_nLen > 0 ) 
    {
        // 上调至 8 的倍数
        SIZET alloc_size = _FAST::fast_round_up((this->m_nLen + 1) * sizeof(CHAR)); 
        rep = (CHAR *) ::malloc(alloc_size); 

        memcpy(rep, this->m_psRep, this->m_nLen * sizeof(CHAR)); 
        rep[this->m_nLen] = '\0'; 
    }

    return rep; 
}

/**
 * This method is designed for high-performance. Please use with
 * care ;-) If the current size of the string is less than <len>,
 * the string is resized to the new length. The data is zero'd
 * out after this operation.
 *
 * @param len  New string size
 * @param c    New input string
 */
template <typename CHAR, SIZET BUFSIZE> 
void FastString_Base<CHAR, BUFSIZE>::resize(SSIZET len, CHAR c)
{
    if( len <= 0 ) 
    {
        clear(); 
        return; 
    }

    this->allocate(len); 

    this->m_nLen = len;
    memset(this->m_psRep, c, this->m_nLen * sizeof(CHAR));
    if( c == '\0' ) 
        this->m_nLen = 0; 
    this->m_psRep[this->m_nLen] = '\0'; 
}

/**
 * Tests if this string starts with the specified prefix beginning
 * a specified index.
 *
 * @param   prefix    the prefix.
 * @param   len       the prefix's length.
 * @param   offset    where to begin looking in the string.
 * @param   ignorecase   compare ignore case or not.
 * @return  <code>true</code> if the character sequence represented by the
 *          argument is a prefix of the substring of this object starting
 *          at index <code>toffset</code>; <code>false</code> otherwise.
 *          The result is <code>false</code> if <code>toffset</code> is
 *          negative or greater than the length of this
 *          <code>String</code> object; otherwise the result is the same
 *          as the result of the expression
 *          <pre>
 *          this.subString(toffset).startsWith(prefix)
 *          </pre>
 */
template <typename CHAR, SIZET BUFSIZE> 
BOOL FastString_Base<CHAR, BUFSIZE>::startsWith(
            const CHAR *prefix, SSIZET len, SSIZET offset, 
            int ignorecase) const
{
    CHAR *ta = this->m_psRep;
    int to = offset;
    CHAR *pa = (CHAR *) prefix;
    int po = 0, pc = 0;

    if( prefix == NULL ) 
        return FALSE; 

    pc = (len <= 0) ? strlen(prefix) : len; 

    // Note: toffset might be near -1>>>1.
    if( offset < 0 || offset > (SSIZET)(this->m_nLen - pc) ) 
        return FALSE;
    
    if( ignorecase ) 
    {
        while( --pc >= 0 ) 
        {
            if( tolower(ta[to++]) != tolower(pa[po++]) ) 
                return FALSE;
            if( ta[to] == '\0' ) 
                return TRUE; 
        }
    }
    else
    {
        while( --pc >= 0 ) 
        {
            if( ta[to++] != pa[po++] ) 
                return FALSE;
            if( ta[to] == '\0' ) 
                return TRUE; 
        }
    }

    return TRUE;
}

/**
 * Compares this <code>String</code> to another <code>String</code>,
 * ignoring case considerations.  Two strings are considered equal
 * ignoring case if they are of the same length, and corresponding
 * characters in the two strings are equal ignoring case.
 * <p>
 * Two characters <code>c1</code> and <code>c2</code> are considered
 * the same, ignoring case if at least one of the following is true:
 * <ul><li>The two characters are the same (as compared by the
 * <code>==</code> operator).
 * <li>Applying the method {@link java.lang.Character#toUpperCase(char)}
 * to each character produces the same result.
 * <li>Applying the method {@link java.lang.Character#toLowerCase(char)}
 * to each character produces the same result.</ul>
 *
 * @param   str   the <code>String</code> to compare this
 *                          <code>String</code> against.
 * @param   len   compare length of the string.
 * @param   ignorecase   compare ignore case or not.
 * @return  <code>true</code> if the argument is not <code>null</code>
 *          and the <code>String</code>s are equal,
 *          ignoring case; <code>false</code> otherwise.
 */
template <typename CHAR, SIZET BUFSIZE> 
BOOL FastString_Base<CHAR, BUFSIZE>::equals(
            const CHAR *str, SSIZET len, int ignorecase) const
{
    CHAR *pbuf = this->m_psRep; 
    CHAR *pstr = (CHAR *) str; 

    if( !str && this->m_nLen == 0 ) 
        return TRUE; 

    if( !str || this->m_nLen <= 0 ) 
        return FALSE; 

    len = (len <= 0) ? strlen(str) : len; 

    if( len != (SSIZET) this->m_nLen ) 
        return FALSE; 

    if( ignorecase ) 
    {
        while( *pbuf && *pstr && --len >= 0 ) 
        {
            if( tolower(*pbuf) != tolower(*pstr) ) 
                return FALSE; 
            pbuf ++; 
            pstr ++; 
        }
    }
    else
    {
        return memcmp(this->m_psRep, 
                    str, 
                    len * sizeof(CHAR)) == 0 ? TRUE : FALSE; 
    }

    return TRUE; 
}

/**
 * Compares two strings lexicographically.
 * The comparison is based on the Unicode value of each character in
 * the strings. The character sequence represented by this
 * <code>String</code> object is compared lexicographically to the
 * character sequence represented by the argument string. The result is
 * a negative integer if this <code>String</code> object
 * lexicographically precedes the argument string. The result is a
 * positive integer if this <code>String</code> object lexicographically
 * follows the argument string. The result is zero if the strings
 * are equal; <code>compareTo</code> returns <code>0</code> exactly when
 * the {@link #equals(Object)} method would return <code>true</code>.
 * <p>
 * This is the definition of lexicographic ordering. If two strings are
 * different, then either they have different characters at some index
 * that is a valid index for both strings, or their lengths are different,
 * or both. If they have different characters at one or more index
 * positions, let <i>k</i> be the smallest such index; then the string
 * whose character at position <i>k</i> has the smaller value, as
 * determined by using the &lt; operator, lexicographically precedes the
 * other string. In this case, <code>compareTo</code> returns the
 * difference of the two character values at position <code>k</code> in
 * the two string -- that is, the value:
 * <blockquote><pre>
 * this.charAt(k)-anotherString.charAt(k)
 * </pre></blockquote>
 * If there is no index position at which they differ, then the shorter
 * string lexicographically precedes the longer string. In this case,
 * <code>compareTo</code> returns the difference of the lengths of the
 * strings -- that is, the value:
 * <blockquote><pre>
 * this.length()-anotherString.length()
 * </pre></blockquote>
 *
 * @param   str   the input <code>String</code> to be compared.
 * @param   len   the input <code>String</code> length.
 * @param   offset2   the input <code>String</code> offset to be compared.
 * @param   offset1   the current <code>String</code> offset to be compared.
 * @return  the value <code>0</code> if the argument string is equal to
 *          this string; a value less than <code>0</code> if this string
 *          is lexicographically less than the string argument; and a
 *          value greater than <code>0</code> if this string is
 *          lexicographically greater than the string argument.
 */
template <typename CHAR, SIZET BUFSIZE> int 
FastString_Base<CHAR, BUFSIZE>::compareTo(const CHAR *str, int len, 
                                          int offset2 = 0, 
                                          int offset1 = 0) 
{
    int len1 = length(); 
    int len2 = len; 
    int n = fast_min(len1, len2); 
    
    if( n >= 0 & str != NULL ) 
    {
        CHAR *v1 = (CHAR *) c_str();
        CHAR *v2 = (CHAR *) str;
        int i = offset1;
        int j = offset2; 
        
        if( i < 0 || i >= len1 ) 
            i = 0; 
        if( j < 0 || j >= len2 ) 
            j = 0; 
        
        if( i == j ) 
        {
            int k = i; 
            int lim = n + i; 
            while( k < lim ) 
            {
                CHAR c1 = v1[k]; 
                CHAR c2 = v2[k]; 
                if( c1 != c2 ) {
                    return c1 - c2;
                }
                k ++;
            }
        } 
        else 
        {
            while( n-- != 0 ) 
            {
                char c1 = v1[i++];
                char c2 = v2[j++];
                if( c1 != c2 ) {
                    return c1 - c2;
                }
            }
        }
    }
    
    return len1 - len2;
}

/**
 * Converts all of the characters in this string to lower case. 
 * <p>
 * @return  a reference to this <code>FastString</code>.
 */
template <typename CHAR, SIZET BUFSIZE> 
FastString_Base<CHAR, BUFSIZE>& 
FastString_Base<CHAR, BUFSIZE>::toLowerCase() 
{
    SIZET i = 0; 

    while( i < this->m_nLen ) 
    {
        this->m_psRep[i] = tolower(this->m_psRep[i]); 
        i ++; 
    }

    return *this; 
}


/**
 * Converts all of the characters in this string to upper case. 
 * <p>
 * @return  a reference to this <code>FastString</code>.
 */
template <typename CHAR, SIZET BUFSIZE> 
FastString_Base<CHAR, BUFSIZE>& 
FastString_Base<CHAR, BUFSIZE>::toUpperCase() 
{
    SIZET i = 0; 

    while( i < this->m_nLen ) 
    {
        this->m_psRep[i] = toupper(this->m_psRep[i]); 
        i ++; 
    }

    return *this; 
}

/**
 * replace substring <code>name</code> in a string with <code>value</code>.
 * 替换字符串缓冲数据中为name的子字符串为新字符串value.
 *
 * @param name    被替换的字符串
 * @param value   新的字符串
 * @param ignorecase   是否忽略大小写 0 不 1是
 * @return        替换后的字符串缓冲数据
 */
template <typename CHAR, SIZET BUFSIZE> 
FastString_Base<CHAR, BUFSIZE>& 
FastString_Base<CHAR, BUFSIZE>::replace(
            const FastString_Base<CHAR, BUFSIZE> &name, 
            const FastString_Base<CHAR, BUFSIZE> &value, 
            const int ignorecase) 
{
    FastString_Base<CHAR, BUFSIZE> result; 
    SIZET i=0, j=0, k=0, oldlen=0; 

    if( this->m_nLen <= 0 ) 
        return *this;
    if( name.empty() || name.equals(value) ) 
        return *this;

    result.resize(this->m_nLen); 
    oldlen = name.length() - 1;

    for( i = 0; i < this->m_nLen; i++ ) 
    {
        if( this->m_psRep[i] == name.charAt(j) || 
          ( ignorecase && tolower(this->m_psRep[i]) == tolower(name.charAt(j)) ) ) 
        {
            if( j >= oldlen ) 
            {
                //result.append(this->substring(k, i-oldlen-k)); 
                result.append(this->m_psRep+k, i-oldlen-k); 
                result.append(value); 
                k = i + 1; j = 0;
            }
            else
                j ++;
        }
        else if( this->m_psRep[i] == name.charAt(0) || 
               ( ignorecase && tolower(this->m_psRep[i]) == tolower(name.charAt(0)) )) 
            j = 1;
        else
            j = 0;
    }
    result.append(this->substring(k)); 

    this->release(); 
    this->append(result); 

    return *this;
}

/**
 * replace substrings <code>name</code> in a string with <code>values</code>.
 * 替换字符串缓冲数据中为names数组的子字符串第n个为新字符串values数组的第n个.
 *
 * @param names   被替换的字符串数组
 * @param values  新的字符串数组
 * @param ignorecase   是否忽略大小写 0 不 1是
 * @return        替换后的字符串缓冲数据
 */
template <typename CHAR, SIZET BUFSIZE> 
FastString_Base<CHAR, BUFSIZE>& 
FastString_Base<CHAR, BUFSIZE>::replaces(
            FastString_Base<CHAR, BUFSIZE> *pNames, 
            FastString_Base<CHAR, BUFSIZE> *pValues, 
            int count, const int ignorecase) 
{
    FastString_Base<CHAR, BUFSIZE> *pName, *pValue; 

    if( pNames == NULL || pValues == NULL || count <= 0 ) 
        return *this; 

    pName = pNames; pValue = pValues; 

    while( pName && pValue && count > 0 ) {
        this->replace(*pName, *pValue, ignorecase); 
        pName ++; pValue ++; 
        count --; 
    }

    return *this; 
}

/**
 * Get one line in input string <code>s</code>'s from a position.
 * Each line separated by specify characters, default "\r\n". 
 *
 * @param pos     current line from positon
 * @param s       current line string
 * @param sep     separate characters
 * @return        next line position
 */
template <typename CHAR, SIZET BUFSIZE> 
int FastString_Base<CHAR, BUFSIZE>::getline(
        int pos, FastString_Base<CHAR, BUFSIZE> &s, const CHAR *sep) 
{
    CHAR dsep[] = "\r\n", *psep = (CHAR *) sep; 
    CHAR *ptr = NULL, *pbuf = NULL, *pc = NULL; 

    if( psep == NULL || strlen(psep) <= 0 ) 
        psep = dsep; 

    s.release(); 

    if( this->m_nLen > 0 && pos >= 0 && pos < (int) this->m_nLen ) 
    {
        ptr = pbuf = this->m_psRep + pos; 

        while( ptr && *ptr && ptr - this->m_psRep < (int) this->m_nLen ) 
        {
            pc = psep; 
            while( pc && *pc ) 
            {
                if( *pc == *ptr ) 
                    break; 
                pc ++; 
            }
            if( *pc == *ptr ) 
                break; 
            ptr ++; 
        }

        s.append(pbuf, ptr - pbuf); 

        while( ptr && *ptr && ptr - this->m_psRep < (int) this->m_nLen ) 
        {
            pc = psep; 
            while( pc && *pc ) 
            {
                if( *pc == *ptr ) 
                    break; 
                pc ++; 
            }
            if( *pc != *ptr ) 
                break; 
            ptr ++; 
        }

        pos = ptr - this->m_psRep; 
    }
    else
    {
        pos = -1; 
    }

    return pos; 
}

/**
 * Extract out the substring behind the <code>name</code> in current
 * string, which be the specified <code>index</code> item that 
 * seperated by each character of the <code>token</code>. 
 * The <code>name</code> maybe the substring of the actually string.
 *
 * @param name    the name string
 * @param value   the returned value string that extracted out
 * @param token   separate characters
 * @return        value's size
 */
template <typename CHAR, SIZET BUFSIZE> 
SIZET FastString_Base<CHAR, BUFSIZE>::extractValueBehind(
                    FastString_Base<CHAR, BUFSIZE> &name, 
                    FastString_Base<CHAR, BUFSIZE> &value, 
                    FastString_Base<CHAR, BUFSIZE> &token, 
                    int index, int ignorecase)
{
    value.clear(); 
    if( name.empty() || this->empty() ) return 0; 
    if( token.empty() ) token = " \t";  // default token is white-space
    if( index < 0 ) index = 0;  // default extract first

    int pos = FastString_Base<CHAR, BUFSIZE>::findIndexOf(
                    this->m_psRep, this->m_nLen, 
                    (CHAR *)name.c_str(), name.length(), 
                    0, ignorecase); 
    if( pos < 0 ) return 0; 

    int start = -1, end = -1; 
    int idx = -1, pc_token = 0; 
    int i = pos + name.length(); 

    if( token.indexOf(this->charAt(i-1)) >= 0 ) 
    {
        start = i + 1; 
        idx = 0; 
    }

    for( ; i < (int) this->length(); i ++ ) 
    {
        CHAR c = this->charAt(i); 
        if( token.indexOf(c) >= 0 ) 
        {
            if( pc_token == 0 ) 
                idx ++; 
            if( idx == index + 1 ) 
            {
                end = i; 
                break; 
            }
            else if( idx == index ) 
            {
                start = i + 1; 
            }
            pc_token = 1; 
        }
        else
            pc_token = 0; 
    }

    if( start >= 0 && end > start ) 
        this->substring(value, start, end - start); 

    return value.length(); 
}

/**
 * 替换字符串缓冲数据中存在于token的字符为新字符串value。
 * 如果被替换字符为连续的存在与token的字符，将只替换一次value。
 * 例如：<pre>
 * FastString s = "aaa - bbb - ccc ddd";
 * s.replace(" -&", "_"); 
 * </pre>
 * 替换的结果为：<pre>
 * "aaa_bbb_ccc_ddd"
 * </pre>
 *
 * @param name    被替换的字符串
 * @param value   新的字符串
 * @param ignorecase   是否忽略大小写 0 不 1是
 * @return        替换后的字符串缓冲数据
 */
template <typename CHAR, SIZET BUFSIZE> 
FastString_Base<CHAR, BUFSIZE>& 
FastString_Base<CHAR, BUFSIZE>::replaceToken(
            FastString_Base<CHAR, BUFSIZE> &token, 
            FastString_Base<CHAR, BUFSIZE> &value) 
{
    if( token.empty() || value.empty() ) 
        return *this; 

    int pc_token = 0; 
    FastString_Base<CHAR, BUFSIZE> tmp(this->length()); 

    for( int i = 0; i < (int) this->length(); i ++ ) 
    {
        CHAR c = this->charAt(i); 
        if( token.indexOf(c) >= 0 ) 
        {
            if( pc_token == 0 ) 
                tmp.append(value); 
            pc_token = 1; 
        }
        else
        {
            tmp.append(c); 
            pc_token = 0; 
        }
    }

    return (*this = tmp); 
}


//=============FastString inline functions==========

/**
 *  Default constructor.
 *
 *  @return  Default string.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline FastString_Base<CHAR, BUFSIZE>::FastString_Base()
: m_psBufLine(), 
  m_psBuf(NULL),
  m_psRep(NULL),
  m_nLen(0),
  m_nBufLen(0)
{
    init(); 
}

/**
 *  Copy constructor.
 *
 *  @param s  Input string to copy
 *  @return  Copy of input string @a s
 */
template <typename CHAR, SIZET BUFSIZE> 
inline FastString_Base<CHAR, BUFSIZE>::FastString_Base(
                    const FastString_Base<CHAR, BUFSIZE> &s) 
: m_psBufLine(), 
  m_psBuf(NULL),
  m_psRep(NULL),
  m_nLen(0),
  m_nBufLen(0)
{
    init(); 
    this->set(s.c_str(), s.length()); 
}

/**
 * Constructor that copies @a s into dynamically allocated memory.
 * If @a release is non-0 then the @a ACE_Allocator is responsible for
 * freeing this memory. Memory is _not_ allocated/freed if @a release
 * is 0.
 *
 * @param str  Zero terminated input string
 * @return  string containing const CHAR *s
 */
template <typename CHAR, SIZET BUFSIZE> 
inline FastString_Base<CHAR, BUFSIZE>::FastString_Base(const CHAR *str)
: m_psBufLine(), 
  m_psBuf(NULL),
  m_psRep(NULL),
  m_nLen(0),
  m_nBufLen(0)
{
    init(); 
    if( str && *str ) 
    {
        this->set(str, strlen(str)); 
    }
}

/**
 * Constructor that copies @a len CHARs of @a s into dynamically
 * allocated memory (will zero terminate the result).  If @a release
 * is non-0 then the @a ACE_allocator is responsible for freeing this
 * memory.  Memory is _not_ allocated/freed if @a release is 0.
 *
 * @param str  Non-zero terminated input string
 * @param len  Length of non-zero terminated input string
 * @return  string containing const CHAR *s
 */
template <typename CHAR, SIZET BUFSIZE> 
inline FastString_Base<CHAR, BUFSIZE>::FastString_Base(
                    const CHAR *str, SSIZET len)
: m_psBufLine(), 
  m_psBuf(NULL),
  m_psRep(NULL),
  m_nLen(0),
  m_nBufLen(0)
{
    init(); 
    this->set(str, len); 
}

/**
 *  Constructor that dynamically allocate @a len long of CHAR array
 *  and initialize it to @a c using @a alloc to allocate the memory.
 *
 *  @param len  Length of character array 'c'
 *  @param c  Input character array
 *  @return  string containing character array 'c'
 */
template <typename CHAR, SIZET BUFSIZE> 
inline FastString_Base<CHAR, BUFSIZE>::FastString_Base(
                    const SSIZET len, const CHAR c)
: m_psBufLine(), 
  m_psBuf(NULL),
  m_psRep(NULL),
  m_nLen(0),
  m_nBufLen(0)
{
    init(); 
    this->resize(len, c); 
}

/**
 *  Deletes the memory...
 */
template <typename CHAR, SIZET BUFSIZE> 
inline FastString_Base<CHAR, BUFSIZE>::~FastString_Base()
{
    if( this->m_psBuf ) 
    {
        free(this->m_psBuf); 
        this->m_psBuf = NULL; 
    }
}

/**
 *  Free the memory...
 */
template <typename CHAR, SIZET BUFSIZE> 
inline void FastString_Base<CHAR, BUFSIZE>::release()
{
    if( this->m_psBuf ) 
    {
        free(this->m_psBuf); 
        this->m_psBuf = NULL; 
    }

    init(); 
}

/**
 *  Free the memory...
 */
template <typename CHAR, SIZET BUFSIZE> 
inline void FastString_Base<CHAR, BUFSIZE>::clear()
{
    setLength(0); 
}

/**
 *  Initialize the string's default buffer ...
 */
template <typename CHAR, SIZET BUFSIZE> 
inline void FastString_Base<CHAR, BUFSIZE>::init() 
{
    memset(this->m_psBufLine, 0, sizeof(this->m_psBufLine));
    this->m_psRep = this->m_psBufLine; 
    this->m_nBufLen = sizeof(this->m_psBufLine); 
    this->m_nLen = 0; 
    this->m_psRep[this->m_nLen] = '\0'; 
}

/**
 * Copy @a len bytes of @a s (will zero terminate the result).
 *
 * @param str  Non-zero terminated input string
 */
template <typename CHAR, SIZET BUFSIZE> inline
void FastString_Base<CHAR, BUFSIZE>::set(const CHAR *str)
{
    this->set(str, str ? ::strlen(str) : 0); 
}

/**
 * This method is designed for high-performance. Please use with
 * care ;-) If the current size of the string is less than <len>,
 * the string is resized to the new length. The data is zero'd
 * out after this operation.
 *
 * @param len  New string size
 */
template <typename CHAR, SIZET BUFSIZE> 
inline void FastString_Base<CHAR, BUFSIZE>::resize(SSIZET len)
{
    if( len > 0 ) 
        allocate(len, 1); 
    else
        clear(); 
}

/**
 *  Set the length of the string.
 *
 *  @param len  Length of stored string
 */
template <typename CHAR, SIZET BUFSIZE> 
inline void FastString_Base<CHAR, BUFSIZE>::setLength(SSIZET len) 
{
    if( len >= 0 && (SSIZET)this->m_nBufLen >= len + 1 ) 
    {
        this->m_nLen = len; 
        this->m_psRep[this->m_nLen] = '\0';
    }
}

/**
 *  Return the length of the string.
 *
 *  @return  Length of stored string
 */
template <typename CHAR, SIZET BUFSIZE> 
inline SIZET FastString_Base<CHAR, BUFSIZE>::length() const
{
    return this->m_nLen; 
}

/**
 *  Return the length of the string.
 *
 *  @return  Length of stored string
 */
template <typename CHAR, SIZET BUFSIZE> 
inline SIZET FastString_Base<CHAR, BUFSIZE>::size() const
{
    return this->m_nLen; 
}

/**
 *  Check the string is empty of not.
 *
 *  @return  true or false
 */
template <typename CHAR, SIZET BUFSIZE> 
inline BOOL FastString_Base<CHAR, BUFSIZE>::empty() const
{
    return this->m_nLen > 0 ? FALSE : TRUE; 
}

/**
 *  Return the size of the string buffer.
 *
 *  @return  Size of string buffer
 */
template <typename CHAR, SIZET BUFSIZE> 
inline SIZET FastString_Base<CHAR, BUFSIZE>::capacity() const
{
    return this->m_nBufLen; 
}

/**
 *  Same as STL String's <c_str> and <rep>.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline const CHAR * FastString_Base<CHAR, BUFSIZE>::c_str() const
{
    return this->m_psRep; 
}

/**
 * Get at the underlying representation directly!
 * _Don't_ even think about casting the result to (CHAR *) and modifying it,
 * if it has length 0!
 *
 * @return  Pointer reference to the stored string data. No guarantee is
 *     that the string is zero terminated.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline CHAR * FastString_Base<CHAR, BUFSIZE>::rep() const
{
    return this->m_psRep; 
}

/**
 * Get at the input index 's CHAR in the string.
 *
 * @return  The CHAR.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline CHAR FastString_Base<CHAR, BUFSIZE>::charAt(const SSIZET i) const
{
    return (this->m_psRep && this->m_nLen > 0 && i >= 0 && i < (SSIZET)this->m_nLen) ? 
            this->m_psRep[i] : '\0'; 
}

/**
 * Get at the input index 's CHAR in the string.
 *
 * @return  The CHAR.
 */
//template <typename CHAR, SIZET BUFSIZE> 
//inline CHAR FastString_Base<CHAR, BUFSIZE>::operator [] (SIZET i) const
//{
//    return this->m_psRep[i]; 
//}

/**
 *  Same as <rep>.
 */
//template <typename CHAR, SIZET B/UFSIZE> 
//inline FastString_Base<CHAR, BUFSIZE>::operator CHAR * () const
//{
//    return this->m_psRep; 
//}

/**
 *  Same as <c_str>.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline FastString_Base<CHAR, BUFSIZE>::operator const CHAR * () const
{
    return this->m_psRep; 
}

/**
 *  Same as <rep>.
 */
//template <typename CHAR, SIZET BUFSIZE> 
//inline FastString_Base<CHAR, BUFSIZE>::operator void * () const
//{
//    return (void *) this->m_psRep; 
//}

/**
 *  Same as <c_str>.
 */
//template <typename CHAR, SIZET BUFSIZE> 
//inline FastString_Base<CHAR, BUFSIZE>::operator const void * () const
//{
//    return (const void *) this->m_psRep; 
//}

/**
 *  Assignment operator (does copy memory).
 *
 *  @param s  Input string to assign to this object.
 *  @return  Return a copy of the this string.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline FastString_Base<CHAR, BUFSIZE>& 
FastString_Base<CHAR, BUFSIZE>::operator = (const FastString_Base<CHAR, BUFSIZE> &s)
{
    if( this != &s ) 
        this->set(s.c_str(), s.length()); 
    
    return *this;
}

/**
 *  Assignment operator (does copy memory).
 *
 *  @param str  Zero terminated input string.
 *  @return  Return a copy of the this string.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline FastString_Base<CHAR, BUFSIZE>& 
FastString_Base<CHAR, BUFSIZE>::operator = (const CHAR *str)
{
    if( str ) 
        this->set(str, strlen(str)); 
    
    return *this;
}

/**
 * Compares this <code>String</code> to another <code>String</code>
 *
 * @param   s   the <code>String</code> to compare this
 *                          <code>String</code> against.
 * @return  <code>true</code> if the argument is not <code>null</code>
 *          and the <code>String</code>s are equal,
 *          <code>false</code> otherwise.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline BOOL FastString_Base<CHAR, BUFSIZE>::operator == (
                    const FastString_Base<CHAR, BUFSIZE> &s) const
{
    return this == &s ? TRUE : this->equals(s.c_str(), s.length()); 
}

/**
 * Compares this <code>String</code> to another <code>String</code>
 * not equal.
 *
 * @param   s   the <code>String</code> to compare this
 *                          <code>String</code> against.
 * @return  <code>true</code> if the argument is not <code>null</code>
 *          and the <code>String</code>s are not equal,
 *          <code>false</code> otherwise.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline BOOL FastString_Base<CHAR, BUFSIZE>::operator != (
                    const FastString_Base<CHAR, BUFSIZE> &s) const
{
    return this == &s ? FALSE : (this->equals(s.c_str(), s.length()) ? FALSE : TRUE); 
}

/**
 * Swap two FastString variables.
 *
 * @param s  other string.
 */
template <typename CHAR, SIZET BUFSIZE> void 
FastString_Base<CHAR, BUFSIZE>::swap(FastString_Base<CHAR, BUFSIZE>& s) 
{
    CHAR temp[BUFSIZE]; 
    ::memcpy(temp, m_psBufLine, BUFSIZE); 
    ::memcpy(m_psBufLine, s.m_psBufLine, BUFSIZE); 
    ::memcpy(s.m_psBufLine, temp, BUFSIZE); 

    _FAST::fast_swap_value<CHAR *>(m_psBuf, s.m_psBuf); 
    _FAST::fast_swap_value<CHAR *>(m_psRep, s.m_psRep); 
    _FAST::fast_swap_value<SIZET>(m_nLen, s.m_nLen); 
    _FAST::fast_swap_value<SIZET>(m_nBufLen, s.m_nBufLen); 
    
    // 检查自己的字符串指针是否指向正确位置，防止与 s 互指存储数据
    if( m_psRep != m_psBuf && m_psRep != m_psBufLine ) 
    {
        if( m_psBuf == 0 ) 
        {
            m_psRep = m_psBufLine; 
            m_nBufLen = sizeof(m_psBufLine); 
        }
        else
            m_psRep = m_psBuf; 
    }

    // 检查 s 的字符串指针是否指向正确位置，防止与 this 互指存储数据
    if( s.m_psRep != s.m_psBuf && s.m_psRep != s.m_psBufLine ) 
    {
        if( s.m_psBuf == 0 ) 
        {
            s.m_psRep = s.m_psBufLine; 
            s.m_nBufLen = sizeof(s.m_psBufLine); 
        }
        else
            s.m_psRep = s.m_psBuf; 
    }
}

/**
 * Return a substring given an offset and length, if length == 0
 * use rest of str.  Return empty substring if offset or
 * offset/length are invalid.
 *
 * @param offset  Index of first desired character of the substring.
 * @param length  How many characters to return starting at the offset.
 * @return The string containing the desired substring
 */
template <typename CHAR, SIZET BUFSIZE>  
inline FastString_Base<CHAR, BUFSIZE> 
FastString_Base<CHAR, BUFSIZE>::substr(const SSIZET offset, int length) const
{
    return this->substring(offset, length); 
}

/**
 * Appends the string to this string buffer. 
 * This string allocate memory by itself to store string.
 *
 * @param s  append string.
 * @return  a reference to this <code>FastString</code>.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline FastString_Base<CHAR, BUFSIZE>& 
FastString_Base<CHAR, BUFSIZE>::append(const FastString_Base<CHAR, BUFSIZE> &s)
{
    if( s.length() > 0 ) 
        this->append(s.c_str(), s.length()); 
    
    return *this;
}

/**
 * Appends the string to this string buffer. 
 * This string allocate memory by itself to store string.
 *
 * @param str  Zero terminated input string.
 * @return  a reference to this <code>FastString</code>.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline FastString_Base<CHAR, BUFSIZE>& 
FastString_Base<CHAR, BUFSIZE>::append(const CHAR *str)
{
    if( str ) 
        this->append(str, strlen(str)); 
    
    return *this;
}

/**
 * Appends the CHAR to this string buffer. 
 * This string allocate memory by itself to store string.
 *
 * @param c  Input char.
 * @return  a reference to this <code>FastString</code>.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline FastString_Base<CHAR, BUFSIZE>& 
FastString_Base<CHAR, BUFSIZE>::append(const CHAR c)
{
    this->append(&c, 1); 
    
    return *this;
}

/**
 * Appends the int number to this string buffer. 
 * This string allocate memory by itself to store string.
 *
 * @param n  Input int number.
 * @return  a reference to this <code>FastString</code>.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline FastString_Base<CHAR, BUFSIZE>& 
FastString_Base<CHAR, BUFSIZE>::append2(const int n)
{
    CHAR tmp[32]; 
    sprintf(tmp, "%d", n); 
    this->append(tmp, strlen(tmp)); 
    
    return *this;
}

/**
 * Appends the int number to this string buffer. 
 * This string allocate memory by itself to store string.
 * If the string of number's length is smaller than 
 * <code>len</code>, then append left number of <code>c</code> 
 * char ahead, default is space char ' '.
 *
 * @param n     Input int number.
 * @param len   length of number string
 * @param c     char appended in ahead, default is space char
 * @return  a reference to this <code>FastString</code>.
 */
template <typename CHAR, SIZET BUFSIZE> 
FastString_Base<CHAR, BUFSIZE>& 
FastString_Base<CHAR, BUFSIZE>::append2(const int n, int len, char c)
{
    CHAR tmp[32]; 
    sprintf(tmp, "%d", n); 

    if( len > 0 ) 
    {
        SIZET numlen = ::strlen(tmp); 
        if( numlen < (SIZET) len ) 
        {
            if( !( c > 0x20 && c <= 0x7E) ) 
                c = ' '; 
            this->append(c, len - numlen); 
        }
    }

    this->append(tmp, strlen(tmp)); 
    
    return *this;
}

/**
 * Appends the unsigned int number to this string buffer. 
 * This string allocate memory by itself to store string.
 *
 * @param n  Input unsigned int number.
 * @return  a reference to this <code>FastString</code>.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline FastString_Base<CHAR, BUFSIZE>& 
FastString_Base<CHAR, BUFSIZE>::append2(const unsigned int n)
{
    CHAR tmp[32]; 
    sprintf(tmp, "%u", n); 
    this->append(tmp, strlen(tmp)); 
    
    return *this;
}

/**
 * Appends the unsigned int number to this string buffer. 
 * This string allocate memory by itself to store string.
 * If the string of number's length is smaller than 
 * <code>len</code>, then append left number of <code>c</code> 
 * char ahead, default is space char ' '.
 *
 * @param n     Input unsigned int number.
 * @param c     char appended in ahead, default is space char
 * @return  a reference to this <code>FastString</code>.
 */
template <typename CHAR, SIZET BUFSIZE> 
FastString_Base<CHAR, BUFSIZE>& 
FastString_Base<CHAR, BUFSIZE>::append2(const unsigned int n, int len, char c)
{
    CHAR tmp[32]; 
    sprintf(tmp, "%u", n); 

    if( len > 0 ) 
    {
        SIZET numlen = ::strlen(tmp); 
        if( numlen < (SIZET) len ) 
        {
            if( !( c > 0x20 && c <= 0x7E) ) 
                c = ' '; 
            this->append(c, len - numlen); 
        }
    }

    this->append(tmp, strlen(tmp)); 
    
    return *this;
}

/**
 * Appends the float number to this string buffer. 
 * This string allocate memory by itself to store string.
 *
 * @param n  Input float number.
 * @return  a reference to this <code>FastString</code>.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline FastString_Base<CHAR, BUFSIZE>& 
FastString_Base<CHAR, BUFSIZE>::append2(const float f)
{
    CHAR tmp[64]; 
#if defined(_WIN32) || defined(__WIN32__)
    _snprintf(tmp, sizeof(tmp), "%f", f); 
#else
    snprintf(tmp, sizeof(tmp), "%f", f); 
#endif
    this->append(tmp, strlen(tmp)); 
    
    return *this;
}

/**
 * Appends the float number to this string buffer. 
 * This string allocate memory by itself to store string.
 * If the string of number's length is smaller than 
 * <code>len</code>, then append left number of <code>c</code> 
 * char ahead, default is space char ' '.
 *
 * @param n     Input float number.
 * @param c     char appended in ahead, default is space char
 * @return  a reference to this <code>FastString</code>.
 */
template <typename CHAR, SIZET BUFSIZE> 
FastString_Base<CHAR, BUFSIZE>& 
FastString_Base<CHAR, BUFSIZE>::append2(const float f, int len, char c)
{
    CHAR tmp[64]; 
#if defined(_WIN32) || defined(__WIN32__)
    _snprintf(tmp, sizeof(tmp), "%f", f); 
#else
    snprintf(tmp, sizeof(tmp), "%f", f); 
#endif

    if( len > 0 ) 
    {
        SIZET numlen = ::strlen(tmp); 
        if( numlen < (SIZET) len ) 
        {
            if( !( c > 0x20 && c <= 0x7E) ) 
                c = ' '; 
            this->append(c, len - numlen); 
        }
    }

    this->append(tmp, strlen(tmp)); 
    
    return *this;
}

/**
 * Tests if this string ends with the specified suffix.
 *
 * @param   suffix   the suffix.
 * @param   len      the suffix length.
 * @return  <code>true</code> if the character sequence represented by the
 *          argument is a suffix of the character sequence represented by
 *          this object; <code>false</code> otherwise. Note that the
 *          result will be <code>true</code> if the argument is the
 *          empty string or is equal to this <code>String</code> object
 *          as determined by the {@link #equals(Object)} method.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline BOOL FastString_Base<CHAR, BUFSIZE>::endsWith(
                    const CHAR *suffix, SSIZET len) const
{
    if( !suffix ) return FALSE; 

    len = (len <= 0) ? strlen(suffix) : len; 

    return this->startsWith(suffix, len, (SSIZET)(this->m_nLen - len), 0); 
}

/**
 * Tests if this string ends with the specified suffix, ignore word case.
 *
 * @param   suffix   the suffix.
 * @param   len      the suffix length.
 * @return  <code>true</code> if the character sequence represented by the
 *          argument is a suffix of the character sequence represented by
 *          this object; <code>false</code> otherwise. Note that the
 *          result will be <code>true</code> if the argument is the
 *          empty string or is equal to this <code>String</code> object
 *          as determined by the {@link #equals(Object)} method.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline BOOL FastString_Base<CHAR, BUFSIZE>::endsWithIgnoreCase(
                    const CHAR *suffix, SSIZET len) const
{
    if( !suffix ) return FALSE; 

    len = (len <= 0) ? strlen(suffix) : len; 

    return this->startsWith(suffix, len, this->m_nLen - len, 1); 
}

/**
 * Tests if this string starts with the specified prefix beginning
 * a specified index, ignore word case.
 *
 * @param   prefix    the prefix.
 * @param   len       the prefix's length.
 * @param   offset    where to begin looking in the string.
 * @return  <code>true</code> if the character sequence represented by the
 *          argument is a prefix of the substring of this object starting
 *          at index <code>toffset</code>; <code>false</code> otherwise.
 *          The result is <code>false</code> if <code>toffset</code> is
 *          negative or greater than the length of this
 *          <code>String</code> object; otherwise the result is the same
 *          as the result of the expression
 *          <pre>
 *          this.subString(toffset).startsWith(prefix)
 *          </pre>
 */
template <typename CHAR, SIZET BUFSIZE> 
inline BOOL FastString_Base<CHAR, BUFSIZE>::startsWithIgnoreCase(
                    const CHAR *prefix, SSIZET len, SSIZET offset) const
{
    return this->startsWith(prefix, len, offset, 1); 
}

/**
 * Compares this <code>String</code> to another <code>String</code>,
 * ignoring case considerations.  Two strings are considered equal
 * ignoring case if they are of the same length, and corresponding
 * characters in the two strings are equal ignoring case.
 * <p>
 * Two characters <code>c1</code> and <code>c2</code> are considered
 * the same, ignoring case if at least one of the following is true:
 * <ul><li>The two characters are the same (as compared by the
 * <code>==</code> operator).
 * <li>Applying the method {@link java.lang.Character#toUpperCase(char)}
 * to each character produces the same result.
 * <li>Applying the method {@link java.lang.Character#toLowerCase(char)}
 * to each character produces the same result.</ul>
 *
 * @param   str   the <code>String</code> to compare this
 *                          <code>String</code> against.
 * @param   len   compare length of the string.
 * @return  <code>true</code> if the argument is not <code>null</code>
 *          and the <code>String</code>s are equal,
 *          ignoring case; <code>false</code> otherwise.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline BOOL FastString_Base<CHAR, BUFSIZE>::equalsIgnoreCase(
                    const CHAR *str, SSIZET len) const
{
    return this->equals(str, len, 1); 
}

/**
 * Compares this <code>String</code> to another <code>String</code>,
 * ignoring case considerations.  Two strings are considered equal
 * ignoring case if they are of the same length, and corresponding
 * characters in the two strings are equal ignoring case.
 * <p>
 * Two characters <code>c1</code> and <code>c2</code> are considered
 * the same, ignoring case if at least one of the following is true:
 * <ul><li>The two characters are the same (as compared by the
 * <code>==</code> operator).
 * <li>Applying the method {@link java.lang.Character#toUpperCase(char)}
 * to each character produces the same result.
 * <li>Applying the method {@link java.lang.Character#toLowerCase(char)}
 * to each character produces the same result.</ul>
 *
 * @param   s   the <code>String</code> to compare this
 *                          <code>String</code> against.
 * @return  <code>true</code> if the argument is not <code>null</code>
 *          and the <code>String</code>s are equal,
 *          ignoring case; <code>false</code> otherwise.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline BOOL FastString_Base<CHAR, BUFSIZE>::equals(
                    const FastString_Base<CHAR, BUFSIZE> &s) const
{
    return this->equals(s.c_str(), s.length()); 
}

/**
 * Compares this <code>String</code> to another <code>String</code>,
 * ignoring case considerations.  Two strings are considered equal
 * ignoring case if they are of the same length, and corresponding
 * characters in the two strings are equal ignoring case.
 * <p>
 * Two characters <code>c1</code> and <code>c2</code> are considered
 * the same, ignoring case if at least one of the following is true:
 * <ul><li>The two characters are the same (as compared by the
 * <code>==</code> operator).
 * <li>Applying the method {@link java.lang.Character#toUpperCase(char)}
 * to each character produces the same result.
 * <li>Applying the method {@link java.lang.Character#toLowerCase(char)}
 * to each character produces the same result.</ul>
 *
 * @param   s   the <code>String</code> to compare this
 *                          <code>String</code> against.
 * @return  <code>true</code> if the argument is not <code>null</code>
 *          and the <code>String</code>s are equal,
 *          ignoring case; <code>false</code> otherwise.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline BOOL FastString_Base<CHAR, BUFSIZE>::equalsIgnoreCase(
                    const FastString_Base<CHAR, BUFSIZE> &s) const
{
    return this->equals(s.c_str(), s.length(), 1); 
}

/**
 * replace substring <code>name</code> in a string with <code>value</code>.
 * 替换字符串缓冲数据中为name的子字符串为新字符串value.
 *
 * @param name    被替换的字符串
 * @param value   新的字符串
 * @return        替换后的字符串缓冲数据
 */
template <typename CHAR, SIZET BUFSIZE> 
inline FastString_Base<CHAR, BUFSIZE>& 
FastString_Base<CHAR, BUFSIZE>::replaceIgnoreCase(
            const FastString_Base<CHAR, BUFSIZE> &name, 
            const FastString_Base<CHAR, BUFSIZE> &value) 
{
    return this->replace(name, value, 1); 
}

/**
 * replace substrings <code>name</code> in a string with <code>values</code>.
 * 替换字符串缓冲数据中为names数组的子字符串第n个为新字符串values数组的第n个.
 *
 * @param names   被替换的字符串数组
 * @param values  新的字符串数组
 * @return        替换后的字符串缓冲数据
 */
template <typename CHAR, SIZET BUFSIZE> 
inline FastString_Base<CHAR, BUFSIZE>& 
FastString_Base<CHAR, BUFSIZE>::replacesIgnoreCase(
            FastString_Base<CHAR, BUFSIZE> *pNames, 
            FastString_Base<CHAR, BUFSIZE> *pValues, 
            int count) 
{
    return this->replaces(pNames, pValues, count, 1); 
}

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the char being searched for.
 *
 * @param   target       the character being searched for.
 * @param   fromIndex    the index to begin searching from.
 */
template <typename CHAR, SIZET BUFSIZE> 
int FastString_Base<CHAR, BUFSIZE>::indexOf(
                   CHAR target, SSIZET fromIndex) 
{
    if( fromIndex < 0 ) 
        return -1; 
    for( SSIZET i = fromIndex; i < (SSIZET)this->m_nLen; i ++ ) 
    {
        if( this->m_psRep[i] == target ) 
            return i; 
    }
    return -1; 
}

/**
 * Copies characters from this string into the destination character
 * array.
 * <p>
 * The first character to be copied is at index <code>srcBegin</code>;
 * the last character to be copied is at index <code>srcEnd-1</code>
 * (thus the total number of characters to be copied is
 * <code>srcEnd-srcBegin</code>). The characters are copied into the
 * subarray of <code>dst</code> starting at index <code>dstBegin</code>
 * and ending at index:
 * <p><blockquote><pre>
 *     dstbegin + (srcEnd-srcBegin) - 1
 * </pre></blockquote>
 *
 * @param      srcBegin   index of the first character in the string
 *                        to copy.
 * @param      srcEnd     index after the last character in the string
 *                        to copy.
 * @param      dst        the destination array.
 * @param      dstBegin   the start offset in the destination array.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline void FastString_Base<CHAR, BUFSIZE>::getChars(
            int srcBegin, int srcEnd, char *dst, int dstBegin) 
{
    if( srcEnd > srcBegin && srcBegin >= 0 && srcEnd <= (int)m_nLen && 
        dst != 0 && dstBegin >= 0 ) 
    {
        ::memcpy(dst + dstBegin, m_psRep + srcBegin, srcEnd - srcBegin); 
    }
}

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the string being searched for.
 *
 * @param   target       the characters being searched for.
 * @param   targetCount  count of the target string.
 * @param   fromIndex    the index to begin searching from.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline int FastString_Base<CHAR, BUFSIZE>::indexOf(
                   const CHAR *target, SSIZET targetCount, 
                   SSIZET fromIndex) 
{
    return FastString_Base<CHAR, BUFSIZE>::findIndexOf(
            this->m_psRep, this->m_nLen, target, targetCount, 
            fromIndex, 0); 
}

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the string being searched for.
 *
 * @param   target       the characters being searched for.
 * @param   fromIndex    the index to begin searching from.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline int FastString_Base<CHAR, BUFSIZE>::indexOf(
                   const CHAR *target, 
                   SSIZET fromIndex) 
{
    SSIZET targetCount = 0; 
    if( target ) targetCount = strlen(target); 
    return FastString_Base<CHAR, BUFSIZE>::findIndexOf(
            this->m_psRep, this->m_nLen, target, targetCount, 
            fromIndex, 0); 
}

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the string being searched for.
 *
 * @param   target       the characters being searched for.
 * @param   fromIndex    the index to begin searching from.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline int FastString_Base<CHAR, BUFSIZE>::indexOf(
                   FastString_Base<CHAR, BUFSIZE> &target, 
                   SSIZET fromIndex) 
{
    return FastString_Base<CHAR, BUFSIZE>::findIndexOf(
            this->m_psRep, this->m_nLen, (CHAR *) target.c_str(), target.length(), 
            fromIndex, 0); 
}

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the char being searched for, and ignore case.
 *
 * @param   target       the character being searched for.
 * @param   fromIndex    the index to begin searching from.
 */
template <typename CHAR, SIZET BUFSIZE> 
int FastString_Base<CHAR, BUFSIZE>::indexOfIgnoreCase(
                   CHAR target, SSIZET fromIndex) 
{
    target = tolower(target); 
    for( SSIZET i = fromIndex; i < this->m_nLen; i ++ ) 
    {
        if( tolower(this->m_psRep[i]) == target ) 
            return i; 
    }
    return -1; 
}

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the string being searched for, and ignore word case.
 *
 * @param   target       the characters being searched for.
 * @param   targetCount  count of the target string.
 * @param   fromIndex    the index to begin searching from.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline int FastString_Base<CHAR, BUFSIZE>::indexOfIgnoreCase(
                   const CHAR *target, SSIZET targetCount,
                   SSIZET fromIndex) 
{
    return FastString_Base<CHAR, BUFSIZE>::findIndexOf(
            this->m_psRep, this->m_nLen, target, targetCount, 
            fromIndex, 1); 
}

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the string being searched for, and ignore word case.
 *
 * @param   target       the characters being searched for.
 * @param   targetCount  count of the target string.
 * @param   fromIndex    the index to begin searching from.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline int FastString_Base<CHAR, BUFSIZE>::indexOfIgnoreCase(
                   const CHAR *target,
                   SSIZET fromIndex) 
{
    SSIZET targetCount = 0; 
    if( target ) targetCount = strlen(target); 
    return FastString_Base<CHAR, BUFSIZE>::findIndexOf(
            this->m_psRep, this->m_nLen, target, targetCount, 
            fromIndex, 1); 
}

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the string being searched for, and ignore word case.
 *
 * @param   target       the characters being searched for.
 * @param   targetCount  count of the target string.
 * @param   fromIndex    the index to begin searching from.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline int FastString_Base<CHAR, BUFSIZE>::indexOfIgnoreCase(
                   FastString_Base<CHAR, BUFSIZE> &target,
                   SSIZET fromIndex) 
{
    return FastString_Base<CHAR, BUFSIZE>::findIndexOf(
            this->m_psRep, this->m_nLen, target.c_str(), target.length(), 
            fromIndex, 1); 
}

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the char being searched for.
 *
 * @param   target       the character being searched for.
 * @param   fromIndex    the index to begin searching from.
 */
template <typename CHAR, SIZET BUFSIZE> 
int FastString_Base<CHAR, BUFSIZE>::lastIndexOf(
                   CHAR target, SSIZET fromIndex) 
{
    if( fromIndex < 0 || fromIndex >= (int)this->m_nLen ) 
        return -1; 
    if( fromIndex == 0 ) 
        fromIndex = this->m_nLen; 
    for( int i = fromIndex; i >= 0; i -- ) 
    {
        if( this->m_psRep[i] == target ) 
            return i; 
    }
    return -1; 
}

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the string being searched for.
 *
 * @param   target       the characters being searched for.
 * @param   targetCount  count of the target string.
 * @param   fromIndex    the index to begin searching from.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline int FastString_Base<CHAR, BUFSIZE>::lastIndexOf(
                       const CHAR *target, SSIZET targetCount, 
                       SSIZET fromIndex) 
{
    return FastString_Base<CHAR, BUFSIZE>::findLastIndexOf(
            this->m_psRep, this->m_nLen, target, targetCount, 
            fromIndex, 0); 
}

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the string being searched for.
 *
 * @param   target       the characters being searched for.
 * @param   fromIndex    the index to begin searching from.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline int FastString_Base<CHAR, BUFSIZE>::lastIndexOf(
                       const CHAR *target,
                       SSIZET fromIndex) 
{
    SSIZET targetCount = 0; 
    if( target ) targetCount = strlen(target); 
    return FastString_Base<CHAR, BUFSIZE>::findLastIndexOf(
            this->m_psRep, this->m_nLen, target, targetCount, 
            fromIndex, 0); 
}

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the string being searched for.
 *
 * @param   target       the characters being searched for.
 * @param   fromIndex    the index to begin searching from.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline int FastString_Base<CHAR, BUFSIZE>::lastIndexOf(
                       FastString_Base<CHAR, BUFSIZE> &target,
                       SSIZET fromIndex) 
{
    return FastString_Base<CHAR, BUFSIZE>::findLastIndexOf(
            this->m_psRep, this->m_nLen, target.c_str(), target.length(), 
            fromIndex, 0); 
}

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the char being searched for, and ignore case.
 *
 * @param   target       the character being searched for.
 * @param   fromIndex    the index to begin searching from.
 */
template <typename CHAR, SIZET BUFSIZE> 
int FastString_Base<CHAR, BUFSIZE>::lastIndexOfIgnoreCase(
                   CHAR target, SSIZET fromIndex) 
{
    if( fromIndex < 0 || fromIndex >= this->m_nLen ) 
        return -1; 
    target = tolower(target); 
    for( SSIZET i = fromIndex; i >= 0; i -- ) 
    {
        if( tolower(this->m_psRep[i]) == target ) 
            return i; 
    }
    return -1; 
}

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the string being searched for, and ignore word case.
 *
 * @param   target       the characters being searched for.
 * @param   targetCount  count of the target string.
 * @param   fromIndex    the index to begin searching from.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline int FastString_Base<CHAR, BUFSIZE>::lastIndexOfIgnoreCase(
                       const CHAR *target, SSIZET targetCount,
                       SSIZET fromIndex) 
{
    return FastString_Base<CHAR, BUFSIZE>::findLastIndexOf(
            this->m_psRep, this->m_nLen, target, targetCount, 
            fromIndex, 1); 
}

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the string being searched for, and ignore word case.
 *
 * @param   target       the characters being searched for.
 * @param   fromIndex    the index to begin searching from.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline int FastString_Base<CHAR, BUFSIZE>::lastIndexOfIgnoreCase(
                       const CHAR *target,
                       SSIZET fromIndex) 
{
    SSIZET targetCount = 0; 
    if( target ) targetCount = strlen(target); 
    return FastString_Base<CHAR, BUFSIZE>::findLastIndexOf(
            this->m_psRep, this->m_nLen, target, targetCount, 
            fromIndex, 1); 
}

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the string being searched for, and ignore word case.
 *
 * @param   target       the characters being searched for.
 * @param   fromIndex    the index to begin searching from.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline int FastString_Base<CHAR, BUFSIZE>::lastIndexOfIgnoreCase(
                       FastString_Base<CHAR, BUFSIZE> &target,
                       SSIZET fromIndex) 
{
    return FastString_Base<CHAR, BUFSIZE>::findLastIndexOf(
            this->m_psRep, this->m_nLen, target.c_str(), target.length(), 
            fromIndex, 1); 
}

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the char being searched for.
 *
 * @param   target       the character being searched for.
 * @param   fromIndex    the index to begin searching from.
 * @return  count of matches char
 */
template <typename CHAR, SIZET BUFSIZE> 
int FastString_Base<CHAR, BUFSIZE>::countOf(
                   CHAR target, SSIZET fromIndex) 
{
    int count = 0; 
    if( fromIndex < 0 ) 
        return count; 
    
    for( SSIZET i = fromIndex; i < (SSIZET)this->m_nLen; i ++ ) 
    {
        if( this->m_psRep[i] == target ) 
            count ++; 
    }
    
    return count; 
}

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the string being searched for.
 *
 * @param   target       the characters being searched for.
 * @param   targetCount  count of the target string.
 * @param   fromIndex    the index to begin searching from.
 * @return  count of matches string
 */
template <typename CHAR, SIZET BUFSIZE> 
int FastString_Base<CHAR, BUFSIZE>::countOf(
                   const CHAR *target, SSIZET targetCount, 
                   SSIZET fromIndex, int ignorecase) 
{
    if( !this->m_psRep || this->m_nLen <= 0 || 
        !target || (targetCount <= 0) || fromIndex < 0 || 
        fromIndex >= this->m_nLen ) 
        return 0; 

    int pos = fromIndex, count = 0; 
    while( pos < this->m_nLen ) 
    {
        pos = FastString_Base<CHAR, BUFSIZE>::findIndexOf(
                this->m_psRep, this->m_nLen, target, targetCount, 
                pos, ignorecase); 
        if( pos < 0 ) 
            break; 
        count ++; 
        pos += targetCount; 
    }

    return count; 
}

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the string being searched for.
 *
 * @param   target       the characters being searched for.
 * @param   fromIndex    the index to begin searching from.
 * @return  count of matches string
 */
template <typename CHAR, SIZET BUFSIZE> 
inline int FastString_Base<CHAR, BUFSIZE>::countOf(
                   const CHAR *target, 
                   SSIZET fromIndex) 
{
    SSIZET targetCount = 0; 
    if( target ) targetCount = strlen(target); 
    return FastString_Base<CHAR, BUFSIZE>::countOf(
            target, targetCount, 
            fromIndex); 
}

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the string being searched for.
 *
 * @param   target       the characters being searched for.
 * @param   fromIndex    the index to begin searching from.
 * @return  count of matches string
 */
template <typename CHAR, SIZET BUFSIZE> 
inline int FastString_Base<CHAR, BUFSIZE>::countOf(
                   FastString_Base<CHAR, BUFSIZE> &target, 
                   SSIZET fromIndex) 
{
    return FastString_Base<CHAR, BUFSIZE>::countOf(
            target.c_str(), target.length(), 
            fromIndex); 
}

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the char being searched for, and ignore case.
 *
 * @param   target       the character being searched for.
 * @param   fromIndex    the index to begin searching from.
 * @return  count of matches char
 */
template <typename CHAR, SIZET BUFSIZE> 
int FastString_Base<CHAR, BUFSIZE>::countOfIgnoreCase(
                   CHAR target, SSIZET fromIndex) 
{
    int count = 0; 
    if( fromIndex < 0 ) 
        return count; 
    
    target = tolower(target); 
    for( SSIZET i = fromIndex; i < this->m_nLen; i ++ ) 
    {
        if( tolower(this->m_psRep[i]) == target ) 
            count ++; 
    }
    
    return count; 
}

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the string being searched for, and ignore word case.
 *
 * @param   target       the characters being searched for.
 * @param   targetCount  count of the target string.
 * @param   fromIndex    the index to begin searching from.
 * @return  count of matches string
 */
template <typename CHAR, SIZET BUFSIZE> 
inline int FastString_Base<CHAR, BUFSIZE>::countOfIgnoreCase(
                   const CHAR *target, SSIZET targetCount,
                   SSIZET fromIndex) 
{
    return FastString_Base<CHAR, BUFSIZE>::countOf(
            target, targetCount, 
            fromIndex, 1); 
}

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the string being searched for, and ignore word case.
 *
 * @param   target       the characters being searched for.
 * @param   targetCount  count of the target string.
 * @param   fromIndex    the index to begin searching from.
 * @return  count of matches string
 */
template <typename CHAR, SIZET BUFSIZE> 
inline int FastString_Base<CHAR, BUFSIZE>::countOfIgnoreCase(
                   const CHAR *target,
                   SSIZET fromIndex) 
{
    SSIZET targetCount = 0; 
    if( target ) targetCount = strlen(target); 
    return FastString_Base<CHAR, BUFSIZE>::countOf(
            target, targetCount, 
            fromIndex, 1); 
}

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the string being searched for, and ignore word case.
 *
 * @param   target       the characters being searched for.
 * @param   targetCount  count of the target string.
 * @param   fromIndex    the index to begin searching from.
 * @return  count of matches string
 */
template <typename CHAR, SIZET BUFSIZE> 
inline int FastString_Base<CHAR, BUFSIZE>::countOfIgnoreCase(
                   FastString_Base<CHAR, BUFSIZE> &target,
                   SSIZET fromIndex) 
{
    return FastString_Base<CHAR, BUFSIZE>::countOf(
            target.c_str(), target.length(), 
            fromIndex, 1); 
}

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the string being searched for.
 *
 * @param   target       the characters being searched for.
 * @param   targetCount  count of the target string.
 * @param   fromIndex    the index to begin searching from.
 * @return  the first index of any of the characters in "target" 
 *          in current string, or -1 if none are found. 
 */
template <typename CHAR, SIZET BUFSIZE> 
inline int FastString_Base<CHAR, BUFSIZE>::indexOfAny(
                   const CHAR *target, SSIZET targetCount, 
                   SSIZET fromIndex) 
{
    FastString_Base<CHAR, BUFSIZE> any; 
    any.append(target, targetCount); 
    return this->indexOfAny(any, fromIndex); 
}

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the string being searched for.
 *
 * @param   target       the characters being searched for.
 * @param   fromIndex    the index to begin searching from.
 * @return  the first index of any of the characters in "target" 
 *          in current string, or -1 if none are found. 
 */
template <typename CHAR, SIZET BUFSIZE> 
inline int FastString_Base<CHAR, BUFSIZE>::indexOfAny(
                   const CHAR *target, 
                   SSIZET fromIndex) 
{
    FastString_Base<CHAR, BUFSIZE> any(target); 
    return this->indexOfAny(any, fromIndex); 
}

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the string being searched for.
 *
 * @param   target       the characters being searched for.
 * @param   fromIndex    the index to begin searching from.
 * @return  the first index of any of the characters in "target" 
 *          in current string, or -1 if none are found. 
 */
template <typename CHAR, SIZET BUFSIZE> 
inline int FastString_Base<CHAR, BUFSIZE>::indexOfAny(
                   FastString_Base<CHAR, BUFSIZE> &target, 
                   SSIZET fromIndex) 
{
    if( target.empty() || this->empty() || fromIndex < 0 ) 
        return -1; 
    CHAR s[2] = {0}; 
    for( SIZET i = fromIndex; i < this->m_nLen; i ++ ) 
    {
        s[0] = this->m_psRep[i]; 
        if( target.indexOf(s) >= 0 ) 
            return i; 
    }
    return -1; 
}

/**
 * 替换字符串缓冲数据中存在于token的字符为新字符串value。
 * 如果被替换字符为连续的存在与token的字符，将只替换一次value。
 * 例如：<pre>
 * FastString s = "aaa - bbb - ccc ddd";
 * s.replace(" -&", "_"); 
 * </pre>
 * 替换的结果为：<pre>
 * "aaa_bbb_ccc_ddd"
 * </pre>
 *
 * @param name    被替换的字符串
 * @param value   新的字符串
 * @param ignorecase   是否忽略大小写 0 不 1是
 * @return        替换后的字符串缓冲数据
 */
template <typename CHAR, SIZET BUFSIZE> 
inline FastString_Base<CHAR, BUFSIZE>& 
FastString_Base<CHAR, BUFSIZE>::replaceToken(
            const CHAR *token, const CHAR *value) 
{
    FastString_Base<CHAR, BUFSIZE> fsToken(token), fsValue(value); 
    return this->replaceToken(fsToken, fsValue); 
}

/**
 * Extract out the substring behind the <code>name</code> in current
 * string, which be the specified <code>index</code> item that 
 * seperated by each character of the <code>token</code>. 
 * The <code>name</code> maybe the substring of the actually string.
 *
 * @param name    the name string
 * @param value   the returned value string that extracted out
 * @param token   separate characters
 * @return        value's size
 */
template <typename CHAR, SIZET BUFSIZE> inline
SIZET FastString_Base<CHAR, BUFSIZE>::extractValueBehind(
                    const CHAR *name, FastString_Base<CHAR, BUFSIZE> &value, 
                    const CHAR *token, 
                    int index)
{
    FastString_Base<CHAR, BUFSIZE> fsName(name), fsToken(token); 
    return this->extractValueBehind(fsName, value, fsToken, index, 0); 
}

/**
 * Extract out the substring behind the <code>name</code> in current
 * string, which be the specified <code>index</code> item that 
 * seperated by each character of the <code>token</code>. 
 * The <code>name</code> maybe the substring of the actually string.
 * Using default token string " \t" (white-space).
 *
 * @param name    the name string
 * @param value   the returned value string that extracted out
 * @return        value's size
 */
template <typename CHAR, SIZET BUFSIZE> inline
SIZET FastString_Base<CHAR, BUFSIZE>::extractValueBehind(
                    const CHAR *name, FastString_Base<CHAR, BUFSIZE> &value, 
                    int index)
{
    FastString_Base<CHAR, BUFSIZE> fsName(name), fsToken; 
    return this->extractValueBehind(fsName, value, fsToken, index, 0); 
}

/**
 * Extract out the substring behind the <code>name</code> in current
 * string, which be the specified <code>index</code> item that 
 * seperated by each character of the <code>token</code>. 
 * The <code>name</code> maybe the substring of the actually string.
 * Ignore case when searching <code>name></code>.
 *
 * @param name    the name string
 * @param value   the returned value string that extracted out
 * @param token   separate characters
 * @return        value's size
 */
template <typename CHAR, SIZET BUFSIZE> inline
SIZET FastString_Base<CHAR, BUFSIZE>::extractValueBehindIgnoreCase(
                    const CHAR *name, FastString_Base<CHAR, BUFSIZE> &value, 
                    const CHAR *token, 
                    int index)
{
    FastString_Base<CHAR, BUFSIZE> fsName(name), fsToken(token); 
    return this->extractValueBehind(fsName, value, fsToken, index, 1); 
}

/**
 * Extract out the substring behind the <code>name</code> in current
 * string, which be the specified <code>index</code> item that 
 * seperated by each character of the <code>token</code>. 
 * The <code>name</code> maybe the substring of the actually string.
 * Using default token string " \t" (white-space).
 * Ignore case when searching <code>name></code>.
 *
 * @param name    the name string
 * @param value   the returned value string that extracted out
 * @return        value's size
 */
template <typename CHAR, SIZET BUFSIZE> inline
SIZET FastString_Base<CHAR, BUFSIZE>::extractValueBehindIgnoreCase(
                    const CHAR *name, FastString_Base<CHAR, BUFSIZE> &value, 
                    int index)
{
    FastString_Base<CHAR, BUFSIZE> fsName(name), fsToken; 
    return this->extractValueBehind(fsName, value, fsToken, index, 1); 
}

/**
 * Extract out value in input string <code>s</code> by name.
 * 根据分隔符分解字符串获取指定名称的内容.
 *
 * @param str     分解的字符串
 * @param substr  指定的名称
 * @param div1    名称与名称之间分隔符，缺省为“&”
 * @param div2    名称与内容之间的分隔符，缺省为“=”
 * @param buf     放置分解出的内容的空间
 * @param size    buf的长度
 * @param vdiv    多个相同名称的值之间的分割符，缺省为空
 * @param count   获取的相同名称的值的数目
 * @return        获取值的长度
 */
template <typename CHAR, SIZET BUFSIZE> 
inline SIZET FastString_Base<CHAR, BUFSIZE>::extractValueByName(
                 const CHAR *substr, 
                 const CHAR *div1, const CHAR *div2, 
                 FastString_Base<CHAR, BUFSIZE> &value, 
                 const CHAR *vdiv, const int count) 
{
    value.resize(this->m_nLen, '\0'); 
    return FastString_Base<CHAR, BUFSIZE>::extractValueByName(
                 this->m_psRep, 
                 substr, div1, div2, 
                 value.rep(), value.capacity(), 
                 vdiv, count); 
}

/**
 * Extract out value in input string <code>s</code> by index.
 * 根据分隔符分解字符串获取指定位置的内容.
 *
 * @param value   放置分解出的内容的空间
 * @param tag     多个相同名称的值之间的分割符，缺省为空
 * @param index   获取的值的指定位置
 * @return        获取值的长度
 */
template <typename CHAR, SIZET BUFSIZE> 
inline SIZET FastString_Base<CHAR, BUFSIZE>::extractValueByIndex(
                FastString_Base<CHAR, BUFSIZE> &value, 
                const CHAR tag, const int index)
{
    value.resize(this->m_nLen, '\0'); 
    return FastString_Base<CHAR, BUFSIZE>::extractValueByIndex(
                this->m_psRep, this->m_nLen, 
                value.rep(), value.capacity(), 
                tag, index); 
}

/**
 * Extract out value in input string <code>s</code> by name.
 * 根据分隔符“&”分解字符串获取指定名称的内容（name1=value1&name2=value2）.
 *
 * @param substr  指定的名称
 * @param value   获取的整数值
 * @param defval  缺省的值
 * @return        获取值的长度
 */
template <typename CHAR, SIZET BUFSIZE> 
inline SIZET FastString_Base<CHAR, BUFSIZE>::extractIntegerByName(
                                const CHAR *substr, 
                                long *value, const long defval) 
{
    return FastString_Base<CHAR, BUFSIZE>::extractIntegerByName(
                this->m_psRep, substr, value, defval); 
}

/**
 * Extract out value in input string <code>s</code> by name.
 * 根据分隔符“&”分解字符串获取指定名称的内容（name1=value1&name2=value2）.
 *
 * @param substr  指定的名称
 * @param value   获取的字符串值
 * @param size    存储获取的值的空间长度
 * @return        获取值的长度
 */
template <typename CHAR, SIZET BUFSIZE> 
inline SIZET FastString_Base<CHAR, BUFSIZE>::extractValueByName(
                                const CHAR *substr, 
                                FastString_Base<CHAR, BUFSIZE> &value, 
                                const int count) 
{
    value.resize(this->m_nLen, '\0'); 
    return FastString_Base<CHAR, BUFSIZE>::extractValueByName(
                this->m_psRep, 
                substr, NULL, NULL, 
                value.rep(), value.capacity(), 
                NULL, count); 
}



//===========FastString static functions================

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the string being searched for.
 *
 * @param   source       the characters being searched.
 * @param   sourceCount  count of the source string.
 * @param   target       the characters being searched for.
 * @param   targetCount  count of the target string.
 * @param   fromIndex    the index to begin searching from.
 * @param   ignorecase   Ignore word case (1) or not (0).
 */
template <typename CHAR, SIZET BUFSIZE> 
int FastString_Base<CHAR, BUFSIZE>::findIndexOf(
                   const CHAR *source, SSIZET sourceCount, 
                   const CHAR *target, SSIZET targetCount,
                   SSIZET fromIndex, 
                   const int ignorecase) 
{
    int max = 0, i = 0, j = 0, k = 0, end = 0; 
    CHAR first = 0; 
    
    if( sourceCount < 0 || targetCount < 0 || fromIndex < 0 ) 
        return -1; 

    /*
     * Check arguments; return immediately where possible. For
     * consistency, don't check for null str.
     */
    if( targetCount > sourceCount || fromIndex >= sourceCount || !source ) { 
        return - 1; 
    }

    if( fromIndex >= sourceCount ) {
        return (targetCount == 0 ? sourceCount : -1);
    }
    //if( fromIndex < 0 ) {
    //    fromIndex = 0;
    //}
    if( targetCount == 0 || !target ) {
        return fromIndex;
    }

    first  = target[0];
    i = fromIndex;
    max = sourceCount - targetCount;

startSearchForFirstChar:
    while( 1 ) {
        /* Look for first character. */
        while( i <= max && ( source[i] != first && ( !ignorecase || 
               tolower(source[i]) != tolower(first) ) ) ) {
            i++;
        }
        if( i > max ) {
            return -1;
        }

        /* Found first character, now look at the rest of v2 */
        j = i + 1;
        end = j + targetCount - 1;
        k = 1;
        
        while( j < end ) {
            if( source[j] != target[k] && ( !ignorecase || 
                tolower(source[j]) != tolower(target[k]) ) ) {
                i ++; j ++; k ++; 
                /* Look for str's first char again. */
                goto startSearchForFirstChar;
            }
            j ++; k ++; 
        }

        /* Found whole string. */
        return i; 
    }
}

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the string being searched for.
 *
 * @param   source       the characters being searched.
 * @param   sourceCount  count of the source string.
 * @param   target       the characters being searched for.
 * @param   targetCount  count of the target string.
 * @param   fromIndex    the index to begin searching from.
 * @param   ignorecase   Ignore word case (1) or not (0).
 */
template <typename CHAR, SIZET BUFSIZE> 
int FastString_Base<CHAR, BUFSIZE>::findLastIndexOf(
                       const CHAR *source, SSIZET sourceCount, 
                       const CHAR *target, SSIZET targetCount,
                       SSIZET fromIndex, 
                       const int ignorecase) 
{
    int strLastIndex = 0, rightIndex = 0; 
    int min = 0, i = 0, j = 0, k = 0, start = 0; 
    CHAR strLastChar = 0; 
    
    if( sourceCount < 0 || targetCount < 0 || fromIndex < 0 ) 
        return -1; 
    
    /*
     * Check arguments; return immediately where possible. For
     * consistency, don't check for null str.
     */
    if( targetCount > sourceCount || fromIndex > sourceCount || !source ) { 
        return - 1; 
    }
    
    rightIndex = sourceCount - targetCount;
    //if( fromIndex < 0 ) {
    //    return -1;
    //}
    if( (int) fromIndex > rightIndex || fromIndex == 0 ) {
        fromIndex = rightIndex;
    }
    /* Empty string always matches. */
    if( targetCount == 0 || !target ) {
        return fromIndex;
    }

    strLastIndex = targetCount - 1;
    strLastChar = target[strLastIndex];
    min = targetCount - 1;
    i = min + fromIndex;

startSearchForLastChar:
    while( 1 ) {
        /* Look for last character. */
        while( i >= min && ( source[i] != strLastChar && ( !ignorecase || 
               tolower(source[i]) != tolower(strLastChar) ) ) ) {
            i --;
        }
        if( i < min ) {
            return -1;
        }

        /* Found last character, now look at the rest of v2 */
        j = i - 1;
        start = j - (targetCount - 1);
        k = strLastIndex - 1;

        while( j > start ) {
            if( source[j] != target[k] && ( !ignorecase || 
                tolower(source[j]) != tolower(target[k]) ) ) {
                i --; j --; k --; 
                goto startSearchForLastChar;
            }
            j --; k --; 
        }

        /* Found whole string. */
        return start + 1;
    }
}

/**
 * Remove any character in input string <code>s</code> which character 
 * include in input <code>chrs</code>.
 *
 * @param s    Input string
 * @param chrs Remove characters
 */
template <typename CHAR, SIZET BUFSIZE> 
CHAR* FastString_Base<CHAR, BUFSIZE>::removechars(CHAR *s, const CHAR *chrs)
{
    CHAR *sc = NULL, *ptr = NULL; 
    CHAR *pchr = NULL, flag = 0;
    if( s == NULL || *s == 0 || chrs == NULL || *chrs == 0 ) 
        return s;
    sc = ptr = s; 
    while( *sc ) {
        pchr = (CHAR *) chrs; 
        flag = 0; 
        while( *pchr ) {
            if( *sc == *pchr ) {
                flag = 1; 
                break; 
            }
            pchr ++; 
        }
        if( flag == 1 ) {
            sc ++;
            continue; 
        }
        if( sc > ptr ) 
            *ptr = *sc; 
        ptr ++; 
        sc ++; 
    }
    *ptr = '\0'; 
    return s;
}

/**
 * Replace the character in input string <code>s</code> witch the new
 * input char <code>chr</code> , which character include in input 
 * <code>chrs</code>.
 *
 * @param s    Input string
 * @param chrs Replace characters
 * @param chr  New character
 */
template <typename CHAR, SIZET BUFSIZE> 
CHAR* FastString_Base<CHAR, BUFSIZE>::replacechars(
                        CHAR *s, const CHAR *chrs, const CHAR chr)
{
    CHAR *sc = NULL, *pchr = NULL;
    if( s == NULL || *s == 0 || chrs == NULL || *chrs == 0 ) 
        return s;
    sc = s; 
    while( *sc ) {
        pchr = (CHAR *) chrs; 
        while( *pchr ) {
            if( *sc == *pchr ) {
                *sc = chr; 
                break; 
            }
            pchr ++; 
        }
        sc ++; 
    }
    return s;
}

/**
 * Trim the character in input string <code>s</code>'s right
 * which character include in input <code>chrs</code>.
 *
 * @param s    Input string
 * @param chrs Trim characters
 */
template <typename CHAR, SIZET BUFSIZE> 
CHAR* FastString_Base<CHAR, BUFSIZE>::rtrimchars(CHAR *s, const CHAR *chrs)
{
    CHAR *sc = NULL, *ptr = NULL; 
    CHAR *pchr = NULL, flag = 0;
    if( s == NULL || *s == 0 || chrs == NULL || *chrs == 0 ) 
        return s;
    ptr = s; 
    sc = s + strlen(s) - 1; 
    while( sc > s ) {
        pchr = (CHAR *) chrs; 
        flag = 0; 
        while( *pchr ) {
            if( *sc == *pchr ) {
                flag = 1; 
                break; 
            }
            pchr ++; 
        }
        if( flag == 1 ) {
            *sc = '\0'; sc --; 
            continue; 
        }
        break;
    }
    return s;
}

/**
 * Trim the character in input string <code>s</code>'s left
 * which character include in input <code>chrs</code>.
 *
 * @param s    Input string
 * @param chrs Trim characters
 */
template <typename CHAR, SIZET BUFSIZE> 
CHAR* FastString_Base<CHAR, BUFSIZE>::ltrimchars(CHAR *s, const CHAR *chrs)
{
    CHAR *sc = NULL, *ptr = NULL; 
    CHAR *pchr = NULL, flag = 0;
    if( s == NULL || *s == 0 || chrs == NULL || *chrs == 0 ) 
        return s;
    sc = ptr = s; 
    while( *sc ) {
        pchr = (CHAR *) chrs; 
        flag = 0; 
        while( *pchr ) {
            if( *sc == *pchr ) {
                flag = 1; 
                break; 
            }
            pchr ++; 
        }
        if( flag == 1 ) {
            sc ++;
            continue; 
        }
        break;
    }
    if( sc > s ) {
        while( *sc ) {
            *ptr = *sc; 
            ptr ++; sc ++; 
        }
        *ptr = '\0'; 
        sc = ptr - 1; 
    }
    return s;
}

/**
 * Get one line in input string <code>s</code>'s from a pos
 * 从字符串中读取一行，是在原来缓冲内处理，不用分配新内存.
 *
 * @param sbuf    指向字符串缓冲
 * @param size    缓冲长度
 * @param pptr    指向下一行的起始位置
 * @param ppchr   指向本行末尾被修改为结束符的字符
 * @param chr     指向记录被修改的字符值以便恢复
 * @return        指向本行头
 */
template <typename CHAR, SIZET BUFSIZE> 
CHAR* FastString_Base<CHAR, BUFSIZE>::nextline(CHAR *sbuf, const int size, 
                                             CHAR **pptr, CHAR **ppchr, 
                                             CHAR *chr) 
{
    CHAR *ptr = NULL, *pchr = NULL; 
    
    if( sbuf == NULL || size <= 0 || pptr == NULL ) 
        return ptr; 
    
    if( (*pptr) == NULL ) (*pptr) = sbuf; 
    
    if( ppchr != NULL && chr != NULL && (*ppchr) != NULL ) {
        (**ppchr) = (*chr); 
        (*ppchr)  = NULL; 
        (*chr)    = 0; 
    }
    
    if( (*pptr) - sbuf >= size || (*pptr) < sbuf ) 
        return ptr; 
    
    ptr = pchr = (*pptr); 
    
    for(;;) {
        if( pchr - sbuf >= size ) {
            if( ((*pptr) = pchr) == ptr ) 
                return NULL; 
            
            (*ppchr) = NULL; 
            (*chr)   = 0; 
            (*pptr)  = sbuf + size; 
            
            return ptr; 
        }
        
        switch( (*pchr) ) 
        {
        case '\0': 
            if( ((*pptr) = pchr) == ptr ) 
                return NULL; 
            else 
                return ptr; 
    
        case '\r':
            if( ppchr != NULL && chr != NULL ) {
                (*ppchr) = pchr; 
                (*chr)   = (*pchr); 
            }
            
            (*pchr) = '\0'; 
            (*pptr) = (++ pchr); 
            
            if( (*pchr) == '\n' ) 
                (*pptr) = pchr + 1; 
            
            return ptr; 
        
        case '\n': 
            if( ppchr != NULL && chr != NULL ) {
                (*ppchr) = pchr; 
                (*chr)   = (*pchr); 
            }
        
            (*pchr) = '\0'; 
            (*pptr) = (++ pchr); 
            return ptr; 
        }
        pchr ++; 
    }
    
    return ptr; 
}

/**
 * Extract out value in input string <code>s</code> by name.
 * 根据分隔符分解字符串获取指定名称的内容.
 *
 * @param str     分解的字符串
 * @param substr  指定的名称
 * @param div1    名称与名称之间分隔符，缺省为“&”
 * @param div2    名称与内容之间的分隔符，缺省为“=”
 * @param buf     放置分解出的内容的空间
 * @param size    buf的长度
 * @param vdiv    多个相同名称的值之间的分割符，缺省为空
 * @param count   获取的相同名称的值的数目
 * @return        获取值的长度
 */
template <typename CHAR, SIZET BUFSIZE> 
SIZET FastString_Base<CHAR, BUFSIZE>::extractValueByName(
                 const CHAR *str, const CHAR *substr, 
                 const CHAR *div1, const CHAR *div2, 
                 CHAR *buf, const SIZET size, 
                 const CHAR *vdiv, const int count) 
{
    SIZET rsize=0, len=0; 
    int num=0, slen=0, dlen1=0, dlen2=0, vlen=0; 
    CHAR *ps=0, *p=0, *pt=0, *pk=0; 
    CHAR sdiv1[16]="&", sdiv2[16]="="; 

    //判断各种参数的合法性
    if(str == NULL || substr == NULL || buf == NULL) 
        return -1; 
    if(size == 0 || count < 0) 
        return -2; 

    if((slen = strlen(substr)) <=0) return -1; 
    if(vdiv != NULL) vlen = strlen(vdiv); 

    //判断div1, div2的合法性，不合法就采用确省值
    if(div1 != NULL) strncpy(sdiv1, div1, 16); 
    if(div2 != NULL) strncpy(sdiv2, div2, 16); 
    if(strcmp(sdiv1, sdiv2) == 0) 
        return -1; 
    dlen1 = strlen(sdiv1); 
    dlen2 = strlen(sdiv2); 
    if(dlen1 <=0 || dlen2 <=0) 
        return -1; 

    //把buf请空
    memset(buf, 0, size); 

    //开始查询
    ps = (CHAR *) str; 
    while((p=strstr(ps, substr)) != NULL) {
        //判断p是不是指向第一字符
        if((p-str) > 0) {
            if((p-str) < dlen1) {
                //否则查询下一个位置
                pk = strstr(p, sdiv1); 
                if(pk != NULL) {
                    ps = pk;
                    continue; 
                }else
                    break; 
            }
            //判断p前面有没有sdiv1分隔字符串
            pt = p - dlen1; 
            if(strncmp(pt, sdiv1, dlen1) != 0) {
                pk = strstr(p, sdiv1); 
                if(pk != NULL) {
                    ps = pk;
                    continue; 
                }else
                    break; 
            }
        }
        //判断p位置的substr后面有没有sdiv2分隔字符串
        p += slen; 
        if(strncmp(p, sdiv2, dlen2) != 0) {
            pk = strstr(p, sdiv1); 
            if(pk != NULL) {
                ps = pk;
                continue; 
            }else
                break; 
        }
        //找到了，再查找下一个sdiv1的位置
        p += dlen2; 
        pk = strstr(p, sdiv1); 
        //根据此位置判断内容及长度
        if(pk == NULL) 
            len = strlen(p); 
        else
            len = pk - p; 
        //如果内容长度超出了buf剩下的空间，只复制能容纳的长度
        if(len > (size-rsize-vlen-1)) 
            len = size - rsize - vlen -1; 
        //判断加入分隔符号vdiv
        if(vdiv != NULL && rsize >0) {
            strcat(buf, vdiv); 
            rsize += vlen; 
        }
        strncat(buf, p, len); 
        rsize += len; 
        num ++; 
        if(count >0 && num >= count) break; 
        //移动ps以便继续下一个substr的查询
        if(pk == NULL) 
            break;
        else
            ps = pk; 
    }

    return rsize; 
}

/**
 * Extract out value in input string <code>s</code> by index.
 * 根据分隔符分解字符串获取指定位置的内容.
 *
 * @param pstr    分解的字符串
 * @param len     分解的字符串长度
 * @param value   放置分解出的内容的空间
 * @param valsize 放置空间的长度
 * @param tag     多个相同名称的值之间的分割符，缺省为空
 * @param index   获取的值的指定位置
 * @return        获取值的长度
 */
template <typename CHAR, SIZET BUFSIZE> 
SIZET FastString_Base<CHAR, BUFSIZE>::extractValueByIndex(
                const CHAR *pstr, const SSIZET len, 
                CHAR *value, const SSIZET valsize, 
                const CHAR tag, const int index)
{
    register int i = 0; 
    int count = index + 1; 
    CHAR *ptr = (CHAR *) pstr; 
    CHAR *p   = (CHAR *) pstr; 
    
    if( pstr == NULL || len <= 0 || value == NULL || valsize <= 0 || 
        index < 0 || index >= len ) 
        return -1; 
    
    memset(value, 0, valsize); 
    
    for( i = 0; i < len; i ++ ) {
        if( (*p) == tag ) {
            count --; 
            if( count == 1 ) 
                ptr = p + 1; 
            else if( count <= 0 ) 
                break; 
        }
        p ++; 
    }
    
    if( count > 1 ) 
        return -2; 
    
    if( p == NULL || ptr == NULL || p < ptr ) 
        return -3; 
        
    if( p - ptr > valsize ) 
        return -4; 
        
    if( p - ptr > 0 ) 
        strncpy(value, ptr, (p-ptr)); 
    
    return 0; 
}

/**
 * Extract out value in input string <code>s</code> by name.
 * 根据分隔符“&”分解字符串获取指定名称的内容（name1=value1&name2=value2）.
 *
 * @param str     分解的字符串
 * @param substr  指定的名称
 * @param value   获取的整数值
 * @param defval  缺省的值
 * @return        获取值的长度
 */
template <typename CHAR, SIZET BUFSIZE> 
SIZET FastString_Base<CHAR, BUFSIZE>::extractIntegerByName(
                                const CHAR *str, const CHAR *substr, 
                                long *value, const long defval) 
{
    SIZET size=0; 
    CHAR strValue[32]; 

    size = FastString_Base<CHAR, BUFSIZE>::extractValueByName(
                    str, substr, NULL, NULL, 
                    strValue, sizeof(strValue), NULL, 1); 
    
    if( strlen(strValue) > 0 ) 
        *value = atol(strValue); 
    else
        *value = defval; 
    
    return size; 
}

/**
 * Extract out value in input string <code>s</code> by name.
 * 根据分隔符“&”分解字符串获取指定名称的内容（name1=value1&name2=value2）.
 *
 * @param str     分解的字符串
 * @param substr  指定的名称
 * @param value   获取的字符串值
 * @param size    存储获取的值的空间长度
 * @return        获取值的长度
 */
template <typename CHAR, SIZET BUFSIZE> 
inline SIZET FastString_Base<CHAR, BUFSIZE>::extractValueByName(
                                const CHAR *str, const CHAR *substr, 
                                CHAR *value, const SSIZET size, 
                                const int count) 
{
    return FastString_Base<CHAR, BUFSIZE>::extractValueByName(
                str, substr, 
                NULL, NULL, value, size, NULL, count); 
}

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the string being searched for.
 *
 * @param   source       the characters being searched.
 * @param   target       the characters being searched for.
 * @param   fromIndex    the index to begin searching from.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline int FastString_Base<CHAR, BUFSIZE>::findIndexOf(
                   const CHAR *source, const CHAR *target, 
                   SSIZET fromIndex) 
{
    SSIZET sourceCount = 0, targetCount = 0; 
    if( source ) sourceCount = strlen(source); 
    if( target ) targetCount = strlen(target); 
    return FastString_Base<CHAR, BUFSIZE>::findIndexOf(
            source, sourceCount, target, targetCount, fromIndex, 0); 
}

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the string being searched for, and ignore word case.
 *
 * @param   source       the characters being searched.
 * @param   sourceCount  count of the source string.
 * @param   target       the characters being searched for.
 * @param   targetCount  count of the target string.
 * @param   fromIndex    the index to begin searching from.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline int FastString_Base<CHAR, BUFSIZE>::findIndexOfIgnoreCase(
                   const CHAR *source, SSIZET sourceCount, 
                   const CHAR *target, SSIZET targetCount,
                   SSIZET fromIndex) 
{
    return FastString_Base<CHAR, BUFSIZE>::findIndexOf(
            source, sourceCount, target, targetCount, fromIndex, 1); 
}

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the string being searched for, and ignore word case.
 *
 * @param   source       the characters being searched.
 * @param   target       the characters being searched for.
 * @param   fromIndex    the index to begin searching from.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline int FastString_Base<CHAR, BUFSIZE>::findIndexOfIgnoreCase(
                   const CHAR *source, const CHAR *target,
                   SSIZET fromIndex) 
{
    SSIZET sourceCount = 0, targetCount = 0; 
    if( source ) sourceCount = strlen(source); 
    if( target ) targetCount = strlen(target); 
    return FastString_Base<CHAR, BUFSIZE>::findIndexOf(
            source, sourceCount, target, targetCount, fromIndex, 1); 
}

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the string being searched for.
 *
 * @param   source       the characters being searched.
 * @param   target       the characters being searched for.
 * @param   fromIndex    the index to begin searching from.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline int FastString_Base<CHAR, BUFSIZE>::findLastIndexOf(
                       const CHAR *source, const CHAR *target,
                       SSIZET fromIndex) 
{
    SSIZET sourceCount = 0, targetCount = 0; 
    if( source ) sourceCount = strlen(source); 
    if( target ) targetCount = strlen(target); 
    return FastString_Base<CHAR, BUFSIZE>::findLastIndexOf(
            source, sourceCount, target, targetCount, fromIndex, 0); 
}

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the string being searched for, and ignore word case.
 *
 * @param   source       the characters being searched.
 * @param   sourceCount  count of the source string.
 * @param   target       the characters being searched for.
 * @param   targetCount  count of the target string.
 * @param   fromIndex    the index to begin searching from.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline int FastString_Base<CHAR, BUFSIZE>::findLastIndexOfIgnoreCase(
                       const CHAR *source, SSIZET sourceCount, 
                       const CHAR *target, SSIZET targetCount,
                       SSIZET fromIndex) 
{
    return FastString_Base<CHAR, BUFSIZE>::findLastIndexOf(
            source, sourceCount, target, targetCount, fromIndex, 1); 
}

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the string being searched for, and ignore word case.
 *
 * @param   source       the characters being searched.
 * @param   target       the characters being searched for.
 * @param   fromIndex    the index to begin searching from.
 */
template <typename CHAR, SIZET BUFSIZE> 
inline int FastString_Base<CHAR, BUFSIZE>::findLastIndexOfIgnoreCase(
                       const CHAR *source, const CHAR *target,
                       SSIZET fromIndex) 
{
    SSIZET sourceCount = 0, targetCount = 0; 
    if( source ) sourceCount = strlen(source); 
    if( target ) targetCount = strlen(target); 
    return FastString_Base<CHAR, BUFSIZE>::findLastIndexOf(
            source, sourceCount, target, targetCount, fromIndex, 1); 
}

/**
 * Remove any character in this string which character 
 * include in input <code>chrs</code>.
 *
 * @param chrs Remove characters
 */
template <typename CHAR, SIZET BUFSIZE> 
inline FastString_Base<CHAR, BUFSIZE>& 
FastString_Base<CHAR, BUFSIZE>::removeChars(const CHAR *chrs)
{
    if( this->m_nLen > 0 && chrs ) 
    {
        FastString_Base<CHAR, BUFSIZE>::removechars(this->m_psRep, chrs); 
        this->m_nLen = strlen(this->m_psRep); 
    }

    return *this; 
}

/**
 * Remove the specify character in this string which character 
 * include in input <code>chrs</code>.
 *
 * @param c  Remove character
 */
template <typename CHAR, SIZET BUFSIZE> 
FastString_Base<CHAR, BUFSIZE>& 
FastString_Base<CHAR, BUFSIZE>::removeChar(const CHAR c)
{
    SIZET i = 0, j = 0; 

    for( i = 0, j = 0; i < this->m_nLen; i ++ ) 
    {
        if( this->m_psRep[i] != c ) 
        {
            if( i > j )
                this->m_psRep[j] = this->m_psRep[i]; 
            j ++; 
        }
    }
    if( j < this->m_nLen ) 
    {
        this->m_nLen = j; 
        this->m_psRep[this->m_nLen] = '\0'; 
    }

    return *this; 
}

/**
 * Replace the character in this string witch the new
 * input CHAR <code>chr</code> , which character include in input 
 * <code>chrs</code>.
 *
 * @param chrs Replace characters
 * @param chr  New character
 */
template <typename CHAR, SIZET BUFSIZE> 
inline FastString_Base<CHAR, BUFSIZE>& 
FastString_Base<CHAR, BUFSIZE>::replaceChars(const CHAR *chrs, const CHAR chr)
{
    if( this->m_nLen > 0 && chrs ) 
    {
        FastString_Base<CHAR, BUFSIZE>::replacechars(this->m_psRep, chrs, chr); 
        this->m_nLen = strlen(this->m_psRep); 
    }

    return *this; 
}

/**
 * Trim the character in this string's left and right
 * which character include in input <code>chrs</code>.
 *
 * @param chrs Trim characters
 */
template <typename CHAR, SIZET BUFSIZE> 
inline FastString_Base<CHAR, BUFSIZE>& 
FastString_Base<CHAR, BUFSIZE>::trimChars(const CHAR *chrs)
{
    if( this->m_nLen > 0 && chrs ) 
    {
        FastString_Base<CHAR, BUFSIZE>::trimchars(this->m_psRep, chrs); 
        this->m_nLen = strlen(this->m_psRep); 
    }

    return *this; 
}

/**
 * Trim the character in this string's left
 * which character include in input <code>chrs</code>.
 *
 * @param chrs Trim characters
 */
template <typename CHAR, SIZET BUFSIZE> 
inline FastString_Base<CHAR, BUFSIZE>& 
FastString_Base<CHAR, BUFSIZE>::ltrimChars(const CHAR *chrs)
{
    if( this->m_nLen > 0 && chrs ) 
    {
        FastString_Base<CHAR, BUFSIZE>::ltrimchars(this->m_psRep, chrs); 
        this->m_nLen = strlen(this->m_psRep); 
    }

    return *this; 
}

/**
 * Trim the character in this string's right
 * which character include in input <code>chrs</code>.
 *
 * @param chrs Trim characters
 */
template <typename CHAR, SIZET BUFSIZE> 
inline FastString_Base<CHAR, BUFSIZE>& 
FastString_Base<CHAR, BUFSIZE>::rtrimChars(const CHAR *chrs)
{
    if( this->m_nLen > 0 && chrs ) 
    {
        FastString_Base<CHAR, BUFSIZE>::rtrimchars(this->m_psRep, chrs); 
        this->m_nLen = strlen(this->m_psRep); 
    }

    return *this; 
}

/**
 * Trim the space and tab character in this string's right.
 *
 */
template <typename CHAR, SIZET BUFSIZE> 
inline FastString_Base<CHAR, BUFSIZE>& 
FastString_Base<CHAR, BUFSIZE>::rtrim()
{
    if( this->m_nLen > 0 ) 
    {
        FastString_Base<CHAR, BUFSIZE>::rtrim(this->m_psRep); 
        this->m_nLen = strlen(this->m_psRep); 
    }

    return *this; 
}

/**
 * Trim the space and tab character in this string's left.
 *
 */
template <typename CHAR, SIZET BUFSIZE> 
inline FastString_Base<CHAR, BUFSIZE>& 
FastString_Base<CHAR, BUFSIZE>::ltrim()
{
    if( this->m_nLen > 0 ) 
    {
        FastString_Base<CHAR, BUFSIZE>::ltrim(this->m_psRep); 
        this->m_nLen = strlen(this->m_psRep); 
    }

    return *this; 
}

/**
 * Trim the space and tab character in this string's right.
 *
 */
template <typename CHAR, SIZET BUFSIZE> 
inline FastString_Base<CHAR, BUFSIZE>& 
FastString_Base<CHAR, BUFSIZE>::trimRight()
{
    return this->rtrim(); 
}

/**
 * Trim the space and tab character in this string's left.
 *
 */
template <typename CHAR, SIZET BUFSIZE> 
inline FastString_Base<CHAR, BUFSIZE>& 
FastString_Base<CHAR, BUFSIZE>::trimLeft()
{
    return this->ltrim(); 
}

/**
 * Trim the space and tab character in this string's right.
 *
 */
template <typename CHAR, SIZET BUFSIZE> 
inline FastString_Base<CHAR, BUFSIZE>& 
FastString_Base<CHAR, BUFSIZE>::trimRight(SSIZET offset)
{
    if( offset >= 0 && offset < (SSIZET)this->m_nLen ) 
    {
        this->m_nLen = offset; 
        this->m_psRep[this->m_nLen] = '\0'; 
    }

    return *this; 
}

/**
 * Trim the space and tab character in this string's left.
 *
 */
template <typename CHAR, SIZET BUFSIZE> 
FastString_Base<CHAR, BUFSIZE>& 
FastString_Base<CHAR, BUFSIZE>::trimLeft(SSIZET offset)
{
    if( offset > 0 && offset <= (SSIZET)this->m_nLen ) 
    {
        for( int i = offset, j = 0; i < (int) this->m_nLen; i ++, j ++ ) 
        {
            this->m_psRep[j] = this->m_psRep[i]; 
        }
        this->m_nLen = this->m_nLen - offset; 
        this->m_psRep[this->m_nLen] = '\0'; 
    }

    return *this; 
}

/**
 * Trim the space and tab character in this string's left and right.
 *
 */
template <typename CHAR, SIZET BUFSIZE> 
inline FastString_Base<CHAR, BUFSIZE>& 
FastString_Base<CHAR, BUFSIZE>::trim()
{
    if( this->m_nLen > 0 ) 
    {
        FastString_Base<CHAR, BUFSIZE>::trim(this->m_psRep); 
        this->m_nLen = strlen(this->m_psRep); 
    }

    return *this; 
}

/**
 * Trim the character in input string <code>s</code>'s left and right
 * which character include in input <code>chrs</code>.
 *
 * @param s    Input string
 * @param chrs Trim characters
 */
template <typename CHAR, SIZET BUFSIZE> 
inline CHAR* FastString_Base<CHAR, BUFSIZE>::trimchars(
                    CHAR *s, const CHAR *chrs)
{
    if( s && chrs ) 
    {
        ltrimchars(s, chrs); 
        rtrimchars(s, chrs); 
    }
    return s; 
}

/**
 * Trim the space and tab character in input string <code>s</code>'s 
 * left and right.
 *
 * @param s    Input string
 */
template <typename CHAR, SIZET BUFSIZE> 
inline CHAR* FastString_Base<CHAR, BUFSIZE>::trim(CHAR *s)
{
    if( s ) 
    {
        ltrimchars(s, " \t"); 
        rtrimchars(s, " \t"); 
    }
    return s; 
}

/**
 * Trim the space and tab character in input string <code>s</code>'s 
 * left.
 *
 * @param s    Input string
 */
template <typename CHAR, SIZET BUFSIZE> 
inline CHAR* FastString_Base<CHAR, BUFSIZE>::ltrim(CHAR *s)
{
    if( s ) 
        ltrimchars(s, " \t"); 
    return s; 
}

/**
 * Trim the space and tab character in input string <code>s</code>'s 
 * right.
 *
 * @param s    Input string
 */
template <typename CHAR, SIZET BUFSIZE> 
inline CHAR* FastString_Base<CHAR, BUFSIZE>::rtrim(CHAR *s)
{
    if( s ) 
        rtrimchars(s, " \t"); 
    return s; 
}




_FAST_END_NAMESPACE

#endif

