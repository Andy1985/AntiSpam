//=============================================================================
/**
 *  @file    MimeUtility.cpp
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
#include "MimeUtility.h" 
#include "MimeActivation.h"


_FASTMIME_BEGIN_NAMESPACE


// Initialize Class Static Members

ShortString MimeUtility::m_sDefaultMIMECharset   = DEFAULT_MIME_CHARSET; 

int         UniqueValue::m_nParts       = 1; 
int         UniqueValue::m_nCIDs        = 1; 
int         UniqueValue::m_nAttachments = 1; 
ShortString UniqueValue::PART           = DEFAULT_BOUNDARY_PART_NAME; 
ShortString UniqueValue::FASTMAIL       = DEFAULT_BOUNDARY_FASTMAIL_NAME; 


// Globale Variables



//===========UniqueValue Functions Implements=============

/**
 * Get a unique value for use in a multipart boundary string.
 *
 * This implementation generates it by concatenating a global
 * part number, a newly created object's <code>hashCode()</code>,
 * and the current time (in milliseconds).
 *
 * @param s   return new boundary value
 */
void UniqueValue::getUniqueBoundaryValue(FastString &s) 
{
    s.clear(); 

    // Unique string is ----=_Part_<part>_<hashcode>.<currentTime>
    s.append("----=_"); 
    s.append(UniqueValue::PART); 
    s.append('_'); 
    s.append(m_nParts++); 
    s.append('_'); 
    s.append(fast_hash_string(s.c_str())); 
    s.append('.'); 
    s.append((unsigned int)::time(NULL));
}

/**
 * Get a unique value for use in a Message-ID.
 *
 * This implementation generates it by concatenating a newly
 * created object's <code>hashCode()</code>, the current
 * time (in milliseconds), the string "FastMail", and
 * this user's local address.
 *
 */
void UniqueValue::getUniqueMessageIDValue(FastString &s) 
{
    FastString suffix;

    //InternetAddress addr = InternetAddress.getLocalAddress(ssn);
    //if( addr != null )
    //    suffix = addr.getAddress();
    //else 
    suffix = "@localhost"; // worst-case default

    unsigned int currentTimeMillis = (unsigned int)::time(NULL); 
    s.append(suffix); 
    s.append(currentTimeMillis); 
    unsigned int hashCode = fast_hash_string(s.c_str()); 

    s.clear();

    // Unique string is <hashcode>.<currentTime>.FastMail.<suffix>
    s.append(hashCode); 
    s.append('.'); 
    s.append(currentTimeMillis); 
    s.append('.'); 
    s.append(UniqueValue::FASTMAIL); 
    s.append('.'); 
    s.append(suffix);
}

/**
 * Get a unique value for use in a Content-ID.
 *
 * This implementation generates it by concatenating a newly
 * created object's <code>hashCode()</code>, the current
 * time (in milliseconds), the string "FastMail", and
 * this user's local address.
 *
 */
void UniqueValue::getUniqueContentIDValue(FastString &s) 
{
    FastString suffix;

    //InternetAddress addr = InternetAddress.getLocalAddress(ssn);
    //if( addr != null )
    //    suffix = addr.getAddress();
    //else 
    suffix = "@local"; // worst-case default

    unsigned int currentTimeMillis = (unsigned int)::time(NULL); 
    s.append(suffix); 
    s.append(currentTimeMillis); 
    unsigned int hashCode = fast_hash_string(s.c_str()); 

    s.clear();

    // Unique string is <hashcode>.<currentTime>.FastMail.<suffix>
    s.append(hashCode); 
    s.append('$'); 
    s.append(currentTimeMillis); 
    s.append('$'); 
    s.append(m_nParts, 2, '0'); 
    s.append(m_nCIDs++, 5, '0'); 
    s.append(suffix);
}

/**
 * Get a unique value for use in a attachment bodypart 
 * without filename.
 */
void UniqueValue::getUniqueAttachmentValue(FastString &s, FastString &mimetype) 
{
    FastString ext; 
    if( !mimetype.empty() ) 
    {
        FastString sType; 
        ContentType cType(mimetype); 
        sType.append(cType.getPrimaryType().c_str(), cType.getPrimaryType().length()); 
        sType.append('/'); 
        sType.append(cType.getSubType().c_str(), cType.getSubType().length()); 

        MimetypesFileTypeMap &maps = MimetypesFileTypeMap::getInstance(); 
        maps.getFileExtend(sType, ext); 
    }
    if( ext.empty() ) 
        ext = "rgs"; 

    s.clear(); 
    s.append("ATT"); 
    s.append(m_nAttachments++, 5, '0'); 
    s.append('.'); 
    s.append(ext); 
}

/**
 * Get a unique value for use in a <code>text/plain</code> 
 * attachment bodypart without filename.
 */
void UniqueValue::getUniqueAttachmentTextPlainValue(FastString &s) 
{
    s.clear(); 
    s.append("ATT"); 
    s.append(m_nAttachments++, 5, '0'); 
    s.append(".txt"); 
}

/**
 * Get a unique value for use in a <code>text/html</code> 
 * attachment bodypart without filename.
 */
void UniqueValue::getUniqueAttachmentTextHtmlValue(FastString &s) 
{
    s.clear(); 
    s.append("ATT"); 
    s.append(m_nAttachments++, 5, '0'); 
    s.append(".html"); 
}


//===========MimeUtility Functions Implements=============

/**
 * Count out all the header field lines count in the header buffer.
 *
 * @param header   header content
 * @param headersize   header content length
 * @return         header count
 */
int MimeUtility::getHeaderCount(const char *header, size_t headersize) 
{
    char *ptr = (char *) header; 
    char *fieldstart = NULL; 
    size_t count = 0; int dofirst = 1; 

    if( !header || headersize <= 0 ) 
        return -1; 

    // Count all the header lines count.
    while( dofirst || (fieldstart = strchr(ptr, '\n')) != NULL ) 
    {
        if( dofirst ) 
        {
            fieldstart = (char *) header; 
            dofirst = 0; 
        }

        if( fieldstart - header >= (int) headersize - 1 ) 
            break; 

        ptr = ++ fieldstart; 

        // ignore line that start with whitespace.
        if( *fieldstart == ' ' || *fieldstart == '\t' || 
            *fieldstart == '\r' || *fieldstart == '\n' ) 
            continue; 

        count ++; 
    }

    return count; 
}

/**
 * Extract out the specify field value in mime mail header content, 
 * if name is empty, return the first field.
 *
 * @param header   header content
 * @param headersize   header content length
 * @param name     field name
 * @param s        field value
 * @param valstart field start in header
 * @param valsize  field value length
 * @param startpos start find offset in header
 * @param len      find data length from offset in header
 * @return         next field offset in header
 */
int MimeUtility::getHeaderField(
        const char *header, size_t headersize, 
        FastString &name, FastString &s, char *&valstart, size_t &valsize, 
        size_t startpos, size_t len) 
{
    int dofirst = 0; 
    char *pbuf = NULL, *pend = NULL, *pstr = NULL; 
    char *pval = NULL, *pvalend = NULL; 

    valstart = NULL; valsize = 0; 
    s.clear(); 

    if( !header || headersize <= 0 || startpos >= headersize || len > headersize ) 
        return -1; 

    if( startpos < 0 ) 
        startpos = 0; 
    
    // Find area must be in header line buffer.
    if( len <= 0 || len > headersize - startpos ) 
        len = headersize - startpos; 
    
    if( len <= 0 ) 
        return -1; 

    // RFC 822 
    // field-name  =  1*<any CHAR, excluding CTLs, SPACE, and ":">
    // So remove whitespace
    if( !name.empty() ) 
        name.removeChars(" \t\r\n:"); 

    // append ':' to compare
    if( !name.empty() ) 
        name.append(":"); 

    FastString curName; 

    dofirst = 1; 
    pstr = pbuf = (char *) header + startpos; 
    pend = (char *) header + startpos + len; 

    while( dofirst || (pstr = strchr(pbuf, '\n')) != NULL ) 
    {
        // First found not strchr line-end char '\n'.
        if( !dofirst ) 
        {
            pstr ++; pbuf = pstr; 
        }
        else 
            dofirst = 0; 

        // Check if goto header buffer's end, that header buffer
        // not end with '\0'.
        if( pstr - header >= (int) headersize || 
            pstr >= pend || pstr[0] == '\0' ) 
        {
            if( pstr - header >= (int) headersize ) 
                pvalend = (char *) header + headersize; 
            else
                pvalend = pstr; 
            break; 
        }

        // RFC 822  3.2.  HEADER FIELD DEFINITIONS
        // field =  field-name ":" [ field-body ] CRLF
        // field-name not start with white-space(" \t\r\n") and 
        // that lines must be in a field-body. 
        if( pstr[0] == ' ' || pstr[0] == '\t' || 
            pstr[0] == '\r' || pstr[0] == '\n' ) 
            continue; 

        // Should found field-body first.
        if( pval != NULL ) 
        {
            pvalend = pstr; 
            break; 
        }

        // If Empty field-name, find the first header line, 
        // Else find the matches field-name's line. 
        if( name.empty() || 
#if    defined(_WIN32) || defined(__WIN32__)
            strnicmp(pstr, name.c_str(), name.length() ) == 0 ) 
#else
            strncasecmp(pstr, name.c_str(), name.length() ) == 0 ) 
#endif
        {
            if( name.empty() ) 
            {
                int gotLineCRLF = 0, gotColon = 0; 
                char *pnamend = pstr; 
                pval = pstr; 
                
                // Find the field-name which end with ':' or whitespace
                while( *pval && pval < pend ) 
                {
                    if( *pval == ':' ) 
                    {
                        pnamend = ++ pval; // Include ':' 
                        gotColon = 1; 
                        break; 
                    }

                    if( *pval == '\r' || *pval == '\n' ) 
                    {
                        //pnamend = pval; 
                        gotLineCRLF = 1; 
                        break; 
                    }

                    pval ++; 
                }

                if( gotColon )  // Found field-name.
                {
                    name.append(pstr, pnamend - pstr); 
                    name.trimChars(" \t\r\n:"); 
                }
                else
                {
                    // This case, field has no field-name.
                    pval = pnamend = pstr; 
                }

                // Goto end..
                if( pval >= pend ) //|| gotLineCRLF ) 
                {
                    pvalend = pval; 
                    break; 
                }
            }
            else
                pval = pstr + name.length(); 
        }
    }

    if( pvalend && pval && pvalend >= pval ) 
    {
        valstart = pval; 
        valsize  = pvalend - pval; 

        s.resize(valsize); 
        s.append(valstart, valsize); 
        s.trimChars(" \t\r\n"); 
    }
    else 
    {
        pval = pvalend = NULL; 
    }

    return pvalend ? pvalend - header : -1; 
}

/**
 * Find the start line in a string buffer escape empty line.
 *
 * @param buf      string buffer
 * @param len      string buffer length
 * @return         start line in buffer
 */
char *MimeUtility::findStartLine(const char *buf, const size_t len)
{
    char *start = (char *)buf;

    if( buf == NULL || buf[0] == 0 )
        return start;

    while( start && *start ) {
        if( *start == '\r' || *start == '\n' ) {
            start ++;
            if( start - buf >= (int) len ) 
                return NULL; 
            else 
                continue; 
        }
        else 
            break; 
    }

    return start; 
}

/**
 * Find the end line in a string buffer escape empty line.
 * include "\r\n".
 *
 * @param buf      string buffer
 * @param len      string buffer length
 * @return         end line in buffer
 */
char *MimeUtility::findEndLine(const char *buf, const size_t len)
{
    char *start = (char *)buf;

    if( buf == NULL || buf[0] == 0 )
        return start;

    while( start && *start ) {
        if( *start != '\n' ) {
            start ++;
            if( start - buf >= (int) len ) 
                return start; 
            else 
                continue; 
        }
        else 
            break; 
    }

    return start; 
}

/**
 * Ignore the specified chars in the end between start and end 
 * by move end pointer forward, and return the new end pointer.
 *
 * @param start    start pointer of string buffer
 * @param end      end pointer of string buffer
 * @param ignore_chars   chars to ignore
 * @return  new start pointer of buffer
 */
char *MimeUtility::ignoreCharsForward(const char *start, const char *end, 
                                      const char *ignore_chars)
{
    char *ptr = (char *)start, *pchr = 0; 
    char flag = 0; 

    if( start == 0 || end == 0 || end <= start || ignore_chars == 0 ) 
        return ptr; 

    while( ptr < end && *ptr ) {
        pchr = (char *) ignore_chars; 
        flag = 0; 
        while( *pchr ) {
            if( *ptr == *pchr ) {
                flag = 1; 
                break; 
            }
            pchr ++; 
        }
        if( flag == 0 ) 
            break; 
        ptr ++;
    }
    if( ptr > start ) 
        ptr --;     // last char should not ignore

    return ptr; 
}

/**
 * Ignore the specified chars in the end between start and end 
 * by move end pointer backward, and return the new end pointer.
 *
 * @param start    start pointer of string buffer
 * @param end      end pointer of string buffer
 * @param ignore_chars   chars to ignore
 * @return  new end pointer of buffer
 */
char *MimeUtility::ignoreCharsBackward(const char *start, const char *end, 
                                       const char *ignore_chars)
{
    char *ptr = (char *)end, *pchr = 0; 
    char flag = 0; 

    if( start == 0 || end == 0 || end <= start || ignore_chars == 0 ) 
        return ptr; 

    while( ptr > start && *ptr ) {
        pchr = (char *) ignore_chars; 
        flag = 0; 
        while( *pchr ) {
            if( *ptr == *pchr ) {
                flag = 1; 
                break; 
            }
            pchr ++; 
        }
        if( flag == 0 ) 
            break; 
        ptr --;
    }
    if( ptr < end ) 
        ptr ++;     // last char should not ignore

    return ptr; 
}

/**
 * Check two string region if equals.
 *
 * @param s1_start    start pointer of string buffer s1
 * @param s1_end      end pointer of string buffer s1
 * @param s2_start    start pointer of string buffer s2
 * @param s2_end      end pointer of string buffer s2
 * @return  TRUE if equals else FALSE
 */
BOOL MimeUtility::equalsRegion(const char *s1_start, const char *s1_end, 
                               const char *s2_start, const char *s2_end)
{
    char *p1 = (char *)s1_start, *p2 = (char *)s2_start; 

    if( s1_start == s1_end && s2_start == s2_end && s1_start == s2_start ) 
        return TRUE; 
    if( s1_start == s2_start && s2_end == s2_end ) 
        return TRUE; 
    if( s1_start > s1_end || s2_start > s2_end ) 
        return FALSE; 
    if( s1_start == 0 || s1_end == 0 || s2_start == 0 || s2_end == 0 ) 
        return FALSE; 

    while( *p1 && *p2 ) {
        if( p1 >= s1_end || p2 >= s2_end ) {
            if( p1 >= s1_end && p2 >= s2_end ) 
                return TRUE; 
            else
                return FALSE; 
        }
        if( *p1 != *p2 ) 
            return FALSE; 
        p1 ++; 
        p2 ++; 
    }

    return TRUE; 
}

/**
 * Find the end position in a mime mail content, reference to RFC 822.
 * Header content ended with a empty line.
 *
 * @param buf      string buffer
 * @param len      string buffer length
 * @return         start line in buffer
 */
char *MimeUtility::findHeaderEnd(const char *buf, const size_t len)
{
    int n = 0; 
    char *start = (char *) buf, *preln = NULL;

    if( buf == NULL || buf[0] == 0 )
        return (char *) buf;

    while( start && *start ) {
        if( *start == '\n' ) {
            preln = start; 
            n = 0; 
            while( preln && *preln && preln >= buf ) {
                if( *preln == '\n' ) {
                    n ++; 
                    if( n >= 2 ) 
                        return preln; 
                    preln --; 
                }
                else if( *preln == '\r' ) {
                    preln --; 
                    char * tmp = preln;
                    if (tmp && '\r' != *tmp && '\n' != *tmp && 
                        ((*tmp >= 0 && *tmp <= 32) || (*tmp == 127)))
                    {
                        if (*(--tmp) == '\n' && *(--tmp) == '\r')
                            preln --;
                    }
                }
                else 
                    break; 
            }
        }
        if( start - buf >= (int) len ) 
            break; 
        start ++; 
    }

    return (char *) buf; 
}

/**
 * Find the begin position of a address in a mime mail address 
 * list content, reference to RFC 822.
 *
 * @param buf      string buffer
 * @return         start position in buffer
 */
char *MimeUtility::findBeginMailPos(const char *buf) 
{
    char *pos = (char *) buf; 
    
    while( pos && *pos ) {
        if( *pos == ' '|| *pos == '\t' || *pos == '\r' || 
            *pos == '\n' || *pos == ',' || *pos == ';' ) {
            pos ++; 
            continue; 
        }
        break; 
    }
    
    return pos; 
}

/**
 * Find the end position of a address in a mime mail address 
 * list content, reference to RFC 822. And split the email 
 * address value into mailname and mailaddr.
 * example: "George Jones <Jones@Group>"
 *      mailname = "George Jones"  mailaddr = "Jones@Group"
 *
 * RFC 822
 *
 *    6.  ADDRESS SPECIFICATION
 *
 *    6.1.  SYNTAX
 *
 *    address     =  mailbox                      ; one addressee
 *                /  group                        ; named list
 *    group       =  phrase ":" [#mailbox] ";"
 *    mailbox     =  addr-spec                    ; simple address
 *                /  phrase route-addr            ; name & addr-spec
 *    route-addr  =  "<" [route] addr-spec ">"
 *    route       =  1#("@" domain) ":"           ; path-relative
 *    addr-spec   =  local-part "@" domain        ; global address
 *    local-part  =  word *("." word)             ; uninterpreted
 *                                                ; case-preserved
 *    domain      =  sub-domain *("." sub-domain)
 *    sub-domain  =  domain-ref / domain-literal
 *    domain-ref  =  atom                         ; symbolic reference
 *
 *    6.2.  SEMANTICS
 *
 *         A mailbox receives mail.  It is a  conceptual  entity  which
 *    does  not necessarily pertain to file storage.  For example, some
 *    sites may choose to print mail on their line printer and  deliver
 *    the output to the addressee's desk.
 *
 *         A mailbox specification comprises a person, system  or  pro-
 *    cess name reference, a domain-dependent string, and a name-domain
 *    reference.  The name reference is optional and is usually used to
 *    indicate  the  human name of a recipient.  The name-domain refer-
 *    ence specifies a sequence of sub-domains.   The  domain-dependent
 *    string is uninterpreted, except by the final sub-domain; the rest
 *    of the mail service merely transmits it as a literal string.
 *
 *    6.2.1.  DOMAINS
 *
 *       A name-domain is a set of registered (mail)  names.   A  name-
 *       domain  specification  resolves  to  a subordinate name-domain
 *       specification  or  to  a  terminal  domain-dependent   string.
 *       Hence,  domain  specification  is  extensible,  permitting any
 *       number of registration levels.
 *
 *
 * @param begin    address start position
 * @param mailname address spec value
 * @param mailaddr address value
 * @return         next address start position in buffer
 */
char *MimeUtility::findEndMailPos(const char *begin, 
        FastString &mailname, FastString &mailaddr) 
{
    int len = 0; 
    char *str_start_pos = NULL,  *str_end_pos = NULL; 
    char *left_start_pos = NULL, *right_end_pos = NULL; 
    char *addr_start_pos = NULL, *addr_end_pos = NULL; 
    char *eml_at_pos = NULL, *eml_pat_pos = NULL; 
    char *eml_start_pos = NULL; 
    char *pos = NULL, *tmppos = NULL; 
    
    eml_start_pos = pos = (char *) begin; 
    
    while( pos && *pos ) 
    {
        if( *pos == '"' ) {
            str_end_pos = NULL; 
            if( str_start_pos != NULL ) 
                str_end_pos = pos; 
            else
                str_start_pos = pos; 
        }
        else if( *pos == '<' ) {
            left_start_pos = pos; 
            right_end_pos = NULL; 
        }
        else if( *pos == '>' ) {
            if( left_start_pos != NULL ) 
                right_end_pos = pos; 
            else
                right_end_pos = NULL; 
        }
        else if( *pos == '@' ) {
            eml_at_pos = pos; 
            
            if( eml_at_pos > begin ) {
                tmppos = eml_at_pos - 1; 
                
                if( tmppos == eml_start_pos || tmppos == right_end_pos ) 
                    eml_at_pos = NULL; 
                else if( tmppos == str_end_pos ) 
                    addr_start_pos = str_start_pos; 
                else if( left_start_pos != NULL && left_start_pos > eml_start_pos ) 
                    addr_start_pos = left_start_pos; 
                else if( str_start_pos != NULL && str_start_pos > eml_start_pos ) 
                    addr_start_pos = str_start_pos; 
                else if( eml_start_pos == begin ) 
                    addr_start_pos = eml_start_pos; 
                else 
                    addr_start_pos = eml_start_pos + 1; 
                
                if( eml_pat_pos > addr_start_pos && eml_pat_pos > str_end_pos ) 
                    addr_start_pos = eml_pat_pos + 1; 
                
                eml_pat_pos = pos; 
            }
            else
                eml_at_pos = NULL; 
        }
        else if( *pos == ' ' || *pos == '\t' ) {
            eml_start_pos = pos; 
            
            if( addr_start_pos != NULL && eml_at_pos != NULL &&
                eml_at_pos > addr_start_pos && eml_at_pos < pos ) 
                addr_end_pos = pos; 
        }
        else if( *pos == ',' || *pos == ';' ) {
            if( addr_start_pos != NULL ) {
                if( addr_end_pos == NULL ) 
                    addr_end_pos = pos; 
                break; 
            }
            else 
                eml_start_pos = pos; 
        }
        else if( *pos == '\r' || *pos == '\n' ) {
            break; 
        }
        pos ++; 
    }
    
    if( addr_start_pos != NULL && eml_at_pos != NULL && 
        eml_at_pos > addr_start_pos && eml_at_pos < pos ) 
        addr_end_pos = pos; 
    
    if( addr_start_pos != NULL && addr_end_pos != NULL && addr_end_pos > addr_start_pos ) {
        len = addr_end_pos - addr_start_pos; 
        mailaddr.append(addr_start_pos, len); 
        mailaddr.trimChars(" \t<>"); 
        
        len = pos - begin; 
        
        if( addr_start_pos != NULL && addr_start_pos < pos ) 
            len = addr_start_pos - begin; 
        
        mailname.append(begin, len); 
        mailname.trimChars(" \t\",;"); 
    }
    
    return *pos ? pos : NULL; 
}

/**
 * Find the end position of a address in a mime mail address 
 * list content, reference to RFC 822. And split the email 
 * address value into mailname and mailaddr.
 * example: "George Jones <Jones@Group>"
 *      mailname = "George Jones"  mailaddr = "Jones@Group"
 *
 * @param buf      string buffer
 * @param mailname address spec value
 * @param mailaddr address value
 * @return         next address start position in buffer
 */
char *MimeUtility::getAddress(const char *buf, 
        FastString &mailname, FastString &mailaddr) 
{
    char *next = (char *) buf; 
    
    mailname.clear(); 
    mailaddr.clear(); 

    if( buf == NULL ) 
        return NULL; 
    
    if( (next = findBeginMailPos(buf) ) != NULL ) 
        next = findEndMailPos(next, mailname, mailaddr); 
    
    return next; 
}

/**
 * Find all address in a mime mail address list content, 
 * reference to RFC 822. And split the email address value 
 * into mailname and mailaddr, and decode mailname if it's 
 * a encode string.
 * 
 * example: 
 *      "George Jones <Jones@Group>, abc <abc@test.com>"
 *      mailname = "George Jones" 
 *      mailaddr = "Jones@Group"
 *
 * @param s      address list string buffer
 */
void MimeUtility::decodeAddress(FastString &s) 
{
    FastString mailname, mailaddr, result; 
    char *pos = (char *) s.c_str(); 

    result.resize(s.length()); 
    
    while( pos ) 
    {
        pos = getAddress(pos, mailname, mailaddr); 
        
        if( mailaddr.empty() ) 
            continue; 
        
        if( mailname.length() > 0 ) { 
            decodeText(mailname); 
            mailname.removeChars(",;<>\""); 
            mailname.trimChars(" \t"); 
        }
        
        if( !result.empty() ) 
            result.append(","); 

        if( !mailname.empty() ) 
        {
            result.append(" \""); 
            result.append(mailname); 
            result.append("\""); 
        }

        result.append(" <"); 
        result.append(mailaddr); 
        result.append(">"); 

        if( pos == NULL ) 
            break; 
    }

    s.clear(); 
    s.append(result); 
}

/**
 * Decode "unstructured" headers, that is, headers that are defined
 * as '*text' as per RFC 822. <p>
 *
 * The string is decoded using the algorithm specified in
 * RFC 2047, Section 6.1.1. If the charset-conversion fails
 * for any sequence, an UnsupportedEncodingException is thrown.
 * If the String is not an RFC 2047 style encoded header, it is
 * returned as-is <p>
 *
 * @param etext  the possibly encoded value
 */
void MimeUtility::decodeText(FastString &etext)
{
    /*
     * We look for sequences separated by "linear-white-space".
     * (as per RFC 2047, Section 6.1.1)
     * RFC 822 defines "linear-white-space" as SPACE | HT | CR | NL.
     */
    char lwsp[] = " \t\n\r"; 

    etext.trimChars(lwsp); // trim the "linear-white-space" first

    /*
     * First, lets do a quick run thru the string and check
     * whether the sequence "=?"  exists at all. If none exists,
     * we know there are no encoded-words in here and we can just
     * return the string as-is, without suffering thru the later 
     * decoding logic. 
     * This handles the most common case of unencoded headers 
     * efficiently.
     */
    if( etext.indexOf("=?") < 0 ) 
        return;

    // Current char and previous char
    char c = '\0', pc = '\0'; 
    size_t i = 0, count = 0, prevWasEncoded = 0; 
    FastString text, word, wsb; 

    text.resize(etext.length()); 

    // Remove the "linear-white-space" characters between "encoded-word"s, 
    // and decode the "encoded-word"s.
    // RFC 2047 defined "encoded-word", 4 '?' like "=?*?*?*?=".
    for( i = 0; i < etext.length(); i ++ ) 
    {
        c = etext.charAt(i); 
        if( ((c == ' ') || (c == '\t') || (c == '\r') || (c == '\n')) &&(count < 1) )// add by henh 2012/7/20 16:13:31 for &&(count < 1)
        {
            if( !word.empty() ) 
            {
                // Append previous "white-space" before current word first.
                if( !wsb.empty() ) 
                {
                    text.append(wsb); 
                    wsb.clear(); 
                }

                // Here must not be "encoded-words".
                text.append(word); 
                word.clear(); 

                prevWasEncoded = 0; 
            }

            wsb.append(c); 
            count = 0; 
            pc = '\0'; 
        }
        else 
        {
            // Check if token is an 'encoded-word' ..
            if( c == '?' ) 
            {
                // First "=?" in "encoded-word".
                if( count == 0 && pc == '=' ) 
                {
                    // If previous word not empty, not include 
                    // previous char '=', append it.
                    if( word.length() > 1 ) 
                    {
                        if( !wsb.empty() ) 
                        {
                            text.append(wsb); 
                            wsb.clear(); 
                        }

                        // trim the '=' at right first.
                        word.trimRight(word.length()-1); 

                        // Here must not be "encoded-words".
                        text.append(word); 
                        word.clear(); 

                        // append '=' again.
                        word.append('='); 
                    }

                    count = 1; 
                }
                // "encoded-word" must only be 4 '?' char.
                else if( count > 0 && count < 4 && pc != '?' ) 
                {
                    count ++; 
                }
                // Else it is NOT a "encoded-word".
                else 
                {
                    prevWasEncoded = 0; 
                    count = 0; 
                }

                // append at last for may have word before 
                // current maybe "encode-word".
                word.append(c); 
                pc = c; 
            }
            // Found last "?=" in "encoded-word".
            else if( c == '=' && pc == '?' && count == 4 ) 
            {
                // '=' is owned by this "encode-word" 
                // append it first.
                word.append(c); 

                // if the previous word was also encoded, we
                // should ignore the collected whitespace. Else
                // we include the whitespace as well.
                if( !prevWasEncoded ) 
                {
                    if( !wsb.empty() ) 
                    {
                        text.append(wsb); 
                        wsb.clear(); 
                    }
                }
                else
                    wsb.clear(); 

                if( !word.empty() ) 
                {
                    // Encoded words found. Start decoding ...
                    decodeWord(word); 
                    text.append(word); 
                    word.clear(); 
                }

                prevWasEncoded = 1; 
                count = 0; 
                pc = '\0'; 
            }
            else 
            {
                word.append(c); 
                pc = c; 
            }
        }
    }

    // Append last token, these must not be encoded.
    if( !wsb.empty() ) 
        text.append(wsb); 
    if( !word.empty() ) 
        text.append(word); 

    etext.clear(); 
    etext.append(text); 
}

/**
 * Look for encoded words within a word.  The MIME spec doesn't
 * allow this, but many broken mailers, especially Japanese mailers,
 * produce such incorrect encodings.
 */
void MimeUtility::decodeInnerWords(FastString &word)
{
    decodeWord(word); 
}

/**
 * The string is parsed using the rules in RFC 2047 for parsing
 * an "encoded-word". If the parse fails, a ParseException is 
 * thrown. Otherwise, it is transfer-decoded, and then 
 * charset-converted into Unicode. If the charset-conversion
 * fails, an UnsupportedEncodingException is thrown.<p>
 *
 * @param    eword    the possibly encoded value
 */
void MimeUtility::decodeWord(FastString &eword)
{
    int pos = 0, startpos = 0; 
    size_t start = 0; 

    // not an encoded word
    if( eword.empty() || (pos = eword.indexOf("=?")) < 0 ) 
        return; 

    // for hold the not encoded string in the eword's left
    startpos = pos; 

    // get charset
    start = pos + 2; 
    if( (pos = eword.indexOf('?', start)) < 0 ) 
        return; 
    //FastString charset; 
    //eword.substring(charset, start, pos - start); 

    // get encoding
    start = pos + 1; 
    if( (pos = eword.indexOf('?', start)) < 0 ) 
        return; 
    FastString encoding; 
    eword.substring(encoding, start, pos - start); 

    // get encoded-sequence
    start = pos + 1; 
    if( (pos = eword.indexOf("?=", start)) < 0 ) 
        return; 
    FastString word, dword; 
    eword.substring(word, start, pos - start); 

    // remove the whitespace.
    word.removeChars(" \t\r\n"); 

    // Get the appropriate decoder
    // Finally, convert the decoded bytes into a String using
    // the specified charset
    if( encoding.equalsIgnoreCase("B") ) 
        ConverterFactory::decode(ConverterFactory::B, word, dword); 
    else if( encoding.equalsIgnoreCase("Q") ) 
        ConverterFactory::decode(ConverterFactory::Q, word, dword); 
    else
        dword = word; 

    // remove the '\0' chars in the decoded string 
    // Base64 decoded may have. 
    dword.removeChar('\0'); 

    // there's still more text in the string
    if( pos + 2 < (int) eword.length() ) 
    {
        FastString res; 
        eword.substring(res, pos + 2); 

        // decode the left string
        decodeWord(res); 
        dword.append(res); 
    }

    eword.trimRight(startpos); 
    eword.append(dword); 
}

/** 
 * Check if the given string contains non US-ASCII characters.
 * @param    s    string
 * @return        ALL_ASCII if all characters in the string 
 *            belong to the US-ASCII charset. MOSTLY_ASCII
 *            if more than half of the available characters
 *            are US-ASCII characters. Else MOSTLY_NONASCII.
 */
int MimeUtility::checkAscii(FastString &s) 
{
    int ascii = 0, non_ascii = 0;
    int l = s.length();

    for( int i = 0; i < l; i++ ) 
    {
        if( nonascii((int)s.charAt(i)) ) // non-ascii
            non_ascii++;
        else
            ascii++;
    }

    if( non_ascii == 0 )
        return ALL_ASCII;
    if( ascii > non_ascii ) 
        return MOSTLY_ASCII;

    return MOSTLY_NONASCII;
}

/**
 * Get the content-transfer-encoding that should be applied
 * to the input stream of this datasource, to make it mailsafe. <p>
 *
 * The algorithm used here is: <br>
 * <ul>
 * <li>
 * If the primary type of this datasource is "text" and if all
 * the bytes in its input stream are US-ASCII, then the encoding
 * is "7bit". If more than half of the bytes are non-US-ASCII, then
 * the encoding is "base64". If less than half of the bytes are
 * non-US-ASCII, then the encoding is "quoted-printable".
 * <li>
 * If the primary type of this datasource is not "text", then if
 * all the bytes of its input stream are US-ASCII, the encoding
 * is "7bit". If there is even one non-US-ASCII character, the
 * encoding is "base64".
 * </ul>
 *
 * @param part    DataSource, MimeBodyPart or MimeMessage
 * @param encoding    the encoding. This is either "7bit",
 *            "quoted-printable" or "base64"
 */ 
void MimeUtility::getEncoding(IMimePart &part, FastString &encoding) 
{
    part.getEncoding(encoding); 

    if( encoding.empty() ) 
    {
        FastString sType; 
        part.getContentType(sType); 

        if( !sType.empty() ) 
        {
            ContentType cType(sType); 
            if( !cType.match("text/*") && !cType.match("message/*") ) 
            {
                encoding = "base64"; // default attachment encoder
                return; 
            }
        }
        
        // if not text, stop processing when we see non-ASCII
        FastString *pContent = part.getContent(); 
        if( pContent ) 
            MimeUtility::getEncoding(*pContent, encoding); 
    }
}

/**
 * Get the input data buffer's encoding that should be.
 *
 * The algorithm used here is: <br>
 * <ul>
 * <li>
 * If the primary type of this datasource is "text" and if all
 * the bytes in its input stream are US-ASCII, then the encoding
 * is "7bit". If more than half of the bytes are non-US-ASCII, then
 * the encoding is "base64". If less than half of the bytes are
 * non-US-ASCII, then the encoding is "quoted-printable".
 * <li>
 * If the primary type of this datasource is not "text", then if
 * all the bytes of its input stream are US-ASCII, the encoding
 * is "7bit". If there is even one non-US-ASCII character, the
 * encoding is "base64".
 * </ul>
 *
 * @param s    DataSource, data string
 * @param encoding    the encoding. This is either "7bit",
 *            "quoted-printable" or "base64"
 */ 
void MimeUtility::getEncoding(FastString &s, FastString &encoding) 
{
    int i = checkAscii(s);
    switch(i) 
    {
    case ALL_ASCII:
        encoding = "7bit"; // all ascii
        break;
    case MOSTLY_ASCII:
        encoding = "quoted-printable"; // mostly ascii
        break;
    default:
        encoding = "base64"; // mostly binary
        break;
    }
}

/*
 * Encode the given string. The parameter 'encodingWord' should
 * be true if a RFC 822 "word" token is being encoded and false if a
 * RFC 822 "text" token is being encoded. This is because the 
 * "Q" encoding defined in RFC 2047 has more restrictions when
 * encoding "word" tokens. (Sigh)
 */ 
void MimeUtility::encodeWord(
     FastString &word, FastString &eword, FastString &charset, 
     FastString &encoding, int encodingWord, 
     int foldEncodedWords) 
{
    // If 'string' contains only US-ASCII characters, just
    // return it.
    int ascii = checkAscii(word);
    if( ascii == ALL_ASCII ) 
    {
        eword = word; 
        return; 
    }

    // Else, apply the specified charset conversion.
    if( charset.empty() ) 
        // use default charset
        getDefaultMIMECharset(charset); // the MIME equivalent

    // If no transfer-encoding is specified, figure one out.
    if( encoding.empty() ) 
    {
        if( ascii != MOSTLY_NONASCII )
            encoding = "Q";
        else
            encoding = "B";
    }

    int b64 = TRUE; // default base64 encode
    if( encoding.equalsIgnoreCase("B") ) 
        b64 = TRUE;
    else if( encoding.equalsIgnoreCase("Q") ) 
        b64 = FALSE;

    // prefix = "=?" + charset + "?" + encoding + "?" 
    FastString prefix; 
    prefix.append("=?"); 
    prefix.append(charset); 
    prefix.append("?"); 
    prefix.append(encoding); 
    prefix.append("?"); 

    eword.clear(); 

    doEncode(word, b64, charset, 
        // As per RFC 2047, size of an encoded string should not
        // exceed 75 bytes.
        // 7 = size of "=?", '?', 'B'/'Q', '?', "?="
        75 - 7 - charset.length(), // the available space
        prefix, TRUE, encodingWord, foldEncodedWords, 
        eword);

}

void MimeUtility::doEncode(FastString &s, int b64, FastString &charset, 
                           int avail, FastString &prefix, 
                           int first, int encodingWord, int foldEncodedWords, 
                           FastString &buf) 
{
    // First find out what the length of the encoded version of
    // 'string' would be.
    int len = 0;
    if( b64 ) // "B" encoding
        len = ConverterFactory::encodedLength(ConverterFactory::B, s);
    else // "Q"
        len = ConverterFactory::encodedLength(ConverterFactory::B, s, encodingWord); 

    int size = 0;
    if( (len > avail) && ((size = s.length()) > 1) ) 
    { 
        // If the length is greater than 'avail', split 'string'
        // into two and recurse.
        FastString substr; 
        s.substring(substr, 0, size/2); 
        doEncode(substr, b64, charset, 
                 avail, prefix, first, encodingWord, foldEncodedWords, 
                 buf);

        s.substring(substr, size/2); 
        doEncode(substr, b64, charset,
                 avail, prefix, false, encodingWord, foldEncodedWords, 
                 buf);
    } 
    else 
    {
        // Now write out the encoded (all ASCII) bytes into our
        // StringBuffer
        if( !first ) // not the first line of this sequence
        {
            if( foldEncodedWords )
                buf.append("\r\n "); // start a continuation line
            else
                buf.append(" "); // line will be folded later
        }

        FastString eword; 
        if( b64 ) 
            ConverterFactory::encode(ConverterFactory::B, s, eword); 
        else
            ConverterFactory::encode(ConverterFactory::Q, s, eword); 

        buf.append(prefix);
        buf.append(eword);
        buf.append("?="); // terminate the current sequence
    }

}

/**
 * Check that the address is a valid "mailbox" per RFC822.
 * (We also allow simple names.)
 *
 * XXX - much more to check
 * XXX - doesn't handle domain-literals properly (but no one uses them) 
 *
 * @param addr          Check address string. 
 * @param routeAddr     If 0 Not Check for a legal "route-addr" Else Check. 
 * @param validate      If 0 Not check RFC822 validate address.
 * @return      If Check OK return 0 Else return -1. 
 */
int MimeUtility::checkAddress(FastString &addr, 
                               int routeAddr, int validate) 
{
    int i = 0, start = 0;
    if( addr.indexOf('"') >= 0 ) 
        return 0; // quote in address, too hard to check

    if( routeAddr ) 
    {
        /*
         * Check for a legal "route-addr":
         *        [@domain[,@domain ...]:]local@domain
         */
        for( start = 0; (i = addr.indexOfAny(",:", start)) >= 0; start = i+1 ) 
        {
            if( addr.charAt(start) != '@' ) 
                //throw new AddressException("Illegal route-addr", addr); 
                return -1; 
            if( addr.charAt(i) == ':' ) 
            {
                // end of route-addr
                start = i + 1;
                break;
            }
        }
    }

    /*
     * The rest should be "local@domain", but we allow simply "local"
     * unless called from validate.
     */
    FastString local, domain;

    if( (i = addr.indexOf('@', start)) >= 0 ) 
    {
        if( i == start ) 
            //throw new AddressException("Missing local name", addr);
            return -1; 
        if( i == (int) addr.length() - 1 )
            //throw new AddressException("Missing domain", addr);
            return -1; 
        addr.substring(local, start, i - start);
        addr.substring(domain, i + 1);
    } 
    else 
    {
        /*
         * Note that the MimeMessage class doesn't remember addresses
         * as separate objects; it writes them out as headers and then
         * parses the headers when the addresses are requested.
         * In order to support the case where a "simple" address is used,
         * but the address also has a personal name and thus looks like
         * it should be a valid RFC822 address when parsed, we only check
         * this if we're explicitly called from the validate method.
         */
        if( validate )
            //throw new AddressException("Missing final '@domain'", addr); 
            return -1; 

        /*
         * No '@' so it's not *really* an RFC822 address, but still
         * we allow some simple forms.
         */
        local = addr;
        domain.clear();
    }
    // there better not be any whitespace in it
    if( addr.indexOfAny(" \t\n\r") >= 0 ) 
        //throw new AddressException("Illegal whitespace in address", addr);
        return -1; 
    // local-part must follow RFC822, no specials except '.'
    if( local.indexOfAny(SPECIALS_NODOT) >= 0 )
        //throw new AddressException("Illegal character in local name", addr);
        return -1; 
    // check for illegal chars in the domain, but ignore domain literals
    if( !domain.empty() && domain.indexOf('[') < 0 ) 
    {
        if( domain.indexOfAny(SPECIALS_NODOT) >= 0 ) 
            //throw new AddressException("Illegal character in domain", addr);
            return -1; 
    }
    return 0; 
}

/**
 * A utility method to quote a word, if the word contains any
 * characters from the specified 'specials' list.<p>
 *
 * The <code>HeaderTokenizer</code> class defines two special
 * sets of delimiters - MIME and RFC 822. <p>
 *
 * This method is typically used during the generation of 
 * RFC 822 and MIME header fields.
 *
 * @param   word    word to be quoted
 * @param   specials the set of special characters
 * @return  the possibly quoted word to param word
 */
void MimeUtility::quote(FastString &word, FastString &specials) 
{
    size_t len = word.length();

    /*
     * Look for any "bad" characters, Escape and
     *  quote the entire string if necessary.
     */
    int needQuoting = FALSE; 

    for( size_t i = 0; i < len; i++ ) 
    {
        char c = word.charAt(i); 

        if( c == '"' || c == '\\' || c == '\r' || c == '\n' ) 
        {
            // need to escape them and then quote the whole string
            FastString sb(len + 3);
            sb.append('"');
            sb.append(word.substring(0, i)); 

            int lastc = 0;
            for( size_t j = 0; j < len; j++ ) 
            {
                char cc = word.charAt(j);

                if( (cc == '"') || (cc == '\\') || 
                    (cc == '\r') || (cc == '\n') ) 
                {
                    if( cc == '\n' && lastc == '\r' ) 
                        ;    // do nothing, CR was already escaped
                    else
                        sb.append('\\');    // Escape the character
                }
                sb.append(cc);
                lastc = cc;
            }
            sb.append('"');
            word = sb;
            return; 
        } 
        else if( c < 040 || c >= 0177 || specials.indexOf(c) >= 0 ) 
            // These characters cause the string to be quoted
            needQuoting = TRUE; 
    }

    if( needQuoting ) 
    {
        FastString sb(len + 2);
        sb.append('"'); 
        sb.append(word); 
        sb.append('"');
        word = sb; 
        return;
    }
}

/*
 * quotePhrase() quotes the words within a RFC822 phrase.
 *
 * This is tricky, since a phrase is defined as 1 or more
 * RFC822 words, separated by LWSP. Now, a word that contains
 * LWSP is supposed to be quoted, and this is exactly what the 
 * MimeUtility.quote() method does. However, when dealing with
 * a phrase, any LWSP encountered can be construed to be the
 * separator between words, and not part of the words themselves.
 * To deal with this funkiness, we have the below variant of
 * MimeUtility.quote(), which essentially ignores LWSP when
 * deciding whether to quote a word.
 *
 * It aint pretty, but it gets the job done :)
 */
void MimeUtility::quotePhrase(FastString &word) 
{
    FastString specials(SPECIALS_RFC822); 
    size_t len = word.length();

    /*
     * Look for any "bad" characters, Escape and
     *  quote the entire string if necessary.
     */
    int needQuoting = FALSE; 

    for( size_t i = 0; i < len; i++ ) 
    {
        char c = word.charAt(i); 

        if( c == '"' || c == '\\' ) 
        {
            // need to escape them and then quote the whole string
            FastString sb(len + 3);
            sb.append('"');
            for( size_t j = 0; j < len; j++ ) 
            {
                char cc = word.charAt(j);
                if( (cc == '"') || (cc == '\\') ) 
                    sb.append('\\');    // Escape the character
                sb.append(cc);
            }
            sb.append('"');
            word = sb;
            return; 
        } 
        else if( (c < 040 && c != '\r' && c != '\n' && c != '\t') || 
                  c >= 0177 || specials.indexOf(c) >= 0 ) 
            // These characters cause the string to be quoted
            needQuoting = TRUE; 
    }

    if( needQuoting ) 
    {
        FastString sb(len + 2);
        sb.append('"'); 
        sb.append(word); 
        sb.append('"');
        word = sb; 
        return;
    }
}

/**
 * Unquote string like "aaa\bb" to "aaabb", 
 * Escaped character '\'. 
 */
void MimeUtility::unquote(FastString &s) 
{
    s.trimChars(LINEAR_WHITE_SPACE); 
    if( s.startsWith("\"") && s.endsWith("\"") ) 
    {
        s.trimRight(s.length()-1);
        s.trimLeft(1); 
        // check for any escaped characters
        if( s.indexOf('\\') >= 0 ) 
        {
            FastString sb(s.length()); // approx
            for( int i = 0; i < (int) s.length(); i++ ) 
            {
                char c = s.charAt(i);
                if( c == '\\' && i < (int) s.length() - 1 )
                    c = s.charAt(++i);
                sb.append(c);
            }
            s = sb;
        }
    }
}

/**
 * Remove the RFC822 comment which between "(" and ")", and 
 * any white-space or "<>".
 */
void MimeUtility::unquoteAddress(FastString &addr) 
{
    int length = addr.length(); 
    char c = 0; 
    FastString sb(length); // approx
    for( int i = 0; i < length; i++ ) 
    {
        c = addr.charAt(i); 
        switch( c ) 
        {
        case '(':  // parsing a Comment.
            {
                int nesting = 0; 
                for( i++, nesting = 1; i < length && nesting > 0; i++ ) 
                {
                    c = addr.charAt(i);
                    switch( c ) 
                    {
                    case '\\':
                        i ++; // skip both '\' and the escaped char
                        break;
                    case '(':
                        nesting ++;
                        break;
                    case ')':
                        nesting --;
                        break;
                    case '\0':
                        goto unquote_end; 
                    default:
                        break;
                    }
                }
                if( nesting > 0 || i >= length ) 
                    goto unquote_end; 
                i --;
            }
            break; 

        case ')':
            goto unquote_end;  // missing '('
            break; 

        case '"':   // parse quoted string
            {
            //outq:
                sb.append(c);
                for( i++; i < length; i++ ) 
                {
                    c = addr.charAt(i);
                    sb.append(c);
                    switch( c ) 
                    {
                    case '\\':
                        c = addr.charAt(++i); 
                        sb.append(c); // append both '\' and the escaped char
                        break;
                    case '"':
                        goto outq;  // out of for loop
                    case '\0':
                        goto unquote_end; 
                    default:
                        break;
                    }
                }
            outq:
                if( i >= length ) 
                    goto unquote_end; 
            }
            break;

        // Ignore whitespace
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            break;

        case '\0':
            goto unquote_end; 

        default:
            sb.append(c);
            break; 
        }
    }
unquote_end: 
    addr = sb;
    addr.trimChars(ADDR_WHITE_SPACE); 
}

/*
 * RFC822 Address parser.
 * Parse the given comma separated sequence of addresses into
 * InternetAddress objects.  Addresses must follow RFC822 syntax.
 *
 * parseHdr = 0
 * Parse the given sequence of addresses into InternetAddress
 * objects.  If <code>strict</code> is false, simple email addresses
 * separated by spaces are also allowed.  If <code>strict</code> is
 * true, many (but not all) of the RFC822 syntax rules are enforced.
 * In particular, even if <code>strict</code> is true, addresses
 * composed of simple names (with no "@domain" part) are allowed.
 * Such "illegal" addresses are not uncommon in real messages. <p>
 *
 * parseHdr = 1
 * To better support the range of "invalid" addresses seen in real
 * messages, this method enforces fewer syntax rules than the
 * <code>parse</code> method when the strict flag is false
 * and enforces more rules when the strict flag is true.  If the
 * strict flag is false and the parse is successful in separating out an
 * email address or addresses, the syntax of the addresses themselves
 * is not checked.
 *
 * Non-strict parsing is typically used when parsing a list of
 * mail addresses entered by a human.  Strict parsing is typically
 * used when parsing address headers in mail messages.
 *
 * XXX - This is complex enough that it ought to be a real parser,
 *       not this ad-hoc mess, and because of that, this is not perfect.
 *
 * XXX - Deal with encoded Headers too.
 *
 * @param s            comma separated address strings
 * @param strict    enforce RFC822 syntax
 * @param addrs     array of InternetAddress objects
 * @return  address count
 */
int MimeUtility::parseAddress(InternetAddressArray &addrs, 
            FastString &s, int strict, int parseHdr) 
{
    int start = 0, end = 0, index = 0, nesting = 0;
    int start_personal = -1, end_personal = -1, start_addr = 0;
    int length = s.length();
    int is_in_group = 0;       // we're processing a group term
    int is_route_addr = 0;     // address came from route-addr term
    int is_rfc822 = 0;         // looks like an RFC822 address
    int is_comment_personal = 0; 
    char c = 0;

    // Count the address's count probably.
    int count = s.countOf('@'); 
    //if( count <= 0 ) 
    //    return -1; // No address found.
    int divcount = s.countOf(','); 
    if( count > divcount + 1 && divcount > 0 ) 
        count = divcount; 
    if( count > 100 ) 
        count = 100; 
    else if( count <= 0 ) 
        count = 2; 

    FastString addr, personal, group, addrline; 
    InternetAddressArray v(count);
    InternetAddress ma; 

    count = 0; 

    for( start = end = -1, index = 0; index < length; index++ ) 
    {
        c = s.charAt(index);

        switch( c ) 
        {
        case '(': 
            {
                // We are parsing a Comment. Ignore everything inside.
                // XXX - comment fields should be parsed as whitespace,
                //     more than one allowed per address
                is_rfc822 = 1;
                if( start == -1 ) 
                    start = index; 
                if( start_personal == -1 ) 
                    start_personal = index + 1;
                for( index++, nesting = 1; index < length && nesting > 0; index++ ) 
                {
                    c = s.charAt(index);
                    switch( c ) 
                    {
                    case '\\':
                        index ++; // skip both '\' and the escaped char
                        break;
                    case '(':
                        nesting ++;
                        break;
                    case ')':
                        nesting --;
                        break;
                    case '\0':
                        goto parse_last; 
                    default:
                        break;
                    }
                }
                /*if( nesting > 0 ) 
                    //throw new AddressException("Missing ')'", s, index);
                    goto failed_parse;*/ 
                index --;  // point to closing paren
                if( end_personal == -1 ) 
                    end_personal = index; 
                is_comment_personal = 1; 
            }
            break;

        case ')':
            //throw new AddressException("Missing '('", s, index);
            break;//goto failed_parse; 

        case '<':
            {
                is_rfc822 = 1;
                /*if( is_route_addr ) 
                    //throw new AddressException("Extra route-addr", s, index);
                    goto failed_parse; */
                start_personal = start;
                if( start_personal >= 0 ) 
                    end_personal = index;
                start = index + 1;

                int is_inquote = 0;
            //outf:
                for( index++; index < length; index++ ) 
                {
                    c = s.charAt(index);
                    switch( c ) 
                    {
                    case '\\':   // XXX - is this needed?
                        index++; // skip both '\' and the escaped char
                        break;
                    case '"':
                        is_inquote = !is_inquote;
                        break;
                    case '>':
                        if( is_inquote )
                            continue;
                        goto outf; // out of for loop
                    case '\0':
                        goto parse_last; 
                    default:
                        break;
                    }
                }
            outf:
                /*if( index >= length ) 
                {
                    if( is_inquote )
                        //throw new AddressException("Missing '\"'", s, index);
                        goto failed_parse; 
                    else
                        //throw new AddressException("Missing '>'", s, index);
                        goto failed_parse; 
                }*/
                is_route_addr = 1;
                end = index;
            }
            break;
        
        case '>':
            //throw new AddressException("Missing '<'", s, index);
            break;//goto failed_parse; 

        case '"':   // parse quoted string
            {
                is_rfc822 = 1;
                if( start == -1 )
                    start = index;
                if( start_personal == -1 ) 
                    start_personal = index;
            //outq:
                for( index++; index < length; index++ ) 
                {
                    c = s.charAt(index);
                    switch( c ) 
                    {
                    case '\\':
                        index++;    // skip both '\' and the escaped char
                        break;
                    case '"':
                        goto outq;  // out of for loop
                    case '\0':
                        goto parse_last; 
                    default:
                        break;
                    }
                }
            outq:
                /*if( index >= length )
                    //throw new AddressException("Missing '\"'", s, index);
                    goto failed_parse; */
                if( end_personal == -1 ) 
                    end_personal = index; 
            }
            break;

        case '[':   // a domain-literal, probably
            {
                is_rfc822 = 1;
            //outb:
                for( index++; index < length; index++ ) 
                {
                    c = s.charAt(index);
                    switch( c ) 
                    {
                    case '\\':
                        index++;    // skip both '\' and the escaped char
                        break;
                    case ']':
                        goto outb;  // out of for loop
                    case '\0':
                        goto parse_last; 
                    default:
                        break;
                    }
                }
            outb:
                break;/*if( index >= length )
                    //throw new AddressException("Missing ']'", s, index);
                    goto failed_parse; */
            }
            break;

        case ',':   // end of an address, probably
            {
                if( start == -1 || start_addr >= index ) 
                {
                    is_route_addr = 0;
                    is_rfc822 = 0;
                    start = end = -1;
                    start_addr = index + 1; 
                    break;  // nope, nothing there
                }
                // got a token, add this to our InternetAddress vector
                if( end == -1 ) 
                    end = index;

                if( !is_comment_personal && !is_route_addr ) 
                    start_personal = end_personal = -1; 

                addrline.clear(); 
                s.substring(addrline, start_addr, index - start_addr); 
                addrline.trimChars(LINEAR_WHITE_SPACE); 
                if( addrline.empty() ) 
                    break; 

                addr.clear(); 
                s.substring(addr, start, end - start); 

                if( is_rfc822 || strict || parseHdr ) 
                {
                    unquoteAddress(addr); 
                    /*if( strict || !parseHdr ) 
                    {
                        if( checkAddress(addr, is_route_addr, 0) != 0 ) 
                            goto failed_parse; 
                    }*/

                    ma.clear(); 
                    ma.setLine(addrline); 
                    if( is_in_group ) 
                        ma.setGroup(group); 
                    ma.setAddress(addr);

                    if( start_personal >= 0 && end_personal > start_personal ) 
                    {
                        personal.clear(); 
                        s.substring(personal, start_personal, end_personal - start_personal); 
                        personal.trimChars(LINEAR_WHITE_SPACE); 
                        MimeUtility::unquote(personal); 
                        MimeUtility::decodeText(personal); 
                        ma.setPersonal(personal); 
                        start_personal = end_personal = -1;
                    }

                    // Append this address item
                    if( (int) v.size() < count + 1 ) 
                        v.resize(count+3); 
                    v.set(ma, count++); 
                } 
                else 
                { 
                    // maybe we passed over more than one space-separated addr
                    FastString addrtmp; int dpos = 0; 
                    while( (dpos = addr.getline(dpos, addrtmp, " \t\r\n")) >= 0 ) 
                    {
                        unquoteAddress(addrtmp); 
                        /*if( MimeUtility::checkAddress(addrtmp, 0, 0) != 0 ) 
                            goto failed_parse; */
                        ma.clear();
                        if( is_in_group ) 
                            ma.setGroup(group); 
                        ma.setAddress(addrtmp);
                        // Append this address item
                        if( (int) v.size() < count + 1 ) 
                            v.resize(count+3); 
                        v.set(ma, count++); 
                    }
                }

                is_route_addr = 0;
                is_rfc822 = 0;
                start = end = -1; 
                start_personal = end_personal = -1; 
                start_addr = index + 1; 
                is_comment_personal = 0; 
            }
            break;

        case ':':
            {
                is_rfc822 = 1;
                /*if( is_in_group )
                    //throw new AddressException("Nested group", s, index);
                    goto failed_parse; */
                is_in_group = 1;
                if( start == -1 )
                    start = index;
                s.substring(group, start, index - start); 
                group.trimChars(LINEAR_WHITE_SPACE); 
                if( !group.empty() ) 
                    MimeUtility::decodeText(group); 
                else
                    is_in_group = 0; 
                start_addr = start = index + 1; 
                start_personal = end_personal = -1; 
                is_comment_personal = 0; 
            }
            break;

        case ';':
            {
                if( start == -1 || start_addr >= index ) 
                {
                    is_route_addr = 0;
                    start = end = -1; 
                    start_personal = end_personal = -1; 
                    start_addr = index + 1; 
                    is_comment_personal = 0; 
                    break; 
                }
                if( end == -1 ) 
                    end = index - 1; 

                if( !is_comment_personal && !is_route_addr ) 
                    start_personal = end_personal = -1; 

                addrline.clear(); 
                s.substring(addrline, start_addr, index - start_addr); 
                addrline.trimChars(LINEAR_WHITE_SPACE); 
                if( addrline.empty() ) 
                    break; 

                addr.clear(); 
                s.substring(addr, start, end - start); 
                unquoteAddress(addr);  

                ma.clear();
                ma.setLine(addrline); 
                if( is_in_group ) 
                    ma.setGroup(group); 
                ma.setAddress(addr);

                if( start_personal >= 0 && end_personal > start_personal ) 
                {
                    personal.clear(); 
                    s.substring(personal, start_personal, end_personal - start_personal); 
                    personal.trimChars(LINEAR_WHITE_SPACE); 
                    MimeUtility::unquote(personal); 
                    MimeUtility::decodeText(personal); 
                    ma.setPersonal(personal); 
                    start_personal = end_personal = -1;
                }

                // Append this address item
                if( (int) v.size() < count + 1 ) 
                    v.resize(count+3); 
                v.set(ma, count++); 

                group.clear(); 
                is_in_group = 0;
                is_route_addr = 0;
                start = end = -1; 
                start_personal = end_personal = -1; 
                start_addr = index + 1; 
                is_comment_personal = 0; 
            }
            break;

        // Ignore whitespace
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            break;

        case '\0':
            goto parse_last; 

        default:
            if( start == -1 )
                start = index;
            break;
        }
    }

parse_last: 
    if( start >= 0 && index > start_addr ) 
    {
        /**
         * The last token, add this to our InternetAddress vector.
         * Note that this block of code should be identical to the
         * block above for "case ','".
         */
        if( end == -1 )
            end = index;

        if( !is_comment_personal && !is_route_addr ) 
            start_personal = end_personal = -1; 

        addrline.clear(); 
        s.substring(addrline, start_addr, index - start_addr); 
        addrline.trimChars(LINEAR_WHITE_SPACE); 
        if( addrline.empty() ) 
            addrline = s;//goto failed_parse; 

        addr.clear(); 
        s.substring(addr, start, end - start);
        if( addr.empty())
        	addr = s;

        if( is_rfc822 || strict || parseHdr ) 
        {
            unquoteAddress(addr); 
            /*if( strict || !parseHdr ) 
            {
                if( checkAddress(addr, is_route_addr, 0) != 0 ) 
                    goto failed_parse; 
            }*/

            ma.clear();
            ma.setLine(addrline); 
            if( is_in_group ) 
                ma.setGroup(group); 
            MimeUtility::decodeText(addr); 
            ma.setAddress(addr);

            if( start_personal >= 0 && end_personal > start_personal ) 
            {
                personal.clear(); 
                s.substring(personal, start_personal, end_personal - start_personal); 
                personal.trimChars(LINEAR_WHITE_SPACE); 
                MimeUtility::unquote(personal); 
                MimeUtility::decodeText(personal); 
                ma.setPersonal(personal);
            }

            // Append this address item
            if( (int) v.size() < count + 1 ) 
                v.resize(count+3); 
            v.set(ma, count++); 
        } 
        else 
        {
            // maybe we passed over more than one space-separated addr
            FastString addrtmp; int dpos = 0; 
            while( (dpos = addr.getline(dpos, addrtmp, " \t\r\n")) >= 0 ) 
            {
                unquoteAddress(addrtmp); 
                /*if( MimeUtility::checkAddress(addrtmp, 0, 0) != 0 ) 
                    goto failed_parse; */
                ma.clear();
                ma.setAddress(addrtmp);
                // Append this address item
                if( (int) v.size() < count + 1 ) 
                    v.resize(count+3); 
                v.set(ma, count++); 
            }
        }
    }

failed_parse: 
    // Copy the parsed address list data.
    if( count > 0 ) 
        addrs.copyFrom(v, count);
    
    return count; 
}

/**
 * Return the groups list of a address list.  
 *
 * @param addrs        array of InternetAddress objects
 * @param groups    Address list's group names list
 * @return  groups count
 */
int MimeUtility::getAddressGroups(
    InternetAddressArray &addrs, FastStringArray &groups) 
{
    if( addrs.size() == 0 ) return 0; 

    FastString list, group; 
    InternetAddress *paddr = NULL; 
    InternetAddressArrayIterator it(addrs); 
    int count = 0; 

    while( it.next(paddr) && paddr ) 
    {
        if( paddr->groupLength() > 0 ) 
        {
            group.clear(); 
            group.append('\0'); 
            group.append(paddr->group(), paddr->groupLength()); 
            group.append('\0'); 
            if( list.indexOf(group) < 0 ) 
            {
                list.append(group); 
                count ++; 
            }
        }
    }

    if( count <= 0 ) return 0; 
    groups.size(count); 
    count = 0; 

    list.clear(); 
    it.first(); 

    while( it.next(paddr) && paddr ) 
    {
        if( paddr->groupLength() > 0 ) 
        {
            group.clear(); 
            group.append('\0'); 
            group.append(paddr->group(), paddr->groupLength()); 
            group.append('\0'); 
            if( list.indexOf(group) < 0 ) 
            {
                list.append(group); 
                group.clear(); 
                paddr->getGroup(group); 
                paddr->getGroup(group); 
                groups.set(group, count++); 
            }
        }
    }

    return count; 
}

/**
 * Return the members of a group address.  A group may have zero,
 * one, or more members.  If this address is not a group, null
 * is returned.  The <code>strict</code> parameter controls whether
 * the group list is parsed using strict RFC 822 rules or not.
 * The parsing is done using the <code>parseHeader</code> method.
 *
 * @param addrs        array of InternetAddress objects
 * @param name      group name
 * @return  group addresses count
 */
int MimeUtility::getGroupAddress(
    InternetAddressArray &addrs, FastString &name, InternetAddressArray &grpaddrs) 
{
    if( addrs.size() == 0 ) return 0; 

    InternetAddress *paddr = NULL; 
    InternetAddressArrayIterator it(addrs); 
    int count = 0; 

    while( it.next(paddr) && paddr ) 
    {
        if( (name.empty() && paddr->groupLength() == 0) || 
            (!name.empty() && paddr->equalsGroup(name)) ) 
        {
            count ++; 
        }
    }

    if( count <= 0 ) return 0; 
    grpaddrs.size(count); 
    count = 0; 

    it.first(); 

    while( it.next(paddr) && paddr ) 
    {
        if( (name.empty() && paddr->groupLength() == 0) || 
            (!name.empty() && paddr->equalsGroup(name)) ) 
        {
            grpaddrs.set(*paddr, count++); 
        }
    }

    return count; 
}

/**
 * Convert the given array of InternetAddress objects into
 * a comma separated sequence of address strings. The
 * resulting string contains only US-ASCII characters, and
 * hence is mail-safe. <p>
 *
 * @param addresses    array of InternetAddress objects
 * @param s     comma separated string of addresses
 */
void MimeUtility::getAddressString(
     InternetAddressArray &addrs, FastString &s) 
{
    size_t linelen = 0; 
    FastString addrline; 
    FastStringArray groups; 
    InternetAddress *paddr = NULL; 
    InternetAddressArrayIterator it(addrs); 
    s.clear(); 

    MimeUtility::getAddressGroups(addrs, groups); 
    if( groups.size() > 0 ) 
    {
        FastString *pgroup = NULL; 
        FastStringArrayIterator git(groups); 
        while( git.next(pgroup) && pgroup ) 
        {
            if( pgroup->length() > 0 ) 
            {
                if( !s.empty() ) 
                    s.append(", "); 
                addrline.clear(); 
                MimeUtility::encodeWord(*pgroup, addrline); 
                MimeUtility::quotePhrase(addrline); 
                if( linelen + addrline.length() + 2 > 70 ) 
                {
                    s.append("\r\n    "); 
                    linelen = 0; 
                }
                s.append(addrline); 
                s.append(": "); 
                linelen += addrline.length() + 2; 

                int isfirst = 1; 
                it.first(); 
                while( it.next(paddr) && paddr ) 
                {
                    if( !paddr->equalsGroup(*pgroup) ) 
                        continue; 
                    addrline.clear(); 
                    paddr->toString(addrline); 
                    if( addrline.empty() ) 
                        continue; 
                    if( !s.empty() && !isfirst ) 
                        s.append(", "); 
                    if( linelen + addrline.length() > 70 ) 
                    {
                        s.append("\r\n    "); 
                        linelen = 0; 
                    }
                    s.append(addrline); 
                    linelen += addrline.length(); 
                    isfirst = 0; 
                }
                s.append(";"); 
            }
        }
    }

    it.first(); 
    while( it.next(paddr) && paddr ) 
    {
        if( paddr->groupLength() > 0 ) 
            continue; 
        addrline.clear(); 
        paddr->toString(addrline); 
        if( addrline.empty() ) 
            continue; 
        if( !s.empty() ) 
            s.append(", "); 
        if( linelen + addrline.length() > 70 ) 
        {
            s.append("\r\n    "); 
            linelen = 0; 
        }
        s.append(addrline); 
        linelen += addrline.length(); 
    }
}

/**
 * Fold a string at linear whitespace so that each line is no longer
 * than 76 characters, if possible.  If there are more than 76
 * non-whitespace characters consecutively, the string is folded at
 * the first whitespace after that sequence.  The parameter
 * <code>used</code> indicates how many characters have been used in
 * the current line; it is usually the length of the header name. <p>
 *
 * Note that line breaks in the string aren't escaped; they probably
 * should be.
 *
 * @param used  characters used in line so far
 * @param s     the string to fold
 */
void MimeUtility::fold(FastString &s, int used) 
{
    // Strip trailing spaces and newlines 
    s.rtrimChars(" \\t\r\n"); 

    // if the string fits now, just return it
    if( used + s.length() <= 76 )
        return;

    // have to actually fold the string
    FastString sb(s.length() + 4);
    char lastc = 0, c = 0;

    while( used + s.length() > 76 ) 
    {
        int lastspace = -1;
        for( int i = 0; i < (int) s.length(); i++ ) 
        {
            if( lastspace != -1 && used + i > 76 )
                break;
            c = s.charAt(i);
            if( c == ' ' || c == '\t' )
                if( !(lastc == ' ' || lastc == '\t') ) 
                    lastspace = i;
            lastc = c;
        }
        if( lastspace == -1 ) 
        {
            // no space, use the whole thing
            sb.append(s);
            s.clear(); 
            used = 0;
            break;
        }
        sb.append(s.substring(0, lastspace));
        sb.append("\r\n");
        lastc = s.charAt(lastspace);
        sb.append(lastc);
        s = s.substring(lastspace + 1);
        used = 1;
    }
    sb.append(s);
    s = sb;
}

/**
 * Unfold a folded header.  Any line breaks that aren't escaped and
 * are followed by whitespace are removed.
 *
 * @param s     the string to unfold
 */
void MimeUtility::unfold(FastString &s) 
{
    FastString sb(s.length());
    int i = 0;
    while( (i = s.indexOfAny("\r\n")) >= 0 ) 
    {
        int start = i;
        int l = s.length();
        i++; // skip CR or NL
        if( i < l && s.charAt(i - 1) == '\r' && s.charAt(i) == '\n' )
            i++; // skip LF
        if( start == 0 || s.charAt(start - 1) != '\\' ) 
        {
            char c;
            // if next line starts with whitespace, skip all of it
            // XXX - always has to be true?
            if( i < l && ((c = s.charAt(i)) == ' ' || c == '\t') ) 
            {
                i ++; // skip whitespace
                while( i < l && ((c = s.charAt(i)) == ' ' || c == '\t') )
                    i++;
                if( start != 0 ) 
                {
                    sb.append(s.substring(0, start));
                    sb.append(' ');
                }
                s = s.substring(i);
                continue;
            }
            // it's not a continuation line, just leave it in
            sb.append(s.substring(0, i));
            s = s.substring(i);
        } 
        else 
        {
            // there's a backslash at "start - 1"
            // strip it out, but leave in the line break
            sb.append(s.substring(0, start - 1));
            sb.append(s.substring(start, i - start));
            s = s.substring(i);
        }
    }
    if( !sb.empty() ) 
    {
        sb.append(s);
        s = sb;
    }
}

/**
 * Wrap an encoder around the given output stream. 
 * All the encodings defined in RFC 2045 are supported here. 
 * They include "base64", "quoted-printable", "7bit", "8bit" and
 * "binary". In addition, "uuencode" is also supported.
 *
 * @param s         input string
 * @param encoding  the encoding of the string. 
 * @param ds        output string that applies the
 *                specified encoding.
 */
void MimeUtility::encode(FastString &s, FastString &ds, FastString &encoding)
{
    if( encoding.empty() )
    {
        ds = s;
    }
    else if( encoding.equalsIgnoreCase("base64") ) 
    {
        // with line break while line length = 77
        ConverterFactory::encode(ConverterFactory::BASE64, s, ds, DEFAULT_ENCODE_LINELENGTH); 
    }
    else if( encoding.equalsIgnoreCase("quoted-printable") ) 
    {
        ConverterFactory::encode(ConverterFactory::QP, s, ds); 
    }
    else if( encoding.equalsIgnoreCase("uuencode") ||
             encoding.equalsIgnoreCase("x-uuencode") ||
             encoding.equalsIgnoreCase("x-uue") ) 
    {
        //ConverterFactory::encode(ConverterFactory::UU, s, ds); 
        ds = s; 
    }
    else if( encoding.equalsIgnoreCase("binary") ||
             encoding.equalsIgnoreCase("7bit") ||
             encoding.equalsIgnoreCase("8bit") ) 
    {
        ds = s; 
    }
    else
    {
        //throw new MessagingException("Unknown encoding: " +encoding);
        ds = s; 
    }
}

/**
 * Decode the given input string. The Input string returned is
 * the decoded string. All the encodings defined in RFC 2045
 * are supported here. They include "base64", "quoted-printable",
 * "7bit", "8bit", and "binary". In addition, "uuencode" is also
 * supported.
 *
 * @param s         input string
 * @param encoding  the encoding of the string.
 * @param ds        output string
 */
void MimeUtility::decode(FastString &s, FastString &ds, FastString &encoding)
{
    if( encoding.equalsIgnoreCase("base64") ) 
    {
        ConverterFactory::decode(ConverterFactory::BASE64, s, ds); 
    }
    else if( encoding.equalsIgnoreCase("quoted-printable") ) 
    {
        ConverterFactory::decode(ConverterFactory::QP, s, ds); 
    }
    else if( encoding.equalsIgnoreCase("uuencode") ||
             encoding.equalsIgnoreCase("x-uuencode") ||
             encoding.equalsIgnoreCase("x-uue") ) 
    {
        //ConverterFactory::decode(ConverterFactory::UU, s, ds); 
        ds = s; 
    }
    else if( encoding.equalsIgnoreCase("binary") ||
             encoding.equalsIgnoreCase("7bit") ||
             encoding.equalsIgnoreCase("8bit") )
    {
        ds = s; 
    }
    else
    {
        ds = s; 
    }
}

/**
 * Replace the src string with cidurl in html. this should use HTML or XML 
 * parser. now is sample replacement. 
 *
 * @param html      html main bodytext string
 * @param src       attachment src name in html, include filename
 * @param cidurl    attachment cidurl
 */
int MimeUtility::replaceSrcWithCIDURL(FastString &html, FastString &src, 
                                      FastString &cidurl) 
{
    if( html.empty() || src.empty() ) 
        return 0; 

    int length = (int) html.length(); 
    int srclen = (int) src.length(); 

    FastString newhtml(length), tmp; 
    int pos = 0, from = 0, count = 0; 

    while( (pos = html.indexOf(src, from)) >= 0 ) 
    {
        BOOL found = TRUE; 
        BOOL gotQuoted = FALSE; 
        int src_start = pos; 
        int src_end   = pos + srclen; 

        {
            BOOL got = FALSE; 
            BOOL gotName = FALSE; 
            int left_pos = pos - 1; 
            int name_start = -1, name_end = -1; 

            while( left_pos >= 0 ) 
            {
                char c = html.charAt(left_pos); 

                switch( c ) 
                {
                case '\"': 
                case '\'':
                    if( got == FALSE ) 
                    {
                        src_start = left_pos + 1; 
                        got = TRUE; 
                        gotQuoted = TRUE; 
                    }
                    break; 

                case '=' :
                    if( got == FALSE ) 
                    {
                        src_start = left_pos + 1; 
                        got = TRUE; 
                    }
                    if( name_start == -1 ) 
                        name_end = left_pos; 
                    break; 

                case '<' :
                    goto left_end; 

                case '>' :
                    found = FALSE;  // not in html elms
                    goto left_end; 

                case ' ' :
                case '\t':
                case '\r':
                case '\n':
                    if( name_start == -1 ) 
                    {
                        if( name_end != -1 && gotName == TRUE ) 
                            name_start = left_pos; 
                    }
                    break;  // ignore whitespace

                default: 
                    if( got == FALSE ) 
                    {
                        found = FALSE; 
                        goto left_end; 
                    }
                    if( name_end != -1 ) 
                        gotName = TRUE; 
                    break; 
                }

                left_pos --; 
            }
left_end: 
            if( found == TRUE ) 
            {
                if( name_start >= 0 && name_end > name_start ) 
                {
                    html.substring(tmp, name_start, name_end - name_start); 
                    tmp.trimChars("\"\'= \t\r\n"); 

                    if( tmp.equalsIgnoreCase("src") || 
                        tmp.equalsIgnoreCase("background") ) 
                        found = TRUE; 
                    else
                        found = FALSE; 
                }
                else
                    found = FALSE; 
            }
        }

        if( found == TRUE ) 
        {
            BOOL got = FALSE; 
            BOOL gotQuotedEnd = FALSE; 
            int right_pos = pos + srclen; 

            while( right_pos < length ) 
            {
                char c = html.charAt(right_pos); 

                switch( c ) 
                {
                case '\"': 
                case '\'':
                    if( got == FALSE ) 
                    {
                        src_end = right_pos; 
                        got = TRUE; 
                        gotQuotedEnd = TRUE; 
                    }
                    break; 

                case '<' :
                    found = FALSE;  // not in html elms
                    goto right_end; 

                case '>' :
                    if( got == FALSE ) 
                    {
                        src_end = right_pos; 
                        got = TRUE; 
                    }
                    goto right_end; 

                case ' ' :
                case '\t':
                case '\r':
                case '\n':
                    if( gotQuoted == FALSE ) 
                    {
                        if( got == FALSE ) 
                        {
                            src_end = right_pos; 
                            got = TRUE; 
                        }
                    }
                    break;  // ignore whitespace

                default: 
                    if( got == FALSE ) 
                    {
                        if( gotQuoted == TRUE && gotQuotedEnd == FALSE ) 
                        {
                            found = FALSE; 
                            goto right_end; 
                        }
                    }
                    break; 
                }

                right_pos ++; 
            }
right_end: 
            ;
        }

        if( found == TRUE ) 
        {
            html.substring(tmp, from, src_start - from); 
            newhtml.append(tmp); 
            newhtml.append(cidurl); 
            count ++; 
        }
        else
        {
            src_end = pos + srclen; 
            html.substring(tmp, from, src_end - from); 
            newhtml.append(tmp); 
        }

        from = src_end; 
    }

    if( from > 0 && from < length ) 
    {
        html.substring(tmp, from); 
        newhtml.append(tmp); 
    }
    
    if( count > 0 ) 
        html = newhtml; 

    return count; 
}


_FASTMIME_END_NAMESPACE
