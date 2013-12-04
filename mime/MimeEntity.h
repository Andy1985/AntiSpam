//=============================================================================
/**
 *  @file    MimeEntity.h
 *
 *  ver 1.0.0 2004/06/16 Naven Exp, for Mime Message Parse Engine.
 *
 *  @author Naven 2004/06/16 created.
 */
//=============================================================================

#ifndef _MIMEENTITY_H
#define _MIMEENTITY_H

#if defined(_WIN32) || defined(__WIN32__)

#if !defined (NAVEN_PRAGMA_ONCE)
# pragma once
#endif /* NAVEN_PRAGMA_ONCE */

#endif /* _WIN32 */


#include "MimeBase.h" 
#include "MimeUtility.h"
#include "MimeObject.h"


_FASTMIME_BEGIN_NAMESPACE



//=================Class MimeBodyPart Define==================

/**
 * This class represents a MIME body part. It implements the 
 * <code>BodyPart</code> abstract class and the <code>MimePart</code>
 * interface. MimeBodyParts are contained in <code>MimeMultipart</code>
 * objects. <p>
 *
 * MimeBodyPart uses the <code>InternetHeaders</code> class to parse
 * and store the headers of that body part. <p>
 *
 * <hr><strong>A note on RFC 822 and MIME headers</strong><p>
 *
 * RFC 822 header fields <strong>must</strong> contain only
 * US-ASCII characters. MIME allows non ASCII characters to be present
 * in certain portions of certain headers, by encoding those characters.
 * RFC 2047 specifies the rules for doing this. The MimeUtility
 * class provided in this package can be used to to achieve this.
 * Callers of the <code>setHeader</code>, <code>addHeader</code>, and
 * <code>addHeaderLine</code> methods are responsible for enforcing
 * the MIME requirements for the specified headers.  In addition, these
 * header fields must be folded (wrapped) before being sent if they
 * exceed the line length limitation for the transport (1000 bytes for
 * SMTP).  Received headers may have been folded.  The application is
 * responsible for folding and unfolding headers as appropriate. <p>
 *
 */
class MimeBodyPart : public IMimePart
{
protected: 
    /**
     * The <code>Multipart</code> object containing this <code>BodyPart</code>,
     * if known.
     */
    IMultipart *m_pParent; 

    /**
     * Pointer to the buffer that holds the bytes of 
     * the content of this Part.
     */
    char *m_psContentBuffer; 
    size_t m_nContentBufferSize;

    /**
     * Pointer to the buffer that holds the bytes of 
     * the header of this Part.
     * header is contained in the content.
     */
    char *m_psHeaderBuffer; 
    size_t m_nHeaderBufferSize; 

    /**
     * Pointer to the buffer that holds the bytes of 
     * the body of this Part.
     * body is contained in the content.
     */
    char *m_psBodyBuffer; 
    size_t m_nBodyBufferSize; 

    /**
     * Header array that holds all the header lines
     */
    InternetHeaders m_ihHeaders; 

    /**
     * The <code>Multipart</code> object contained by this <code>BodyPart</code>,
     * if has.
     */
    IMultipart *m_pMultipart; 

    /**
     * The text/plain or attachment data contained by this <code>BodyPart</code>,
     * if has.
     */
    FastString *m_psContent; 

    /**
     * mime type of this part, used by getMimeType(). 
     */
    int m_nMimeType; 

    /**
     * Flag to mark the content header parsed or not
     */
    BOOL m_bHeaderParsed; 

    /**
     * Flag to mark the content body parsed or not
     */
    BOOL m_bBodyParsed; 

    /**
     * =1 to enforce RFC822 address parse or =0 not.
     */
    BOOL m_bStrict; 

    /**
     * default charset for this to encode/decode text string. 
     */
    BOOL m_bSetDefaultTextCharset; 

protected: 
    MimeBodyPart(MimeBodyPart &part, BOOL swap_value);
    MimeBodyPart(char *psContent);
    MimeBodyPart(char *psContent, size_t len);
    void parseheader(); 
    void parsebody(); 
    void releaseContent(); 
    void updateParent(); 
    void updateHeaders(); 
    IMultipart *newMimeMultipart(IMultipart *mp); 

public:
    MimeBodyPart();
    MimeBodyPart(const MimeBodyPart &part);
    virtual ~MimeBodyPart();
    MimeBodyPart& operator=(const MimeBodyPart &part); 
    void swap(MimeBodyPart &part); 
    void release(); 
    virtual void dump(); 

    virtual BOOL isMimeBodyPart(); 
    virtual BOOL isMimeMessage(); 

    BOOL isSetDefaultTextCharset(); 
    MimeMessage *getMessage(); 
    IMultipart *getParent(); 
    void setParent(IMultipart *parent); 
    const char * getContentBuffer(); 
    int getContentBufferSize(); 
    const char * getHeaderBuffer(); 
    int getHeaderBufferSize(); 
    const char * getBodyBuffer(); 
    int getBodyBufferSize(); 

    int  getHeader(FastString &name, FastString &s, const char *sep = NULL); 
    int  getHeader(const char *name, FastString &s, const char *sep = NULL); 
    int  getHeader(FastString &name, FastStringArray &values); 
    int  getHeader(const char *name, FastStringArray &values); 
    int  getMatchingHeaders(FastStringArray &names, hdrArray &headers); 
    int  getNonMatchingHeaders(FastStringArray &names, hdrArray &headers); 
    int  getAllHeaders(hdrArray &headers); 
    int  getMatchingHeaderLines(FastStringArray &names, FastString &lines); 
    int  getNonMatchingHeaderLines(FastStringArray &names, FastString &lines); 
    int  getAllHeaderLines(FastString &lines); 
    void setHeader(FastString &name, FastString &s); 
    void setHeader(const char *name, FastString &s); 
    void setHeader(const char *name, const char *s); 
    void addHeader(FastString &name, FastString &s, int index = -1); 
    void addHeader(const char *name, FastString &s, int index = -1); 
    void addHeader(const char *name, const char *s, int index = -1); 
    void addHeaderBehind(FastString &field, FastString &name, FastString &s); 
    void addHeaderBehind(const char *field, FastString &name, FastString &s); 
    void addHeaderBehind(const char *field, const char *name, FastString &s); 
    void addHeaderBehind(FastString &field, const char *name, FastString &s); 
    void addHeaderBehind(const char *field, const char *name, const char *s); 
    void addHeaderBefore(FastString &field, FastString &name, FastString &s); 
    void addHeaderBefore(const char *field, FastString &name, FastString &s); 
    void addHeaderBefore(const char *field, const char *name, FastString &s); 
    void addHeaderBefore(FastString &field, const char *name, FastString &s); 
    void addHeaderBefore(const char *field, const char *name, const char *s); 
    void addHeaderLine(FastString &line); 
    void addHeaderLine(const char *line, int len = -1); 
    void removeHeader(FastString &name); 
    void removeHeader(const char *name); 
    void removeAllHeaders(); 
    hdrArrayIterator getHeaderIterator(); 
    size_t getHeaderCount() const; 
    void setAddressHeader(FastString &name, InternetAddressArray &addrs, FastString &group); 
    void setAddressHeader(const char *name, InternetAddressArray &addrs, const char *group); 
    void setAddressHeader(FastString &name, InternetAddressArray &addrs); 
    void setAddressHeader(const char *name, InternetAddressArray &addrs); 
    void setAddressHeader(FastString &name, InternetAddress &addr); 
    void setAddressHeader(const char *name, InternetAddress &addr); 
    void setAddressHeader(const char *name, const char *addr, const char *personal = NULL, const char *group = NULL); 
    void addAddressHeader(FastString &name, InternetAddressArray &addrs, FastString &group); 
    void addAddressHeader(const char *name, InternetAddressArray &addrs, const char *group); 
    void addAddressHeader(FastString &name, InternetAddressArray &addrs); 
    void addAddressHeader(const char *name, InternetAddressArray &addrs); 
    void addAddressHeader(FastString &name, InternetAddress &addr, FastString &group); 
    void addAddressHeader(const char *name, InternetAddress &addr, const char *group); 
    void addAddressHeader(const char *name, const char *addr, const char *personal = NULL, const char *group = NULL); 
    int  getAddressHeader(FastString &name, InternetAddressArray &addrs); 
    int  getAddressHeader(const char *name, InternetAddressArray &addrs); 
    void removeAddress(FastString &name, InternetAddressArray &addrs); 
    void removeAddress(FastString &name, FastStringArray &addrs); 
    void removeAddress(FastString &name, FastString &addr); 
    void removeAddress(const char *name, const char *addr); 
    void removeAddressGroup(FastString &name, FastString &group); 
    void removeAddressGroup(const char *name, const char *group); 
    int  getSender(InternetAddress &addr); 
    void setSender(InternetAddress &addr); 
    void setSender(const char *addr, const char *personal = NULL); 
    int  getFrom(InternetAddressArray &addrs); 
    int  getFrom(InternetAddress &addr); 
    void setFrom(InternetAddressArray &addrs); 
    void setFrom(InternetAddress &addr); 
    void setFrom(const char *addr, const char *personal = NULL, const char *group = NULL); 
    void addFrom(InternetAddressArray &addrs, const char *group = NULL); 
    void addFrom(InternetAddress &addr, const char *group = NULL); 
    void addFrom(const char *addr, const char *personal = NULL, const char *group = NULL); 
    int  getRecipients(int type, InternetAddressArray &addrs); 
    void setRecipients(int type, InternetAddressArray &addrs); 
    void setRecipients(int type, InternetAddress &addr); 
    void setRecipients(int type, const char *addr, const char *personal = NULL, const char *group = NULL); 
    void addRecipients(int type, InternetAddressArray &addrs, const char *group = NULL); 
    void addRecipients(int type, InternetAddress &addr, const char *group = NULL); 
    void addRecipients(int type, const char *addr, const char *personal = NULL, const char *group = NULL); 
    int  getAllRecipients(InternetAddressArray &addrs); 
    const char *getRecipientsType(int type, FastString &name); 
    int  getTo(InternetAddressArray &addrs); 
    void setTo(InternetAddressArray &addrs); 
    void setTo(InternetAddress &addr); 
    void setTo(const char *addr, const char *personal = NULL, const char *group = NULL); 
    void addTo(InternetAddressArray &addrs, const char *group = NULL); 
    void addTo(InternetAddress &addr, const char *group = NULL); 
    void addTo(const char *addr, const char *personal = NULL, const char *group = NULL); 
    int  getCc(InternetAddressArray &addrs); 
    void setCc(InternetAddressArray &addrs); 
    void setCc(InternetAddress &addr); 
    void setCc(const char *addr, const char *personal = NULL, const char *group = NULL); 
    void addCc(InternetAddressArray &addrs, const char *group = NULL); 
    void addCc(InternetAddress &addr, const char *group = NULL); 
    void addCc(const char *addr, const char *personal = NULL, const char *group = NULL); 
    int  getBcc(InternetAddressArray &addrs); 
    void setBcc(InternetAddressArray &addrs); 
    void setBcc(InternetAddress &addr); 
    void setBcc(const char *addr, const char *personal = NULL, const char *group = NULL); 
    void addBcc(InternetAddressArray &addrs, const char *group = NULL); 
    void addBcc(InternetAddress &addr, const char *group = NULL); 
    void addBcc(const char *addr, const char *personal = NULL, const char *group = NULL); 
    void getReceivedID(FastString &s, const int index = 0); 
    void getContentID(FastString &s); 
    void setContentID(FastString &s); 
    void getContentMD5(FastString &s); 
    void setContentMD5(FastString &s); 
    void getMessageID(FastString &s); 
    void getSubject(FastString &s); 
    // add by henh 2012/7/20 16:13:31 for filename empty
    void getSubject(FastString &s, FastString &charset); 
    // add end
    void setSubject(FastString &s, const char *charset = NULL); 
    void setSubject(const char *s, const char *charset = NULL); 
    time_t getSentDate(); 
    void setSentDate(const time_t &timer); 

    int  getMimeType(); 
    void setMimeType(int mimetype); 
    BOOL isMimeType(FastString &mimeType); 
    BOOL isMimeType(const char *mimeType); 
    BOOL isText(); 
    BOOL isText(FastString &subType); 
    BOOL isText(const char *subType); 
    BOOL isTextPlain(); 
    BOOL isTextHtml(); 
    BOOL isTextEnriched(); 
    BOOL isTextUnrecognized(); 
    BOOL isMultipart(); 
    BOOL isMultipart(FastString &subType); 
    BOOL isMultipart(const char *subType); 
    BOOL isMultipartMixed(); 
    BOOL isMultipartRelated(); 
    BOOL isMultipartAlternative(); 
    BOOL isMessage(); 
    BOOL isMessage(FastString &subType); 
    BOOL isMessage(const char *subType); 
    BOOL isMessageRFC822(); 
    // add by henh 2012/7/20 16:13:31 for message types
    BOOL isMessageDELStatus(); 
    // add end
    BOOL isImage(); 
    BOOL isImage(FastString &subType); 
    BOOL isImage(const char *subType); 
    BOOL isImageJpeg(); 
    BOOL isImageGif(); 
    BOOL isImagePng(); 
    BOOL isAudio(); 
    BOOL isAudio(FastString &subType); 
    BOOL isAudio(const char *subType); 
    BOOL isAudioBasic(); 
    BOOL isAudioMp3(); 
    BOOL isVideo(); 
    BOOL isVideo(FastString &subType); 
    BOOL isVideo(const char *subType); 
    BOOL isVideoMpeg(); 
    BOOL isApplication(); 
    BOOL isApplication(FastString &subType); 
    BOOL isApplication(const char *subType); 
    BOOL isApplicationOctetStream(); 
    BOOL isApplicationPostScript(); 

    void getContentType(FastString &s); 
    void setContentType(FastString &s); 
    void setContentType(const char *s); 
    void getDescription(FastString &s); 
    void setDescription(FastString &s, const char *charset = NULL); 
    void setDescription(const char *s, const char *charset = NULL); 
    void getDisposition(FastString &s); 
    void setDisposition(FastString &s); 
    void setDisposition(const char *s); 
    void getFileName(FastString &s); 
    // add by henh 2012/7/20 16:13:31 for getCharset
    void getFileName(FastString &s, FastString &charset); 
    // add end
    void setFileName(FastString &s); 
    void setFileName(const char *s); 
    void setFileName(FastString &s,const char *charset); 
    void getEncoding(FastString &s); 
    void setEncoding(FastString &s); 
    void setEncoding(const char *s); 
    void getContentLanguage(FastStringArray &languages); 
    void setContentLanguage(FastStringArray &languages); 
    void setContentLanguage(const char *language); 
    void getBodyLines(FastString &s); 
    void getContentLines(FastString &s); 
    void getMultipartLines(FastString &s); 
    void getText(FastString &s); 
    void getContent(FastString &s); 
    FastString* getContent(); 
    MimeMultipart* getMultipart(); 
    void setContent(FastString &s, FastString &type); 
    void setContent(FastString &s, const char *type = 0); 
    void setContent(IMultipart *mp); 
    void setContent(MimeMultipart &mp); 
    void setMultipart(IMultipart *mp); 
    void setMultipart(MimeMultipart &mp); 
    void setText(FastString &s, FastString &charset, FastString &stype); 
    void setText(FastString &s, FastString &charset, const char *stype); 
    void setText(FastString &s, const char *charset, const char *stype); 
    void setText(FastString &s, FastString &charset); 
    void setText(FastString &s, const char *charset = 0); 
    int  getSize(); 
    int  getLineCount(); 
    virtual void appendTo(FastString &s); 
    virtual void toString(FastString &s); 

public:
    static BOOL isMimeType(IMimePart &part, FastString &mimeType); 
    static void getDescription(IMimePart &part, FastString &s); 
    static void setDescription(IMimePart &part, FastString &s, const char *charset = NULL); 
    static void getDisposition(IMimePart &part, FastString &s); 
    static void setDisposition(IMimePart &part, FastString &s); 
    static void getFileName(IMimePart &part, FastString &s); 
    // add by henh 2012/7/20 16:13:31 for getCharset
    static void getFileName(IMimePart &part, FastString &s, FastString &charset); 
    // add end
    static void setFileName(IMimePart &part, FastString &s);
    static void setFileName(IMimePart &part, FastString &s,  const char* charset); 
    static void getEncoding(IMimePart &part, FastString &s); 
    static void setEncoding(IMimePart &part, FastString &s); 
    static void getContentLanguage(IMimePart &part, FastStringArray &languages); 
    static void setContentLanguage(IMimePart &part, FastStringArray &languages); 
    static void updateHeaders(IMimePart &part); 

    friend class MimeMultipart; 
};


//=============MimeBodyPart Inline functions==================

/**
 * Default Constructs a MimeBodyPart.
 *
 */
inline MimeBodyPart::MimeBodyPart()
: m_pParent(0), 
  m_psContentBuffer(0), 
  m_nContentBufferSize(0), 
  m_psHeaderBuffer(0), 
  m_nHeaderBufferSize(0), 
  m_psBodyBuffer(0), 
  m_nBodyBufferSize(0), 
  m_ihHeaders(), 
  m_pMultipart(0), 
  m_psContent(0), 
  m_nMimeType(MimeBodyPart::UNKNOWN_MIMETYPE), 
  m_bHeaderParsed(FALSE), 
  m_bBodyParsed(FALSE), 
  m_bStrict(TRUE), 
  m_bSetDefaultTextCharset(TRUE) 
{
    this->m_ihHeaders.setStrict(m_bStrict); 
}

/**
 * Assign Constructs a MimeBodyPart.
 *
 * @param part  body part object
 */
inline MimeBodyPart::MimeBodyPart(const MimeBodyPart &part) 
: m_pParent(0), 
  m_psContentBuffer(0), 
  m_nContentBufferSize(0), 
  m_psHeaderBuffer(0), 
  m_nHeaderBufferSize(0), 
  m_psBodyBuffer(0), 
  m_nBodyBufferSize(0), 
  m_ihHeaders(), 
  m_pMultipart(0), 
  m_psContent(0), 
  m_nMimeType(MimeBodyPart::UNKNOWN_MIMETYPE), 
  m_bHeaderParsed(FALSE), 
  m_bBodyParsed(FALSE), 
  m_bStrict(TRUE), 
  m_bSetDefaultTextCharset(TRUE) 
{
    operator=(part); 
}

/**
 * Assign Constructs a MimeBodyPart.
 *
 * @param part  body part object
 * @param swap_value  TRUE is swap them else only 
 *          copy a new object
 */
inline MimeBodyPart::MimeBodyPart(MimeBodyPart &part, BOOL swap_value) 
: m_pParent(0), 
  m_psContentBuffer(0), 
  m_nContentBufferSize(0), 
  m_psHeaderBuffer(0), 
  m_nHeaderBufferSize(0), 
  m_psBodyBuffer(0), 
  m_nBodyBufferSize(0), 
  m_ihHeaders(), 
  m_pMultipart(0), 
  m_psContent(0), 
  m_nMimeType(MimeBodyPart::UNKNOWN_MIMETYPE), 
  m_bHeaderParsed(FALSE), 
  m_bBodyParsed(FALSE), 
  m_bStrict(TRUE), 
  m_bSetDefaultTextCharset(TRUE) 
{
    if( swap_value == TRUE ) 
        this->swap(part); 
    else
        operator=(part); 
}

/**
 * Constructs a MimeBodyPart by reading and parsing the data from the
 * specified string. The Input string will be left positioned
 * at the end of the data for the message. Note that the input string
 * parse is done within this constructor itself. <p>
 *
 * This method is for providers subclassing <code>MimeBodyPart</code>.
 *
 * @param psContent the message input string
 */
inline MimeBodyPart::MimeBodyPart(char *psContent)
: m_pParent(0), 
  m_psContentBuffer(psContent), 
  m_nContentBufferSize(psContent ? ::strlen(psContent) : 0), 
  m_psHeaderBuffer(0), 
  m_nHeaderBufferSize(0), 
  m_psBodyBuffer(0), 
  m_nBodyBufferSize(0), 
  m_ihHeaders(), 
  m_pMultipart(0), 
  m_psContent(0), 
  m_nMimeType(MimeBodyPart::UNKNOWN_MIMETYPE), 
  m_bHeaderParsed(FALSE), 
  m_bBodyParsed(FALSE), 
  m_bStrict(TRUE), 
  m_bSetDefaultTextCharset(TRUE) 
{
    this->m_ihHeaders.setStrict(m_bStrict); 
    this->parseheader(); 
}

/**
 * Constructs a MimeBodyPart by reading and parsing the data from the
 * specified string. The Input string will be left positioned
 * at the end of the data for the message. Note that the input string
 * parse is done within this constructor itself. <p>
 *
 * This method is for providers subclassing <code>MimeBodyPart</code>.
 *
 * @param psContent the message input string
 * @param len       the message input string length
 */
inline MimeBodyPart::MimeBodyPart(char *psContent, size_t len)
: m_pParent(0), 
  m_psContentBuffer(psContent), 
  m_nContentBufferSize(len), 
  m_psHeaderBuffer(0), 
  m_nHeaderBufferSize(0), 
  m_psBodyBuffer(0), 
  m_nBodyBufferSize(0), 
  m_ihHeaders(), 
  m_pMultipart(0), 
  m_psContent(0), 
  m_nMimeType(MimeBodyPart::UNKNOWN_MIMETYPE), 
  m_bHeaderParsed(FALSE), 
  m_bBodyParsed(FALSE), 
  m_bStrict(TRUE), 
  m_bSetDefaultTextCharset(TRUE) 
{
    this->m_ihHeaders.setStrict(m_bStrict); 
    this->parseheader(); 
}

/**
 * Assign operator =.
 *
 * @param part  body part object
 */
inline MimeBodyPart& MimeBodyPart::operator=(const MimeBodyPart &part) 
{
    release(); 

    m_pParent                   = part.m_pParent; 
    m_psContentBuffer           = part.m_psContentBuffer; 
    m_nContentBufferSize        = part.m_nContentBufferSize; 
    m_psHeaderBuffer            = part.m_psHeaderBuffer; 
    m_nHeaderBufferSize         = part.m_nHeaderBufferSize; 
    m_psBodyBuffer              = part.m_psBodyBuffer; 
    m_nBodyBufferSize           = part.m_nBodyBufferSize; 
    m_ihHeaders                 = part.m_ihHeaders; 
    m_pMultipart                = newMimeMultipart(part.m_pMultipart);
    m_psContent                 = part.m_psContent ? new FastString(*(part.m_psContent)) : 0; 
    m_nMimeType                 = part.m_nMimeType; 
    m_bHeaderParsed             = part.m_bHeaderParsed; 
    m_bBodyParsed               = part.m_bBodyParsed; 
    m_bStrict                   = part.m_bStrict; 
    m_bSetDefaultTextCharset    = part.m_bSetDefaultTextCharset; 

    updateParent(); 

    return *this; 
}

/**
 * Swap two variables, only swap their points and reference.
 * used for swap two large objects. 
 */
inline void MimeBodyPart::swap(MimeBodyPart &part)
{
    fast_swap_value(m_pParent,                  part.m_pParent); 
    fast_swap_value(m_psContentBuffer,          part.m_psContentBuffer); 
    fast_swap_value(m_nContentBufferSize,       part.m_nContentBufferSize); 
    fast_swap_value(m_psHeaderBuffer,           part.m_psHeaderBuffer); 
    fast_swap_value(m_nHeaderBufferSize,        part.m_nHeaderBufferSize); 
    fast_swap_value(m_psBodyBuffer,             part.m_psBodyBuffer); 
    fast_swap_value(m_nBodyBufferSize,          part.m_nBodyBufferSize); 
    fast_swap_value(m_pMultipart,               part.m_pMultipart); 
    fast_swap_value(m_psContent,                part.m_psContent); 
    fast_swap_value(m_nMimeType,                part.m_nMimeType); 
    fast_swap_value(m_bHeaderParsed,            part.m_bHeaderParsed); 
    fast_swap_value(m_bBodyParsed,              part.m_bBodyParsed); 
    fast_swap_value(m_bStrict,                  part.m_bStrict); 
    fast_swap_value(m_bSetDefaultTextCharset,   part.m_bSetDefaultTextCharset); 

    m_ihHeaders.swap(part.m_ihHeaders); 

    updateParent(); 
}

/**
 *  Destructor ~
 */ 
inline MimeBodyPart::~MimeBodyPart() 
{
    release(); 
}

/**
 * Release all the memory of this object.
 */
inline void MimeBodyPart::release()
{
    releaseContent(); 
}

/**
 * Release all the content memory of this object.
 */
inline void MimeBodyPart::releaseContent() 
{
    if( m_pMultipart ) 
    {
        delete m_pMultipart; 
        m_pMultipart = 0; 
    }
    if( m_psContent ) 
    {
        delete m_psContent; 
        m_psContent = 0; 
    }
    m_bBodyParsed = FALSE; 
}

/**
 * Return TRUE if this is a MimeBodyPart object.
 */
inline BOOL MimeBodyPart::isMimeBodyPart() 
{
    return TRUE; 
}

/**
 * Return TRUE if this is a MimeMessage object.
 */
inline BOOL MimeBodyPart::isMimeMessage() 
{
    return FALSE; 
}

/**
 * Return TRUE if this is SetDefaultTextCharset.
 */
inline BOOL MimeBodyPart::isSetDefaultTextCharset() 
{
    return m_bSetDefaultTextCharset; 
}

/**
 * Return the containing <code>Multipart</code> object,
 * or <code>null</code> if not known.
 */
inline IMultipart* MimeBodyPart::getParent() 
{
    return m_pParent; 
}

/**
 * Set the parent of this <code>BodyPart</code> to be the specified
 * <code>Multipart</code>.  Normally called by <code>Multipart</code>'s
 * <code>addBodyPart</code> method.  <code>parent</code> may be
 * <code>null</code> if the <code>BodyPart</code> is being removed
 * from its containing <code>Multipart</code>.
 */
inline void MimeBodyPart::setParent(IMultipart *parent) 
{
    m_pParent = parent; 
}

/**
 * Return the pointer to the all content buffer..
 */
inline const char * MimeBodyPart::getContentBuffer() 
{
    return m_psContentBuffer; 
}

/**
 * Return the size of the all content buffer..
 */
inline int MimeBodyPart::getContentBufferSize() 
{
    return m_nContentBufferSize; 
}

/**
 * Return the pointer to the header content buffer..
 */
inline const char * MimeBodyPart::getHeaderBuffer() 
{
    return m_psHeaderBuffer; 
}

/**
 * Return the size of the header content buffer..
 */
inline int MimeBodyPart::getHeaderBufferSize() 
{
    return m_nHeaderBufferSize; 
}

/**
 * Return the pointer to the body content buffer..
 */
inline const char * MimeBodyPart::getBodyBuffer() 
{
    return m_psBodyBuffer; 
}

/**
 * Return the size of the all content buffer..
 */
inline int MimeBodyPart::getBodyBufferSize() 
{
    return m_nBodyBufferSize; 
}

// Convenience methods from InternetHeader class.
inline int MimeBodyPart::getHeader(
    const char *name, FastString &s, const char *sep)
{
    return this->m_ihHeaders.getHeader(name, s, sep); 
}

inline int MimeBodyPart::getHeader(
    FastString &name, FastString &s, const char *sep)
{
    return this->m_ihHeaders.getHeader(name, s, sep); 
}

inline int MimeBodyPart::getHeader(
    const char *name, FastStringArray &values)
{
    return this->m_ihHeaders.getHeader(name, values); 
}

inline int MimeBodyPart::getHeader(
    FastString &name, FastStringArray &values)
{
    return this->m_ihHeaders.getHeader(name, values); 
}

inline void MimeBodyPart::setHeader(
    FastString &name, FastString &s) 
{
    this->m_ihHeaders.setHeader(name, s); 
}

inline void MimeBodyPart::setHeader(
    const char *name, FastString &s) 
{
    this->m_ihHeaders.setHeader(name, s); 
}

inline void MimeBodyPart::setHeader(
    const char *name, const char *s) 
{
    FastString faValue(s); 
    this->m_ihHeaders.setHeader(name, faValue); 
}

inline void MimeBodyPart::addHeader(
    FastString &name, FastString &s, int index) 
{
    this->m_ihHeaders.addHeader(name, s, index); 
}

inline void MimeBodyPart::addHeader(
    const char *name, FastString &s, int index) 
{
    this->m_ihHeaders.addHeader(name, s, index); 
}

inline void MimeBodyPart::addHeader(
    const char *name, const char *s, int index) 
{
    FastString faValue(s); 
    this->m_ihHeaders.addHeader(name, faValue, index); 
}

inline void MimeBodyPart::addHeaderBehind(
    FastString &field, FastString &name, FastString &s) 
{
    this->m_ihHeaders.addHeaderBehind(field, name, s); 
}

inline void MimeBodyPart::addHeaderBehind(
    const char *field, FastString &name, FastString &s) 
{
    FastString faField(field); 
    this->m_ihHeaders.addHeaderBehind(faField, name, s); 
}

inline void MimeBodyPart::addHeaderBehind(
    const char *field, const char *name, FastString &s) 
{
    FastString faField(field), faName(name); 
    this->m_ihHeaders.addHeaderBehind(faField, faName, s); 
}

inline void MimeBodyPart::addHeaderBehind(
    FastString &field, const char *name, FastString &s) 
{
    FastString faName(name); 
    this->m_ihHeaders.addHeaderBehind(field, faName, s); 
}

inline void MimeBodyPart::addHeaderBehind(
    const char *field, const char *name, const char *s) 
{
    FastString faField(field), faName(name), faValue(s); 
    this->m_ihHeaders.addHeaderBehind(faField, faName, faValue); 
}

inline void MimeBodyPart::addHeaderBefore(
    FastString &field, FastString &name, FastString &s) 
{
    this->m_ihHeaders.addHeaderBehind(field, name, s, 1); 
}

inline void MimeBodyPart::addHeaderBefore(
    const char *field, FastString &name, FastString &s) 
{
    FastString faField(field); 
    this->m_ihHeaders.addHeaderBefore(faField, name, s); 
}

inline void MimeBodyPart::addHeaderBefore(
    const char *field, const char *name, FastString &s) 
{
    FastString faField(field), faName(name); 
    this->m_ihHeaders.addHeaderBefore(faField, faName, s); 
}

inline void MimeBodyPart::addHeaderBefore(
    FastString &field, const char *name, FastString &s) 
{
    FastString faName(name); 
    this->m_ihHeaders.addHeaderBefore(field, faName, s); 
}

inline void MimeBodyPart::addHeaderBefore(
    const char *field, const char *name, const char *s) 
{
    FastString faField(field), faName(name), faValue(s); 
    this->m_ihHeaders.addHeaderBefore(faField, faName, faValue); 
}

inline void MimeBodyPart::addHeaderLine(FastString &line) 
{
    this->m_ihHeaders.addHeaderLine(line); 
}

inline void MimeBodyPart::addHeaderLine(const char *line, int len) 
{
    this->m_ihHeaders.addHeaderLine(line, len); 
}

inline void MimeBodyPart::removeHeader(FastString &name) 
{
    this->m_ihHeaders.removeHeader(name); 
}

inline void MimeBodyPart::removeHeader(const char *name) 
{
    this->m_ihHeaders.removeHeader(name); 
}

inline void MimeBodyPart::removeAllHeaders() 
{
    this->m_ihHeaders.removeAllHeaders(); 
}

inline hdrArrayIterator MimeBodyPart::getHeaderIterator() 
{
    return this->m_ihHeaders.getHeaderIterator(); 
}

inline size_t MimeBodyPart::getHeaderCount() const 
{
    return this->m_ihHeaders.getHeaderCount(); 
}

inline int MimeBodyPart::getMatchingHeaders(
    FastStringArray &names, hdrArray &headers) 
{
    return this->m_ihHeaders.getMatchingHeaders(names, headers); 
}

inline int MimeBodyPart::getNonMatchingHeaders(
    FastStringArray &names, hdrArray &headers) 
{
    return this->m_ihHeaders.getNonMatchingHeaders(names, headers); 
}

inline int MimeBodyPart::getAllHeaders(hdrArray &headers) 
{
    return this->m_ihHeaders.getAllHeaders(headers); 
}

inline int MimeBodyPart::getMatchingHeaderLines(
    FastStringArray &names, FastString &lines)
{
    return this->m_ihHeaders.getMatchingHeaderLines(names, lines); 
}

inline int MimeBodyPart::getNonMatchingHeaderLines(
    FastStringArray &names, FastString &lines) 
{
    return this->m_ihHeaders.getNonMatchingHeaderLines(names, lines); 
}

inline int MimeBodyPart::getAllHeaderLines(FastString &lines) 
{
    return this->m_ihHeaders.getAllHeaderLines(lines); 
}

// Convenience method to get addresses
inline int MimeBodyPart::getAddressHeader(
    FastString &name, InternetAddressArray &addrs) 
{
    return this->m_ihHeaders.getAddressHeader(name, addrs); 
}

inline int MimeBodyPart::getAddressHeader(
    const char *name, InternetAddressArray &addrs) 
{
    FastString faName(name); 
    return this->m_ihHeaders.getAddressHeader(faName, addrs); 
}

// Convenience method to set addresses
inline void MimeBodyPart::setAddressHeader(
    FastString &name, InternetAddressArray &addrs, FastString &group) 
{
    this->m_ihHeaders.setAddressHeader(name, addrs, group); 
}

inline void MimeBodyPart::setAddressHeader(
    const char *name, InternetAddressArray &addrs, const char *group) 
{
    FastString faName(name), faGroup(group); 
    this->setAddressHeader(faName, addrs, faGroup); 
}

inline void MimeBodyPart::setAddressHeader(
    FastString &name, InternetAddressArray &addrs) 
{
    this->m_ihHeaders.setAddressHeader(name, addrs); 
}

inline void MimeBodyPart::setAddressHeader(
    const char *name, InternetAddressArray &addrs) 
{
    FastString faName(name); 
    this->setAddressHeader(faName, addrs); 
}

inline void MimeBodyPart::setAddressHeader(
    FastString &name, InternetAddress &addr) 
{
    InternetAddressArray addrs(1); 
    addrs.set(addr, 0); 
    this->setAddressHeader(name, addrs); 
}

inline void MimeBodyPart::setAddressHeader(
    const char *name, InternetAddress &addr) 
{
    FastString faName(name); 
    this->setAddressHeader(faName, addr); 
}

inline void MimeBodyPart::setAddressHeader(
    const char *name, const char *addr, const char *personal, 
    const char *group) 
{
    FastString faName(name), faAddr(addr); 
    FastString faPersonal(personal), faGroup(group); 
    InternetAddress address; 
    address.setAddress(faAddr); 
    address.setPersonal(faPersonal); 
    this->setAddressHeader(faName, address); 
}

// Convenience method to add addresses
inline void MimeBodyPart::addAddressHeader(
    FastString &name, InternetAddressArray &addrs, FastString &group) 
{
    this->m_ihHeaders.addAddressHeader(name, addrs, group); 
}

inline void MimeBodyPart::addAddressHeader(
    const char *name, InternetAddressArray &addrs, const char *group) 
{
    FastString faName(name), faGroup(group); 
    this->m_ihHeaders.addAddressHeader(faName, addrs, faGroup); 
}

inline void MimeBodyPart::addAddressHeader(
    FastString &name, InternetAddress &addr, FastString &group) 
{
    InternetAddressArray addrs(1); 
    addrs[0] = addr; 
    this->addAddressHeader(name, addrs, group); 
}

inline void MimeBodyPart::addAddressHeader(
    const char *name, InternetAddress &addr, const char *group) 
{
    FastString sName(name), sGroup(group); 
    this->addAddressHeader(sName, addr, sGroup); 
}

inline void MimeBodyPart::addAddressHeader(
    const char *name, const char *addr, const char *personal, 
    const char *group) 
{
    FastString faName(name), faGroup(group); 
    FastString faAddr(addr), faPersonal(personal); 
    InternetAddressArray addrs(1); 
    addrs[0].setAddress(faAddr); 
    addrs[0].setPersonal(faPersonal); 
    addrs[0].setGroup(faGroup); 
    this->m_ihHeaders.addAddressHeader(faName, addrs, faGroup); 
}

inline void MimeBodyPart::addAddressHeader(
    FastString &name, InternetAddressArray &addrs) 
{
    this->m_ihHeaders.addAddressHeader(name, addrs); 
}

inline void MimeBodyPart::addAddressHeader(
    const char *name, InternetAddressArray &addrs) 
{
    FastString faName(name); 
    this->addAddressHeader(faName, addrs); 
}

// Convenience method to remove addresses in header.
inline void MimeBodyPart::removeAddress(
    FastString &name, InternetAddressArray &addrs) 
{
    this->m_ihHeaders.removeAddress(name, addrs); 
}

// Convenience method to remove addresses in header.
inline void MimeBodyPart::removeAddress(
    FastString &name, FastStringArray &addrs) 
{
    this->m_ihHeaders.removeAddress(name, addrs); 
}

inline void MimeBodyPart::removeAddress(
    FastString &name, FastString &addr) 
{
    FastStringArray addrs(1); 
    addrs.set(addr, 0); 
    this->m_ihHeaders.removeAddress(name, addrs); 
}

inline void MimeBodyPart::removeAddress(
    const char *name, const char *addr) 
{
    FastString faName(name), faAddr(addr); 
    this->removeAddress(faName, faAddr); 
}

// Convenience method to remove a group addresses in header.
inline void MimeBodyPart::removeAddressGroup(
    FastString &name, FastString &group) 
{
    this->m_ihHeaders.removeAddressGroup(name, group); 
}

inline void MimeBodyPart::removeAddressGroup(
    const char *name, const char *group) 
{
    FastString faName(name), faGroup(group); 
    this->m_ihHeaders.removeAddressGroup(faName, faGroup); 
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
 * Returns null if the header specified by the type is not found
 * or if its value is empty. <p>
 *
 * This implementation uses the <code>getHeader</code> method
 * to obtain the requisite header field.
 *
 * @param type      Type of recepient
 * @param addrs     array of Address objects
 * @return          Address count
 */
inline int MimeBodyPart::getRecipients(
    int type, InternetAddressArray &addrs) 
{
    FastString name; 
    this->getRecipientsType(type, name); 
    return this->getAddressHeader(name, addrs); 
}

/**
 * Set the specified recipient type to the given addresses.
 * If the addresses parameter is <code>null</code>, the corresponding
 * recipient field is removed.
 * 
 * @param type      Recipient type
 * @param addrs     Addresses
 */
inline void MimeBodyPart::setRecipients(
    int type, InternetAddressArray &addrs) 
{
    FastString name; 
    this->getRecipientsType(type, name); 
    this->setAddressHeader(name, addrs); 
}

/**
 * Set the specified recipient type to the given address.
 * If the address parameter is <code>null</code>, the corresponding
 * recipient field is removed.
 * 
 * @param type      Recipient type
 * @param addr      Address
 */
inline void MimeBodyPart::setRecipients(
    int type, InternetAddress &addr) 
{
    FastString name; 
    this->getRecipientsType(type, name); 
    this->setAddressHeader(name, addr); 
}

/**
 * Set the specified recipient type to the given address.
 * If the address parameter is <code>null</code>, the corresponding
 * recipient field is removed.
 * 
 * @param type      Recipient type
 * @param addr      Address string
 * @param personal  Address personal string
 * @param group     Address group name
 */
inline void MimeBodyPart::setRecipients(
    int type, const char *addr, const char *personal, 
    const char *group) 
{
    FastString name; 
    this->getRecipientsType(type, name); 
    this->setAddressHeader(name.c_str(), addr, personal, group); 
}

/**
 * Add the given addresses to the specified recipient type.
 * 
 * @param type      Recipient type
 * @param addrs     Addresses
 * @param group     Address group name
 */
inline void MimeBodyPart::addRecipients(
    int type, InternetAddressArray &addrs, const char *group) 
{
    FastString name; 
    this->getRecipientsType(type, name); 
    this->addAddressHeader(name, addrs, group); 
}

/**
 * Add the given address to the specified recipient type.
 * 
 * @param type      Recipient type
 * @param addr      Address
 * @param group     Address group name
 */
inline void MimeBodyPart::addRecipients(
    int type, InternetAddress &addr, const char *group) 
{
    FastString name; 
    this->getRecipientsType(type, name); 
    this->addAddressHeader(name, addr, group); 
}

/**
 * Add the given address to the specified recipient type.
 * 
 * @param type      Recipient type
 * @param addr      Address string
 * @param personal  Address personal string
 * @param group     Address group name
 */
inline void MimeBodyPart::addRecipients(
    int type, const char *addr, const char *personal, 
    const char *group) 
{
    FastString name; 
    this->getRecipientsType(type, name); 
    this->addAddressHeader(name, addr, personal, group); 
}

/** 
 * Returns the value of the RFC 822 "Sender" header field.
 * If the "Sender" header field is absent, <code>null</code>
 * is returned.<p>
 *
 * This implementation uses the <code>getHeader</code> method
 * to obtain the requisite header field.
 *
 * @param addr  Address object
 * @return      Address count, must be 1.
 */
inline int MimeBodyPart::getSender(InternetAddress &addr) 
{
    InternetAddressArray addrs; 
    int count = this->getAddressHeader("Sender", addrs); 
    if( count < 1 ) return -1; 
    addr = addrs[0]; 
    // add by henh 2012/7/20 16:13:31 for Sender decode
    FastString personal = addr.personal();
    if(personal.size()>0)
    {
    	MimeUtility::decodeText(personal);
    	addr.setPersonal(personal);
    }
    FastString address = addr.address();
    if(address.size()>0)
    {
    	MimeUtility::decodeText(address);
    	addr.setAddress(address);
    }
    // and end
    return 1; 
}

/**
 * Set the RFC 822 "Sender" header field. Any existing values are 
 * replaced with the given address. If address is <code>null</code>,
 * this header is removed.
 *
 * @param address    the sender of this message
 */
inline void MimeBodyPart::setSender(InternetAddress &addr) 
{
    this->setAddressHeader("Sender", addr); 
}

/**
 * Set the RFC 822 "Sender" header field. Any existing values are 
 * replaced with the given address. If address is <code>null</code>,
 * this header is removed.
 *
 * @param address    the sender of this message
 * @param personal    the sender's personal of this message
 */
inline void MimeBodyPart::setSender(const char *addr, const char *personal) 
{
    this->setAddressHeader("Sender", addr, personal); 
}

/** 
 * Returns the value of the RFC 822 "From" header fields. If this 
 * header field is absent, the "Sender" header field is used. 
 * If the "Sender" header field is also absent, <code>null</code>
 * is returned.<p>
 *
 * This implementation uses the <code>getHeader</code> method
 * to obtain the requisite header field.
 *
 * @param addrs  array of Address objects
 * @return      Address count
 */
inline int MimeBodyPart::getFrom(InternetAddressArray &addrs) 
{
    int count = this->getAddressHeader("From", addrs); 
    if( count < 1 ) 
        return this->getAddressHeader("Sender", addrs); 
    else
        return count; 
}

inline int MimeBodyPart::getFrom(InternetAddress &addr) 
{
    InternetAddressArray addrs; 
    int count = this->getFrom(addrs); 
    if( count > 0 ) 
    {
        addr = addrs[0]; 
        return count; 
    }
    return count; 
}


/**
 * Set the given addresses to the specified recipient From.
 *
 * @param addrs      email addresses
 */
inline void MimeBodyPart::setFrom(InternetAddressArray &addrs) 
{
    this->setAddressHeader("From", addrs); 
}

/**
 * Set the given address to the specified recipient From.
 *
 * @param addr      email address
 */
inline void MimeBodyPart::setFrom(InternetAddress &addr) 
{
    this->setAddressHeader("From", addr); 
}

/**
 * Set the given address to the specified recipient From.
 *
 * @param addr      email address string
 * @param personal  address personal string
 * @param group     address group name
 */
inline void MimeBodyPart::setFrom(
    const char *addr, const char *personal, const char *group) 
{
    this->setAddressHeader("From", addr, personal, group); 
}

/**
 * Add the given addresses to the specified recipient From.
 *
 * @param addrs     email addresses
 * @param group     address group name
 */
inline void MimeBodyPart::addFrom(
    InternetAddressArray &addrs, const char *group)
{
    this->addAddressHeader("From", addrs, group); 
}

/**
 * Add the given address to the specified recipient From.
 *
 * @param addr      email address
 * @param group     address group name
 */
inline void MimeBodyPart::addFrom(
    InternetAddress &addr, const char *group) 
{
    this->addAddressHeader("From", addr, group); 
}

/**
 * Add the given address to the specified recipient From.
 *
 * @param addr      email address string
 * @param personal  address personal string
 * @param group     address group name
 */
inline void MimeBodyPart::addFrom(
    const char *addr, const char *personal, const char *group) 
{
    this->addAddressHeader("From", addr, personal, group); 
}

/** 
 * Get the value of the RFC 822 "To" header fields. 
 * 
 * @param addrs      email addresses
 */
inline int MimeBodyPart::getTo(InternetAddressArray &addrs) 
{
    return this->getRecipients(MimeBodyPart::TO, addrs); 
}

/**
 * Set the given addresses to the specified recipient To.
 *
 * @param addrs      email addresses
 */
inline void MimeBodyPart::setTo(InternetAddressArray &addrs) 
{
    this->setRecipients(MimeBodyPart::TO, addrs); 
}

/**
 * Set the given address to the specified recipient To.
 *
 * @param addr      email address
 */
inline void MimeBodyPart::setTo(InternetAddress &addr) 
{
    this->setRecipients(MimeBodyPart::TO, addr); 
}

/**
 * Set the given address to the specified recipient To.
 *
 * @param addr      email address string
 * @param personal  address personal string
 * @param group     address group name
 */
inline void MimeBodyPart::setTo(
    const char *addr, const char *personal, const char *group) 
{
    this->setRecipients(MimeBodyPart::TO, addr, personal, group); 
}

/**
 * Add the given addresses to the specified recipient To.
 *
 * @param addrs     email addresses
 * @param group     address group name
 */
inline void MimeBodyPart::addTo(
    InternetAddressArray &addrs, const char *group) 
{
    this->addRecipients(MimeBodyPart::TO, addrs, group); 
}

/**
 * Add the given address to the specified recipient To.
 *
 * @param addr      email address
 * @param group     address group name
 */
inline void MimeBodyPart::addTo(
    InternetAddress &addr, const char *group) 
{
    this->addRecipients(MimeBodyPart::TO, addr, group); 
}

/**
 * Add the given address to the specified recipient To.
 *
 * @param addr      email address string
 * @param personal  address personal string
 * @param group     address group name
 */
inline void MimeBodyPart::addTo(
    const char *addr, const char *personal, const char *group) 
{
    this->addRecipients(MimeBodyPart::TO, addr, personal, group); 
}

/** 
 * Get the value of the RFC 822 "Cc" header fields. 
 *
 * @param addrs      email addresses
 */
inline int MimeBodyPart::getCc(InternetAddressArray &addrs) 
{
    return this->getRecipients(MimeBodyPart::CC, addrs); 
}

/**
 * Set the given addresses to the specified recipient Cc.
 *
 * @param addrs      email addresses
 */
inline void MimeBodyPart::setCc(InternetAddressArray &addrs) 
{
    this->setRecipients(MimeBodyPart::CC, addrs); 
}

/**
 * Set the given address to the specified recipient Cc.
 *
 * @param addr      email address
 */
inline void MimeBodyPart::setCc(InternetAddress &addr) 
{
    this->setRecipients(MimeBodyPart::CC, addr); 
}

/**
 * Set the given address to the specified recipient Cc.
 *
 * @param addr      email address string
 * @param personal  address personal string
 * @param group     address group name
 */
inline void MimeBodyPart::setCc(
    const char *addr, const char *personal, const char *group) 
{
    this->setRecipients(MimeBodyPart::CC, addr, personal, group); 
}

/**
 * Add the given addresses to the specified recipient Cc.
 *
 * @param addrs     email addresses
 * @param group     address group name
 */
inline void MimeBodyPart::addCc(
    InternetAddressArray &addrs, const char *group) 
{
    this->addRecipients(MimeBodyPart::CC, addrs, group); 
}

/**
 * Add the given address to the specified recipient Cc.
 *
 * @param addr      email address
 * @param group     address group name
 */
inline void MimeBodyPart::addCc(
    InternetAddress &addr, const char *group) 
{
    this->addRecipients(MimeBodyPart::CC, addr, group); 
}

/**
 * Add the given address to the specified recipient Cc.
 *
 * @param addr      email address string
 * @param personal  address personal string
 * @param group     address group name
 */
inline void MimeBodyPart::addCc(
    const char *addr, const char *personal, const char *group) 
{
    this->addRecipients(MimeBodyPart::CC, addr, personal, group); 
}

/** 
 * Get the value of the RFC 822 "Bcc" header fields. 
 *
 * @param addrs      email addresses
 */
inline int MimeBodyPart::getBcc(InternetAddressArray &addrs) 
{
    return this->getRecipients(MimeBodyPart::BCC, addrs); 
}

/**
 * Set the given addresses to the specified recipient Bcc.
 *
 * @param addrs      email addresses
 */
inline void MimeBodyPart::setBcc(InternetAddressArray &addrs) 
{
    this->setRecipients(MimeBodyPart::BCC, addrs); 
}

/**
 * Set the given address to the specified recipient Bcc.
 *
 * @param addr      email address
 */
inline void MimeBodyPart::setBcc(InternetAddress &addr) 
{
    this->setRecipients(MimeBodyPart::BCC, addr); 
}

/**
 * Set the given address to the specified recipient Bcc.
 *
 * @param addr      email address string
 * @param personal  address personal string
 * @param group     address group name
 */
inline void MimeBodyPart::setBcc(
    const char *addr, const char *personal, const char *group) 
{
    this->setRecipients(MimeBodyPart::BCC, addr, personal, group); 
}

/**
 * Add the given addresses to the specified recipient Bcc.
 *
 * @param addrs     email addresses
 * @param group     address group name
 */
inline void MimeBodyPart::addBcc(
    InternetAddressArray &addrs, const char *group) 
{
    this->addRecipients(MimeBodyPart::BCC, addrs, group); 
}

/**
 * Add the given address to the specified recipient Bcc.
 *
 * @param addr      email address
 * @param group     address group name
 */
inline void MimeBodyPart::addBcc(
    InternetAddress &addr, const char *group) 
{
    this->addRecipients(MimeBodyPart::BCC, addr, group); 
}

/**
 * Add the given address to the specified recipient Bcc.
 *
 * @param addr      email address string
 * @param personal  address personal string
 * @param group     address group name
 */
inline void MimeBodyPart::addBcc(
    const char *addr, const char *personal, const char *group) 
{
    this->addRecipients(MimeBodyPart::BCC, addr, personal, group); 
}

/**
 * Returns the value of the "Content-ID" header field. Returns
 * <code>null</code> if the field is unavailable or its value is 
 * absent. <p>
 *
 * This implementation uses the <code>getHeader</code> method
 * to obtain the requisite header field.
 *
 * @param s  content-ID
 */
inline void MimeBodyPart::getContentID(FastString &s) 
{
    this->getHeader("Content-ID", s);
    if( !s.empty() ) 
    {
        ContentID cid(s); 
        cid.getContentID(s); 
    }
}

/**
 * Set the "Content-ID" header field of this Message.
 * If the <code>cid</code> parameter is null, any existing 
 * "Content-ID" is removed.
 *
 * @param s  content-ID
 */
inline void MimeBodyPart::setContentID(FastString &s) 
{
    if( !s.empty() ) 
    {
        ContentID cid(s); 
        cid.toString(s); 
    }
    if( !s.empty() ) 
        this->setHeader("Content-ID", s);
    else
        this->removeHeader("Content-ID"); 
}

/**
 * Return the value of the "Content-MD5" header field. Returns 
 * <code>null</code> if this field is unavailable or its value
 * is absent. <p>
 *
 * This implementation uses the <code>getHeader</code> method
 * to obtain the requisite header field.
 *
 * @param s   content-MD5
 */
inline void MimeBodyPart::getContentMD5(FastString &s) 
{
    this->getHeader("Content-MD5", s);
}

/**
 * Set the "Content-MD5" header field of this Message.
 *
 * @param s   content-MD5
 */
inline void MimeBodyPart::setContentMD5(FastString &s) 
{
    this->setHeader("Content-MD5", s);
}

/**
 * Returns the value of the "Message-ID" header field. Returns
 * null if this field is unavailable or its value is absent. <p>
 *
 * The default implementation provided here uses the
 * <code>getHeader</code> method to return the value of the
 * "Message-ID" field.
 *
 * @param s    Message-ID
 */
inline void MimeBodyPart::getMessageID(FastString &s) 
{
    this->getHeader("Message-ID", s);
}

/**
 * Returns the value of the "Subject" header field. Returns null 
 * if the subject field is unavailable or its value is absent. <p>
 *
 * If the subject is encoded as per RFC 2047, it is decoded and
 * converted into Unicode. If the decoding or conversion fails, the
 * raw data is returned as is. <p>
 *
 * This implementation uses the <code>getHeader</code> method
 * to obtain the requisite header field.
 *
 * @param s     Subject
 */
inline void MimeBodyPart::getSubject(FastString &s) 
{
    this->getHeader("Subject", s); 
    MimeUtility::decodeText(s); 
}

inline void MimeBodyPart::getSubject(FastString &s, FastString &charset)
{
    this->getHeader("Subject", s);
    FastString rmword;
    if(MimeUtility::emptyOfSubject(s, rmword))
    {
    	MimeUtility::getCharset(s,charset);
    	MimeUtility::decodeText(s);
    	s.replace(rmword,FastString(" "));
    	s.removeChar(' ');
    }
    else
    {
    	MimeUtility::getCharset(s,charset);
    	MimeUtility::decodeText(s); 
    }
}

/**
 * Set the "Subject" header field. If the subject contains non 
 * US-ASCII characters, it will be encoded using the specified
 * charset. If the subject contains only US-ASCII characters, no 
 * encoding is done and it is used as-is. If the subject is null, 
 * the existing "Subject" header field is removed. <p>
 *
 * The application must ensure that the subject does not contain
 * any line breaks. <p>
 *
 * Note that if the charset encoding process fails, a
 * MessagingException is thrown, and an UnsupportedEncodingException
 * is included in the chain of nested exceptions within the
 * MessagingException.
 *
 * @param   s           The subject
 * @param   charset     The charset 
 */
inline void MimeBodyPart::setSubject(FastString &s, const char *charset) 
{
    FastString subject; 
    MimeUtility::encodeWord(s, subject, TRUE, charset); 
    this->setHeader("Subject", subject); 
}

/**
 * Set the "Subject" header field. 
 *
 * @param   s           The subject
 * @param   charset     The charset 
 */
inline void MimeBodyPart::setSubject(const char *s, const char *charset) 
{
    FastString subject(s); 
    this->setSubject(subject, charset); 
}

/**
 * Set the mime type of this bodypart's content.
 *
 * @param s  content mime type
 */
inline void MimeBodyPart::setMimeType(int mimetype)
{
    if( mimetype >= MimeBodyPart::UNKNOWN_MIMETYPE && mimetype < MimeBodyPart::MAX_MIMETYPE ) 
        m_nMimeType = mimetype; 
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
 * @param mimeType  content description
 */
inline BOOL MimeBodyPart::isMimeType(FastString &mimeType)
{
    return MimeBodyPart::isMimeType(*this, mimeType); 
}

/**
 * Is this Part of the specified MIME type?  This method
 * compares <strong>only the <code>primaryType</code> and 
 * <code>subType</code></strong>.
 *
 * @param mimeType  content description
 */
inline BOOL MimeBodyPart::isMimeType(const char *mimeType)
{
    FastString sMimeType(mimeType); 
    return MimeBodyPart::isMimeType(*this, sMimeType); 
}

/**
 * Current bodypart is text/ Content-Type.
 */
inline BOOL MimeBodyPart::isText() 
{
    switch( getMimeType() ) 
    {
    case MimeBodyPart::TEXT:
    case MimeBodyPart::TEXT_PLAIN:
    case MimeBodyPart::TEXT_HTML:
        return TRUE; 
    }
    return FALSE; 
}

/**
 * Current bodypart is text/ Content-Type 
 * with specified subType.
 */
inline BOOL MimeBodyPart::isText(FastString &subType) 
{
    FastString mimeType("text/"); 
    mimeType.append(subType); 
    return isMimeType(mimeType); 
}

/**
 * Current bodypart is text/ Content-Type 
 * with specified subType.
 */
inline BOOL MimeBodyPart::isText(const char *subType) 
{
    FastString mimeType("text/"); 
    mimeType.append(subType); 
    return isMimeType(mimeType); 
}

/**
 * Current bodypart is text/plain Content-Type.
 */
inline BOOL MimeBodyPart::isTextPlain() 
{
    return getMimeType() == MimeBodyPart::TEXT_PLAIN ? TRUE : FALSE; 
}

/**
 * Current bodypart is text/html Content-Type.
 */
inline BOOL MimeBodyPart::isTextHtml() 
{
    return getMimeType() == MimeBodyPart::TEXT_HTML ? TRUE : FALSE; 
}

/**
 * Current bodypart is text/enriched Content-Type.
 */
inline BOOL MimeBodyPart::isTextEnriched() 
{
    return getMimeType() == MimeBodyPart::TEXT_ENRICHED ? TRUE : FALSE; 
}

/**
 * Current bodypart is text/unrecognized Content-Type.
 */
inline BOOL MimeBodyPart::isTextUnrecognized() 
{
    return getMimeType() == MimeBodyPart::TEXT_UNRECOGNIZED ? TRUE : FALSE; 
}

/**
 * Current bodypart is multipart/ Content-Type.
 */
inline BOOL MimeBodyPart::isMultipart() 
{
    switch( getMimeType() ) 
    {
    case MimeBodyPart::MULTIPART:
    case MimeBodyPart::MULTIPART_MIXED:
    case MimeBodyPart::MULTIPART_RELATED:
    case MimeBodyPart::MULTIPART_ALTERNATIVE:
        return TRUE; 
    }
    return FALSE; 
}

/**
 * Current bodypart is multipart/ Content-Type 
 * with specified subType.
 */
inline BOOL MimeBodyPart::isMultipart(FastString &subType) 
{
    FastString mimeType("multipart/"); 
    mimeType.append(subType); 
    return isMimeType(mimeType); 
}

/**
 * Current bodypart is multipart/ Content-Type 
 * with specified subType.
 */
inline BOOL MimeBodyPart::isMultipart(const char *subType) 
{
    FastString mimeType("multipart/"); 
    mimeType.append(subType); 
    return isMimeType(mimeType); 
}

/**
 * Current bodypart is multipart/mixed Content-Type.
 */
inline BOOL MimeBodyPart::isMultipartMixed() 
{
    return getMimeType() == MimeBodyPart::MULTIPART_MIXED ? TRUE : FALSE; 
}

/**
 * Current bodypart is multipart/related Content-Type.
 */
inline BOOL MimeBodyPart::isMultipartRelated() 
{
    return getMimeType() == MimeBodyPart::MULTIPART_RELATED ? TRUE : FALSE; 
}

/**
 * Current bodypart is multipart/related Content-Type.
 */
inline BOOL MimeBodyPart::isMultipartAlternative() 
{
    return getMimeType() == MimeBodyPart::MULTIPART_ALTERNATIVE ? TRUE : FALSE; 
}

/**
 * Current bodypart is message/ Content-Type.
 */
inline BOOL MimeBodyPart::isMessage() 
{
    switch( getMimeType() ) 
    {
    case MimeBodyPart::MESSAGE:
    case MimeBodyPart::MESSAGE_RFC822:
  	// add by henh 2012/7/20 16:13:31 for message types
  	case MimeBodyPart::MESSAGE_DELSTATUS: 
  	// add end
        return TRUE; 
    }
    return FALSE; 
}

/**
 * Current bodypart is message/ Content-Type 
 * with specified subType.
 */
inline BOOL MimeBodyPart::isMessage(FastString &subType) 
{
    FastString mimeType("message/"); 
    mimeType.append(subType); 
    return isMimeType(mimeType); 
}

/**
 * Current bodypart is message/ Content-Type 
 * with specified subType.
 */
inline BOOL MimeBodyPart::isMessage(const char *subType) 
{
    FastString mimeType("message/"); 
    mimeType.append(subType); 
    return isMimeType(mimeType); 
}

/**
 * Current bodypart is message/rfc822 Content-Type.
 */
inline BOOL MimeBodyPart::isMessageRFC822() 
{
    return getMimeType() == MimeBodyPart::MESSAGE_RFC822 ? TRUE : FALSE; 
}

/**
 * Current bodypart is message/delivery-status Content-Type.
 */
inline BOOL MimeBodyPart::isMessageDELStatus()
{
    return getMimeType() == MimeBodyPart::MESSAGE_DELSTATUS ? TRUE : FALSE; 
}

/**
 * Current bodypart is image/ Content-Type.
 */
inline BOOL MimeBodyPart::isImage() 
{
    switch( getMimeType() ) 
    {
    case MimeBodyPart::IMAGE:
        return TRUE; 
    }
    return FALSE; 
}

/**
 * Current bodypart is image/ Content-Type 
 * with specified subType.
 */
inline BOOL MimeBodyPart::isImage(FastString &subType) 
{
    FastString mimeType("image/"); 
    mimeType.append(subType); 
    return isMimeType(mimeType); 
}

/**
 * Current bodypart is message/ Content-Type 
 * with specified subType.
 */
inline BOOL MimeBodyPart::isImage(const char *subType) 
{
    FastString mimeType("image/"); 
    mimeType.append(subType); 
    return isMimeType(mimeType); 
}

/**
 * Current bodypart is image/jpeg Content-Type.
 */
inline BOOL MimeBodyPart::isImageJpeg() 
{
    return getMimeType() == MimeBodyPart::IMAGE_JPEG ? TRUE : FALSE; 
}

/**
 * Current bodypart is image/gif Content-Type.
 */
inline BOOL MimeBodyPart::isImageGif() 
{
    return getMimeType() == MimeBodyPart::IMAGE_GIF ? TRUE : FALSE; 
}

/**
 * Current bodypart is image/png Content-Type.
 */
inline BOOL MimeBodyPart::isImagePng() 
{
    return getMimeType() == MimeBodyPart::IMAGE_PNG ? TRUE : FALSE; 
}

/**
 * Current bodypart is audio/ Content-Type.
 */
inline BOOL MimeBodyPart::isAudio() 
{
    switch( getMimeType() ) 
    {
    case MimeBodyPart::AUDIO:
        return TRUE; 
    }
    return FALSE; 
}

/**
 * Current bodypart is audio/ Content-Type 
 * with specified subType.
 */
inline BOOL MimeBodyPart::isAudio(FastString &subType) 
{
    FastString mimeType("audio/"); 
    mimeType.append(subType); 
    return isMimeType(mimeType); 
}

/**
 * Current bodypart is audio/ Content-Type 
 * with specified subType.
 */
inline BOOL MimeBodyPart::isAudio(const char *subType) 
{
    FastString mimeType("audio/"); 
    mimeType.append(subType); 
    return isMimeType(mimeType); 
}

/**
 * Current bodypart is audio/basic Content-Type.
 */
inline BOOL MimeBodyPart::isAudioBasic() 
{
    return getMimeType() == MimeBodyPart::IMAGE_PNG ? TRUE : FALSE; 
}

/**
 * Current bodypart is audio/mp3 Content-Type.
 */
inline BOOL MimeBodyPart::isAudioMp3() 
{
    return getMimeType() == MimeBodyPart::AUDIO_MP3 ? TRUE : FALSE; 
}

/**
 * Current bodypart is video/ Content-Type.
 */
inline BOOL MimeBodyPart::isVideo() 
{
    switch( getMimeType() ) 
    {
    case MimeBodyPart::VIDEO:
        return TRUE; 
    }
    return FALSE; 
}

/**
 * Current bodypart is video/ Content-Type 
 * with specified subType.
 */
inline BOOL MimeBodyPart::isVideo(FastString &subType) 
{
    FastString mimeType("video/"); 
    mimeType.append(subType); 
    return isMimeType(mimeType); 
}

/**
 * Current bodypart is video/ Content-Type 
 * with specified subType.
 */
inline BOOL MimeBodyPart::isVideo(const char *subType) 
{
    FastString mimeType("video/"); 
    mimeType.append(subType); 
    return isMimeType(mimeType); 
}

/**
 * Current bodypart is video/mpeg Content-Type.
 */
inline BOOL MimeBodyPart::isVideoMpeg() 
{
    return getMimeType() == MimeBodyPart::VIDEO_MPEG ? TRUE : FALSE; 
}

/**
 * Current bodypart is application/ Content-Type.
 */
inline BOOL MimeBodyPart::isApplication() 
{
    switch( getMimeType() ) 
    {
    case MimeBodyPart::APPLICATION:
        return TRUE; 
    }
    return FALSE; 
}

/**
 * Current bodypart is application/ Content-Type 
 * with specified subType.
 */
inline BOOL MimeBodyPart::isApplication(FastString &subType) 
{
    FastString mimeType("application/"); 
    mimeType.append(subType); 
    return isMimeType(mimeType); 
}

/**
 * Current bodypart is application/ Content-Type 
 * with specified subType.
 */
inline BOOL MimeBodyPart::isApplication(const char *subType) 
{
    FastString mimeType("application/"); 
    mimeType.append(subType); 
    return isMimeType(mimeType); 
}

/**
 * Current bodypart is application/octet-stream Content-Type.
 */
inline BOOL MimeBodyPart::isApplicationOctetStream() 
{
    return getMimeType() == MimeBodyPart::APPLICATION_OCTET_STREAM ? TRUE : FALSE; 
}

/**
 * Current bodypart is application/PostScript Content-Type.
 */
inline BOOL MimeBodyPart::isApplicationPostScript() 
{
    return getMimeType() == MimeBodyPart::APPLICATION_POSTSCRIPT ? TRUE : FALSE; 
}

/**
 * Returns the value of the RFC 822 "Content-Type" header field. 
 * This represents the content-type of the content of this 
 * message. This value must not be empty. If this field is 
 * unavailable, "text/plain" should be returned. <p>
 *
 * This implementation uses the <code>getHeader</code> method
 * to obtain the requisite header field.
 *
 * @param s     The ContentType of this part
 */
inline void MimeBodyPart::getContentType(FastString &s)
{
    getHeader("Content-Type", s);
    if( s.empty() ) s = "text/plain";
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
inline void MimeBodyPart::setContentType(const char *s)
{
    FastString type(s); 
    setContentType(type); 
}

/**
 * Returns the "Content-Description" header field of this body part.
 * This typically associates some descriptive information with 
 * this part. Returns null if this field is unavailable or its
 * value is absent. <p>
 *
 * If the Content-Description field is encoded as per RFC 2047,
 * it is decoded and converted into Unicode. If the decoding or 
 * conversion fails, the raw data is returned as is. <p>
 *
 * This implementation uses <code>getHeader(name)</code>
 * to obtain the requisite header field.
 * 
 * @param s     content description
 */
inline void MimeBodyPart::getDescription(FastString &s) 
{
    MimeBodyPart::getDescription(*this, s); 
}

/**
 * Set the "Content-Description" header field for this body part.
 * If the description parameter is <code>null</code>, then any 
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
 * @param s     content description
 * @param charset  Charset of content in mime body
 */
inline void MimeBodyPart::setDescription(FastString &s, const char *charset) 
{
    MimeBodyPart::setDescription(*this, s, charset); 
}

/**
 * Set the "Content-Description" header field for this body part.
 * If the description parameter is <code>null</code>, then any 
 * existing "Content-Description" fields are removed. <p>
 * 
 * @param s     content description
 * @param charset  Charset of content in mime body
 */
inline void MimeBodyPart::setDescription(const char *s, const char *charset) 
{
    FastString des(s); 
    setDescription(des, charset); 
}

/**
 * Returns the value of the "Content-Disposition" header field.
 * This represents the disposition of this part. The disposition
 * describes how the part should be presented to the user. <p>
 *
 * If the Content-Disposition field is unavailable,
 * empty is returned. <p>
 *
 * This implementation uses <code>getHeader(name)</code>
 * to obtain the requisite header field.
 *
 * @param s     content Disposition
 * @see #headers
 */
inline void MimeBodyPart::getDisposition(FastString &s) 
{
    MimeBodyPart::getDisposition(*this, s); 
}

/**
 * Set the "Content-Disposition" header field of this body part.
 * If the disposition is null, any existing "Content-Disposition"
 * header field is removed.
 *
 * @param s     content Disposition
 * @see #headers
 */
inline void MimeBodyPart::setDisposition(FastString &s) 
{
    MimeBodyPart::setDisposition(*this, s); 
}

/**
 * Set the "Content-Disposition" header field of this body part.
 * If the disposition is null, any existing "Content-Disposition"
 * header field is removed.
 *
 * @param s     content Disposition
 * @see #headers
 */
inline void MimeBodyPart::setDisposition(const char *s) 
{
    FastString disp(s); 
    setDisposition(disp); 
}

/**
 * Get the filename associated with this body part. <p>
 *
 * Returns the value of the "filename" parameter from the
 * "Content-Disposition" header field of this body part. If its
 * not available, returns the value of the "name" parameter from
 * the "Content-Type" header field of this body part.
 * Returns <code>empty</code> if both are absent.
 *
 * @param s     filename
 */
inline void MimeBodyPart::getFileName(FastString &s) 
{
    MimeBodyPart::getFileName(*this, s); 
}

inline void MimeBodyPart::getFileName(FastString &s, FastString &charset) 
{
    MimeBodyPart::getFileName(*this, s, charset); 
}

/**
 * Set the filename associated with this body part, if possible. <p>
 *
 * Sets the "filename" parameter of the "Content-Disposition"
 * header field of this body part.
 *
 * @param s     filename
 */
inline void MimeBodyPart::setFileName(FastString &s) 
{
    MimeBodyPart::setFileName(*this, s); 
}

/**
 * Set the filename associated with this body part, if possible. <p>
 *
 * Sets the "filename" parameter of the "Content-Disposition"
 * header field of this body part.
 *
 * @param s     filename
 */
inline void MimeBodyPart::setFileName(const char *s) 
{
    FastString filename(s); 
    setFileName(filename); 
}

// modify by ljh begin
inline void MimeBodyPart::setFileName(FastString &s,const char *charset)
{
    MimeBodyPart::setFileName(*this, s, charset); 
}
// modify by ljh begin

/**
 * Get the content transfer encoding from the
 * "Content-Transfer-Encoding" header field of this body part. 
 *
 * @param s     encoding type
 */
inline void MimeBodyPart::getEncoding(FastString &s) 
{
    MimeBodyPart::getEncoding(*this, s); 
}

/**
 * Set the content transfer encoding from the
 * "Content-Transfer-Encoding" header field. 
 *
 * This implementation uses <code>setHeader(name,value)</code>
 * to obtain the requisite header field.
 *
 * @param part  point to mime part
 * @param s     encoding type
 */
inline void MimeBodyPart::setEncoding(IMimePart &part, FastString &s) 
{
    FastString name("Content-Transfer-Encoding"); 
    part.setHeader(name, s); 
}

/**
 * Set the content transfer encoding from the
 * "Content-Transfer-Encoding" header field of this body part. 
 *
 * @param s     encoding type
 */
inline void MimeBodyPart::setEncoding(FastString &s) 
{
    MimeBodyPart::setEncoding(*this, s); 
}

/**
 * Set the content transfer encoding from the
 * "Content-Transfer-Encoding" header field of this body part. 
 *
 * @param s     encoding type
 */
inline void MimeBodyPart::setEncoding(const char *s) 
{
    FastString encoding(s); 
    setEncoding(encoding); 
}

/**
 * Get the languages specified in the Content-Language header
 * of this MimePart. The Content-Language header is defined by
 * RFC 1766. Returns <code>null</code> if this header is not
 * available or its value is absent. <p>
 *
 * This implementation uses <code>getHeader(name)</code>
 * to obtain the requisite header field.
 */
inline void MimeBodyPart::getContentLanguage(FastStringArray &languages) 
{
    MimeBodyPart::getContentLanguage(*this, languages); 
}

/**
 * Set the Content-Language header of this MimePart. The
 * Content-Language header is defined by RFC 1766.
 *
 * @param languages array of language tags
 */
inline void MimeBodyPart::setContentLanguage(FastStringArray &languages)
{
    MimeBodyPart::setContentLanguage(*this, languages); 
}

/**
 * Set the Content-Language header of this MimePart. The
 * Content-Language header is defined by RFC 1766.
 *
 * @param languages language tags
 */
inline void MimeBodyPart::setContentLanguage(const char *language)
{
    FastStringArray languages(1); 
    languages.set(FastString(language), 0); 
    MimeBodyPart::setContentLanguage(*this, languages); 
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
inline void MimeBodyPart::updateHeaders() 
{
    MimeBodyPart::updateHeaders(*this); 
}

/**
 * Return the content as a String. The type of this
 * object is the native format of a "text/plain" content.
 * This function will copy the content buffer to s.
 *
 */
inline void MimeBodyPart::getBodyLines(FastString &s) 
{
    s.set(m_psBodyBuffer, m_nBodyBufferSize); 
}

/**
 * Return the text/ content as a String pointer. The type of this
 * object is the native format of a "text/" content.
 *
 */
inline void MimeBodyPart::getText(FastString &s) 
{
    parsebody(); s.clear(); 
    if( isText() && m_psContent ) 
        s.set(m_psContent->c_str(), m_psContent->length()); 
}

/**
 * Return the content as a String pointer. The type of this
 * object is the native format of a "text/plain" content or 
 * other attachment data.
 *
 */
inline FastString* MimeBodyPart::getContent() 
{
    parsebody(); 
    return m_psContent; 
}

/**
 * Return the content as a String pointer. The type of this
 * object is the native format of a "text/plain" content or 
 * other attachment data.
 *
 */
inline void MimeBodyPart::getContent(FastString &s) 
{
    parsebody(); s.clear(); 
    if( m_psContent ) 
        s.set(m_psContent->c_str(), m_psContent->length()); 
}

/**
 * Return the multipart content as a MimeMultipart object pointer. 
 * The type of this object is the native format for a "multipart"
 * message is always a Multipart subclass. <p>
 *
 */
inline MimeMultipart* MimeBodyPart::getMultipart() 
{
    parsebody(); 
    return (MimeMultipart*)m_pMultipart; 
}

/**
 * Return the multipart content as a String
 *
 */
inline void MimeBodyPart::getMultipartLines(FastString &s) 
{
    parsebody(); 
    if( isMultipart() ) 
        getContentLines(s); 
    else
        s.clear(); 
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
inline void MimeBodyPart::setContent(FastString &s, const char *type) 
{
    FastString sType(type); 
    setContent(s, sType); 
}

/**
 * This method sets the body part's content to a Multipart object.
 *
 * @param mp    The multipart object that is the Message's content
 */
inline void MimeBodyPart::setContent(MimeMultipart &mp) 
{
    setContent((IMultipart *)&mp); 
}

/**
 * This method sets the body part's content to a Multipart object.
 *
 * @param mp    The multipart object that is the Message's content
 */
inline void MimeBodyPart::setMultipart(IMultipart *mp) 
{
    setContent(mp); 
}

/**
 * This method sets the body part's content to a Multipart object.
 *
 * @param mp    The multipart object that is the Message's content
 */
inline void MimeBodyPart::setMultipart(MimeMultipart &mp) 
{
    setContent((IMultipart *)&mp); 
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
inline void MimeBodyPart::setText(FastString &s, FastString &charset, const char *stype)
{
    FastString sType(stype); 
    setText(s, charset, sType); 
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
inline void MimeBodyPart::setText(FastString &s, const char *charset, const char *stype)
{
    FastString sCharset(charset), sType(stype); 
    setText(s, sCharset, sType); 
}

/**
 * Convenience method that sets the given String as this part's
 * content, with a MIME type of "text/plain" and the specified
 * charset. The given Unicode string will be charset-encoded
 * using the specified charset. The charset is also used to set
 * the "charset" parameter.
 */
inline void MimeBodyPart::setText(FastString &s, FastString &charset)
{
    setText(s, charset, NULL); 
}

/**
 * Convenience method that sets the given String as this part's
 * content, with a MIME type of "text/plain" and the specified
 * charset. The given Unicode string will be charset-encoded
 * using the specified charset. The charset is also used to set
 * the "charset" parameter.
 */
inline void MimeBodyPart::setText(FastString &s, const char *charset)
{
    FastString sCharset(charset); 
    setText(s, sCharset, NULL); 
}

/**
 * Convert this multipart into a RFC 822 / RFC 2047 encoded string.
 * The resulting string contains only US-ASCII characters, and
 * hence is mail-safe.
 *
 * @param s     possibly encoded mime string
 */
inline void MimeBodyPart::toString(FastString &s) 
{
    // safely check if s is current contentbuffer
    if( s.c_str() >= m_psContentBuffer && 
        s.c_str() <= m_psContentBuffer + m_nContentBufferSize ) 
        return; 

    s.clear(); 
    appendTo(s); 
}

/**
 * Dump the object's state.
 */
inline void MimeBodyPart::dump() 
{
    FAST_TRACE_BEGIN("MimeBodyPart::dump()"); 
    FAST_TRACE("sizeof(MimeBodyPart) = %d", sizeof(MimeBodyPart)); 
    FAST_TRACE("this -> 0x%08X", this); 
    FAST_TRACE("m_pParent -> 0x%08X", m_pParent); 
    FAST_TRACE("m_psContentBuffer -> 0x%08X", m_psContentBuffer); 
    FAST_TRACE("m_nContentBufferSize = %d", m_nContentBufferSize); 
    FAST_TRACE("m_psHeaderBuffer -> 0x%08X", m_psHeaderBuffer); 
    FAST_TRACE("m_nHeaderBufferSize = %d", m_nHeaderBufferSize); 
    FAST_TRACE("m_psBodyBuffer -> 0x%08X", m_psBodyBuffer); 
    FAST_TRACE("m_nBodyBufferSize = %d", m_nBodyBufferSize); 
    FAST_TRACE("m_pMultipart -> 0x%08X", m_pMultipart); 
    FAST_TRACE("m_psContent -> 0x%08X size() = %d", m_psContent, m_psContent ? m_psContent->size() : 0); 
    FAST_TRACE("m_nMimeType = %d", m_nMimeType); 
    FAST_TRACE("m_bHeaderParsed = %d", m_bHeaderParsed); 
    FAST_TRACE("m_bBodyParsed = %d", m_bBodyParsed); 
    FAST_TRACE("m_bStrict = %d", m_bStrict); 
    FAST_TRACE("m_bSetDefaultTextCharset = %d", m_bSetDefaultTextCharset); 
    FAST_TRACE_END("MimeBodyPart::dump()"); 
}


_FASTMIME_END_NAMESPACE

#endif
