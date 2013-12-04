//=============================================================================
/**
 *  @file    MimeParser.cpp
 *
 *  ver 1.0.0 2004/06/16 Naven Exp, for Mime Message Parse Engine.
 *
 *  @author Naven 2004/06/16 created.
 */
//=============================================================================

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#if defined(_WIN32) || defined(__WIN32__)
    #include <io.h>
#else
#endif 

#include "MimeObject.h"
#include "MimeParser.h" 


_FASTMIME_BEGIN_NAMESPACE


// Initialize Class Static Members

HeaderTokenizer::Token HeaderTokenizer::EOFToken(HeaderTokenizer::Token::TOKEN_EOF, FastString());

const char * HeaderTokenizer::MIME      = SPECIALS_MIME; 
const char * HeaderTokenizer::RFC822    = SPECIALS_RFC822; 

/*  Day names must be Three character abbreviations strung together */
const char MailDateFormat::_dnames[] = {
        "SunMonTueWedThuFriSat"
};
/*  Month names must be Three character abbreviations strung together */
const char MailDateFormat::_mnames[] = {
        "JanFebMarAprMayJunJulAugSepOctNovDec"
};

const int MailDateFormat::_lpdays[] = {
        -1, 30, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365
};

const int MailDateFormat::_days[] = {
        -1, 30, 58, 89, 119, 150, 180, 211, 242, 272, 303, 333, 364
};

const long MailDateFormat::DAY_SEC           = (24 * 60 * 60);                      // secs in a day
const long MailDateFormat::YEAR_SEC          = (365 * MailDateFormat::DAY_SEC);     // secs in a year
const long MailDateFormat::FOUR_YEAR_SEC     = (1461 * MailDateFormat::DAY_SEC);    // secs in a 4 year interval
const long MailDateFormat::DEC_SEC           = 315532800;                           // secs in 1970-1979
const long MailDateFormat::BASE_YEAR         = 70;                                  // 1970 is the base year
const long MailDateFormat::BASE_DOW          = 4;                                   // 01-01-70 was a Thursday
const long MailDateFormat::LEAP_YEAR_ADJUST  = 17;                                  // Leap years 1900 - 1970
const long MailDateFormat::MAX_YEAR          = 138;                                 // 2038 is the max year


// Globale Variables



//===========HeaderTokenizer Functions Implements=============

/*
 * Return the next token starting from 'currentPos'. After the
 * parse, 'currentPos' is updated to point to the start of the 
 * next token.
 */
int HeaderTokenizer::getNext(Token &tk) 
{
    // If we're already at end of string, return EOF
    if( m_nCurrentPos >= m_nMaxPos ) 
    {
        tk = EOFToken; 
        return 0; 
    }

    // Skip white-space, position currentPos beyond the space
    if( skipWhiteSpace() == Token::TOKEN_EOF ) 
    {
        tk = EOFToken; 
        return 0; 
    }

    char c = '\0'; 
    int start = 0; 
    BOOL filter = FALSE;
    
    c = m_sString.charAt(m_nCurrentPos);

    // Check or Skip comments and position currentPos
    // beyond the comment
    while( c == '(' ) 
    {
        // Parsing comment ..
        int nesting;
        for( start = ++ m_nCurrentPos, nesting = 1; 
             nesting > 0 && m_nCurrentPos < m_nMaxPos;
             m_nCurrentPos ++ ) 
        {
            c = m_sString.charAt(m_nCurrentPos);
            if( c == '\\' ) 
            {
                // Escape sequence
                m_nCurrentPos ++; // skip the escaped character
                filter = TRUE;
            } 
            else if( c == '\r' )
                filter = TRUE;
            else if( c == '(' )
                nesting ++;
            else if( c == ')' )
                nesting --; 
        }
        if( nesting != 0 )
            //throw new ParseException("Unbalanced comments");
            return -1; 

        if( !m_bSkipComments ) 
        {
            // Return the comment, if we are asked to.
            // Note that the comment start & end markers are ignored.
            FastString s;
            if( filter ) // need to go thru the token again.
                filterToken(m_sString, s, start, m_nCurrentPos-1);
            else
                m_sString.substring(s, start, m_nCurrentPos-1-start);

            tk.set(Token::COMMENT, s); 
            return m_nCurrentPos; 
        }

        // Skip any whitespace after the comment.
        if( skipWhiteSpace() == Token::TOKEN_EOF ) 
        {
            tk = EOFToken; 
            return 0; 
        }

        c = m_sString.charAt(m_nCurrentPos);
    }

    // Check for quoted-string and position currentPos 
    //  beyond the terminating quote
    if( c == '"' ) 
    {
        for( start = ++ m_nCurrentPos; m_nCurrentPos < m_nMaxPos; m_nCurrentPos ++ ) 
        {
            c = m_sString.charAt(m_nCurrentPos);
            if( c == '\\' ) 
            {
                // Escape sequence
                m_nCurrentPos ++;
                filter = TRUE;
            }
            else if( c == '\r' )
                filter = TRUE;
            else if( c == '"' ) 
            {
                m_nCurrentPos ++;

                FastString s;
                if( filter )
                    filterToken(m_sString, s, start, m_nCurrentPos-1);
                else
                    m_sString.substring(s, start, m_nCurrentPos-1-start);

                tk.set(Token::QUOTEDSTRING, s); 
                return m_nCurrentPos; 
            }
        }

        //throw new ParseException("Unbalanced quoted string"); 
        return -1; 
    }
    
    // Check for SPECIAL or CTL
    if( c < 040 || c >= 0177 || m_sDelimiters.indexOf(c) >= 0 ) 
    {
        m_nCurrentPos ++; // re-position currentPos
        FastString sch; sch.append(c); 
        tk.set((int)c, sch); 
        return m_nCurrentPos; 
    }

    // Check for ATOM
    for( start = m_nCurrentPos; m_nCurrentPos < m_nMaxPos; m_nCurrentPos ++ ) 
    {
        c = m_sString.charAt(m_nCurrentPos); 

        // ATOM is delimited by either SPACE, CTL, "(", <"> 
        // or the specified SPECIALS
        if( c < 040 || c >= 0177 || c == '(' || c == ' ' || c == '"' || 
            m_sDelimiters.indexOf(c) >= 0 )
            break;
    }

    FastString left_str; 
    m_sString.substring(left_str, start, m_nCurrentPos-start); 
    tk.set(Token::ATOM, left_str);
    return m_nCurrentPos; 
}

// Skip SPACE, HT, CR and NL
int HeaderTokenizer::skipWhiteSpace() 
{
    char c = '\0';
    for( ; m_nCurrentPos < m_nMaxPos; m_nCurrentPos ++ ) 
    {
        c = m_sString.charAt(m_nCurrentPos); 
        if( (c != ' ') && (c != '\t') && (c != '\r') && (c != '\n') ) 
            return m_nCurrentPos;
    }
    return Token::TOKEN_EOF;
}

/** 
 * Process escape sequences and embedded LWSPs from a comment or
 * quoted string.
 * 
 * @param s         input string to escape
 * @param output    output string that escaped
 * @param start     start position at input string
 * @param end       end position at input string
 */
void HeaderTokenizer::filterToken(
        FastString &s, FastString &output, int start, int end) 
{
    char c = '\0';
    BOOL gotEscape = FALSE;
    BOOL gotCR = FALSE; 

    output.clear(); 
    output.resize(s.length()); 

    for( int i = start; i < end; i ++ ) 
    {
        c = s.charAt(i);
        if( c == '\n' && gotCR ) 
        {
            // This LF is part of an unescaped 
            // CRLF sequence (i.e, LWSP). Skip it.
            gotCR = FALSE;
            continue;
        }

        gotCR = FALSE;
        if( !gotEscape ) 
        {
            // Previous character was NOT '\'
            if( c == '\\' ) // skip this character
                gotEscape = TRUE;
            else if (c == '\r') // skip this character
                gotCR = TRUE;
            else // append this character
                output.append(c);
        } 
        else 
        {
            // Previous character was '\'. So no need to 
            // bother with any special processing, just 
            // append this character
            output.append(c);
            gotEscape = FALSE;
        }
    }
}


//===========MailDateParser Functions Implements=============

/**
 * skips chars until it finds a number (0-9)
 *
 * if it does not find a number, it will return
 * -1 else return 0
 */    
int MailDateParser::skipUntilNumber()
{
    if( m_pOrig == NULL ) 
        return failed(); 
    while( m_nIndex < m_nLength ) 
    {
        switch( m_pOrig[m_nIndex] ) 
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                return 0;

            default:
                m_nIndex ++;
                break;    
        }
    }
    return failed(); 
}

/**
 * skips any number of tabs, spaces, CR, and LF - folding whitespace
 */
void MailDateParser::skipWhiteSpace() 
{
    if( m_pOrig == NULL ) 
        return; 
    while( m_nIndex < m_nLength ) 
    {
        switch(m_pOrig[m_nIndex]) 
        {
            case ' ':  // space
            case '\t': // tab
            case '\r': // CR
            case '\n': // LF
                m_nIndex ++;
                break;

            default:
                return;
        }
    }
}

/**
 * used to look at the next character without "parsing" that
 * character.
 */
int MailDateParser::peekChar()
{
    if( m_nIndex < m_nLength )
        return m_pOrig[m_nIndex];
    else
        //throw new ParseException("No more characters", index);
        return 0; 
}

/**
 * skips the given character.  if the current char does not
 * match -1 will return, else return 0.
 */
int MailDateParser::skipChar(char c)
{
    if( m_nIndex < m_nLength ) 
    {
        if( m_pOrig[m_nIndex] == c ) 
            m_nIndex ++;
        else
            //throw new ParseException("Wrong char", index);
            return failed(); 
    }
    else
        //throw new ParseException("No more characters", index);
        return failed(); 
    return 0; 
}

/**
 * will only skip the current char if it matches the given
 * char
 */
BOOL MailDateParser::skipIfChar(char c)
{
    if( m_nIndex < m_nLength ) 
    {
        if( m_pOrig[m_nIndex] == c ) 
        {
            m_nIndex ++;
            return TRUE;
        }
        else
            return FALSE;
    }
    else
    {
        //throw new ParseException("No more characters", index);
        failed(); 
        return FALSE; 
    }
}

/**
 * current char must point to a number.  the number will be
 * parsed and the resulting number will be returned.  if a
 * number is not found, -1 will returned.
 */
int MailDateParser::parseNumber()
{
    BOOL gotNum = FALSE;
    int result = 0;
                            
    while( m_nIndex < m_nLength ) 
    {
        switch( m_pOrig[m_nIndex] ) 
        {
            case '0':
                result *= 10;
                gotNum = TRUE;
                break;
                
            case '1':
                result = result * 10 + 1;
                gotNum = TRUE;
                break;
                
            case '2':
                result = result * 10 + 2;
                gotNum = TRUE;
                break;
                
            case '3':
                result = result * 10 + 3;
                gotNum = TRUE;
                break;
                
            case '4':
                result = result * 10 + 4;
                gotNum = TRUE;
                break;
                
            case '5':
                result = result * 10 + 5;
                gotNum = TRUE;
                break;
                
            case '6':
                result = result * 10 + 6;
                gotNum = TRUE;
                break;
                
            case '7':
                result = result * 10 + 7;
                gotNum = TRUE;
                break;
                
            case '8':
                result = result * 10 + 8;
                gotNum = TRUE;
                break;
                
            case '9':
                result = result * 10 + 9;
                gotNum = TRUE;
                break;
    
            default:
                if( gotNum )
                    return result;
                else
                    //throw new ParseException("No Number found", index);
                    return failed(); 
        }
        
        m_nIndex ++;
    }
            
    // check the result
    if( gotNum )
        return result;
                    
    // else, throw a parse error
    //throw new ParseException("No Number found", index);
    return failed(); 
}

/**
 * will look for one of "Jan/Feb/Mar/Apr/May/Jun/Jul/Aug/Sep/Oct/Nov/Dev"
 * and return the numerical version of the month. (0-11).  a ParseException
 * error is thrown if a month cannot be found.
 */
int MailDateParser::parseMonth() 
{
    char curr = '\0';
            
    if( m_nIndex < m_nLength ) 
    {
        switch( m_pOrig[m_nIndex++] ) 
        {
        case 'J':
        case 'j': // "Jan" (0) /  "Jun" (5) /  "Jul" (6)
            // check next char
            if( m_nIndex >= m_nLength ) 
                goto ArrayIndexOutOfBoundsException; 

            switch( m_pOrig[m_nIndex++] ) 
            {
            case 'A':
            case 'a':
                if( m_nIndex >= m_nLength ) 
                    goto ArrayIndexOutOfBoundsException; 

                curr = m_pOrig[m_nIndex++];
                if( curr == 'N' || curr == 'n' ) 
                    return 0;
                break;
                                                    
            case 'U':
            case 'u':
                if( m_nIndex >= m_nLength ) 
                    goto ArrayIndexOutOfBoundsException; 

                curr = m_pOrig[m_nIndex++];
                if( curr == 'N' || curr == 'n' ) 
                    return 5;
                else if( curr == 'L' || curr == 'l' ) 
                    return 6;
                break;
            }
            break;
                            
        case 'F':
        case 'f': // "Feb"
            if( m_nIndex >= m_nLength ) 
                goto ArrayIndexOutOfBoundsException; 

            curr = m_pOrig[m_nIndex++];
            if( curr == 'E' || curr == 'e' ) 
            {
                if( m_nIndex >= m_nLength ) 
                    goto ArrayIndexOutOfBoundsException; 

                curr = m_pOrig[m_nIndex++];
                if( curr == 'B' || curr == 'b' ) 
                    return 1;
            }
            break;
                            
        case 'M':
        case 'm': // "Mar" (2) /  "May" (4)
            if( m_nIndex >= m_nLength ) 
                goto ArrayIndexOutOfBoundsException; 

            curr = m_pOrig[m_nIndex++];
            if( curr == 'A' || curr == 'a' ) 
            {
                if( m_nIndex >= m_nLength ) 
                    goto ArrayIndexOutOfBoundsException; 

                curr = m_pOrig[m_nIndex++];
                if( curr == 'R' || curr == 'r' ) 
                    return 2;
                else if( curr == 'Y' || curr == 'y' ) 
                    return 4;
            }
            break;                        
                            
        case 'A':
        case 'a': // "Apr" (3) /  "Aug" (7)
            if( m_nIndex >= m_nLength ) 
                goto ArrayIndexOutOfBoundsException; 

            curr = m_pOrig[m_nIndex++];
            if( curr == 'P' || curr == 'p' ) 
            {
                if( m_nIndex >= m_nLength ) 
                    goto ArrayIndexOutOfBoundsException; 

                curr = m_pOrig[m_nIndex++];
                if( curr == 'R' || curr == 'r' ) 
                    return 3;
            }
            else if( curr == 'U' || curr == 'u' ) 
            {
                if( m_nIndex >= m_nLength ) 
                    goto ArrayIndexOutOfBoundsException; 

                curr = m_pOrig[m_nIndex++];
                if( curr == 'G' || curr == 'g' ) 
                    return 7;
            }
            break;                        
                            
        case 'S':
        case 's': // "Sep" (8)
            if( m_nIndex >= m_nLength ) 
                goto ArrayIndexOutOfBoundsException; 

            curr = m_pOrig[m_nIndex++];
            if( curr == 'E' || curr == 'e' ) 
            {
                if( m_nIndex >= m_nLength ) 
                    goto ArrayIndexOutOfBoundsException; 

                curr = m_pOrig[m_nIndex++];
                if( curr == 'P' || curr == 'p' ) 
                    return 8;
            }
            break;                        
                            
        case 'O':
        case 'o': // "Oct"
            if( m_nIndex >= m_nLength ) 
                goto ArrayIndexOutOfBoundsException; 

            curr = m_pOrig[m_nIndex++];
            if( curr == 'C' || curr == 'c' ) 
            {
                if( m_nIndex >= m_nLength ) 
                    goto ArrayIndexOutOfBoundsException; 

                curr = m_pOrig[m_nIndex++];
                if( curr == 'T' || curr == 't' ) 
                    return 9;
            }
            break;                        
                            
        case 'N':
        case 'n': // "Nov"
            if( m_nIndex >= m_nLength ) 
                goto ArrayIndexOutOfBoundsException; 

            curr = m_pOrig[m_nIndex++];
            if( curr == 'O' || curr == 'o' ) 
            {
                if( m_nIndex >= m_nLength ) 
                    goto ArrayIndexOutOfBoundsException; 

                curr = m_pOrig[m_nIndex++];
                if( curr == 'V' || curr == 'v' ) 
                    return 10;
            }
            break;                        

        case 'D':
        case 'd': // "Dec"
            if( m_nIndex >= m_nLength ) 
                goto ArrayIndexOutOfBoundsException; 

            curr = m_pOrig[m_nIndex++];
            if( curr == 'E' || curr == 'e' ) 
            {
                if( m_nIndex >= m_nLength ) 
                    goto ArrayIndexOutOfBoundsException; 

                curr = m_pOrig[m_nIndex++];
                if( curr == 'C' || curr == 'c' ) 
                    return 11;
            }
            break;        
        }
    }

ArrayIndexOutOfBoundsException: 
    //throw new ParseException("Bad Month", index);
    return failed(); 
}

/**
 * will parse the timezone - either Numerical version (e.g. +0800, -0500)
 * or the alpha version (e.g. PDT, PST).  the result will be returned in
 * minutes needed to be added to the date to bring it to GMT.
 */
int MailDateParser::parseTimeZone()
{
    if( m_nIndex >= m_nLength ) 
        //throw new ParseException("No more characters", index);
        return failed(); 
            
    char test = m_pOrig[m_nIndex];
    if( test == '+' || test == '-' ) 
        return parseNumericTimeZone();
    else 
        return parseAlphaTimeZone();
}

/**
 * will parse the Numerical time zone version (e.g. +0800, -0500)
 * the result will be returned in minutes needed to be added
 * to the date to bring it to GMT.
 */
int MailDateParser::parseNumericTimeZone()
{
    // we switch the sign if it is a '+'
    // since the time in the string we are
    // parsing is off from GMT by that amount.
    // and we want to get the time back into
    // GMT, so we substract it.
    BOOL switchSign = FALSE;
    char first = m_pOrig[m_nIndex++];
    if( first == '+' ) 
        switchSign = TRUE;
    else if( first != '-' ) 
        //throw new ParseException("Bad Numeric TimeZone", index);        
        return failed(); 
            
    int tz = parseNumber();
    int offset = (tz / 100) * 60  + (tz % 100);
    if( switchSign ) 
        return -offset;
    else
        return offset;
}

/**
 * will parse the alpha time zone version (e.g. PDT, PST).
 * the result will be returned in minutes needed to be added
 * to the date to bring it to GMT.
 */
int MailDateParser::parseAlphaTimeZone() 
{
    int result = 0;
    BOOL foundCommon = FALSE;
    char curr = '\0';
            
    if( m_nIndex < m_nLength ) 
    {
        switch( m_pOrig[m_nIndex++] ) 
        {
        case 'U':
        case 'u': // "UT"        /        Universal Time
            if( m_nIndex >= m_nLength ) 
                goto ArrayIndexOutOfBoundsException; 

            curr = m_pOrig[m_nIndex++];
            if( curr == 'T' || curr == 't' ) 
            {
                result = 0;
                break;
            }
            //throw new ParseException("Bad Alpha TimeZone", index);
            return failed(); 
                                    
        case 'G':
        case 'g': // "GMT" ; Universal Time
            if( m_nIndex >= m_nLength ) 
                goto ArrayIndexOutOfBoundsException; 

            curr = m_pOrig[m_nIndex++];
            if( curr == 'M' || curr == 'm' ) 
            {
                if( m_nIndex >= m_nLength ) 
                    goto ArrayIndexOutOfBoundsException; 

                curr = m_pOrig[m_nIndex++];
                if( curr == 'T' || curr == 't' ) 
                {
                    result = 0;
                    break;
                }                
            }
            //throw new ParseException("Bad Alpha TimeZone", index); 
            return failed(); 
                                    
        case 'E':
        case 'e': // "EST" / "EDT" ;  Eastern:  - 5/ - 4
            result = 300;
            foundCommon = TRUE;
            break;
                                    
        case 'C':
        case 'c': // "CST" / "CDT" ;  Central:  - 6/ - 5
            result = 360;
            foundCommon = TRUE;
            break;
                                    
        case 'M':
        case 'm': // "MST" / "MDT" ;  Mountain: - 7/ - 6
            result = 420;
            foundCommon = TRUE;
            break;
                                    
        case 'P':
        case 'p': // "PST" / "PDT" ;  Pacific:  - 8/ - 7 
            result = 480;
            foundCommon = TRUE;
            break;

        default:
            //throw new ParseException("Bad Alpha TimeZone", index);
            return failed(); 
        }
    }
    else
        goto ArrayIndexOutOfBoundsException; 
            
    if( foundCommon ) 
    {
        if( m_nIndex >= m_nLength ) 
            goto ArrayIndexOutOfBoundsException; 

        curr = m_pOrig[m_nIndex++];
        if( curr == 'S' || curr == 's' ) 
        {
            if( m_nIndex >= m_nLength ) 
                goto ArrayIndexOutOfBoundsException; 

            curr = m_pOrig[m_nIndex++];
            if( curr != 'T' && curr != 't' )
                //throw new ParseException("Bad Alpha TimeZone", index);
                return failed(); 
        }
        else if( curr == 'D' || curr == 'd' ) 
        {
            if( m_nIndex >= m_nLength ) 
                goto ArrayIndexOutOfBoundsException; 

            curr = m_pOrig[m_nIndex++];
            if( curr == 'T' || curr != 't') 
                // for daylight time
                result -= 60;
            else
                //throw new ParseException("Bad Alpha TimeZone", index);
                return failed(); 
        }
    }

    return result;

ArrayIndexOutOfBoundsException: 
    return failed(); 
}

/**
 * Valid Examples:
 *
 * Date: Sun, 21 Mar 1993 23:56:48 -0800 (PST)
 * Date: 
 * Date: Mon, 22 Mar 1993 09:41:09 -0800 (PST)
 * Date:     26 Aug 76 14:29 EDT
 *
 * method of what to look for:
 *
 *
 *        skip WS
 *        skip day ","          (this is "Mon", "Tue")
 *        skip WS
 *
 *        parse number (until WS) ==> 1*2DIGIT (day of month)
 *
 *        skip WS
 *
 *        parse alpha chars (until WS) ==> find month
 *
 *        skip WS
 *
 *        parse number (until WS) ==> 2*4DIGIT (year)
 *
 *        skip WS
 *
 *        // now looking for time
 *        parse number (until ':') ==> hours
 *        parse number (until ':') ==> minutes
 *        parse number (until WS) ==> seconds
 *
 *        // now look for Time Zone
 *        skip WS
 *        if ('+' or '-') then numerical time zone offset
 *  if (alpha) then alpha time zone offset
 *
 *  create a Date time_t by parsing the char array
 *
 */
time_t MailDateFormat::parse(const FastString &orig)
{
    int day = -1;
    int month = -1;
    int year = -1;
    int hours = 0;
    int minutes = 0;
    int seconds = 0;
    int offset = 0;

    if( !orig.empty() ) 
    {
        MailDateParser p(orig.c_str(), orig.length());

        // get the day
        p.skipUntilNumber();
        day = p.parseNumber();

        if( p.isFailed() ) 
            goto ParseFailedException; 

        if( !p.skipIfChar('-') )   // for IMAP internal Date
            p.skipWhiteSpace();

        if( p.isFailed() ) 
            goto ParseFailedException; 

        // get the month                
        month = p.parseMonth();
        if( !p.skipIfChar('-') )    // for IMAP internal Date
            p.skipWhiteSpace();
        
        if( p.isFailed() ) 
            goto ParseFailedException; 

        // get the year
        year = p.parseNumber();     // should not return a negative number

        if( p.isFailed() ) 
            goto ParseFailedException; 

        if( year < 50 ) 
            year += 2000;
        else if ( year < 100 )
            year += 1900;
        // otherwise the year is correct (and should be 4 digits)

        // get the time
        // first get hours
        p.skipWhiteSpace();
        hours = p.parseNumber();

        if( p.isFailed() ) 
            goto ParseFailedException; 

        // get minutes
        p.skipChar(':');
        minutes = p.parseNumber();

        // get seconds  (may be no seconds)
        if( p.skipIfChar(':') ) 
            seconds = p.parseNumber();

        if( p.isFailed() ) 
            goto ParseFailedException; 
                    
        // try to get a Time Zone
        p.skipWhiteSpace();
        offset = p.parseTimeZone();

        if( p.isFailed() ) 
            offset = 0; 

        return ourUTC(year, month, day, hours, minutes, seconds, offset); 
    }

ParseFailedException: 
    return -1; 
}

time_t MailDateFormat::ourUTC(int year, int mon, int mday, 
                              int hour, int min, int sec, 
                              int tzoffset)
{
    struct tm t; 
    time_t t_of_day; 

    t.tm_year = year - 1900; 
    t.tm_mon  = mon; 
    t.tm_mday = mday; 
    t.tm_hour = hour; 
    t.tm_min  = min; 
    t.tm_sec  = sec; 
    t.tm_isdst = 0; 

    MailDateFormat df; 
    df.setTimeZone("GST"); 

    t_of_day = df.maketime(t); 
    t_of_day += tzoffset * 60;  // adjusted for the timezone

    return t_of_day; 
}

/**
 * Converts a struct tm value to a time_t value, then updates the struct
 * tm value. Only UTC is supported. DST not supported.
 *
 * @param tb    pointer to a tm time structure to convert and
 *                       normalize
 * @return      If successful, mktime returns the specified calender time 
 *              encoded as a time_t value. Otherwise, (time_t)(-1) is 
 *              returned to indicate an error.
 */
time_t MailDateFormat::maketime(struct tm &tb) 
{
    time_t tmptm1 = 0, tmptm2 = 0, tmptm3 = 0;

    /**
     * First, make sure tm_year is reasonably close to being in range.
     */
    if( ((tmptm1 = tb.tm_year) < BASE_YEAR - 1) || (tmptm1 > MAX_YEAR + 1) )
        goto err_mktime;

    /**
     * Adjust month value so it is in the range 0 - 11.  This is because
     * we don't know how many days are in months 12, 13, 14, etc.
     */

    if( (tb.tm_mon < 0) || (tb.tm_mon > 11) ) 
    {
        /**
         * no danger of overflow because the range check above.
         */
        tmptm1 += (tb.tm_mon / 12);

        if( (tb.tm_mon %= 12) < 0 ) 
        {
            tb.tm_mon += 12;
            tmptm1 --;
        }

        /**
         * Make sure year count is still in range.
         */
        if( (tmptm1 < BASE_YEAR - 1) || (tmptm1 > MAX_YEAR + 1) )
            goto err_mktime;
    }

    /***** HERE: tmptm1 holds number of elapsed years *****/

    /**
     * Calculate days elapsed minus one, in the given year, to the given
     * month. Check for leap year and adjust if necessary.
     */
    tmptm2 = _days[tb.tm_mon];
    if( !(tmptm1 & 3) && (tb.tm_mon > 1) )
        tmptm2 ++;

    /**
     * Calculate elapsed days since base date (midnight, 1/1/70, UTC)
     *
     *
     * 365 days for each elapsed year since 1970, plus one more day for
     * each elapsed leap year. no danger of overflow because of the range
     * check (above) on tmptm1.
     */
    tmptm3 = (tmptm1 - BASE_YEAR) * 365L + ((tmptm1 - 1L) >> 2) - LEAP_YEAR_ADJUST;

    /**
     * elapsed days to current month (still no possible overflow)
     */
    tmptm3 += tmptm2;

    /**
     * elapsed days to current date. overflow is now possible.
     */
    tmptm1 = tmptm3 + (tmptm2 = (time_t)(tb.tm_mday));
    if( ChkAdd(tmptm1, tmptm3, tmptm2) )
        goto err_mktime;

    /***** HERE: tmptm1 holds number of elapsed days *****/

    /**
     * Calculate elapsed hours since base date
     */
    tmptm2 = tmptm1 * 24L;
    if( ChkMul(tmptm2, tmptm1, 24L) )
        goto err_mktime;

    tmptm1 = tmptm2 + (tmptm3 = (time_t)tb.tm_hour);
    if( ChkAdd(tmptm1, tmptm2, tmptm3) )
        goto err_mktime;

    /***** HERE: tmptm1 holds number of elapsed hours *****/

    /**
     * Calculate elapsed minutes since base date
     */

    tmptm2 = tmptm1 * 60L;
    if( ChkMul(tmptm2, tmptm1, 60L) )
        goto err_mktime;

    tmptm1 = tmptm2 + (tmptm3 = (time_t)tb.tm_min);
    if( ChkAdd(tmptm1, tmptm2, tmptm3) )
        goto err_mktime;

    /***** HERE: tmptm1 holds number of elapsed minutes *****/

    /**
     * Calculate elapsed seconds since base date
     */

    tmptm2 = tmptm1 * 60L;
    if( ChkMul(tmptm2, tmptm1, 60L) )
        goto err_mktime;

    tmptm1 = tmptm2 + (tmptm3 = (time_t)tb.tm_sec);
    if( ChkAdd(tmptm1, tmptm2, tmptm3) )
        goto err_mktime;

    /***** HERE: tmptm1 holds number of elapsed seconds *****/

    struct tm tbtemp;
    if( this->gmtime(tmptm1, tbtemp) == -1 )
        goto err_mktime;

    /***** HERE: tmptm1 holds number of elapsed seconds, adjusted *****/
    /*****       for local time if requested                      *****/

    ::memcpy(&tb, &tbtemp, sizeof(struct tm));
    return (time_t)tmptm1;

err_mktime:
    /*
     * All errors come to here
     */
    return (time_t)(-1);
}

/**
 * Converts the calendar time value, in internal format (time_t), to
 * broken-down time (tm structure) with the corresponding UTC time.
 * 
 * @param timer     pointer to time_t value to convert
 * @param tb        pointer to filled-in tm structure.
 * @return          returns -1 if timer < 0L else return 0.
 */
int MailDateFormat::gmtime(const time_t &timer, struct tm &tb) 
{
    time_t caltim = timer;      // calendar time to convert
    int islpyr = 0;             // is-current-year-a-leap-year flag
    int tmptim = 0;
    int *mdays = NULL;          // pointer to days or lpdays
    struct tm ptb;              // will point to gmtime buffer

    if( caltim < 0 )
        return (-1);

    /**
     * Determine years since 1970. First, identify the four-year interval
     * since this makes handling leap-years easy (note that 2000 IS a
     * leap year and 2100 is out-of-range).
     */
    tmptim = (int)(caltim / FOUR_YEAR_SEC);
    caltim -= ((time_t)tmptim * FOUR_YEAR_SEC);

    /**
     * Determine which year of the interval
     */
    tmptim = (tmptim * 4) + 70;         // 1970, 1974, 1978,...,etc.

    if( caltim >= YEAR_SEC ) 
    {
        tmptim ++;                      // 1971, 1975, 1979,...,etc. 
        caltim -= YEAR_SEC;

        if( caltim >= YEAR_SEC ) 
        {
            tmptim ++;                  // 1972, 1976, 1980,...,etc. 
            caltim -= YEAR_SEC;

            /**
             * Note, it takes 366 days-worth of seconds to get past a leap
             * year.
             */
            if( caltim >= (YEAR_SEC + DAY_SEC) ) 
            {
                tmptim ++;              // 1973, 1977, 1981,...,etc. 
                caltim -= (YEAR_SEC + DAY_SEC);
            }
            else 
            {
                /**
                 * In a leap year after all, set the flag.
                 */
                islpyr ++;
            }
        }
    }

    /**
     * tmptim now holds the value for tm_year. caltim now holds the
     * number of elapsed seconds since the beginning of that year.
     */
    ptb.tm_year = tmptim;

    /**
     * Determine days since January 1 (0 - 365). This is the tm_yday value.
     * Leave caltim with number of elapsed seconds in that day.
     */
    ptb.tm_yday = (int)(caltim / DAY_SEC);
    caltim -= (time_t)(ptb.tm_yday) * DAY_SEC;

    /**
     * Determine months since January (0 - 11) and day of month (1 - 31)
     */
    if( islpyr )
        mdays = (int *) _lpdays;
    else
        mdays = (int *) _days;

    for( tmptim = 1 ; mdays[tmptim] < ptb.tm_yday ; tmptim++ ) ;

    ptb.tm_mon  = -- tmptim;

    ptb.tm_mday = ptb.tm_yday - mdays[tmptim];

    /**
     * Determine days since Sunday (0 - 6)
     */
    ptb.tm_wday = ((int)(timer / DAY_SEC) + BASE_DOW) % 7;

    /**
     *  Determine hours since midnight (0 - 23), minutes after the hour
     *  (0 - 59), and seconds after the minute (0 - 59).
     */
    ptb.tm_hour = (int)(caltim / 3600);
    caltim -= (time_t)ptb.tm_hour * 3600L;

    ptb.tm_min = (int)(caltim / 60);
    ptb.tm_sec = (int)(caltim - (ptb.tm_min) * 60);

    ptb.tm_isdst = 0;

    ::memcpy(&tb, &ptb, sizeof(struct tm)); 
    return 0; 
}

/**
 * Convert a value in internal (time_t) format to a tm struct
 * containing the corresponding local time.
 *
 * NOTES:
 *       (1) gmtime must be called before _isindst to ensure that the tb time
 *           structure is initialized.
 *       (2) gmtime and localtime use a single statically allocated buffer.
 *           Each call to one of these routines destroys the contents of the
 *           previous call.
 *       (3) It is assumed that time_t is a 32-bit long integer representing
 *           the number of seconds since 00:00:00, 01-01-70 (UTC) (i.e., the
 *           Posix/Unix Epoch. Only non-negative values are supported.
 *       (4) It is assumed that the maximum adjustment for local time is
 *           less than three days (include Daylight Savings Time adjustment).
 *           This only a concern in Posix where the specification of the TZ
 *           environment restricts the combined offset for time zone and
 *           Daylight Savings Time to 2 * (24:59:59), just under 50 hours.
 *       DST NOT SUPPORTED!
 *
 * @param timer     pointer to a long time value
 * @return 
 *       If timer is non-negative, returns a pointer to the tm structure.
 *       Otherwise, returns -1.
 *
 *Exceptions:
 *       See items (3) and (4) in the NOTES above. If these assumptions are
 *       violated, behavior is undefined.
 */
int MailDateFormat::localtime(const time_t &timer, struct tm &tb) 
{
    struct tm ptm;
    time_t ltime = 0;
    int ret = 0; 

    /**
     * Check for illegal time_t value
     */
    if( timer < 0 )
        return (-1);

    if( (timer > 3 * DAY_SEC) && (timer < LONG_MAX - 3 * DAY_SEC) ) 
    {
        /**
         * The date does not fall within the first three, or last
         * three, representable days of the Epoch. Therefore, there
         * is no possibility of overflowing or underflowing the
         * time_t representation as we compensate for timezone and
         * Daylight Savings Time.
         */

        ltime = timer + m_nTimezoneOffset;
        ret = this->gmtime(ltime, ptm);
    }
    else 
    {
        ret = this->gmtime(timer, ptm);

        /**
         * DST not supported.
         *
         * The date falls with the first three, or last three days
         * of the Epoch. It is possible the time_t representation
         * would overflow or underflow while compensating for
         * timezone and Daylight Savings Time. Therefore, make the
         * timezone and Daylight Savings Time adjustments directly
         * in the tm structure. The beginning of the Epoch is
         * 00:00:00, 01-01-70 (UTC) and the last representable second
         * in the Epoch is 03:14:07, 01-19-2038 (UTC). This will be
         * used in the calculations below.
         *
         * First, adjust for the timezone.
         */

        ltime = (time_t)ptm.tm_sec + m_nTimezoneOffset;
        ptm.tm_sec = (int)(ltime % 60);
        if( ptm.tm_sec < 0 ) 
        {
            ptm.tm_sec += 60;
            ltime -= 60;
        }

        ltime = (time_t)ptm.tm_min + ltime/60;
        ptm.tm_min = (int)(ltime % 60);
        if( ptm.tm_min < 0 ) 
        {
            ptm.tm_min += 60;
            ltime -= 60;
        }

        ltime = (time_t)ptm.tm_hour + ltime/60;
        ptm.tm_hour = (int)(ltime % 24);
        if( ptm.tm_hour < 0 ) 
        {
            ptm.tm_hour += 24;
            ltime -=24;
        }

        ltime /= 24;

        if( ltime > 0 ) 
        {
            /**
             * There is no possibility of overflowing the tm_mday
             * and tm_yday fields since the date can be no later
             * than January 19.
             */
            ptm.tm_wday = (ptm.tm_wday + (int)ltime) % 7;
            ptm.tm_mday += (int)ltime;
            ptm.tm_yday += (int)ltime;
        }
        else if( ltime < 0 ) 
        {
            /**
             * It is possible to underflow the tm_mday and tm_yday
             * fields. If this happens, then adjusted date must
             * lie in December 1969.
             */
            ptm.tm_wday = (ptm.tm_wday + 7 + (int)ltime) % 7;
            if( (ptm.tm_mday += (int)ltime) <= 0 ) 
            {
                ptm.tm_mday += 31;
                ptm.tm_yday = 364;
                ptm.tm_mon  = 11;
                ptm.tm_year --;
            }
            else 
            {
                ptm.tm_yday += (int)ltime;
            }
        }
    }

    ::memcpy(&tb, &ptm, sizeof(struct tm)); 
    return ret; 
}

/**
 * Sets the time zone for the timezoneoffset of this MailDateFormat object.
 *
 * @param tz  the given new time zone in string.
 */
void MailDateFormat::setTimeZone(const char *tz) 
{
    m_bMaked = FALSE; 

    if( tz == NULL || tz[0] == '\0' || ::strlen(tz) < 3 ) 
    {
        // MailDateFormat timezone = local time - UTC 
        // like java, CHINA +0800, +28800, but 
        // CRT _timezone = -28800 = UTC - local time
        m_nTimezoneOffset = FASTMIME_CRT_TIMEZONE; 
        return; 
    }

    /*
     * Process TZ value and update m_nTimezoneOffset.
     */
    char *TZ = (char *) tz;
    int defused = 0;
    int negdiff = 0;

    /*
     * time difference is of the form:
     *
     *      [+|-]hh[:mm[:ss]]
     *
     * check minus sign first.
     */
    if( *(TZ += 3) == '-' ) 
    {
        negdiff ++;
        TZ ++;
    }

    /*
     * process, then skip over, the hours
     */
    m_nTimezoneOffset = ::atol(TZ) * 3600;

    while( (*TZ == '+') || ((*TZ >= '0') && (*TZ <= '9')) ) TZ++;

    /*
     * check if minutes were specified
     */
    if( *TZ == ':' ) 
    {
        /*
         * process, then skip over, the minutes
         */
        m_nTimezoneOffset += ::atol(++TZ) * 60;
        while( (*TZ >= '0') && (*TZ <= '9') ) TZ++;

        /*
         * check if seconds were specified
         */
        if( *TZ == ':' ) 
        {
            /*
             * process, then skip over, the seconds
             */
            m_nTimezoneOffset += ::atol(++TZ);
            while( (*TZ >= '0') && (*TZ <= '9') ) TZ++;
        }
    }

    if( negdiff )
        m_nTimezoneOffset = -m_nTimezoneOffset;
}

/**
 * Formats the given date in the format specified by 
 * draft-ietf-drums-msg-fmt-08 in the current TimeZone.
 *
 * @param tb        the time to format
 * @param s         the formatted string
 */
void MailDateFormat::format(const struct tm &tb, FastString &s) 
{
    // Converts a time stored in a struct tm to a charcater string.
    // The string is always exactly 32 characters of the form
    //     Sun, 21 Mar 1993 23:56:48 -0800\0

    // copy day and month names into the buffer
    char buf[40] = {0}; 
    char *p = buf; 
    int i = 0;
    int day = tb.tm_wday * 3;       // index to correct day string
    int mon = tb.tm_mon * 3;        // index to correct month string

    for( i = 0; i < 3; i ++, p ++ ) 
        *p = *(_dnames + day + i);

    *p++ = (',');
    *p++ = (' ');                   // ", " between week and day
    p = store_dt(p, tb.tm_mday);    // day of the month (1-31)
    *p++ = (' ');                   // blank between day and month

    for( i = 0; i < 3; i ++, p ++ ) 
        *(p) = *(_mnames + mon + i);

    *p++ = (' ');                   // blank between month and year
    p = store_dt(p, 19 + (tb.tm_year/100)); // year (after 1900) 
    p = store_dt(p, tb.tm_year%100);
    *p++ = (' ');
    p = store_dt(p, tb.tm_hour);    // hours (0-23) 
    *p++ = (':');
    p = store_dt(p, tb.tm_min);     // minutes (0-59) 
    *p++ = (':');
    p = store_dt(p, tb.tm_sec);     // seconds (0-59) 
    *p++ = (' ');

    int offset = m_nTimezoneOffset; 
    if( offset < 0 ) offset = offset * (-1); 

    int rawOffsetInMins = offset / 60; // offset from GMT in mins
    int offsetInHrs  = rawOffsetInMins / 60;
    int offsetInMins = rawOffsetInMins % 60;

    *p++ = m_nTimezoneOffset >= 0 ? ('+') : ('-');
    p = store_dt(p, offsetInHrs);   // hours in timezone
    p = store_dt(p, offsetInMins);  // hours in timezone

    *p = ('\0');

    s.clear(); 
    s.append(buf, p - buf); 
}

/**
 * Formats the given date in the format specified by 
 * draft-ietf-drums-msg-fmt-08 in the current TimeZone.
 *
 * @param timer     the time to format
 * @param s         the formatted string
 */
void MailDateFormat::format(const time_t &timer, FastString &s) 
{
    struct tm tmtemp;

    if( this->localtime(timer, tmtemp) == 0 ) 
        format(tmtemp, s); 
    else
        s.clear(); 
}


_FASTMIME_END_NAMESPACE
