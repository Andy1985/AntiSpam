//=============================================================================
/**
 *  @file    StringTokenizer.h
 *
 *  ver 1.0.0 2004/12/20 Naven Exp, for Fast Common Framework.
 *
 *  @author Naven (Navy Chen) 2004/12/20 created.
 */
//=============================================================================

#ifndef _FAST_COMM_STRINGTOKENIZER_H
#define _FAST_COMM_STRINGTOKENIZER_H

#if defined(_WIN32) || defined(__WIN32__)

#if !defined (NAVEN_PRAGMA_ONCE)
# pragma once
#endif /* NAVEN_PRAGMA_ONCE */

#endif /* _WIN32 */


#include "FastBase.h"



_FAST_BEGIN_NAMESPACE



// Forward declaration.

template <class StringT> 
class StringTokenizer_Base;

typedef StringTokenizer_Base<FastString> StringTokenizer; 



//================class StringTokenizer define=================

/**
 * The string tokenizer class allows an application to break a 
 * string into tokens. The tokenization method is much simpler than 
 * the one used by the <code>StreamTokenizer</code> class. The 
 * <code>StringTokenizer</code> methods do not distinguish among 
 * identifiers, numbers, and quoted strings, nor do they recognize 
 * and skip comments. 
 * <p>
 * The set of delimiters (the characters that separate tokens) may 
 * be specified either at creation time or on a per-token basis. 
 * <p>
 * An instance of <code>StringTokenizer</code> behaves in one of two 
 * ways, depending on whether it was created with the 
 * <code>returnDelims</code> flag having the value <code>true</code> 
 * or <code>false</code>: 
 * <ul>
 * <li>If the flag is <code>false</code>, delimiter characters serve to 
 *     separate tokens. A token is a maximal sequence of consecutive 
 *     characters that are not delimiters. 
 * <li>If the flag is <code>true</code>, delimiter characters are themselves 
 *     considered to be tokens. A token is thus either one delimiter 
 *     character, or a maximal sequence of consecutive characters that are 
 *     not delimiters.
 * </ul><p>
 * A <tt>StringTokenizer</tt> object internally maintains a current 
 * position within the string to be tokenized. Some operations advance this 
 * current position past the characters processed.<p>
 * A token is returned by taking a substring of the string that was used to 
 * create the <tt>StringTokenizer</tt> object.
 * <p>
 * The following is one example of the use of the tokenizer. The code:
 * <blockquote><pre>
 *     StringTokenizer st = new StringTokenizer("this is a test");
 *     while (st.hasMoreTokens()) {
 *         System.out.println(st.nextToken());
 *     }
 * </pre></blockquote>
 * <p>
 * prints the following output:
 * <blockquote><pre>
 *     this
 *     is
 *     a
 *     test
 * </pre></blockquote>
 *
 * <p>
 * <tt>StringTokenizer</tt> is a legacy class that is retained for
 * compatibility reasons although its use is discouraged in new code. It is
 * recommended that anyone seeking this functionality use the <tt>split</tt>
 * method of <tt>String</tt> or the java.util.regex package instead.
 * <p>
 * The following example illustrates how the <tt>String.split</tt>
 * method can be used to break up a string into its basic tokens:
 * <blockquote><pre>
 *     String[] result = "this is a test".split("\\s");
 *     for (int x=0; x&lt;result.length; x++)
 *         System.out.println(result[x]);
 * </pre></blockquote>
 * <p>
 * prints the following output:
 * <blockquote><pre>
 *     this
 *     is
 *     a
 *     test
 * </pre></blockquote>
 *
 * @see     java.util.StringTokenizer
 */
template <class StringT> 
class StringTokenizer_Base 
{
public: 
    // Traits 
    typedef StringT string_type; 
    typedef typename StringT::char_type char_type; 
    
protected: 
    int _currentPosition;
    int _newPosition;
    int _maxPosition;
    string_type &_str;
    string_type _delimiters;
    BOOL _retDelims;
    BOOL _delimsChanged; 
    
    /**
     * maxDelimChar stores the value of the delimiter character with the
     * highest value. It is used to optimize the detection of delimiter
     * characters.
     */
    char_type _maxDelimChar;
    
    /**
     * Set maxDelimChar to the highest char in the delimiter set.
     */
    void setMaxDelimChar(); 
    
    /**
     * Skips delimiters starting from the specified position. If retDelims
     * is false, returns the index of the first non-delimiter character at or
     * after startPos. If retDelims is true, startPos is returned.
     */
    int skipDelimiters(int startPos); 
    
    /**
     * Skips ahead from startPos and returns the index of the next delimiter
     * character encountered, or maxPosition if no such delimiter is found.
     */
    int scanToken(int startPos); 
    
public: 
    /**
     * Constructs a string tokenizer for the specified string. All  
     * characters in the <code>delim</code> argument are the delimiters 
     * for separating tokens. 
     * <p>
     * If the <code>returnDelims</code> flag is <code>true</code>, then 
     * the delimiter characters are also returned as tokens. Each 
     * delimiter is returned as a string of length one. If the flag is 
     * <code>false</code>, the delimiter characters are skipped and only 
     * serve as separators between tokens. 
     * <p>
     * Note that if <tt>delim</tt> is <tt>null</tt>, this constructor does
     * not throw an exception. However, trying to invoke other methods on the
     * resulting <tt>StringTokenizer</tt> may result in a 
     * <tt>NullPointerException</tt>.
     *
     * @param   str            a string to be parsed.
     * @param   delim          the delimiters.
     * @param   returnDelims   flag indicating whether to return the delimiters
     *                         as tokens.
     */
    StringTokenizer_Base(string_type &str, string_type delim, 
                         BOOL returnDelims = FALSE) 
     : 	_currentPosition( 0 ), 
	_newPosition( -1 ), 
	_maxPosition( str.length() ), 
	_str( str ), 
	_delimiters( delim ), 
	_retDelims( returnDelims ), 
	_delimsChanged( FALSE ), 
	_maxDelimChar( 0 ) 
    {
        setMaxDelimChar();
    }
    
    /**
     * Constructs a string tokenizer for the specified string. The 
     * tokenizer uses the default delimiter set, which is 
     * <code>"&nbsp;&#92;t&#92;n&#92;r&#92;f"</code>: the space character, 
     * the tab character, the newline character, the carriage-return character,
     * and the form-feed character. Delimiter characters themselves will 
     * not be treated as tokens.
     *
     * @param   str   a string to be parsed.
     */
    StringTokenizer_Base(string_type &str, 
                         BOOL returnDelims = FALSE) 
     : 	_currentPosition( 0 ), 
	_newPosition( -1 ), 
	_maxPosition( str.length() ), 
	_str( str ), 
	_delimiters( (const char_type*)(" \t\n\r\f") ), 
	_retDelims( returnDelims ), 
	_delimsChanged( FALSE ), 
	_maxDelimChar( 0 ) 
    {
        setMaxDelimChar();
    }
    
    /**
     * Tests if there are more tokens available from this tokenizer's string. 
     * If this method returns <tt>true</tt>, then a subsequent call to 
     * <tt>nextToken</tt> with no argument will successfully return a token.
     *
     * @return  <code>true</code> if and only if there is at least one token 
     *          in the string after the current position; <code>false</code> 
     *          otherwise.
     */
    BOOL hasMoreTokens() 
    {
	/*
	 * Temporary store this position and use it in the following
	 * nextToken() method only if the delimiters have'nt been changed in
	 * that nextToken() invocation.
	 */
	_newPosition = skipDelimiters(_currentPosition);
	return (_newPosition < _maxPosition) ? TRUE : FALSE;
    }
    
    /**
     * Returns the next token from this string tokenizer.
     *
     * @return     the next token from this string tokenizer.
     * @exception  NoSuchElementException  if there are no more tokens in this
     *               tokenizer's string.
     */
    string_type nextToken(); 
    
    /**
     * Returns the next token in this string tokenizer's string. First, 
     * the set of characters considered to be delimiters by this 
     * <tt>StringTokenizer</tt> object is changed to be the characters in 
     * the string <tt>delim</tt>. Then the next token in the string
     * after the current position is returned. The current position is 
     * advanced beyond the recognized token.  The new delimiter set 
     * remains the default after this call. 
     *
     * @param      delim   the new delimiters.
     * @return     the next token, after switching to the new delimiter set.
     * @exception  NoSuchElementException  if there are no more tokens in this
     *               tokenizer's string.
     */
    string_type nextToken(string_type &delim) 
    {
	_delimiters = delim;

	/* delimiter string specified, so set the appropriate flag. */
	_delimsChanged = TRUE;

        setMaxDelimChar();
	return nextToken();
    }
    
    /**
     * Returns the same value as the <code>hasMoreTokens</code>
     * method. It exists so that this class can implement the
     * <code>Enumeration</code> interface. 
     *
     * @return  <code>true</code> if there are more tokens;
     *          <code>false</code> otherwise.
     * @see     java.util.Enumeration
     * @see     java.util.StringTokenizer#hasMoreTokens()
     */
    BOOL hasMoreElements() { return hasMoreTokens(); } 
    
    /**
     * Returns the same value as the <code>nextToken</code> method,
     * except that its declared return value is <code>Object</code> rather than
     * <code>String</code>. It exists so that this class can implement the
     * <code>Enumeration</code> interface. 
     *
     * @return     the next token in the string.
     * @exception  NoSuchElementException  if there are no more tokens in this
     *               tokenizer's string.
     * @see        java.util.Enumeration
     * @see        java.util.StringTokenizer#nextToken()
     */
    string_type nextElement() { return nextToken(); } 
    
    /**
     * Calculates the number of times that this tokenizer's 
     * <code>nextToken</code> method can be called before it generates an 
     * exception. The current position is not advanced.
     *
     * @return  the number of tokens remaining in the string using the current
     *          delimiter set.
     * @see     java.util.StringTokenizer#nextToken()
     */
    int countTokens(); 
    
    /**
     * Dump the object's state.
     */
    void dump(); 
    
}; 


//============StringTokenizer Functions Implements================

/**
 * Set maxDelimChar to the highest char in the delimiter set.
 */
template <class StringT> void 
StringTokenizer_Base<StringT>::setMaxDelimChar() 
{
    if( _delimiters.length() == 0 ) 
    {
        _maxDelimChar = 0;
        return;
    }

    char_type m = 0;
    for( int i = 0; i < (int)_delimiters.length(); i ++ ) 
    {
        char_type c = _delimiters.charAt(i);
        if( m < c )
    	m = c;
    }
    _maxDelimChar = m;
}

/**
 * Skips delimiters starting from the specified position. If retDelims
 * is false, returns the index of the first non-delimiter character at or
 * after startPos. If retDelims is true, startPos is returned.
 */
template <class StringT> int 
StringTokenizer_Base<StringT>::skipDelimiters(int startPos) 
{
    if( _delimiters.length() == 0 )
        //throw new NullPointerException(); 
        return -1; 

    int position = startPos;
    while( !_retDelims && position < _maxPosition ) 
    {
        char_type c = _str.charAt(position);
        if( (c > _maxDelimChar) || (_delimiters.indexOf(c) < 0) )
            break;
        position ++;
    }
    return position;
}

/**
 * Skips ahead from startPos and returns the index of the next delimiter
 * character encountered, or maxPosition if no such delimiter is found.
 */
template <class StringT> int 
StringTokenizer_Base<StringT>::scanToken(int startPos) 
{
    int position = startPos;
    while( position < _maxPosition ) 
    {
        char_type c = _str.charAt(position);
        if( (c <= _maxDelimChar) && (_delimiters.indexOf(c) >= 0) )
            break;
        position ++;
    }
    if( _retDelims && (startPos == position) ) 
    {
        char_type c = _str.charAt(position);
        if( (c <= _maxDelimChar) && (_delimiters.indexOf(c) >= 0) )
            position ++;
    }
    return position;
}

/**
 * Returns the next token from this string tokenizer.
 *
 * @return     the next token from this string tokenizer.
 * @exception  NoSuchElementException  if there are no more tokens in this
 *               tokenizer's string.
 */
template <class StringT> StringT 
StringTokenizer_Base<StringT>::nextToken() 
{
    /* 
     * If next position already computed in hasMoreElements() and
     * delimiters have changed between the computation and this invocation,
     * then use the computed value.
     */
    _currentPosition = (_newPosition >= 0 && !_delimsChanged) ?  
                        _newPosition : skipDelimiters(_currentPosition);
    
    /* Reset these anyway */
    _delimsChanged = FALSE;
    _newPosition = -1;
    
    if( _currentPosition >= _maxPosition )
        //throw new NoSuchElementException(); 
        return string_type(); 
    
    int start = _currentPosition;
    _currentPosition = scanToken(_currentPosition); 
    
    string_type retstr; 
    _str.substring(retstr, start, _currentPosition - start); 
    return retstr; 
}

/**
 * Calculates the number of times that this tokenizer's 
 * <code>nextToken</code> method can be called before it generates an 
 * exception. The current position is not advanced.
 *
 * @return  the number of tokens remaining in the string using the current
 *          delimiter set.
 * @see     java.util.StringTokenizer#nextToken()
 */
template <class StringT> int 
StringTokenizer_Base<StringT>::countTokens() 
{
    int count = 0;
    int currpos = _currentPosition;
    while( currpos < _maxPosition ) 
    {
        currpos = skipDelimiters(currpos);
        if( currpos >= _maxPosition )
    	    break;
        currpos = scanToken(currpos);
        count ++; 
    }
    return count;
}

/**
 * Dump the object's state.
 */
template <class StringT> inline void 
StringTokenizer_Base<StringT>::dump() 
{
    FAST_TRACE_BEGIN("StringTokenizer::dump()"); 
    FAST_TRACE("sizeof(StringTokenizer) = %d", sizeof(StringTokenizer_Base<StringT>)); 
    FAST_TRACE("this -> 0x%08X", this); 
    FAST_TRACE_END("StringTokenizer::dump()"); 
}


_FAST_END_NAMESPACE

#endif
