//=============================================================================
/**
 *  @file    MimeObject.cpp
 *
 *  ver 1.0.0 2004/06/16 Naven Exp, for Mime Message Parse Engine.
 *
 *  @author Naven 2004/06/16 created.
 */
//=============================================================================

#include "MimeObject.h" 


_FASTMIME_BEGIN_NAMESPACE


//============ParameterList Functions Implements================

/**
 * Initialization that takes a parameter-list string. The String
 * is parsed and the parameters are collected and stored internally.
 * A ParseException is thrown if the parse fails. 
 * Note that an empty parameter-list string is valid and will be 
 * parsed into an empty ParameterList.
 * RFC 1521
 * content   := "Content-Type"  ":"  type  "/"  subtype  *(";"parameter)
 * parameter := attribute "=" value
 * attribute := token   ; case-insensitive
 * value     := token / quoted-string
 * token     := 1*<any (ASCII) CHAR except SPACE, CTLs,
 *                 or tspecials>
 *
 * @param s    the parameter-list string.
 * @return  -1 if the parse fails else 0 if success.
 * @see RFC 1521
 */
int ParameterList::initialize(FastString &s) 
{
    HeaderTokenizer ht(s, HeaderTokenizer::MIME);
    HeaderTokenizer::Token tk; 

    int type = 0, result = 1;
    NameString name; 
    ValueString value;

    while( (result = ht.next(tk)) > 0 ) 
    {
        type = tk.getType();
        if( type == HeaderTokenizer::Token::TOKEN_EOF ) // done
            break;

        // earch parameter must has ';' before in MIME, see RFC822
        if( (char)type == ';' ) 
        {
            // expect parameter name
            if( (result = ht.next(tk)) <= 0 ) 
                break; 

            // tolerate trailing semicolon, even though it violates the spec
            if( tk.getType() == HeaderTokenizer::Token::TOKEN_EOF )
                break;

            // parameter name must be a MIME Atom
            if( tk.getType() != HeaderTokenizer::Token::ATOM ) 
                // throw new ParseException(); 
                return -1; 

            name.set(tk.getValue().c_str(), tk.getValue().length()); 

            // expect '='
            if( (result = ht.next(tk)) <= 0 ) 
                break; 

            if( (char)tk.getType() != '=' ) 
                // throw new ParseException(); 
                return -1; 
            
            // expect parameter value
            if( (result = ht.next(tk)) <= 0 ) 
                break; 

            type = tk.getType();
            // parameter value must be a MIME Atom or Quoted String
            if( type != HeaderTokenizer::Token::ATOM &&
                type != HeaderTokenizer::Token::QUOTEDSTRING )
                // throw new ParseException(); 
                return -1; 
            
            value.set(tk.getValue().c_str(), tk.getValue().length()); 
            m_hmList.put(name, value);
        } 
        else
            // throw new ParseException();
            return -1; 
    }

    return result > 0 ? 0 : -1; 
}

/**
 * Return an array of the names of all parameters in this list.
 *
 * @return   array of all parameter names in this list.
 */
int ParameterList::getNames(NameArray &names) 
{
    names.resize(m_hmList.size()); 
    ParameterList::ParamHashMap::iterator it(m_hmList); 
    int count = 0; 
    while( !it.done() ) 
    {
        count ++; 
        names.set(it->first(), count-1); 
        it.advance(); 
    }
    return count; 
}

/**
 * Convert this ParameterList into a MIME String. If this is
 * an empty list, an empty string is returned.
 *   
 * The 'used' parameter specifies the number of character positions
 * already taken up in the field into which the resulting parameter
 * list is to be inserted. It's used to determine where to fold the
 * resulting parameter list.
 *
 * @param used      number of character positions already used, in
 *                  the field into which the parameter list is to
 *                  be inserted.
 * @param s         String
 */  
void ParameterList::toString(FastString &s, int used) 
{
    BOOL firstline = TRUE; 
    
    iterator it = this->begin(); 
    s.clear();

    while( !it.done() ) 
    {
        NameString name(it->first().c_str(), it->first().length());
        FastString value(it->second().c_str(), it->second().length()); 
        
        if( !name.empty() && !value.empty() ) 
        {
            this->quote(value); 

            s.append("; ");
            used += 2;
            int len = name.length() + value.length() + 1;
            if( used + len > 76 || firstline ) 
            {
                // overflows ...
                s.append("\r\n\t"); // .. start new continuation line
                used = 8;           // account for the starting <tab> char
                firstline = FALSE;  // first line append "\r\n\t" not RFC2046 rule
            }

            s.append(name.c_str(), name.length()); 
            s.append('='); 

            used += name.length() + 1;
            if( used + value.length() > 76 ) 
            {   // still overflows ...
                // have to fold value
                MimeUtility::fold(value, used);
                s.append(value);
                int lastlf = s.lastIndexOf('\n');
                if( lastlf >= 0 )  // always true
                    used += s.length() - lastlf - 1;
                else
                    used += s.length();
            } 
            else 
            {
                s.append(value);
                used += value.length();
            }
        }

        it.advance(); 
    }
}


//============ContentType Functions Implements================

/**
 * Initialize that takes a Content-Type string. The String
 * is parsed into its constituents: primaryType, subType
 * and parameters. A ParseException is thrown if the parse fails. 
 *
 * @param s    the Content-Type string.
 * @return     -1 if the parse fails else 0 if success.
 */
int ContentType::initialize(FastString &s) 
{
    HeaderTokenizer ht(s, HeaderTokenizer::MIME);
    HeaderTokenizer::Token tk; 

    // First "type" ..
    ht.next(tk); 
    if( tk.getType() != HeaderTokenizer::Token::ATOM )
        //throw new ParseException();
        return -1; 
    FastString primary; 
    tk.getValue(primary);
    m_sPrimaryType.set(primary.c_str(), primary.length()); 

    // The '/' separator ..
    ht.next(tk); 
    if( (char)tk.getType() != '/' )
        //throw new ParseException();
        return -1; 

    // Then "subType" ..
    ht.next(tk);
    if( tk.getType() != HeaderTokenizer::Token::ATOM )
        //throw new ParseException();
        return -1; 
    FastString subtype; 
    tk.getValue(subtype); 
    m_sSubType.set(subtype.c_str(), subtype.length()); 

    // Finally parameters ..
    FastString rem; 
    ht.getRemainder(rem); 
    if( !rem.empty() ) 
        m_pList.load(rem);

    return 0; 
}

/**
 * Match with the specified ContentType object. This method
 * compares <strong>only the <code>primaryType</code> and 
 * <code>subType</code> </strong>. The parameters of both operands
 * are ignored. <p>
 *
 * For example, this method will return <code>true</code> when
 * comparing the ContentTypes for <strong>"text/plain"</strong>
 * and <strong>"text/plain; charset=foobar"</strong>.
 *
 * If the <code>subType</code> of either operand is the special
 * character '*', then the subtype is ignored during the match. 
 * For example, this method will return <code>true</code> when 
 * comparing the ContentTypes for <strong>"text/plain"</strong> 
 * and <strong>"text/" </strong>
 *
 * @param cType    ContentType to compare this against
 */
BOOL ContentType::match(ContentType &cType) 
{
    // Match primaryType
    if( !m_sPrimaryType.equalsIgnoreCase(cType.getPrimaryType()) )
        return FALSE;

    FastString sType; 
    cType.getSubType(sType);

    // If either one of the subTypes is wildcarded, return true
    if( (m_sSubType.charAt(0) == '*') || (sType.charAt(0) == '*') )
        return TRUE;

    // Match subType
    if( !m_sSubType.equalsIgnoreCase(sType) ) 
        return FALSE;

    return TRUE;
}

/**
 * Retrieve a RFC2045 style string representation of
 * this Content-Type. Returns <code>empty</code> if
 * the conversion failed.
 *
 * @return    RFC2045 style string
 */
void ContentType::toString(FastString &s) 
{
    s.clear(); 

    if( m_sPrimaryType.empty() || m_sSubType.empty() ) // need both
        return;

    s.append(m_sPrimaryType.c_str(), m_sPrimaryType.length()); 
    s.append('/'); 
    s.append(m_sSubType.c_str(), m_sSubType.length()); 
    
    if( m_pList.size() > 0 ) 
    {
        // append the parameter list 
        // use the length of the string buffer + the length of
        // the header name formatted as follows "Content-Type: "
        FastString tmp; 
        m_pList.toString(tmp, s.length() + 14); 
        s.append(tmp);
    }
}


//============ContentDisposition Functions Implements================

/**
 * Constructor that takes a ContentDisposition string. The String
 * is parsed into its constituents: dispostion and parameters. 
 * A ParseException is thrown if the parse fails. 
 *
 * @param s    the ContentDisposition string.
 * @return  -1 if the parse fails else 0 if success.
 */
int ContentDisposition::initialize(FastString &s) 
{
    HeaderTokenizer ht(s, HeaderTokenizer::MIME);
    HeaderTokenizer::Token tk;

    // First "disposition" ..
    ht.next(tk);
    if( tk.getType() != HeaderTokenizer::Token::ATOM )
        //throw new ParseException(); 
        return -1; 
    m_sDisposition.set(tk.getValue().c_str(), tk.getValue().length());

    // Then parameters ..
    FastString rem; 
    ht.getRemainder(rem);
    if( !rem.empty() ) 
        m_pList.load(rem); 

    return 0; 
}

/**
 * Retrieve a RFC2045 style string representation of
 * this ContentDisposition. Returns <code>empty</code> if
 * the conversion failed.
 *
 * @return    RFC2045 style string
 */
void ContentDisposition::toString(FastString &s) 
{
    s.clear(); 

    if( m_sDisposition.empty() )
        return;

    s.append(m_sDisposition.c_str(), m_sDisposition.length()); 

    if( m_pList.size() > 0 )
    {
        // append the parameter list  
        // use the length of the string buffer + the length of 
        // the header name formatted as follows "Content-Disposition: "
        FastString tmp; 
        m_pList.toString(tmp, s.length() + 21); 
        s.append(tmp);
    }
}


//============ContentID Functions Implements================

/**
 * Constructor that takes a ContentID string. The String
 * is parsed into its constituents: ContentID. 
 *
 * @see RFC822
 *
 *  content-id  = url-addr-spec
 *  url-addr-spec = addr-spec  ; URL encoding of RFC 822 addr-spec
 *  addr-spec   =  local-part "@" domain        ; global address
 *  domain      =  sub-domain *("." sub-domain)
 *  local-part  =  word *("." word)
 *  word        =  atom / quoted-string
 *  atom        =  1*<any CHAR except specials, SPACE and CTLs>
 *  quoted-string = <"> *(qtext/quoted-pair) <">; 
 *  qtext       =  <any CHAR excepting <">,     ; => may be folded
 *                 "\" & CR, and including
 *                 linear-white-space>
 *  quoted-pair =  "\" CHAR                     ; may quote any char
 *
 * @param s    the ContentID string.
 * @return  -1 if the parse fails else 0 if success.
 */
int ContentID::initialize(FastString &s) 
{
    InternetAddress addr(s); 

    m_sCID.set(addr.address(), addr.addressLength());
    
    if( m_sCID.empty() ) 
        return -1; 
    else
        return 0; 
}

/**
 * RFC2392
 *
 *   A "cid" URL is converted to the corresponding Content-ID message
 *   header [MIME] by removing the "cid:" prefix, converting the % encoded
 *   character to their equivalent US-ASCII characters, and enclosing the
 *   remaining parts with an angle bracket pair, "<" and ">".  For
 *   example, "cid:foo4%25foo1@bar.net" corresponds to
 *
 *     Content-ID: <foo4%25foo1@bar.net>
 *
 *   Reversing the process and converting URL special characters to their
 *   % encodings produces the original cid.
 *
 * @return    cidurl string
 */
void ContentID::toCIDURL(FastString &s) 
{
    s.clear(); 

    if( m_sCID.empty() )
        return;

    FastString cid(m_sCID.c_str(), m_sCID.length()); 
    MimeUtility::unquoteAddress(cid); 

    FastString cidurl; 
    ConverterFactory::encode(ConverterFactory::URL, cid, cidurl); 

    if( !cidurl.empty() ) 
    {
        s.append("cid:"); 
        s.append(cidurl); 
    }
}

/**
 * Retrieve a RFC2045 style string representation of
 * this ContentID. Returns <code>empty</code> if
 * the conversion failed.
 *
 * @return    RFC2045 style string
 */
void ContentID::toString(FastString &s) 
{
    s.clear(); 

    if( m_sCID.empty() )
        return;

    FastString cid(m_sCID.c_str(), m_sCID.length()); 
    MimeUtility::unquoteAddress(cid); 

    if( !cid.empty() ) 
    {
        s.append('<'); 
        s.append(cid); 
        s.append('>'); 
    }
}


//==============hdr functions implements======================

/**
 * Constructor that takes a line and splits out
 * the header name. Line can be mutilines.
 *
 * @param line     line string
 * @param len      line length
 */
void hdr::setLine(const char *line, size_t len) 
{
    if( line ) 
    {
        if( len <= 0 ) len = strlen(line); 
        char *start = (char *)strchr(line, ':'); 
        if( start ) 
        {
            this->m_sName.append(line, start - line); 
            this->m_sValue.append(start + 1, line + len - start + 1); 
            this->m_sName.trim(); 
        }
        else
        {
            // should never happen
            this->m_sName.clear(); 
            this->m_sValue.clear(); 
            this->m_sName.append(line, len); 
            this->m_sName.trim(); 
        }
    }
}

/**
 * Get the "address" part of the header line.
 */
int hdr::getAddress(InternetAddressArray &addrs) 
{
    size_t i = 0; 
    if( this->m_nAddressCount == 0 && !this->m_sValue.empty() ) 
        this->parseAddressHeader(); 
    if( this->m_nAddressCount > 0 ) 
    {
        addrs.size(this->m_nAddressCount); 
        InternetAddress *paddr = NULL; 
        InternetAddressArrayIterator it(this->m_arAddresses); 
        while( it.next(paddr) && paddr ) 
        {
            if( paddr->addressLength() > 0 ) 
                addrs.set(*paddr, i++); 
            if( i >= this->m_nAddressCount ) 
                break; 
        }
    }
    return (this->m_nAddressCount = i); 
}

/**
 * Clear the array of addresses when modified value.
 */
void hdr::clearAddress() 
{
    InternetAddress empty; 
    for( size_t i = 0; i < this->m_arAddresses.size(); i ++ ) 
    {
        this->m_arAddresses.set(empty, i); 
    }
    this->m_nAddressCount = 0; 
    this->m_sValue.clear(); 
}

/**
 * Update the array of addresses and the header line.
 */
void hdr::setAddressHeader(InternetAddressArray &addrs) 
{
    this->m_arAddresses = addrs; 
    this->m_nAddressCount = addrs.size(); 
    this->updateAddressHeader(); 
}

/**
 * Add new addresses to the header and update the array of 
 * addresses and the header line.
 */
void hdr::addAddress(InternetAddressArray &addrs) 
{
    if( this->m_nAddressCount == 0 && !this->m_sValue.empty() ) 
        this->parseAddressHeader(); 

    this->m_arAddresses.add(addrs); 
    this->m_nAddressCount = this->m_arAddresses.size(); 
    this->updateAddressHeader(); 
}

// Convenience method to remove addresses
void hdr::removeAddress(InternetAddressArray &addrs) 
{
    FastStringArray saddrs(addrs.size()); 
    InternetAddress *paddr = NULL; 
    InternetAddressArrayIterator it(addrs); 
    size_t i = 0; 
    while( it.next(paddr) && paddr ) 
    {
        saddrs.set(paddr->address(), i++); 
    }
    this->removeAddress(saddrs); 
}

// Convenience method to remove addresses
void hdr::removeAddress(FastStringArray &addrs) 
{
    if( this->m_nAddressCount == 0 && !this->m_sValue.empty() ) 
        this->parseAddressHeader(); 
    
    InternetAddress *paddr = NULL; 
    InternetAddressArrayIterator it(this->m_arAddresses); 
    FastString *ps = NULL; 
    FastStringArrayIterator sit(addrs); 

    size_t i = 0; 
    int foundfirst = 0, found = 0; 

    while( it.next(paddr) && paddr ) 
    {
        found = 0; 
        sit.first(); 
        while( sit.next(ps) && ps ) 
        {
            if( paddr->equalsAddress(*ps) ) 
            {
                foundfirst = found = 1; 
                break; 
            }
        }

        if( foundfirst ) 
        {
            if( !found ) 
                this->m_arAddresses.set(*paddr, i++); 
        }
        else
            i ++; 
    }

    this->m_nAddressCount = i; 

    if( i < this->m_arAddresses.size() ) 
    {
        InternetAddress empty; 
        for( ; i < this->m_arAddresses.size(); i ++ ) 
            this->m_arAddresses.set(empty, i); 
        
        this->updateAddressHeader(); 
    }
}

// Convenience method to remove group addresses
void hdr::removeAddressGroup(FastString &group) 
{
    if( this->m_nAddressCount == 0 && !this->m_sValue.empty() ) 
        this->parseAddressHeader(); 

    InternetAddress *paddr = NULL; 
    InternetAddressArrayIterator it(this->m_arAddresses); 

    size_t i = 0; 
    int foundfirst = 0, found = 0; 

    while( it.next(paddr) && paddr ) 
    {
        found = 0; 
        if( (group.empty() && paddr->groupLength() == 0 ) || 
            (!group.empty() && paddr->equalsGroup(group) ) ) 
        {
            foundfirst = found = 1; 
        }

        if( foundfirst ) 
        {
            if( !found ) 
                this->m_arAddresses.set(*paddr, i++); 
        }
        else
            i ++; 
    }

    this->m_nAddressCount = i; 

    if( i < this->m_arAddresses.size() ) 
    {
        InternetAddress empty; 
        for( ; i < this->m_arAddresses.size(); i ++ ) 
            this->m_arAddresses.set(empty, i); 
        
        this->updateAddressHeader(); 
    }
}

//===========InternetHeaders Functions Implement============

/**
 * Create an empty InternetHeaders object, 
 * reference to RFC 822. 
 */
void InternetHeaders::initHeaders() 
{
/*
    hdr header; 
    m_nHeaderCount = 21; 
    m_arHeaders.size(m_nHeaderCount); 
    m_arHeaders.set(header.set("Return-Path", NULL), 0);
    m_arHeaders.set(header.set("Received", NULL), 1);
    m_arHeaders.set(header.set("Message-Id", NULL), 2);
    m_arHeaders.set(header.set("Resent-Date", NULL), 3);
    m_arHeaders.set(header.set("Date", NULL), 4);
    m_arHeaders.set(header.set("Resent-From", NULL), 5);
    m_arHeaders.set(header.set("From", NULL), 6);
    m_arHeaders.set(header.set("Reply-To", NULL), 7);
    m_arHeaders.set(header.set("Sender", NULL), 8);
    m_arHeaders.set(header.set("To", NULL), 9);
    m_arHeaders.set(header.set("Subject", NULL), 10);
    m_arHeaders.set(header.set("Cc", NULL), 11);
    m_arHeaders.set(header.set("In-Reply-To", NULL), 12);
    m_arHeaders.set(header.set("Resent-Message-Id", NULL), 13);
    m_arHeaders.set(header.set("Errors-To", NULL), 14);
    m_arHeaders.set(header.set("Mime-Version", NULL), 15);
    m_arHeaders.set(header.set("Content-Type", NULL), 16);
    m_arHeaders.set(header.set("Content-Transfer-Encoding", NULL), 17);
    m_arHeaders.set(header.set("Content-MD5", NULL), 18);
    m_arHeaders.set(header.set("Content-Length", NULL), 19);
    m_arHeaders.set(header.set("Status", NULL), 20);
*/
}

/**
 * Extract out all the specify field values in mime mail 
 * header content, reference to RFC 822.
 *
 * @param header     pointer to header buffer.
 * @param len        header buffer length.
 */
void InternetHeaders::load(const char *header, size_t len) 
{
    size_t i = 0; 
    hdr hdrField; 

    this->m_nHeaderCount = 0; 

    if( header ) 
    {
        if( len == 0 ) return;//len = strlen(header); 
        if( len < 0 ) len = strlen(header); 

        int count = 0; 

        if( (count = MimeUtility::getHeaderCount(header, len)) <= 0 ) 
            return; 

        this->m_nHeaderCount = count; 
        this->m_arHeaders.size(count); 

        char *ptr = (char *) header; 
        char *fieldstart = NULL; 
        size_t fieldlen = 0; 
        FastString name, value; 
        int pos = 0; 

        while( pos >= 0 ) 
        {
            pos = MimeUtility::getHeaderField(
                                        header, len, 
                                        name, value, 
                                        fieldstart, fieldlen, 
                                        pos); 

            // trim whitespace and ':'
            name.trimChars(" \t\r\n:"); 

            if( name.empty() && value.empty() ) 
                continue; 

            hdrField.setName(name); 
            hdrField.setValue(value); 
            name.clear(); 
            value.clear(); 

            if( i >= this->m_arHeaders.size() ) 
                this->m_arHeaders.resize(i+2);  // decrease resize times
            
            this->m_arHeaders.set(hdrField, i); 
            i ++; 
        }

        this->m_nHeaderCount = i; // Actually header count
    }

    hdrField.clear(); 

    while( i < this->m_arHeaders.size() ) 
    {
        this->m_arHeaders.set(hdrField, i); 
        i ++; 
    }
}

/**
 * Get all the headers for this header name, returned as a single
 * String, with headers separated by the delimiter. If the
 * delimiter is <code>null</code>, only the first header is 
 * returned.  Returns <code>0</code>
 * if no headers with the specified name exist. 
 * reference to RFC 822.
 *
 * @param name     header field name.
 * @param s        header field value.
 * @param sep      delimiter
 * @return         count of specified header.
 */
int InternetHeaders::getHeader(
    FastString &name, FastString &s, const char *sep)  
{
    s.clear(); 
    if( name.empty() ) return 0; 

    // trim whitespace and ':'
    name.trimChars(" \t\r\n:"); 

    hdr *pheader = NULL; 
    FastString value; 
    size_t count = 0; 

    hdrArrayIterator it(this->m_arHeaders); 

    while( it.next(pheader) && pheader ) 
    {
        if( pheader->equalsName(name) ) 
        {
            if( sep ) 
            {
                pheader->getValue(value); 
                if( count > 0 ) 
                    s.append(sep); 
                s.append(value); 
                count ++; 
            }
            else 
            {
                pheader->getValue(s); 
                return 1; 
            }
        }
    }

    return count; 
}

/**
 * Get all the values for the specified header. The
 * values are String objects.  Returns <code>0</code>
 * if no headers with the specified name exist.
 *
 * @param name    header name
 * @param values  array of header values.
 * @return        count of specified header.
 */
int InternetHeaders::getHeader(FastString &name, FastStringArray &values)
{
    size_t i = 0; 
    FastString value; 

    while( i < values.size() ) 
    {
        values.set(value, i); 
        i ++; 
    }

    // trim whitespace and ':'
    name.trimChars(" \t\r\n:"); 

    if( name.empty() ) return 0; 

    hdr *pheader = NULL; 
    size_t count = 0; 

    hdrArrayIterator it(this->m_arHeaders); 

    // Count the specified header
    while( it.next(pheader) && pheader ) 
    {
        if( pheader->equalsName(name) ) 
            count ++; 
    }

    if( count == 0 ) return 0; 
    values.size(count); 
    count = 0; 

    it.first(); 
    while( it.next(pheader) && pheader ) 
    {
        if( pheader->equalsName(name) ) 
        {
            pheader->getValue(value); 
            values.set(value, count); 
            count ++; 
        }
    }

    return count; 
}

/**
 * Change the first header line that matches name
 * to have value, adding a new header if no existing header
 * matches. Remove all matching headers but the first. <p>
 *
 * Note that RFC822 headers can only contain US-ASCII characters
 *
 * @param name     header field name.
 * @param s        header field value.
 */
void InternetHeaders::setHeader(FastString &name, FastString &s)  
{
    // trim whitespace and ':'
    name.trimChars(" \t\r\n:"); 

    hdr header; 
    header.setName(name); 
    header.setValue(s); 

    this->setHeader(header); 
}

/**
 * Change the first header line that matches name
 * to have value, adding a new header if no existing header
 * matches. Remove all matching headers but the first. <p>
 *
 * Note that RFC822 headers can only contain US-ASCII characters
 *
 * @param header   new header.
 */
void InternetHeaders::setHeader(hdr &header)  
{
    FastString name; 
    header.getName(name); 

    if( name.empty() ) return; 

    int found = 0; 
    size_t i = 0, j = 0; 
    hdr *pheader = NULL; 

    hdrArrayIterator it(this->m_arHeaders); 

    // Find and change the first header line
    while( it.next(pheader) && pheader ) 
    {
        if( !found ) 
        {
            if( pheader->equalsName(name) ) 
            {
                this->m_arHeaders.set(header, j); 
                found = 1; 
            }
            j ++; 
        }
        else
        {
            // Remove other header lines that matches name
            if( !pheader->equalsName(name) ) 
            {
                if( j < i ) 
                    this->m_arHeaders.set(*pheader, j); 
                j ++; 
            }
        }
        i ++; 

        if( i >= this->m_nHeaderCount ) 
            break; 
    }

    this->m_nHeaderCount = j; 

    if( found ) 
    {
        hdr empty; 
        for( ; j < this->m_arHeaders.size(); j ++ ) 
        {
            this->m_arHeaders.set(empty, j); 
        }
    }
    else 
    {
        this->addHeader(header); 
    }
}

/**
 * Add a header with the specified name and value to the header list. <p>
 *
 * Note that RFC822 headers can only contain US-ASCII characters.
 *
 * @param name      header name
 * @param value     header value
 * @param index     append to index in header array
 */ 
void InternetHeaders::addHeader(FastString &name, FastString &s, int index)  
{
    // trim whitespace and ':'
    name.trimChars(" \t\r\n:"); 

    hdr header; 
    header.setName(name); 
    header.setValue(s); 

    this->addHeader(header, index); 
}

/**
 * Add a header with the specified name and value to the header list. <p>
 *
 * Note that RFC822 headers can only contain US-ASCII characters.
 *
 * @param header    new header
 * @param index     append to index in header array
 */ 
void InternetHeaders::addHeader(hdr &header, int index)  
{
    FastString name; 
    header.getName(name); 

    if( name.empty() ) return; 

    this->m_nHeaderCount ++; 

    if( this->m_nHeaderCount >= this->m_arHeaders.size() ) 
    {
        // new more item to increase efficiency
        this->m_arHeaders.resize( this->m_nHeaderCount + 2 ); 
    }

    if( name.equalsIgnoreCase("Received") ) 
        index = 0; 

    if( index < 0 ) 
    {
        hdrArrayIterator it(this->m_arHeaders); 
        hdr *pheader = NULL; 
        int i = this->m_arHeaders.size(); 

        it.last(); 
        while( it.prev(pheader) && pheader ) 
        {
            if( pheader->equalsName(name) ) 
                break; 
            i --; 
        }

        if( i > 0 && i < (int) this->m_nHeaderCount ) 
            index = i; 
    }

    if( index >= 0 && index < (int) this->m_nHeaderCount - 1 ) 
    {
        hdrArrayIterator it(this->m_arHeaders); 

        int j = this->m_arHeaders.size() - 1; 
        int i = j - 1; 
        hdr *pheader = NULL; 
        
        it.last(); 
        it.prev(pheader); // Last one escape to store previous

        // Move all the fields to hind, for append new to the index.
        while( it.prev(pheader) && pheader ) 
        {
            if( i < index ) 
                break; 
            this->m_arHeaders.set(*pheader, j --); 
            i --; 
        }
    }
    else
        index = this->m_nHeaderCount - 1; 

    // append it to the new index
    this->m_arHeaders.set(header, index); 
}

/**
 * Add a header with the specified name and value to the header list. <p>
 *
 * Note that RFC822 headers can only contain US-ASCII characters.
 *
 * @param name      header name
 * @param value     header value
 * @param field     header item that add behind
 * @param before    if set 1, add before the specified field.
 */ 
void InternetHeaders::addHeaderBehind(FastString &field, 
                        FastString &name, FastString &s, int before) 
{
    // trim whitespace and ':'
    name.trimChars(" \t\r\n:"); 

    hdr header; 
    header.setName(name); 
    header.setValue(s); 

    this->addHeaderBehind(field, header, before); 
}

/**
 * Add a header with the specified name and value to the header list. <p>
 *
 * Note that RFC822 headers can only contain US-ASCII characters.
 *
 * @param header    new header
 * @param field     header item that add behind
 * @param before    if set 1, add before the specified field.
 */ 
void InternetHeaders::addHeaderBehind(FastString &field, 
                        const hdr &header, int before) 
{
    FastString name; 
    header.getName(name); 

    // trim whitespace and ':'
    field.trimChars(" \t\r\n:"); 

    if( name.empty() ) return; 

    this->m_nHeaderCount ++; 

    if( this->m_nHeaderCount >= this->m_arHeaders.size() ) 
    {
        // new more item to increase efficiency
        this->m_arHeaders.resize( this->m_nHeaderCount + 2 ); 
    }

    // Default append to end.
    size_t index = this->m_nHeaderCount - 1; 

    if( !field.empty() ) 
    {
        hdrArrayIterator it(this->m_arHeaders); 

        int i = 0, j = 0; 
        hdr *pheader = NULL; 

        // If append before the field, search it from first to last, 
        // to append new before the first one of matched fields.
        if( before ) 
        {
            i = 0; 
            it.first(); 
            while( it.next(pheader) && pheader ) 
            {
                if( pheader->equalsName(field) ) 
                    break; 
                i ++; 
            }
            index = i; 
        }
        // Else search it from last to first.
        else
        {
            i = this->m_arHeaders.size() - 1; 
            it.last(); 
            it.prev(pheader); // Last one escape
            while( it.prev(pheader) && pheader ) 
            {
                if( pheader->equalsName(field) ) 
                    break; 
                i --; 
            }
            index = i; 
        }
        
        j = this->m_arHeaders.size() - 1; 
        i = j - 1; 

        it.last(); 
        it.prev(pheader); // Last one escape to store previous

        // Move all the fields to hind, for append new to the index.
        while( it.prev(pheader) && pheader ) 
        {
            if( i < (int) index ) 
                break; 
            this->m_arHeaders.set(*pheader, j --); 
            i --; 
        }
    }

    // append it to the new index
    this->m_arHeaders.set(header, index); 
}

/**
 * Add an RFC822 header line to the header store.
 * If the line starts with a space or tab (a continuation line),
 * add it to the last header line in the list. <p>
 *
 * Note that RFC822 headers can only contain US-ASCII characters
 *
 * @param line  raw RFC822 header line
 */
void InternetHeaders::addHeaderLine(FastString &line)
{
    char c = line.charAt(0); 
    if( c == ' ' || c == '\t' ) 
    {
        size_t i = 0; 
        hdr *pheader = NULL; 
        hdrArrayIterator it(this->m_arHeaders); 
        while( it.next(pheader) && pheader ) 
        {
            if( i >= this->m_nHeaderCount - 1 ) 
            {
                FastString value; 
                pheader->getValue(value); 
                value.append("\r\n"); 
                value.append(line); 
                pheader->setValue(value); 
                break; 
            }
            i ++; 
        }
    }
    else
    {
        hdr header(line); 
        this->addHeader(header); 
    }
}

/**
 * Remove all header entries that match the given name
 * @param name  header name
 */
void InternetHeaders::removeHeader(FastString &name) 
{
    // trim whitespace and ':'
    name.trimChars(" \t\r\n:"); 

    if( name.empty() ) return; 

    size_t i = 0, j = 0; 
    hdr *pheader = NULL; 

    hdrArrayIterator it(this->m_arHeaders); 

    // Find and change the first header line
    while( it.next(pheader) && pheader ) 
    {
        // Remove other header lines that matches name
        if( !pheader->equalsName(name) ) 
        {
            if( j < i ) 
                this->m_arHeaders.set(*pheader, j); 
            j ++; 
        }
        i ++; 

        if( i >= this->m_nHeaderCount ) 
            break; 
    }

    this->m_nHeaderCount = j; 

    hdr empty; 
    for( ; j < this->m_arHeaders.size(); j ++ ) 
    {
        this->m_arHeaders.set(empty, j); 
    }

}

/**
 * Remove all header entries
 */
void InternetHeaders::removeAllHeaders() 
{
    hdr empty; 

    for( size_t i = 0; i < this->m_arHeaders.size(); i ++ ) 
    {
        this->m_arHeaders.set(empty, i); 
    }

    this->m_nHeaderCount = 0; 
}

/**
 * Return all matching header objects.
 *
 * @param names      header names
 * @param headers    matching header objects
 * @param nonmatch   0 macth else 1 none match
 * @return  matching Header objects count.
 */
int InternetHeaders::getMatchingHeaders(
        FastStringArray &names, hdrArray &headers, int nonmatch) 
{
    size_t i = 0, count = 0; 
    hdr empty; 

    // Clear the output values
    for( i = 0; i < headers.size(); i ++ ) 
        headers.set(empty, i); 

    if( names.size() <= 0 ) 
        return 0; 

    FastString *pfs = NULL; 
    hdr *pheader = NULL; 
    FastStringArrayIterator fsit(names); 
    hdrArrayIterator it(this->m_arHeaders); 

    fsit.first(); 
    while( fsit.next(pfs) && pfs ) 
    {
        pfs->trimChars(" \t\r\n:"); 
    }

    count = i = 0; 
    int gotfound = 0; 

    // Count the matches headers number
    it.first(); 
    while( it.next(pheader) && pheader ) 
    {
        if( nonmatch ) 
            gotfound = 1; 
        else
            gotfound = 0; 

        fsit.first(); 
        while( fsit.next(pfs) && pfs ) 
        {
            if( pheader->equalsName(*pfs) ) 
            {
                if( nonmatch ) 
                    gotfound = 0; 
                else
                    gotfound = 1; 
                break; 
            }
        }

        if( gotfound ) count ++; 
        i ++; 

        if( i >= this->m_nHeaderCount ) 
            break; 
    }

    if( count <= 0 ) return 0; 

    headers.size(count); 

    count = i = 0; 

    // Copy the matched headers
    it.first(); 
    while( it.next(pheader) && pheader ) 
    {
        if( nonmatch ) 
            gotfound = 1; 
        else
            gotfound = 0; 

        fsit.first(); 
        while( fsit.next(pfs) && pfs ) 
        {
            if( pheader->equalsName(*pfs) ) 
            {
                if( nonmatch ) 
                    gotfound = 0; 
                else
                    gotfound = 1; 
                break; 
            }
        }

        if( gotfound ) 
        {
            headers.set(*pheader, count); 
            count ++; 
        }
        i ++; 

        if( i >= this->m_nHeaderCount ) 
            break; 
    }

    return count; 
}

/**
 * Return all header objects.
 *
 * @param headers    all header objects
 * @return  all Header objects count.
 */
int InternetHeaders::getAllHeaders(hdrArray &headers) 
{
    size_t i = 0; 
    hdr empty; 

    // Clear the output values
    for( i = 0; i < headers.size(); i ++ ) 
        headers.set(empty, i); 

    if( this->m_nHeaderCount <= 0 ) 
        return 0; 

    i = 0; 
    headers.size(this->m_nHeaderCount); 

    hdr *pheader = NULL; 
    hdrArrayIterator it(this->m_arHeaders); 

    // Copy all the headers
    while( it.next(pheader) && pheader ) 
    {
        headers.set(*pheader, i ++); 
        if( i >= this->m_nHeaderCount ) 
            break; 
    }

    return this->m_nHeaderCount; 
}

/**
 * Return all the matching headers as an line string.
 *
 * @param names  header names
 * @param lines  matching header lines 
 * @return         lines count    
 */
int InternetHeaders::getMatchingHeaderLines(
        FastStringArray &names, FastString &lines) 
{
    lines.clear(); 
    hdrArray headers; 
    int count = this->getMatchingHeaders(names, headers); 
    
    hdr *pheader = NULL; 
    hdrArrayIterator it(headers); 
    size_t i = 0; 
    int appended = 0; 

    while( it.next(pheader) && pheader ) 
    {
        appended = 0; 
        if( pheader->nameLength() > 0 ) 
        {
            lines.append(pheader->name()); 
            lines.append(": "); 
            appended = 1; 
        }
        if( pheader->valueLength() > 0 ) 
        {
            lines.append(pheader->value()); 
            appended = 1; 
        }
        if( appended ) 
            lines.append("\r\n"); 
        if( (int) ++i >= count ) 
            break; 
    }

    lines.trimChars("\r\n"); 

    return count; 
}

/**
 * Return all the non-matching headers as an line string.
 *
 * @param names  header names
 * @param lines  non-matching header lines 
 * @return         lines count    
 */
int InternetHeaders::getNonMatchingHeaderLines(
        FastStringArray &names, FastString &lines) 
{
    lines.clear(); 
    hdrArray headers; 
    int count = this->getNonMatchingHeaders(names, headers); 
    
    hdr *pheader = NULL; 
    hdrArrayIterator it(headers); 
    size_t i = 0; 
    int appended = 0; 

    while( it.next(pheader) && pheader ) 
    {
        appended = 0; 
        if( pheader->nameLength() > 0 ) 
        {
            lines.append(pheader->name()); 
            lines.append(": "); 
            appended = 1; 
        }
        if( pheader->valueLength() > 0 ) 
        {
            lines.append(pheader->value()); 
            appended = 1; 
        }
        if( appended ) 
            lines.append("\r\n"); 
        if( (int) ++i >= count ) 
            break; 
    }

    lines.trimChars("\r\n"); 

    return count; 
}

/**
 * Return all the headers as an line string.
 *
 * @param lines  Header lines 
 * @return         lines count    
 */
int InternetHeaders::getAllHeaderLines(FastString &lines) 
{
    lines.clear(); 
    
    hdr *pheader = NULL; 
    hdrArrayIterator it(this->m_arHeaders); 
    size_t i = 0; 
    int appended = 0; 

    while( it.next(pheader) && pheader ) 
    {
        appended = 0; 
        if( pheader->nameLength() > 0 ) 
        {
            lines.append(pheader->name()); 
            lines.append(": "); 
            appended = 1; 
        }
        if( pheader->valueLength() > 0 ) 
        {
            lines.append(pheader->value()); 
            appended = 1; 
        }
        if( appended ) 
            lines.append("\r\n"); 
        if( ++i >= this->m_nHeaderCount ) 
            break; 
    }

    lines.trimChars("\r\n"); 

    return this->m_nHeaderCount; 
}

/**
 * Return the matching header lines count.
 */
size_t InternetHeaders::getHeaderCount(FastString &name) 
{
    hdr *pheader = NULL; 
    hdrArrayIterator it(this->m_arHeaders); 
    size_t count = 0, i = 0; 

    while( it.next(pheader) && pheader ) 
    {
        if( pheader->equalsName(name) ) 
            count ++; 
        if( ++i >= this->m_nHeaderCount ) 
            break; 
    }

    return count; 
}

/**
 * Convenience method to get addresses.
 *
 * @param name   Header name 
 * @param addrs  address array 
 * @return         address count    
 */
int InternetHeaders::getAddressHeader(
    FastString &name, InternetAddressArray &addrs) 
{
    if( name.empty() ) return -1; 
    int count = this->getHeaderCount(name); 
    if( count > 1 ) 
    {
        FastString s; 
        this->getHeader(name, s, ","); 
        return MimeUtility::parseHeaderAddress(addrs, s, m_bStrict); 
    }
    else if( count == 1 ) 
    {
        //HACK!!! this is fast way to get one header address
        hdr *pheader = NULL; 
        hdrArrayIterator it(this->m_arHeaders); 

        while( it.next(pheader) && pheader ) 
        {
            if( pheader->equalsName(name) ) 
            {
                return pheader->getAddress(addrs); 
            }
        }
    }
    return -1; 
}

/**
 * Convenience method to set addresses to header.
 *
 * @param name   Header name 
 * @param addrs  address array 
 * @param group  address group name
 */
void InternetHeaders::setAddressHeader(
    FastString &name, InternetAddressArray &addrs, FastString &group) 
{
    if( name.empty() ) return; 
    if( !group.empty() ) 
    {
        InternetAddress *paddr = NULL; 
        InternetAddressArrayIterator it(addrs); 
        while( it.next(paddr) && paddr ) 
        {
            if( paddr->addressLength() > 0 ) 
                paddr->setGroup(group); 
        }
    }
    hdr header; 
    header.setName(name); 
    header.setAddress(addrs); 
    this->setHeader(header); 
}

/**
 * Convenience method to add addresses to header.
 *
 * @param name   Header name 
 * @param addrs  address array 
 * @param group  address group name
 */
void InternetHeaders::addAddressHeader(
    FastString &name, InternetAddressArray &addrs, FastString &group) 
{
    if( name.empty() ) return; 
    if( !group.empty() ) 
    {
        InternetAddress *paddr = NULL; 
        InternetAddressArrayIterator it(addrs); 
        while( it.next(paddr) && paddr ) 
        {
            if( paddr->addressLength() > 0 ) 
                paddr->setGroup(group); 
        }
    }
    int count = this->getHeaderCount(name); 
    if( count <= 0 ) 
    {
        hdr header; 
        header.setName(name); 
        header.setAddress(addrs); 
        this->addHeader(header); 
    }
    else if( count == 1 ) 
    {
        //HACK!!! this is fast way to set one header address
        hdr *pheader = NULL; 
        hdrArrayIterator it(this->m_arHeaders); 

        while( it.next(pheader) && pheader ) 
        {
            if( pheader->equalsName(name) ) 
            {
                pheader->addAddress(addrs); 
                break; 
            }
        }
    }
    else
    {
        FastString addrline; 
        this->getHeader(name, addrline, ","); 
        hdr header; 
        header.setName(name); 
        header.setValue(addrline); 
        header.parseAddressHeader(this->m_bStrict); 
        header.addAddress(addrs); 
        this->setHeader(header); 
    }
}

/**
 * Convenience method to remove addresses in header.
 *
 * @param name   Header name 
 * @param addrs  address array 
 */
void InternetHeaders::removeAddress(FastString &name, InternetAddressArray &addrs) 
{
    FastStringArray saddrs(addrs.size()); 
    InternetAddress *paddr = NULL; 
    InternetAddressArrayIterator it(addrs); 
    size_t i = 0; 
    while( it.next(paddr) && paddr ) 
    {
        saddrs.set(paddr->address(), i++); 
    }
    this->removeAddress(name, saddrs); 
}

/**
 * Convenience method to remove addresses in header.
 *
 * @param name   Header name 
 * @param addrs  address array 
 */
void InternetHeaders::removeAddress(FastString &name, FastStringArray &addrs) 
{
    if( name.empty() ) return; 
    int count = this->getHeaderCount(name); 
    if( count == 1 ) 
    {
        //HACK!!! this is fast way to set one header address
        hdr *pheader = NULL; 
        hdrArrayIterator it(this->m_arHeaders); 

        while( it.next(pheader) && pheader ) 
        {
            if( pheader->equalsName(name) ) 
            {
                pheader->removeAddress(addrs); 
                break; 
            }
        }
    }
    else if( count > 1 ) 
    {
        FastString addrline; 
        this->getHeader(name, addrline, ","); 
        hdr header; 
        header.setName(name); 
        header.setValue(addrline); 
        header.parseAddressHeader(this->m_bStrict); 
        header.removeAddress(addrs); 
        this->setHeader(header); 
    }
}

/**
 * Convenience method to remove a group addresses in header.
 *
 * @param name   Header name 
 * @param group  addresses group name 
 */
void InternetHeaders::removeAddressGroup(FastString &name, FastString &group) 
{
    if( name.empty() ) return; 
    int count = this->getHeaderCount(name); 
    if( count == 1 ) 
    {
        //HACK!!! this is fast way to set one header address
        hdr *pheader = NULL; 
        hdrArrayIterator it(this->m_arHeaders); 

        while( it.next(pheader) && pheader ) 
        {
            if( pheader->equalsName(name) ) 
            {
                pheader->removeAddressGroup(group); 
                break; 
            }
        }
    }
    else if( count > 1 ) 
    {
        FastString addrline; 
        this->getHeader(name, addrline, ","); 
        hdr header; 
        header.setName(name); 
        header.setValue(addrline); 
        header.parseAddressHeader(this->m_bStrict); 
        header.removeAddressGroup(group); 
        this->setHeader(header); 
    }
}


//===========InternetAddress Functions Implement============

/**
 * Convert this address into a RFC 822 / RFC 2047 encoded address.
 * The resulting string contains only US-ASCII characters, and
 * hence is mail-safe.
 *
 * @param s     possibly encoded address string
 */
void InternetAddress::toString(FastString &s) 
{
    s.clear(); 
    if( !this->m_sAddress.empty() ) 
    {
        FastString address(this->m_sAddress); 
        MimeUtility::unquoteAddress(address); 

        if( address.empty() ) 
            return; 

        if( this->m_sPersonal.empty() ) 
        {
            s = address; 
        }
        else
        {
            FastString personal; 
            FastString sPersonal(m_sPersonal.c_str(), m_sPersonal.length()); 
            MimeUtility::encodeWord(sPersonal, personal); 
            MimeUtility::quotePhrase(personal); 
            s.clear(); 
            s.append(personal); 
            s.append(" <"); 
            s.append(address); 
            s.append(">"); 
        }
    }
}

/**
 * Returns a properly formatted address (RFC 822 syntax) of
 * Unicode characters.
 *   
 * @param s     Unicode address string
 */  
void InternetAddress::toUnicodeString(FastString &s) 
{
    this->toString(s); 
}


_FASTMIME_END_NAMESPACE
