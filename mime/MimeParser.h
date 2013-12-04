//=============================================================================
/**
 *  @file    MimeParser.h
 *
 *  ver 1.0.0 2004/06/16 Naven Exp, for Mime Message Parse Engine.
 *
 *  @author Naven 2004/06/16 created.
 */
//=============================================================================

#ifndef _MIMEPARSER_H
#define _MIMEPARSER_H

#if defined(_WIN32) || defined(__WIN32__)

#if !defined (NAVEN_PRAGMA_ONCE)
# pragma once
#endif /* NAVEN_PRAGMA_ONCE */

#endif /* _WIN32 */


#include <time.h>
#include "MimeBase.h" 


_FASTMIME_BEGIN_NAMESPACE



//=============class HeaderTokenizer define==============

/**
 * This class tokenizes RFC822 and MIME headers into the basic
 * symbols specified by RFC822 and MIME. <p>
 *
 * This class handles folded headers (ie headers with embedded
 * CRLF SPACE sequences). The folds are removed in the returned
 * tokens. 
 *
 * This class handles headers string by reference pointer, so 
 * does not allocate new memory to store it.
 *
 */

class HeaderTokenizer
{
public: 
    /**
     * The Token class represents tokens returned by the 
     * HeaderTokenizer.
     */
    class Token
    {
    private: 
        int m_nType;            // Token type
        ShortString m_sValue;   // Token value

    public: 
        enum {
            /**
             * Token type indicating an ATOM.
             */
            ATOM = -1, 

            /**
             * Token type indicating a quoted string. The value 
             * field contains the string without the quotes.
             */
            QUOTEDSTRING = -2, 

            /**
             * Token type indicating a comment. The value field 
             * contains the comment string without the comment 
             * start and end symbols.
             */
            COMMENT = -3, 

            /**
             * Token type indicating end of input.
             */
            TOKEN_EOF = -4
        }; 

        /**
         * Default Constructor.
         *
         */
        Token() 
         : m_nType(EOFToken.m_nType), 
           m_sValue(EOFToken.m_sValue) 
        {
        }

        /**
         * Constructor.
         *
         * @param type  Token type
         * @param value Token value
         */
        Token(int type, FastString &value) 
         : m_nType(type), 
           m_sValue(ShortString(value.c_str(), value.length())) 
        {
        }

        /**
         * Constructor.
         *
         * @param type  Token type
         * @param value Token value
         */
        Token(int type, const FastString &value) 
         : m_nType(type), 
           m_sValue(ShortString(value.c_str(), value.length())) 
        {
        }

        /**
         * Constructor.
         *
         * @param tk  another token
         */
        Token(const Token &tk) 
         : m_nType(tk.m_nType), 
           m_sValue(tk.m_sValue) 
        {
        }

        /**
         * Assign operator=().
         *
         * @param tk  another token
         */
        Token& operator=(const Token &tk) 
        {
            m_nType = tk.m_nType; 
            m_sValue = tk.m_sValue; 
            return *this; 
        }

        /**
         * Compare equal operator==().
         *
         * @param tk  another token
         * @return TRUE if equals else FALSE
         */
        BOOL operator==(const Token &tk) 
        {
            return (m_nType == tk.m_nType && m_sValue == tk.m_sValue) ? TRUE : FALSE; 
        }

        /**
         * Compare not equal operator!=().
         *
         * @param tk  another token
         * @return TRUE if not equals else FALSE
         */
        BOOL operator!=(const Token &tk) 
        {
            return (m_nType == tk.m_nType && m_sValue == tk.m_sValue) ? FALSE : TRUE; 
        }

        /**
         * Set token's type and value.
         *
         * @param type  Token type
         * @param value Token value
         */
        void set(int type, FastString &value) 
        {
            m_nType = type; 
            m_sValue = value; 
        }

        /**
         * Return the type of the token. If the token represents a
         * delimiter or a control character, the type is that character
         * itself, converted to an integer. Otherwise, it's value is 
         * one of the following:
         * <ul>
         * <li><code>ATOM</code> A sequence of ASCII characters 
         *    delimited by either SPACE, CTL, "(", <"> or the 
         *    specified SPECIALS
         * <li><code>QUOTEDSTRING</code> A sequence of ASCII characters
         *    within quotes
         * <li><code>COMMENT</code> A sequence of ASCII characters 
         *    within "(" and ")".
         * <li><code>EOF</code> End of header
         * </ul>
         */
        int getType() 
        {
            return m_nType;
        }

        /**
         * Returns the value of the token just read. When the current
         * token is a quoted string, this field contains the body of the
         * string, without the quotes. When the current token is a comment,
         * this field contains the body of the comment.
         *
         * @return    token value
         */
        const ShortString& getValue() 
        {
            return m_sValue;
        }

        const void getValue(FastString &s) 
        {
            s.set(m_sValue.c_str(), m_sValue.length());
        }
    };

public: 
    static Token EOFToken;      // end of token
    static const char *MIME;    // MIME specials
    static const char *RFC822;  // RFC822 specials

protected:
    FastString &m_sString;      // the string to be tokenized, by reference pointer
    BOOL m_bSkipComments;       // should comments be skipped ?
    ShortString m_sDelimiters;  // delimiter string
    int m_nCurrentPos;          // current parse position
    int m_nNextPos;             // track start of next Token for next()
    int m_nPeekPos;             // track start of next Token for peek()
    int m_nMaxPos;              // string length

private: 
    /*
     * Return the next token starting from 'currentPos'. After the
     * parse, 'currentPos' is updated to point to the start of the 
     * next token.
     */
    int getNext(Token &tk); 

    // Skip SPACE, HT, CR and NL
    int skipWhiteSpace(); 

    /* Process escape sequences and embedded LWSPs from a comment or
     * quoted string.
     */
    static void filterToken(FastString &s, FastString &output, int start, int end); 

public: 
    /**
     * Constructor that takes a rfc822 style header.
     *
     * @param    header    The rfc822 header to be tokenized
     * @param    delimiters      Set of delimiter characters 
     *                to be used to delimit ATOMS. These
     *                are usually <code>RFC822</code> or 
     *                <code>MIME</code>
     * @param   skipComments  If true, comments are skipped and
     *                not returned as tokens
     */
    HeaderTokenizer(FastString &header, FastString &delimiters,
                    BOOL skipComments = TRUE) 
     : m_sString(header), 
       m_bSkipComments(skipComments), 
       m_sDelimiters(ShortString(delimiters.c_str(), delimiters.length())), 
       m_nCurrentPos(0), 
       m_nNextPos(0), 
       m_nPeekPos(0), 
       m_nMaxPos(m_sString.length())
    {
    }

    /**
     * Constructor that takes a rfc822 style header.
     *
     * @param    header    The rfc822 header to be tokenized
     * @param    delimiters      Set of delimiter characters 
     *                to be used to delimit ATOMS. These
     *                are usually <code>RFC822</code> or 
     *                <code>MIME</code>
     * @param   skipComments  If true, comments are skipped and
     *                not returned as tokens
     */
    HeaderTokenizer(FastString &header, const char *delimiters,
                    BOOL skipComments = TRUE) 
     : m_sString(header), 
       m_bSkipComments(skipComments), 
       m_sDelimiters(ShortString(delimiters)), 
       m_nCurrentPos(0), 
       m_nNextPos(0), 
       m_nPeekPos(0), 
       m_nMaxPos(m_sString.length())
    {
    }

    /**
     * Constructor. The RFC822 defined delimiters - RFC822 - are
     * used to delimit ATOMS. Also comments are skipped and not
     * returned as tokens
     */
    HeaderTokenizer(FastString &header, BOOL skipComments = TRUE) 
     : m_sString(header), 
       m_bSkipComments(skipComments), 
       m_sDelimiters(ShortString(RFC822)), 
       m_nCurrentPos(0), 
       m_nNextPos(0), 
       m_nPeekPos(0), 
       m_nMaxPos(m_sString.length())
    {
    }

    /**
     * Parses the next token from this String. <p>
     *
     * Clients sit in a loop calling next() to parse successive
     * tokens until an EOF Token is returned.
     *
     * @param tk    the next Token
     * @return      -1 if the parse fails else next token position 
     *              if success
     */
    int next(Token &tk) 
    { 
        m_nCurrentPos = m_nNextPos;                 // setup currentPos
        int result = getNext(tk);
        m_nNextPos = m_nPeekPos = m_nCurrentPos;    // update currentPos and peekPos
        return result;
    }

    /**
     * Peek at the next token, without actually removing the token
     * from the parse stream. Invoking this method multiple times
     * will return successive tokens, until <code>next()</code> is
     * called. <p>
     *
     * @param tk    the next Token
     * @return      -1 if the parse fails else next token position 
     *              if success
     */
    int peek(Token &tk) 
    {
        m_nCurrentPos = m_nPeekPos;     // setup currentPos
        int result = getNext(tk);
        m_nPeekPos = m_nCurrentPos;     // update peekPos
        return result;
    }

    /**
     * Return the rest of the Header.
     *
     * @param s rest of header. empty is returned if we are
     *            already at end of header
     */
    void getRemainder(FastString &s) 
    {
        m_sString.substring(s, m_nNextPos);
    }

    /**
     * Dump the object state. 
     */
    void dump(); 
};

/**
 * Dump the object state.
 */
inline void HeaderTokenizer::dump() 
{
    FAST_TRACE_BEGIN("HeaderTokenizer::dump()"); 
    FAST_TRACE("sizeof(HeaderTokenizer) = %d", sizeof(HeaderTokenizer)); 
    FAST_TRACE("m_sString.size() = %d", m_sString.size()); 
    FAST_TRACE("m_sDelimiters.c_str() = \"%s\"", m_sDelimiters.c_str()); 
    FAST_TRACE("m_sString -> 0x%08X", &m_sString); 
    FAST_TRACE("m_bSkipComments = %d", m_bSkipComments); 
    FAST_TRACE("m_nCurrentPos = %d", m_nCurrentPos); 
    FAST_TRACE("m_nMaxPos = %d", m_nMaxPos); 
    FAST_TRACE("m_nNextPos = %d", m_nNextPos); 
    FAST_TRACE("m_nPeekPos = %d", m_nPeekPos); 
    FAST_TRACE_END("HeaderTokenizer::dump()"); 
}


//=============class MailDateFormat define==============

/**
 * Formats and parses date specification based on the
 * draft-ietf-drums-msg-fmt-08 dated January 26, 2000. This is a followup
 * spec to RFC822.<p>
 *
 * This class does not take pattern strings. It always formats the
 * date based on the specification below.<p>
 *
 * 3.3 Date and Time Specification<p>
 *
 * Date and time occur in several header fields of a message. This section
 * specifies the syntax for a full date and time specification. Though folding
 * whitespace is permitted throughout the date-time specification, it is
 * recommended that only a single space be used where FWS is required and no
 * space be used where FWS is optional in the date-time specification; some
 * older implementations may not interpret other occurrences of folding
 * whitespace correctly.<p>
 *
 * date-time = [ day-of-week "," ] date FWS time [CFWS]<p>
 *
 * day-of-week = ([FWS] day-name) / obs-day-of-week<p>
 *
 * day-name = "Mon" / "Tue" / "Wed" / "Thu" / "Fri" / "Sat" / "Sun"<p>
 *
 * date = day month year<p>
 *
 * year = 4*DIGIT / obs-year<p>
 *
 * month = (FWS month-name FWS) / obs-month<p>
 *
 *<pre>month-name = "Jan" / "Feb" / "Mar" / "Apr" /
 *             "May" / "Jun" / "Jul" / "Aug" /
 *             "Sep" / "Oct" / "Nov" / "Dec"
 * </pre><p>
 * day = ([FWS] 1*2DIGIT) / obs-day<p>
 *
 * time = time-of-day FWS zone<p>
 *
 * time-of-day = hour ":" minute [ ":" second ]<p>
 *
 * hour = 2DIGIT / obs-hour<p>
 *
 * minute = 2DIGIT / obs-minute<p>
 *
 * second = 2DIGIT / obs-second<p>
 *
 * zone = (( "+" / "-" ) 4DIGIT) / obs-zone<p>
 *
 *
 * The day is the numeric day of the month. The year is any numeric year in
 * the common era.<p>
 *
 * The time-of-day specifies the number of hours, minutes, and optionally
 * seconds since midnight of the date indicated.<p>
 *
 * The date and time-of-day SHOULD express local time.<p>
 *
 * The zone specifies the offset from Coordinated Universal Time (UTC,
 * formerly referred to as "Greenwich Mean Time") that the date and
 * time-of-day represent. The "+" or "-" indicates whether the time-of-day is
 * ahead of or behind Universal Time. The first two digits indicate the number
 * of hours difference from Universal Time, and the last two digits indicate
 * the number of minutes difference from Universal Time. (Hence, +hhmm means
 * +(hh * 60 + mm) minutes, and -hhmm means -(hh * 60 + mm) minutes). The form
 * "+0000" SHOULD be used to indicate a time zone at Universal Time. Though
 * "-0000" also indicates Universal Time, it is used to indicate that the time
 * was generated on a system that may be in a local time zone other than
 * Universal Time.<p>
 *
 * A date-time specification MUST be semantically valid. That is, the
 * day-of-the week (if included) MUST be the day implied by the date, the
 * numeric day-of-month MUST be between 1 and the number of days allowed for
 * the specified month (in the specified year), the time-of-day MUST be in the
 * range 00:00:00 through 23:59:60 (the number of seconds allowing for a leap
 * second; see [STD-12]), and the zone MUST be within the range -9959 through
 * +9959.<p>
 *
 */
class MailDateFormat
{
protected:
    static const long DAY_SEC;          // secs in a day
    static const long YEAR_SEC;         // secs in a year
    static const long FOUR_YEAR_SEC;    // secs in a 4 year interval
    static const long DEC_SEC;          // secs in 1970-1979
    static const long BASE_YEAR;        // 1970 is the base year
    static const long BASE_DOW;         // 01-01-70 was a Thursday
    static const long LEAP_YEAR_ADJUST; // Leap years 1900 - 1970
    static const long MAX_YEAR;         // 2038 is the max year

    static const int _lpdays[], _days[]; 
    static const char _dnames[], _mnames[]; 

    /**
     * MailDateFormat m_nTimezoneOffset = local time - UTC 
     * like java, CHINA +0800, +28800, but 
     * CRT _timezone = -28800 = UTC - local time
     */ 
    long m_nTimezoneOffset;     // in second
    struct tm m_tTimer;         // local time
    time_t m_nTimer;            // UTC time, seconds from 1970-01-01 00:00:00
    BOOL m_bMaked;              // timer maked to time_t, true of false
    
    static time_t ourUTC(int year, int mon, int mday, int hour, int min, int sec, int tzoffset); 
    static char * store_dt(char *p, int val); 
    static BOOL ChkAdd(long dest, long src1, long src2); 
    static BOOL ChkMul(long dest, long src1, long src2); 

    void maketime(); 
    time_t maketime(struct tm &tb); 
    int gmtime(const time_t &timer, struct tm &tb); 
    int localtime(const time_t &timer, struct tm &tb); 

public: 
    MailDateFormat(); 
    void setTimeZone(const char *tz); 
    void setTimeZoneOffset(const long offset) { m_bMaked = FALSE; m_nTimezoneOffset = offset; }
    long getTimeZone() { return m_nTimezoneOffset; } 
    int  getYear() { return m_tTimer.tm_year + 1900; }  // Year (current year started with 1900)
    void setYear(const int year) { m_bMaked = FALSE; m_tTimer.tm_year = year - 1900; }
    int  getMonth() { return m_tTimer.tm_mon; }     // Month (0每11; January = 0)
    void setMonth(const int mon) { m_bMaked = FALSE; m_tTimer.tm_mon = mon; }
    int  getDate() { return m_tTimer.tm_mday; }     // Day of month (1每31)
    void setDate(const int mday) { m_bMaked = FALSE; m_tTimer.tm_mday = mday; }
    int  getHourOfDay() { return m_tTimer.tm_hour; }    // Hours since midnight (0每23)
    void setHourOfDay(const int hour) { m_bMaked = FALSE; m_tTimer.tm_hour = hour; }
    int  getMinute() { return m_tTimer.tm_min; }    // Minutes after hour (0每59)
    void setMinute(const int min) { m_bMaked = FALSE; m_tTimer.tm_min = min; }
    int  getSecond() { return m_tTimer.tm_sec; }    // Seconds after minute (0每59)
    void setSecond(const int sec) { m_bMaked = FALSE; m_tTimer.tm_sec = sec; }
    void setTime(const time_t &timer); 
    time_t getTime(); 
    void format(FastString &s); 
    void format(const struct tm &tb, FastString &s); 
    void format(const time_t &timer, FastString &s); 
    static time_t parse(const FastString &orig); 
    static time_t parse(const char *orig, int len = -1); 

}; 

/**
 * Helper class to deal with parsing the characters
 */
class MailDateParser
{
protected:
    int   m_nIndex;
    int   m_nLength; 
    const char* m_pOrig;
    BOOL  m_nFailed; 

    int getIndex() { return m_nIndex; } 
    int failed() { m_nFailed = TRUE; return -1; } 

public: 
    MailDateParser(const char *orig); 
    MailDateParser(const char *orig, int len); 
    int skipUntilNumber(); 
    void skipWhiteSpace(); 
    int peekChar(); 
    int skipChar(char c); 
    BOOL skipIfChar(char c); 
    int parseNumber(); 
    int parseMonth(); 
    int parseTimeZone(); 
    int parseNumericTimeZone(); 
    int parseAlphaTimeZone(); 
    BOOL isFailed(); 

};


//============MailDateFormat Functions Implements================

/**
 * Constructor.
 */
inline MailDateParser::MailDateParser(const char *orig, int len)
: m_nIndex(0), 
  m_nLength(orig && len > 0 ? len : 0), 
  m_pOrig((const char *)(orig && len > 0 ? orig : 0)), 
  m_nFailed(FALSE) 
{

}

/**
 * Constructor.
 */
inline MailDateParser::MailDateParser(const char *orig)
: m_nIndex(0), 
  m_nLength(orig ? ::strlen(orig) : 0), 
  m_pOrig((const char *)(orig)), 
  m_nFailed(FALSE) 
{

}

/**
 * Check if parse failed.
 */
inline BOOL MailDateParser::isFailed() 
{ 
    if( m_nFailed == TRUE ) 
        return TRUE; 
    else 
        return ( m_pOrig == NULL || m_nIndex < 0 || m_nIndex > m_nLength ) ? TRUE : FALSE; 
} 

/**
 * Default Constructor.
 */
inline MailDateFormat::MailDateFormat() 
 : m_nTimezoneOffset(FASTMIME_CRT_TIMEZONE), 
   m_tTimer(), 
   m_nTimer(0), 
   m_bMaked(FALSE)
{ 
    ::memset(&m_tTimer, 0, sizeof(struct tm)); 
} 

inline void MailDateFormat::maketime() 
{
    if( m_bMaked == FALSE ) 
    {
        if( (m_nTimer = this->maketime(m_tTimer)) != -1 ) 
            m_nTimer = m_nTimer - m_nTimezoneOffset; 
        m_bMaked = TRUE; 
    }
}

inline char * MailDateFormat::store_dt(char *p, int val)
{
    *p++ = (char)(('0') + val / 10);
    *p++ = (char)(('0') + val % 10);
    return (p);
}

/**
 * ChkAdd evaluates to TRUE if dest = src1 + src2 has overflowed
 */
inline BOOL MailDateFormat::ChkAdd(long dest, long src1, long src2)
{
    if( ((src1 >= 0L) && (src2 >= 0L) && (dest < 0L)) || ((src1 < 0L) && (src2 < 0L) && (dest >= 0L)) ) 
        return TRUE; 
    else
        return FALSE; 
}

/**
 * ChkMul evaluates to TRUE if dest = src1 * src2 has overflowed
 */
inline BOOL MailDateFormat::ChkMul(long dest, long src1, long src2)
{
    return ( src1 ? ((dest/src1 != src2) ? TRUE : FALSE) : FALSE ); 
}

/**
 * Set current datetime.
 */
inline void MailDateFormat::setTime(const time_t &timer) 
{
    m_nTimer = timer; 
    this->localtime(timer, m_tTimer); 
}

/**
 * Return current datetime.
 */
inline time_t MailDateFormat::getTime() 
{
    this->maketime(); 
    return m_nTimer; 
}

/**
 * Format current datetime to MailDateFormat. like
 *         Mon, 22 Mar 1994 13:34:51 +0000
 */
inline void MailDateFormat::format(FastString &s) 
{
    this->maketime(); 
    format(m_tTimer, s); 
}

/**
 * Parses the given date in the format specified by
 * draft-ietf-drums-msg-fmt-08 in the current TimeZone.
 *
 * @param   orig    the formatted date to be parsed
 * @param   len     the current parse string length
 * @return  Date    the parsed date in a time_t object
 */
inline time_t MailDateFormat::parse(const char *orig, int len)
{
    if( orig && len <= 0 ) 
        len = ::strlen(orig); 
    return parse(FastString(orig, len)); 
}


_FASTMIME_END_NAMESPACE

#endif
