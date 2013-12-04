//=============================================================================
/**
 *  @file    MimeEntity.cpp
 *
 *  ver 1.0.0 2004/06/16 Naven Exp, for Mime Message Parse Engine.
 *
 *  @author Naven 2004/06/16 created.
 */
//=============================================================================

#include "MimeEntity.h"
#include "MimeContainer.h" 
#include "MimeActivation.h"


_FASTMIME_BEGIN_NAMESPACE


// Initialize Class Static Members

const char * IMimePart::ATTACHMENT = MIME_ATTACHMENT; 
const char * IMimePart::INLINE     = MIME_INLINE; 


//===========MimeBodyPart Functions Implement============

/**
 * Extract out all the header fields in a mime mail content, 
 * reference to RFC 822.
 *
 * RFC 822
 *
 *    3.2.  HEADER FIELD DEFINITIONS
 *
 *         These rules show a field meta-syntax, without regard for the
 *    particular  type  or internal syntax.  Their purpose is to permit
 *    detection of fields; also, they present to  higher-level  parsers
 *    an image of each field as fitting on one line.
 *
 *    field       =  field-name ":" [ field-body ] CRLF
 *
 *    field-name  =  1*<any CHAR, excluding CTLs, SPACE, and ":">
 *
 *    field-body  =  field-body-contents
 *                   [CRLF LWSP-char field-body]
 *
 *    field-body-contents =
 *                  <the ASCII characters making up the field-body, as
 *                   defined in the following sections, and consisting
 *                   of combinations of atom, quoted-string, and
 *                   specials tokens, or else consisting of texts>
 *
 */
void MimeBodyPart::parseheader() 
{
    if( m_bHeaderParsed ) 
        return; 

    if( !m_psContentBuffer || m_nContentBufferSize == 0 ) 
    {
        m_psContentBuffer       = 0; 
        m_nContentBufferSize    = 0; 
        m_psHeaderBuffer        = 0; 
        m_nHeaderBufferSize     = 0; 
        m_psBodyBuffer          = 0; 
        m_nBodyBufferSize       = 0; 

        return; 
    }

    // replace '\0' with ' ' in buffer, without the last char
    for( int i = 0; i < (int) m_nContentBufferSize - 1; i ++ ) 
    {
        if( m_psContentBuffer[i] == '\0' ) 
            m_psContentBuffer[i] = ' '; 
    }

    m_psHeaderBuffer = MimeUtility::findStartLine(m_psContentBuffer, m_nContentBufferSize); 

    if( !m_psHeaderBuffer ) 
        return; 

    char *end  = MimeUtility::findHeaderEnd(m_psHeaderBuffer, 
                        m_nContentBufferSize - (m_psHeaderBuffer - m_psContentBuffer)); 
    if( !end ) end = m_psHeaderBuffer; 

		// add by henh 2012/7/20 16:13:31 for end == m_psHeaderBuffer
		if(end != m_psHeaderBuffer)
		{
	    m_nHeaderBufferSize = end - m_psHeaderBuffer; 
	    m_psBodyBuffer      = end; 
	    m_nBodyBufferSize   = m_nContentBufferSize - (m_psBodyBuffer - m_psContentBuffer); 
  	}
  	else
  	{
  		m_nHeaderBufferSize = m_nContentBufferSize - (m_psHeaderBuffer - m_psContentBuffer); 
	    m_psBodyBuffer      = m_psHeaderBuffer + m_nHeaderBufferSize; 
	    m_nBodyBufferSize   = m_nContentBufferSize - (m_psBodyBuffer - m_psContentBuffer); 
  	}
  	// add end

    m_ihHeaders.load(m_psHeaderBuffer, m_nHeaderBufferSize); 

    if( isMimeBodyPart() ) 
    {
        /**
         * 5.2.1.  RFC822 Subtype
         *
         *   A media type of "message/rfc822" indicates that the body contains an
         *   encapsulated message, with the syntax of an RFC 822 message.
         *   However, unlike top-level RFC 822 messages, the restriction that each
         *   "message/rfc822" body must include a "From", "Date", and at least one
         *   destination header is removed and replaced with the requirement that
         *   at least one of "From", "Subject", or "Date" must be present.
         *
         * MimeMessage Only check At least one of "From", "Subject", or "Date" 
         *     must be present.
         *
         * MimeBodyPart Only check At least one of "Content-Type" must be present.
         */
        int count = 0; 
        FastString s; 
        getHeader("From", s);         if( !s.empty() ) count ++; 
        getHeader("Subject", s);      if( !s.empty() ) count ++; 
        getHeader("Date", s);         if( !s.empty() ) count ++; 
        getHeader("Content-Type", s); if( !s.empty() ) count ++; 

        if( count == 0 ) 
        {
            removeAllHeaders(); 

            m_psHeaderBuffer    = 0; 
            m_nHeaderBufferSize = 0; 
            m_psBodyBuffer      = m_psContentBuffer; 
            m_nBodyBufferSize   = m_nContentBufferSize; 
        }
    }

    m_bHeaderParsed = TRUE; 
}

/**
 * Parse the content as a String. 
 *
 */
void MimeBodyPart::getContentLines(FastString &s) 
{
    char *pstart = MimeUtility::findStartLine(m_psBodyBuffer, m_nBodyBufferSize); 
    int buffersize = (int) ((m_psBodyBuffer + m_nBodyBufferSize) - pstart); 
    if( pstart != 0 && buffersize > 0 ) 
    {
        s.set(pstart, buffersize); 
        s.rtrimChars("\r\n"); 
    }
    else
        s.clear(); 
}

/**
 * Parse the content as a String. The type of this
 * object is dependent on the content itself. For 
 * example, the native format of a "text/plain" content
 * is usually a String object. The native format for a "multipart"
 * message is always a Multipart subclass. For content types that are
 * unknown to the DataHandler system, an input stream is returned
 * as the content. <p>
 *
 */
void MimeBodyPart::parsebody() 
{
    if( m_bBodyParsed && !(m_psContent == 0 && m_pMultipart == 0) ) 
        return; 

    if( m_psContentBuffer == 0 || m_nContentBufferSize == 0 ) 
    {
        m_psContentBuffer       = 0; 
        m_nContentBufferSize    = 0; 
        m_psHeaderBuffer        = 0; 
        m_nHeaderBufferSize     = 0; 
        m_psBodyBuffer          = 0; 
        m_nBodyBufferSize       = 0; 
        return; 
    }

    if( m_psBodyBuffer == 0 || m_nBodyBufferSize == 0 ) 
        return; 

    releaseContent(); 

    if( isMultipart() ) 
    {
        m_pMultipart = (IMultipart*) new MimeMultipart((IMimePart*)this); 
    }
    else 
    {
        FastString encoding, s; 
        this->getEncoding(encoding); 
        this->getContentLines(s); 
        m_psContent = new FastString(); 
        MimeUtility::decode(s, *m_psContent, encoding); 
    }

    m_bBodyParsed = TRUE; 
}

/**
 * New a MimeMultipart object from another Multipart pointer.
 *
 */
IMultipart * MimeBodyPart::newMimeMultipart(IMultipart *mp) 
{
    if( mp ) 
        return (IMultipart *) new MimeMultipart(*((MimeMultipart *)mp)); 
    else
        return (IMultipart *) 0; 
}

/**
 * Update all Multiparts and BodyParts's parent pointer.
 *
 */
void MimeBodyPart::updateParent() 
{
    if( m_pMultipart ) 
    {
        m_pMultipart->setParent(this); 
        
        for( int i = 0; i < m_pMultipart->getCount(); i ++ ) 
        {
            MimeBodyPart *part = m_pMultipart->getBodyPart(i); 
            if( part ) 
            {
                part->setParent(m_pMultipart); 
                part->updateParent(); 
            }
        }
    }
}

/**
 * Return the Message that contains the content.
 * Follows the parent chain up through containing Multipart
 * objects until it comes to a Message object, or null.
 *
 * @return    the containing Message, or null if not known
 */
MimeMessage * MimeBodyPart::getMessage() 
{
    IMimePart *p = this; 
    while( p ) 
    {
        if( p->isMimeMessage() ) 
            return (MimeMessage *) p; 
        IMultipart *mp = p->getParent(); 
        if( mp ) 
            p = mp->getParent(); 
        else 
            break; 
    }

    return (MimeMessage *)0; 
}

/**
 * Returns the recepients specified by the type. The mapping
 * between the type and the corresponding RFC 822 header is
 * as follows:
 * <pre>
 *   RecipientType
 *      MimeBodyPart.TO          "To"
 *      MimeBodyPart.CC          "Cc"
 *      MimeBodyPart.BCC         "Bcc"
 *      MimeBodyPart.NEWSGROUPS  "Newsgroups"
 * </pre><br>
 *
 * @param type      Type of recepient
 * @param name      recepient type name
 * @return          recepient type name
 */
const char * MimeBodyPart::getRecipientsType(int type, FastString &name) 
{
    name.clear(); 
    switch( type ) 
    {
    case MimeBodyPart::TO:
        name = "To"; 
        break; 
    case MimeBodyPart::CC:
        name = "Cc"; 
        break; 
    case MimeBodyPart::BCC:
        name = "Bcc"; 
        break; 
    case MimeBodyPart::NEWSGROUPS:
        name = "Newsgroups"; 
        break; 
    }
    if( name.empty() ) 
        return NULL; 
    else 
        return name.c_str(); 
}

/**
 * Get all the recipient addresses for the message.
 * The default implementation extracts the TO, CC, and BCC
 * recipients using the <code>getRecipients</code> method. <p>
 *
 * This method returns <code>empty</code> if none of the recipient
 * headers are present in this message.  It may Return an empty array
 * if any recipient header is present, but contains no addresses.
 *
 * @param addrs array of Address objects
 * @see Message.RecipientType#TO
 * @see Message.RecipientType#CC
 * @see Message.RecipientType#BCC
 * @see #getRecipients
 */
int MimeBodyPart::getAllRecipients(InternetAddressArray &addrs) 
{
    InternetAddressArray addrs_to, addrs_cc, addrs_bcc; 
    int count_to  = this->getRecipients(MimeBodyPart::TO,  addrs_to); 
    int count_cc  = this->getRecipients(MimeBodyPart::CC,  addrs_cc); 
    int count_bcc = this->getRecipients(MimeBodyPart::BCC, addrs_bcc); 
    if( count_to < 0 && count_cc < 0 && count_bcc < 0 ) 
        return -1; 
    int count = 0, i = 0; 
    if( count_to > 0 )  count += count_to; 
    if( count_cc > 0 )  count += count_cc; 
    if( count_bcc > 0 ) count += count_bcc; 
    addrs.size(count); 
    if( count == 0 ) return count; 

    InternetAddress addr; 
    count = 0; 

    for( i = 0; i < count_to; i ++ ) 
    {
        addrs_to.get(addr, i); 
        if( addr.addressLength() > 0 ) 
            addrs.set(addr, count++); 
    }

    for( i = 0; i < count_cc; i ++ ) 
    {
        addrs_cc.get(addr, i); 
        if( addr.addressLength() > 0 ) 
            addrs.set(addr, count++); 
    }

    for( i = 0; i < count_bcc; i ++ ) 
    {
        addrs_bcc.get(addr, i); 
        if( addr.addressLength() > 0 ) 
            addrs.set(addr, count++); 
    }

    return count; 
}

/**
 * Return the mail id in <code>index</code> "Received" header which
 * is appended by any mx server, the mail id identitied by the mx server 
 * for queue list, and usually behind "ESMTP id" or "SMTP id" in 
 * "Received" header. 
 * NOTE: not RFC822
 *
 * @param s  content-ID
 */
void MimeBodyPart::getReceivedID(FastString &s, int index) 
{
    FastStringArray headers; 
    getHeader("Received", headers); 
    s.clear(); 
    index = index < 0 ? 0 : (index >= (int)headers.size() ? headers.size() -1 : index); 
    FastStringArrayIterator it(headers); 
    FastString *pheader = NULL; 
    int i = 0; 
    while( it.next(pheader) && pheader ) 
    {
        if( i == index ) 
        {
            FastString tmp(pheader->c_str(), pheader->length()); 
            tmp.replaceToken(LINEAR_WHITE_SPACE, " "); 
            tmp.extractValueBehindIgnoreCase("SMTP id", s, LINEAR_WHITE_SPACE); 
            s.trimChars(" \t\r\n;,[]()\"'"); 
            break; 
        }
        i ++; 
    }
}

/**
 * Get the mime type of this bodypart's content.
 *
 * @param s  content mime type
 */
int MimeBodyPart::getMimeType() 
{
    if( m_nMimeType == MimeBodyPart::UNKNOWN_MIMETYPE ) 
    {
        FastString s; 
        this->getContentType(s); 
        ContentType ct(s); 

        if( ct.match("text/*") ) 
        {
            if( ct.match("text/plain") ) 
                m_nMimeType = MimeBodyPart::TEXT_PLAIN; 
            else if( ct.match("text/html") ) 
                m_nMimeType = MimeBodyPart::TEXT_HTML; 
            else if( ct.match("text/enriched") ) 
                m_nMimeType = MimeBodyPart::TEXT_ENRICHED; 
            else if( ct.match("text/unrecognized") ) 
                m_nMimeType = MimeBodyPart::TEXT_UNRECOGNIZED;
            //add by yao 20101011 for other text types
            else if( ct.match("text/css") )
                m_nMimeType = MimeBodyPart::TEXT_CSS;
            else if( ct.match("text/h323") )
                m_nMimeType = MimeBodyPart::TEXT_H323;
            else if( ct.match("text/uls") )
                m_nMimeType = MimeBodyPart::TEXT_ULS;
            else if( ct.match("text/richtext") )
                m_nMimeType = MimeBodyPart::TEXT_RTX;
            else if( ct.match("text/scriptlet") )
                m_nMimeType = MimeBodyPart::TEXT_SCT;
            else if( ct.match("text/tab-separated-values") )
                m_nMimeType = MimeBodyPart::TEXT_TSV;
            else if( ct.match("text/webviewhtml") )
                m_nMimeType = MimeBodyPart::TEXT_HTT;
            else if( ct.match("text/x-component") )
                m_nMimeType = MimeBodyPart::TEXT_HTC;
            else if( ct.match("text/x-setext") )
                m_nMimeType = MimeBodyPart::TEXT_ETX;
            else if( ct.match("text/x-vcard") )
                m_nMimeType = MimeBodyPart::TEXT_VCF;
            //end add
            else
                m_nMimeType = MimeBodyPart::TEXT; 
        }
        else if( ct.match("multipart/*") ) 
        {
            if( ct.match("multipart/mixed") ) 
                m_nMimeType = MimeBodyPart::MULTIPART_MIXED; 
            else if( ct.match("multipart/related") ) 
                m_nMimeType = MimeBodyPart::MULTIPART_RELATED; 
            else if( ct.match("multipart/alternative") ) 
                m_nMimeType = MimeBodyPart::MULTIPART_ALTERNATIVE; 
            else
                m_nMimeType = MimeBodyPart::MULTIPART; 
        }
        else if( ct.match("message/*") ) 
        {
            if( ct.match("message/rfc822") ) 
                m_nMimeType = MimeBodyPart::MESSAGE_RFC822; 
            // add by henh 2012/7/20 16:13:31 for message types
           	else if( ct.match("message/delivery-status") )
           		  m_nMimeType = MimeBodyPart::MESSAGE_DELSTATUS;  
            // add end
            else
                m_nMimeType = MimeBodyPart::MESSAGE; 
        }
        else if( ct.match("image/*") ) 
        {
            if( ct.match("image/jpeg") ) 
                m_nMimeType = MimeBodyPart::IMAGE_JPEG; 
            else if( ct.match("image/gif") ) 
                m_nMimeType = MimeBodyPart::IMAGE_GIF; 
            else if( ct.match("image/png") ) 
                m_nMimeType = MimeBodyPart::IMAGE_PNG;
            //add by yao 20101011 for other image types
            else if( ct.match("image/bmp") )
                m_nMimeType = MimeBodyPart::IMAGE_BMP;
            else if( ct.match("image/cis-cod") )
                m_nMimeType = MimeBodyPart::IMAGE_COD;
            else if( ct.match("image/ief") )
                m_nMimeType = MimeBodyPart::IMAGE_IEF;
            else if( ct.match("image/pipeg") )
                m_nMimeType = MimeBodyPart::IMAGE_PIPEG;
            else if( ct.match("image/svg+xml") )
                m_nMimeType = MimeBodyPart::IMAGE_SVG;
            else if( ct.match("image/tiff") )
                m_nMimeType = MimeBodyPart::IMAGE_TIFF;
            else if( ct.match("image/x-cmu-raster") )
                m_nMimeType = MimeBodyPart::IMAGE_RAS; 
            else if( ct.match("image/x-cmx") )
                m_nMimeType = MimeBodyPart::IMAGE_CMX;
            else if( ct.match("image/x-icon") )
                m_nMimeType = MimeBodyPart::IMAGE_ICO;
            else if( ct.match("image/x-portable-anymap") )
                m_nMimeType = MimeBodyPart::IMAGE_PNM;
            else if( ct.match("image/x-portable-bitmap") )
                m_nMimeType = MimeBodyPart::IMAGE_PBM;
            else if( ct.match("image/x-portable-graymap") )
                m_nMimeType = MimeBodyPart::IMAGE_PGM;
            else if( ct.match("image/x-portable-pixmap") )
                m_nMimeType = MimeBodyPart::IMAGE_PPM;
            else if( ct.match("image/x-rgb") )
                m_nMimeType = MimeBodyPart::IMAGE_RGB;
            else if( ct.match("image/x-xbitmap") )
                m_nMimeType = MimeBodyPart::IMAGE_XBM;
            else if( ct.match("image/x-xpixmap") )
                m_nMimeType = MimeBodyPart::IMAGE_XPM;
            else if( ct.match("image/x-xwindowdump") )
                m_nMimeType = MimeBodyPart::IMAGE_XWD;
            //end add
            else
                m_nMimeType = MimeBodyPart::IMAGE; 
        }
        else if( ct.match("audio/*") ) 
        {
            if( ct.match("audio/basic") ) 
                m_nMimeType = MimeBodyPart::AUDIO_BASIC;
            else if( ct.match("audio/mpeg") ) 
                m_nMimeType = MimeBodyPart::AUDIO_MP3;
            //add by yao 20101011 for other audio types
            else if( ct.match("audio/mid") )
                m_nMimeType = MimeBodyPart::AUDIO_MID;
            else if( ct.match("audio/aiff") )
                m_nMimeType = MimeBodyPart::AUDIO_AIF;
            else if( ct.match("audio/x-mpegurl") )
                m_nMimeType = MimeBodyPart::AUDIO_M3U;
            else if( ct.match("audio/x-pn-realaudio") )
                m_nMimeType = MimeBodyPart::AUDIO_RA;
            else if( ct.match("audio/x-wav") )
                m_nMimeType = MimeBodyPart::AUDIO_WAV;
            //end add
            else 
                m_nMimeType = MimeBodyPart::AUDIO; 
        }
        else if( ct.match("video/*") ) 
        {
            if( ct.match("video/mpeg") ) 
                m_nMimeType = MimeBodyPart::VIDEO_MPEG;
            //add by yao 20101011 for other video types
            else if( ct.match("video/quicktime") )
                m_nMimeType = MimeBodyPart::VIDEO_QT;
            else if( ct.match("video/x-la-asf") )
                m_nMimeType = MimeBodyPart::VIDEO_LSF;
            else if( ct.match("video/x-ms-asf") )
                m_nMimeType = MimeBodyPart::VIDEO_ASF;
            else if( ct.match("video/x-msvideo") )
                m_nMimeType = MimeBodyPart::VIDEO_AVI;
            else if( ct.match("video/x-sgi-movie") )
                m_nMimeType = MimeBodyPart::VIDEO_MOV;
            //end add
            else 
                m_nMimeType = MimeBodyPart::VIDEO; 
        }
        else if( ct.match("application/*") ) 
        {
            if( ct.match("application/octet-stream") ) 
                m_nMimeType = MimeBodyPart::APPLICATION_OCTET_STREAM;
            else if( ct.match("application/PostScript") ) 
                m_nMimeType = MimeBodyPart::APPLICATION_POSTSCRIPT;
            //add by yao 20101122 for other application types
            else if( ct.match("application/envoy") )
                m_nMimeType = MimeBodyPart::APPLICATION_EVY;
            else if( ct.match("application/fractals") )
                m_nMimeType = MimeBodyPart::APPLICATION_FIF;
            else if( ct.match("application/futuresplash") )
                m_nMimeType = MimeBodyPart::APPLICATION_SPL;
            else if( ct.match("application/hta") )
                m_nMimeType = MimeBodyPart::APPLICATION_HTA;
            else if( ct.match("application/internet-property-stream") )
                m_nMimeType = MimeBodyPart::APPLICATION_ACX;
            else if( ct.match("application/mac-binhex40") )
                m_nMimeType = MimeBodyPart::APPLICATION_HQX;
            else if( ct.match("application/msword") )
                m_nMimeType = MimeBodyPart::APPLICATION_DOC;
            else if( ct.match("application/pdf") )
                m_nMimeType = MimeBodyPart::APPLICATION_PDF;
            else if( ct.match("application/rtf") )
                m_nMimeType = MimeBodyPart::APPLICATION_RTF;
            else if( ct.match("application/vnd.ms-excel") )
                m_nMimeType = MimeBodyPart::APPLICATION_EXCEL;
            else if( ct.match("application/vnd.ms-outlook") )
                m_nMimeType = MimeBodyPart::APPLICATION_OUTLOOK;
            else if( ct.match("application/vnd.ms-pkicertstore") )
                m_nMimeType = MimeBodyPart::APPLICATION_SST;
            else if( ct.match("application/vnd.ms-pkiseccat") )
                m_nMimeType = MimeBodyPart::APPLICATION_CAT;
            else if( ct.match("application/vnd.ms-pkistl") )
                m_nMimeType = MimeBodyPart::APPLICATION_STL;
            else if( ct.match("application/vnd.ms-powerpoint") )
                m_nMimeType = MimeBodyPart::APPLICATION_PPT;
            else if( ct.match("application/vnd.ms-project") )
                m_nMimeType = MimeBodyPart::APPLICATION_PROJECT;
            else if( ct.match("application/vnd.ms-works") )
                m_nMimeType = MimeBodyPart::APPLICATION_WORKS;
            else if( ct.match("application/cdf") )
                m_nMimeType = MimeBodyPart::APPLICATION_CDF;
            else if( ct.match("application/x-compress") )
                m_nMimeType = MimeBodyPart::APPLICATION_Z;
            else if( ct.match("application/x-compressed") )
                m_nMimeType = MimeBodyPart::APPLICATION_TGZ;
            else if( ct.match("application/x-csh") )
                m_nMimeType = MimeBodyPart::APPLICATION_CSH;
            else if( ct.match("application/x-director") )
                m_nMimeType = MimeBodyPart::APPLICATION_DIREC;
            else if( ct.match("application/x-dvi") )
                m_nMimeType = MimeBodyPart::APPLICATION_DVI;
            else if( ct.match("application/x-gtar") )
                m_nMimeType = MimeBodyPart::APPLICATION_GTAR;
            else if( ct.match("application/x-gzip") )
                m_nMimeType = MimeBodyPart::APPLICATION_GZ;
            else if( ct.match("application/x-latex") )
                m_nMimeType = MimeBodyPart::APPLICATION_LATEX;
            else if( ct.match("application/x-hdf") )
                m_nMimeType = MimeBodyPart::APPLICATION_HDF;
            else if( ct.match("application/x-javascript") )
                m_nMimeType = MimeBodyPart::APPLICATION_JS;
            else if( ct.match("application/x-msaccess") )
                m_nMimeType = MimeBodyPart::APPLICATION_MDB;
            else if( ct.match("application/x-pkcs12") )
                m_nMimeType = MimeBodyPart::APPLICATION_P12;
            else if( ct.match("application/x-shockwave-flash") )
                m_nMimeType = MimeBodyPart::APPLICATION_SWF;
            else if( ct.match("application/x-tar") )
                m_nMimeType = MimeBodyPart::APPLICATION_TAR;
            else if( ct.match("application/x-tcl") )
                m_nMimeType = MimeBodyPart::APPLICATION_TCL;
            else if( ct.match("application/x-tex") )
                m_nMimeType = MimeBodyPart::APPLICATION_TEX;
            else if( ct.match("application/x-x509-ca-cert") )
                m_nMimeType = MimeBodyPart::APPLICATION_CERT;
            else if( ct.match("application/zip") )
                m_nMimeType = MimeBodyPart::APPLICATION_ZIP;
            //end add
            else 
                m_nMimeType = MimeBodyPart::APPLICATION; 
        }
    }
    return m_nMimeType; 
}

/**
 * Is this Part of the specified MIME type?  This method
 * compares <strong>only the <code>primaryType</code> and 
 * <code>subType</code></strong>.
 * The parameters of the content types are ignored. <p>
 *
 * For example, this method will return <code>true</code> when
 * comparing a Part of content type <strong>"text/plain"</strong>
 * with <strong>"text/plain; charset=foobar"</strong>. <p>
 *
 * If the <code>subType</code> of <code>mimeType</code> is the
 * special character '*', then the subtype is ignored during the
 * comparison.
 *
 * @param part  point to mime part
 * @param mimeType  content description
 */
BOOL MimeBodyPart::isMimeType(IMimePart &part, FastString &mimeType)
{
    // XXX - lots of room for optimization here!
    BOOL result = FALSE; 
    if( !mimeType.empty() ) 
    {
        FastString s; 
        part.getContentType(s); 
        ContentType ct(s);
        result = ct.match(mimeType);
        if( result == FALSE ) 
            result = s.equalsIgnoreCase(mimeType); 
    }
    return result; 
}

/**
 * Returns the "Content-Description" header field of the specified part.
 * This typically associates some descriptive information with 
 * this part. Returns empty if this field is unavailable or its
 * value is absent. <p>
 *
 * If the Content-Description field is encoded as per RFC 2047,
 * it is decoded and converted into Unicode. If the decoding or 
 * conversion fails, the raw data is returned as is. <p>
 *
 * This implementation uses <code>getHeader(name)</code>
 * to obtain the requisite header field.
 * 
 * @param part  point to mime part
 * @param s     content description
 */
void MimeBodyPart::getDescription(IMimePart &part, FastString &s) 
{
    FastString name("Content-Description"); 
    part.getHeader(name, s); 
    //MimeUtility::unfold(s); 
    MimeUtility::decodeText(s); 
}

/**
 * Set the "Content-Description" header field for the specified part.
 * If the description parameter is <code>empty</code>, then any 
 * existing "Content-Description" fields are removed. <p>
 *
 * If the description contains non US-ASCII characters, it will 
 * be encoded using the platform's default charset. If the 
 * description contains only US-ASCII characters, no encoding 
 * is done and it is used as is. <p>
 *
 * Note that if the charset encoding process fails, a
 * MessagingException is thrown, and an UnsupportedEncodingException
 * is included in the chain of nested exceptions within the
 * MessagingException.
 * 
 * @param part  point to mime part
 * @param s     content description
 * @param charset  Charset of content in mime part
 */
void MimeBodyPart::setDescription(IMimePart &part, FastString &s, 
                                  const char *charset) 
{
    FastString name("Content-Description"); 
    if( s.empty() ) 
    {
        part.removeHeader(name); 
        return; 
    }
    FastString es; 
    MimeUtility::encodeWord(s, es, 1, charset); 
    MimeUtility::fold(es, 21); 
    part.setHeader(name, es); 
}

/**
 * Returns the value of the "Content-Disposition" header field.
 * This represents the disposition of the input part. The disposition
 * describes how the part should be presented to the user. <p>
 *
 * If the Content-Disposition field is unavailable,
 * empty is returned. <p>
 *
 * This implementation uses <code>getHeader(name)</code>
 * to obtain the requisite header field.
 *
 * @param part  point to mime part
 * @param s     content Disposition
 * @see #headers
 */
void MimeBodyPart::getDisposition(IMimePart &part, FastString &s) 
{
    FastString header, name("Content-Disposition"); 
    part.getHeader(name, header);

    if( !header.empty() ) 
    {
        ContentDisposition cd(s);
        cd.getDisposition(s);
    }
    else
        s.clear(); 
}

/**
 * Set the "Content-Disposition" header field of the input part.
 * If the disposition is empty, any existing "Content-Disposition"
 * header field is removed.
 *
 * @param part  point to mime part
 * @param s     content Disposition
 * @see #headers
 */
void MimeBodyPart::setDisposition(IMimePart &part, FastString &s) 
{
    FastString name("Content-Disposition"); 
    if( s.empty() )
        part.removeHeader(name);
    else 
    {
        FastString header; 
        part.getHeader(name, header);
        if( !header.empty() ) 
        { 
            /* A Content-Disposition header already exists ..
             *
             * Override disposition, but attempt to retain 
             * existing disposition parameters
             */
            ContentDisposition cd(header);
            cd.setDisposition(s);
            cd.toString(s);
        }
        part.setHeader(name, s);
    }
}

/**
 * Get the filename associated with the input part. <p>
 *
 * Returns the value of the "filename" parameter from the
 * "Content-Disposition" header field of this body part. If its
 * not available, returns the value of the "name" parameter from
 * the "Content-Type" header field of this body part.
 * Returns <code>empty</code> if both are absent.
 *
 * @param part  point to mime part
 * @param s     filename
 */
void MimeBodyPart::getFileName(IMimePart &part, FastString &s) 
{
    FastString header, name("Content-Disposition");
    part.getHeader(name, header);
    s.clear(); 
    if( !header.empty() ) 
    {
        // Parse the header ..
        ContentDisposition cd(header);
        cd.getParameter("filename", s);
        MimeUtility::decodeWord(s);
    }
    if( s.empty() ) 
    {
        // Still no filename ? Try the "name" ContentType parameter
        name = "Content-Type"; 
        part.getHeader(name, header);
        if( !header.empty() ) 
        {
            ContentType ct(header);
            ct.getParameter("name", s);
            MimeUtility::decodeWord(s); 
        }
    }
    // add by henh 2012/7/20 16:13:31 for special file name
    int pos1;
    FastString keyName;
    if(s.empty())
    {
    	name = "Content-Disposition"; 
      part.getHeader(name, header);
    	if( !header.empty() ) 
    	{
    		keyName = "filename";
	    	pos1 = header.indexOf(keyName);
	    	if(pos1>0)
	    		s = header.substr(pos1+keyName.size()+1,header.size()-pos1-keyName.size()-1);
    	}
    }
    if(s.empty())
    {
    	name = "Content-Disposition"; 
      part.getHeader(name, header);
    	if( !header.empty() ) 
    	{
	    	keyName = "name";
	    	pos1 = header.indexOf(keyName);
	    	if(pos1>0)
	    		s = header.substr(pos1+keyName.size()+1,header.size()-pos1-keyName.size()-1);
	    }
    }
    // add end
}

void MimeBodyPart::getFileName(IMimePart &part, FastString &s, FastString &charset) 
{
    FastString header, name("Content-Disposition");
    part.getHeader(name, header);
    s.clear(); 
    charset.clear();
    if( !header.empty() ) 
    {
        // Parse the header ..
        ContentDisposition cd(header);
        cd.getParameter("filename", s);
        MimeUtility::getCharset(s,charset);
        MimeUtility::decodeWord(s);
    }
    if( s.empty() ) 
    {
        // Still no filename ? Try the "name" ContentType parameter
        name = "Content-Type"; 
        part.getHeader(name, header);
        if( !header.empty() ) 
        {
            ContentType ct(header);
            ct.getParameter("name", s);
            MimeUtility::getCharset(s,charset);
            MimeUtility::decodeWord(s); 
        }
    }
    // add by henh 2012/7/20 16:13:31 for special file name
    int pos1;
    FastString keyName;
    if(s.empty())
    {
    	name = "Content-Disposition"; 
      part.getHeader(name, header);
    	if( !header.empty() ) 
    	{
    		keyName = "filename";
	    	pos1 = header.indexOf(keyName);
	    	if(pos1>0)
	    	{
	    		s = header.substr(pos1+keyName.size()+1,header.size()-pos1-keyName.size()-1);
	    		MimeUtility::decodeWord(s); 
	    	}
    	}
    }
    if(s.empty())
    {
    	name = "Content-Disposition"; 
      part.getHeader(name, header);
    	if( !header.empty() ) 
    	{
	    	keyName = "name";
	    	pos1 = header.indexOf(keyName);
	    	if(pos1>0)
	    	{
	    		s = header.substr(pos1+keyName.size()+1,header.size()-pos1-keyName.size()-1);
	    		MimeUtility::decodeWord(s); 
	    	}
	    }
    }
    if(charset.empty())
    	MimeUtility::getCharset(s,charset);
    // add end
}

void MimeBodyPart::setFileName(IMimePart &part, FastString &s)
{
	setFileName(part, s, NULL);
}

/**
 * Set the filename associated with the input part, if possible. <p>
 *
 * Sets the "filename" parameter of the "Content-Disposition"
 * header field of this body part.
 *
 * @param part  point to mime part
 * @param s     filename
 */
// add by ljh, 增加参数charset，以便设置正确的编码
void MimeBodyPart::setFileName(IMimePart &part, FastString &s, const char* charset) 
{
    FastString header, name("Content-Disposition"); 
    part.getHeader(name, header);
    if( header.empty() )
        header = IMimePart::ATTACHMENT; 

    FastString es; 
// modify by ljh begin
//    MimeUtility::encodeWord(s, es, 0); 
	if(charset == NULL)
	{
		MimeUtility::encodeWord(s, es, 0); 
	}
	else
	{
		MimeUtility::encodeWord(s, es, 0, charset,"B"); 
	}

// modify by ljh end

    // Set the Content-Disposition "filename" parameter
    ContentDisposition cd(header);
    cd.setParameter("filename", es);
    cd.toString(header); 
    part.setHeader(name, header);

    /** 
     * Also attempt to set the Content-Type "name" parameter,
     * to satisfy ancient MUAs. 
     * XXX: This is not RFC compliant, and hence should really
     * be conditional based on some property. Fix this once we
     * figure out how to get at Properties from here !
     */
    name = "Content-Type"; 
    part.getHeader(name, header);
    if( !header.empty() ) 
    {
        ContentType cType(header);

        /**
         * Find the mime-type with file type in the mimetypes map 
         * if the filename has extern file type, ignore case. 
         */
        if( s.lastIndexOf('.') >= 0 ) 
        {
            MimetypesFileTypeMap &map = MimetypesFileTypeMap::getInstance(); 
            FastString mimeType; 
            map.getContentType(s, mimeType); 
            if( !mimeType.empty() ) 
            {
                ContentType mType(mimeType); 
                cType.setPrimaryType(mType.primaryType()); 
                cType.setSubType(mType.subType()); 
            }
        }

        cType.setParameter("name", es);
        cType.toString(header); 
        part.setHeader(name, header);
    }
}

/**
 * Get the content transfer encoding from the
 * "Content-Transfer-Encoding" header
 * field. Returns <code>empty</code> if the header is unavailable
 * or its value is absent. <p>
 *
 * This implementation uses <code>getHeader(name)</code>
 * to obtain the requisite header field.
 *
 * @param part  point to mime part
 * @param s     encoding type
 */
void MimeBodyPart::getEncoding(IMimePart &part, FastString &s) 
{
    FastString header, name("Content-Transfer-Encoding"); 
    part.getHeader(name, header);

    s.clear(); 
    if( header.empty() )
        return;

    header.trim(); // get rid of trailing spaces
    // quick check for known values to avoid unnecessary use
    // of tokenizer.
    if( header.equalsIgnoreCase("7bit") || header.equalsIgnoreCase("8bit") ||
        header.equalsIgnoreCase("quoted-printable") ||
        header.equalsIgnoreCase("base64") ) 
    {
        s = header; 
        return; 
    }

    // Tokenize the header to obtain the encoding (skip comments)
    HeaderTokenizer ht(header, HeaderTokenizer::MIME);

    HeaderTokenizer::Token tk;
    int tkType = 0;

    for(;;) 
    {
        ht.next(tk); // get a token
        tkType = tk.getType();
        if( tkType == HeaderTokenizer::Token::TOKEN_EOF )
            break; // done
        else if( tkType == HeaderTokenizer::Token::ATOM )
            tk.getValue(s);
        else // invalid token, skip it.
            continue;
        break; 
    }
}

/**
 * Get the languages specified in the Content-Language header
 * of the input MimePart. The Content-Language header is defined by
 * RFC 1766. Returns <code>null</code> if this header is not
 * available or its value is absent. <p>
 *
 * This implementation uses <code>getHeader(name)</code>
 * to obtain the requisite header field.
 */
void MimeBodyPart::getContentLanguage(IMimePart &part, FastStringArray &languages) 
{
    FastString header, name("Content-Language"); 
    part.getHeader(name, header);

    languages.release(); 
    if( header.empty() )
        return;

    // Tokenize the header to obtain the Language-tags (skip comments)
    HeaderTokenizer ht(header, HeaderTokenizer::MIME);
    HeaderTokenizer::Token tk;
    int tkType = 0;
    FastVector<FastString> v; 

    while(true) 
    {
        ht.next(tk); // get a language-tag
        tkType = tk.getType();
        if( tkType == HeaderTokenizer::Token::TOKEN_EOF )
            break; // done
        else if( tkType == HeaderTokenizer::Token::ATOM ) 
        {
            FastString s; 
            tk.getValue(s); 
            v.addElement(s);
        }
        else // invalid token, skip it.
            continue;
    }

    if( v.size() > 0 ) 
        v.copyInto(languages);
}

/**
 * Set the Content-Language header of the input MimePart. The
 * Content-Language header is defined by RFC 1766.
 *
 * @param part  mime body part or message
 * @param languages array of language tags
 */
void MimeBodyPart::setContentLanguage(IMimePart &part, FastStringArray &languages)
{
    FastString sb, name("Content-Language");
    if( languages.size() > 0 ) 
    {
        sb.append(languages[0]); 
        for( size_t i = 1; i < languages.size(); i++ ) 
        {
            sb.append(','); 
            sb.append(languages[i]); 
        }
    }
    part.setHeader(name, sb);
}

/**
 * Returns the value of the RFC 822 "Date" field. This is the date 
 * on which this message was sent. Returns -1 if this field is 
 * unavailable or its value is absent. <p>
 * 
 * This implementation uses the <code>getHeader</code> method
 * to obtain the requisite header field.
 *
 * @return       The sent Date
 */
time_t MimeBodyPart::getSentDate() 
{
    FastString header, name("Date"); 
    this->getHeader(name, header); 
    if( !header.empty() ) 
        return MailDateFormat::parse(header); 
    else
        return (time_t)(-1); 
}

/**
 * Set the RFC 822 "Date" header field. This is the date on which the
 * creator of the message indicates that the message is complete
 * and ready for delivery. If the date parameter is 
 * <code>-1</code>, the existing "Date" field is removed.
 *
 */
void MimeBodyPart::setSentDate(const time_t &timer) 
{
    FastString header, name("Date"); 
    if( timer > 0 ) 
    {
        MailDateFormat df; 
        df.format(timer, header); 
        if( !header.empty() ) 
        {
            this->setHeader(name, header);
            return; 
        }
    }
    this->removeHeader(name); 
}

/**
 * A convenience method for setting this body part's content. <p>
 *
 * The content is wrapped in a attachment object or text string. 
 * That is, to do <code>setContent(foobar, "application/x-foobar")</code>,
 * a DataContentHandler for "application/x-foobar" should be installed.
 *
 * @param s     the content data string
 * @param type  Mime type of the content
 */
void MimeBodyPart::setContent(FastString &s, FastString &type) 
{
    releaseContent(); 

    m_psContent = new FastString(); 
    m_psContent->swap(s); 

    if( type.empty() ) 
    {
        getContentType(type); 
        if( type.empty() ) 
        {
            type = "text/plain"; 
            setContentType(type); 
        }
    }
    else 
        setContentType(type); 

    m_bBodyParsed = TRUE; 
}

/**
 * This method sets the body part's content to a Multipart object.
 *
 * @param mp    The multipart object that is the Message's content
 */
void MimeBodyPart::setContent(IMultipart *mp) 
{
    if( mp == 0 ) 
        return; 

    releaseContent(); 

    m_pMultipart = (IMultipart *) new MimeMultipart(); 
    ((MimeMultipart *)m_pMultipart)->swap(*((MimeMultipart *)mp)); 
    ((MimeMultipart *)m_pMultipart)->updateParent(); 
    ((MimeMultipart *)m_pMultipart)->setParent(this); 

    FastString s; 
    m_pMultipart->getContentType(s); 
    setContentType(s); 

    m_bBodyParsed = TRUE; 
}

/**
 * Convenience method that sets the given String as this part's
 * content, with a MIME type of "text/plain" and the specified
 * charset. The given Unicode string will be charset-encoded
 * using the specified charset. The charset is also used to set
 * the "charset" parameter.
 *
 * @param s         text body content string 
 * @param charset   text content encode charset
 * @param stype     text body content-type
 */
void MimeBodyPart::setText(FastString &s, FastString &charset, FastString &stype)
{
    if( charset.empty() ) 
    {
        if( MimeUtility::checkAscii(s) != MimeUtility::ALL_ASCII )
            MimeUtility::getDefaultMIMECharset(charset);
        else
            charset = "us-ascii";
    }

    MimeUtility::quote(charset, HeaderTokenizer::MIME); 

    if( stype.empty() ) 
    {
        stype = "text/plain; charset="; 
        stype.append(charset); 
    }
    else
    {
        ContentType cType(stype); 
        cType.setParameter("charset", charset.c_str()); 
        cType.toString(stype); 
    }

    setContent(s, stype);

    //setEncoding("quoted-printable"); 
    //setEncoding("base64"); 
}

/**
 * Return the size of the m_psContent of this body part in bytes.
 * Return -1 if the size cannot be determined. <p>
 *
 * Note that this number may not be an exact measure of the
 * m_psContent size and may or may not account for any transfer
 * encoding of the m_psContent. <p>
 *
 * This implementation returns the size of the <code>m_psContent</code>
 * (if not null), or, if <code>m_psContent</code> is not
 * null, and the <code>available</code> method returns a positive
 * number, it returns that number as the size.  Otherwise, it returns
 * -1.
 *
 * @return size in bytes, or -1 if not known
 */
int MimeBodyPart::getSize()
{
    parsebody(); 
    if( m_psContent ) 
        return m_psContent->length(); 
    else
        return -1; 
}

/**
 * Return the number of lines for the m_psContent of this message.
 * Return -1 if this number cannot be determined. <p>
 *
 * Note that this number may not be an exact measure of the 
 * m_psContent length and may or may not account for any transfer 
 * encoding of the m_psContent. <p>
 *
 * This implementation returns -1.
 *
 * @return  number of lines in the m_psContent.
 */
int MimeBodyPart::getLineCount()
{
    parsebody(); 
    return -1; 
}

/**
 * Examine the content of this body part and update the appropriate
 * MIME headers.  Typical headers that get set here are
 * <code>Content-Type</code> and <code>Content-Transfer-Encoding</code>.
 * Headers might need to be updated in two cases:
 *
 * <br>
 * - A message being crafted by a mail application will certainly
 * need to activate this method at some point to fill up its internal
 * headers.
 *
 * <br>
 * - A message read in from a Store will have obtained
 * all its headers from the store, and so doesn't need this.
 * However, if this message is editable and if any edits have
 * been made to either the content or message structure, we might
 * need to resync our headers.
 *
 * <br>
 * In both cases this method is typically called by the
 * <code>Message.saveChanges</code> method.
 */
void MimeBodyPart::updateHeaders(IMimePart &part) 
{
    FastString type, s; 
    part.getContentType(type);

    BOOL composite = FALSE;
    BOOL needCTHeader = type.empty() ? TRUE : FALSE;

    ContentType cType(type);
    if( cType.match("multipart/*") ) 
    {
        // If multipart, recurse
        composite = TRUE; 
        MimeMultipart *mp = part.getMultipart(); 
        if( mp ) mp->updateHeaders(); 
    } 
    else if( cType.match("message/rfc822") ) 
        composite = TRUE;
    
    // Content-Transfer-Encoding, but only if we don't
    // already have one
    if( !composite ) // not allowed on composite parts
    {
        part.getHeader("Content-Transfer-Encoding", s); 
        if( s.empty() ) 
        {
            MimeUtility::getEncoding(part, s); 
            setEncoding(part, s);
        }

        cType.getParameter("charset", s); 

        if( needCTHeader && part.isSetDefaultTextCharset() &&
            cType.match("text/*") && s.empty() ) 
        {
            /*
             * Set a default charset for text parts.
             * We really should examine the data to determine
             * whether or not it's all ASCII, but that's too
             * expensive so we make an assumption:  If we
             * chose 7bit encoding for this data, it's probably
             * ASCII.  (MimeUtility.getEncoding will choose
             * 7bit only in this case, but someone might've
             * set the Content-Transfer-Encoding header manually.)
             */
            FastString charset, enc;
            part.getEncoding(enc);
            if( enc.equalsIgnoreCase("7bit") ) 
                charset = "us-ascii";
            else
                MimeUtility::getDefaultMIMECharset(charset);
            cType.setParameter("charset", charset);
            cType.toString(type);
        }

        // Now, let's update our own headers ...

        // Content-type, but only if we don't already have one
        if( needCTHeader ) 
        {
            /*
             * Pull out "filename" from Content-Disposition, and
             * use that to set the "name" parameter. This is to
             * satisfy older MUAs (DtMail, Roam and probably
             * a bunch of others).
             */
            part.getHeader("Content-Disposition", s);
            if( !s.empty() ) 
            {
                // Parse the header ..
                ContentDisposition cd(s);
                cd.getParameter("filename", s);
                if( !s.empty() ) 
                {
                    cType.setParameter("name", s);
                    cType.toString(type);
                }
            }
            
            part.setHeader("Content-Type", type);

            // Must reparse MimeType 
            part.setMimeType(MimeBodyPart::UNKNOWN_MIMETYPE); 
        }
    }
}

/**
 * Set the value of the RFC 822 "Content-Type" header field. 
 * This represents the content-type of the content of this 
 * message. <p>
 *
 * This implementation uses the <code>setHeader</code> method
 * to obtain the requisite header field.
 *
 * @param s     The ContentType of this part
 */
void MimeBodyPart::setContentType(FastString &s)
{
    if( !s.empty() ) 
    {
        FastString sType; 
        ContentType cType(s); 

        /*
         * Pull out "filename" from Content-Disposition, and
         * use that to set the "name" parameter. This is to
         * satisfy older MUAs (DtMail, Roam and probably
         * a bunch of others).
         */
        getHeader("Content-Disposition", sType);
        if( !sType.empty() ) 
        {
            // Parse the header ..
            ContentDisposition cd(sType);
            cd.getParameter("filename", sType);
            if( !sType.empty() ) 
                cType.setParameter("name", sType);
        }

        cType.toString(sType); 
        if( !sType.empty() ) 
            setHeader("Content-Type", sType); 

        // Must reparse MimeType 
        setMimeType(MimeBodyPart::UNKNOWN_MIMETYPE); 
    }
}

/**
 * Convert this bodypart into a RFC 822 / RFC 2047 encoded string, 
 * and append it to a existed string buffer. 
 * The resulting string contains only US-ASCII characters, and
 * hence is mail-safe.
 *
 * @param s     possibly encoded mime string that appended
 */
void MimeBodyPart::appendTo(FastString &s) 
{
    // safely check if s is current contentbuffer
    if( s.c_str() >= m_psContentBuffer && 
        s.c_str() <= m_psContentBuffer + m_nContentBufferSize ) 
        return; 

    parsebody(); 

    // First, write out the header
    FastString hdrLines; 
    getAllHeaderLines(hdrLines); 
    s.append(hdrLines); 

    // The CRLF separator between header and content
    s.append("\r\n\r\n");

    // Finally, the content. Encode if required.
    // XXX: May need to account for ESMTP ?
    if( isMultipart() && m_pMultipart ) 
    {
        m_pMultipart->appendTo(s); 
    }
    else if( m_psContent ) 
    {
        FastString bodylines, encoding; 
        getEncoding(encoding); 
        MimeUtility::encode(*m_psContent, bodylines, encoding); 
        s.append(bodylines); 
    }
}


_FASTMIME_END_NAMESPACE
