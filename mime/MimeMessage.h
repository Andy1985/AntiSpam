//=============================================================================
/**
 *  @file    MimeMessage.h
 *
 *  ver 1.0.0 2004/06/16 Naven Exp, for Mime Message Parse Engine.
 *
 *  @author Naven 2004/06/16 created.
 */
//=============================================================================

#ifndef _MIMEMESSAGE_H
#define _MIMEMESSAGE_H

#if defined(_WIN32) || defined(__WIN32__)

#if !defined (NAVEN_PRAGMA_ONCE)
# pragma once
#endif /* NAVEN_PRAGMA_ONCE */

#endif /* _WIN32 */


#include "MimeUtility.h"
#include "MimeObject.h"
#include "MimeEntity.h"
#include "MimeContainer.h"
#include "MimeActivation.h"


_FASTMIME_BEGIN_NAMESPACE


//=============class MimeMessage define==============

/**
 * These classes represents a MIME style email message. 
 * It implements the Message abstract class and the MimePart interface. 
 * 
 * Clients wanting to create new MIME style messages will instantiate 
 * an empty MimeMessage object and then fill it with appropriate 
 * attributes and content. 
 *
 * Service providers that implement MIME compliant backend stores 
 * may want to subclass MimeMessage and override certain methods to 
 * provide specific implementations. The simplest case is probably 
 * a provider that generates a MIME style input stream and leaves 
 * the parsing of the stream to this class. 
 *
 * MimeMessage uses the InternetHeaders class to parse and store 
 * the top level RFC 822 headers of a message. 
 *
 * A note on RFC 822 and MIME headers
 * RFC 822 header fields must contain only US-ASCII characters. 
 * MIME allows non ASCII characters to be present in certain portions 
 * of certain headers, by encoding those characters. RFC 2047 specifies 
 * the rules for doing this. The MimeUtility class provided in this 
 * package can be used to to achieve this. Callers of the setHeader, 
 * addHeader, and addHeaderLine methods are responsible for enforcing 
 * the MIME requirements for the specified headers. In addition, 
 * these header fields must be folded (wrapped) before being sent 
 * if they exceed the line length limitation for the transport 
 * (1000 bytes for SMTP). Received headers may have been folded. 
 * The application is responsible for folding and unfolding headers 
 * as appropriate. 
 *
 *    Multiparts May Like This:
 *
 * +------------------------- multipart/mixed ----------------------------+
 * |  +----------------- multipart/related ------------------+            |
 * |  |  +----- multipart/alternative ------+  +----------+  |  +------+  |
 * |  |  |                                  |  | rlt-attr |  |  | attr |  |
 * |  |  |  +------------+  +------------+  |  +----------+  |  +------+  |
 * |  |  |  | text/plain |  | text/html  |  |                |            |
 * |  |  |  +------------+  +------------+  |  +----------+  |  +------+  |
 * |  |  |                                  |  | rlt-attr |  |  | attr |  |
 * |  |  +----------------------------------+  +----------+  |  +------+  |
 * |  +------------------------------------------------------+            |
 * +----------------------------------------------------------------------+
 *
 * @hisotry v1.0.0 2004/5/30 Naven created
 * @author Naven
 */

typedef FastHashMap<ShortString, ShortString> HeaderMap; 

class MimeMessage : public MimeBodyPart
{
public: 
    // Traits 
    //typedef HashMap<ShortString, ShortString> HeaderMap; 
    
protected:
    BOOL m_bSaved; 

    void updateHeaders(); 
    void checkRFC822(); 
    void setDefaultHeaders(); 
    void setText(FastString &s, FastString &charset, FastString &stype); 
    void setText(FastString &s, FastString &charset, const char *stype); 
    void setText(FastString &s, const char *charset, const char *stype); 
    void setText(FastString &s, FastString &charset); 
    void setText(FastString &s, const char *charset = NULL); 
    void setTextPlainHtml(FastString &s, FastString &charset, BOOL isHtml); 
    void getTextPlainHtml(FastString &s, FastString &charset, BOOL isHtml); 
    BOOL moveAlternativeRelatedToFirst(MimeMultipart *mp); 
    BOOL moveTextToFirst(MimeMultipart *mp); 

public:
    MimeMessage();
    MimeMessage(char *psContent);
    MimeMessage(char *psContent, size_t len);
    MimeMessage(FastString &sContent);
    ~MimeMessage();
    void swap(MimeMessage &part); 
    void release(); 
    void dump(); 

    virtual BOOL isMimeBodyPart(); 
    virtual BOOL isMimeMessage(); 

    void getTextPlain(FastString &s); 
    void getTextPlain(FastString &s, FastString &charset); 
    void setTextPlain(FastString &s, FastString &charset); 
    void setTextPlain(FastString &s, const char *charset = NULL); 
    void setTextPlain(const char *s, const char *charset = NULL); 
    void getTextHtml(FastString &s); 
    void getTextHtml(FastString &s, FastString &charset); 
    void setTextHtml(FastString &s, FastString &charset); 
    void setTextHtml(FastString &s, const char *charset = NULL); 
    void setTextHtml(const char *s, const char *charset = NULL); 
    void addAttachment(FastString &s, FastString &filename, FastString &stype, HeaderMap &hdr, const char *charset = NULL); 
    void addAttachment(FastString &s, FastString &filename, FastString &stype); 
    void addAttachment(FastString &s, FastString &filename, const char *stype); 
    void addAttachment(FastString &s, const char *filename, const char *stype); 
    void addAttachment(FastString &s, FastString &filename, ParameterList &params); 
    void addAttachment(FastString &s, const char *filename, ParameterList &params); 
    void addAttachment(FastString &s, FastString &filename); 
    void addAttachment(FastString &s, const char *filename); 
    void addRFC822Message(FastString &s, FastString &filename); 
    void addRFC822Message(FastString &s, const char *filename); 
    int  addRelatedAttachment(FastString &s, FastString &src, FastString &cid, FastString &stype, HeaderMap &hdr); 
    int  addRelatedAttachment(FastString &s, FastString &src, FastString &cid, FastString &stype); 
    int  addRelatedAttachment(FastString &s, FastString &src, FastString &cid, const char *stype = NULL); 
    int  addRelatedAttachment(FastString &s, const char *src, FastString &cid, const char *stype = NULL); 
    int  addRelatedAttachment(FastString &s, FastString &src, const char *stype = NULL); 
    int  addRelatedAttachment(FastString &s, const char *src, const char *stype = NULL); 
    int  getAllAttachments(AttachmentPtrArray &attachments); 
    int  getAllAttachmentFilenames(FastStringArray &names); 
    MimeBodyPart * getAttachment(int index); 
    MimeBodyPart * getAttachment(FastString &filename); 
    MimeBodyPart * getAttachment(const char *filename); 
    int  getAttachment(int index, FastString &s, FastString &filename); 
	int  getAttachment(int index, FastString &s, FastString &filename, FastString &charset);
    int  getAttachment(FastString &filename, FastString &s); 
    int  getAttachment(const char *filename, FastString &s); 
    int  getAllRelatedAttachments(AttachmentPtrArray &attachments); 
    int  getAllRelatedAttachmentCIDs(FastStringArray &cids); 
    MimeBodyPart * getRelatedAttachment(int index); 
    MimeBodyPart * getRelatedAttachment(FastString &cid); 
    MimeBodyPart * getRelatedAttachment(const char *cid); 
    int  getRelatedAttachment(int index, FastString &s, FastString &cid, FastString &filename); 
    int  getRelatedAttachment(int index, FastString &s, FastString &cid); 
    int  getRelatedAttachment(int index, FastString &s); 
    int  getRelatedAttachment(FastString &cid, FastString &s, FastString &filename); 
    int  getRelatedAttachment(FastString &cid, FastString &s); 
    int  getRelatedAttachment(const char *cid, FastString &s, FastString &filename); 
    int  getRelatedAttachment(const char *cid, FastString &s); 
    int  getAttachmentCount(); 
    int  getRelatedAttachmentCount(); 
    void removeTextPlain(); 
    void removeTextHtml(); 
    BOOL removeRelatedAttachment(int index); 
    BOOL removeRelatedAttachment(FastString &cid); 
    BOOL removeRelatedAttachment(const char *cid); 
    void removeAllRelatedAttachment(); 
    BOOL removeAttachment(int index); 
    BOOL removeAttachment(FastString &filename); 
    BOOL removeAttachment(const char *filename); 
    void removeAllAttachment(); 
    void updateRelatedAttachments(); 

    void saveChanges(BOOL forceUpdate = FALSE); 
    void appendTo(FastString &s); 

};


//=============MimeMessage Inline functions==================

/**
 * Default Constructor. 
 */
inline MimeMessage::MimeMessage() 
: MimeBodyPart(), 
  m_bSaved(FALSE) 
{
    setDefaultHeaders(); 
}

/**
 * Constructs a MimeMessage by reading and parsing the data from the
 * specified string. The Input string will be left positioned
 * at the end of the data for the message. Note that the input string
 * parse is done within this constructor itself. <p>
 *
 * This method is for providers subclassing <code>MimeMessage</code>.
 *
 * @param psContent the message input string
 */
inline MimeMessage::MimeMessage(char *psContent)
: MimeBodyPart(psContent), 
  m_bSaved(FALSE) 
{
    checkRFC822(); 
}

/**
 * Constructs a MimeMessage by reading and parsing the data from the
 * specified string. The Input string will be left positioned
 * at the end of the data for the message. Note that the input string
 * parse is done within this constructor itself. <p>
 *
 * This method is for providers subclassing <code>MimeMessage</code>.
 *
 * @param psContent the message input string
 * @param len       the message input string length
 */
inline MimeMessage::MimeMessage(char *psContent, size_t len)
: MimeBodyPart(psContent, len), 
  m_bSaved(FALSE) 
{
    checkRFC822(); 
}

/**
 * Constructs a MimeMessage by reading and parsing the data from the
 * specified string. The Input string will be left positioned
 * at the end of the data for the message. Note that the input string
 * parse is done within this constructor itself. <p>
 *
 * This method is for providers subclassing <code>MimeMessage</code>.
 *
 * @param psContent the message input string
 * @param len       the message input string length
 */
inline MimeMessage::MimeMessage(FastString &sContent)
: MimeBodyPart((char *)sContent.c_str(), sContent.length()), 
  m_bSaved(FALSE) 
{
    checkRFC822(); 
}

inline MimeMessage::~MimeMessage() 
{
    release(); 
}

/**
 * Release all the memory of this object.
 */
inline void MimeMessage::release()
{
}

/**
 * Swap two variables, only swap their points and reference.
 * used for swap two large objects. 
 */
inline void MimeMessage::swap(MimeMessage &part)
{
    MimeBodyPart::swap(*((MimeBodyPart*)&part)); 
}

/**
 * Return TRUE if this is a MimeBodyPart object.
 */
inline BOOL MimeMessage::isMimeBodyPart() 
{
    return FALSE; 
}

/**
 * Return TRUE if this is a MimeMessage object.
 */
inline BOOL MimeMessage::isMimeMessage() 
{
    return TRUE; 
}

/**
 * Set the default headers where MimeMessage is empty.
 */
inline void MimeMessage::setDefaultHeaders() 
{
    setSubject(DEFAULT_MESSAGE_SUBJECT); 
    setSentDate(::time(NULL)); 
}

/**
 * Set the text/plain body text of Message, a message can only contains 
 * one <code>text/plain</code> and one <code>text/html</code> body text
 * part that contained by a Multipart which subtype is "alternative"; 
 * @See RFC2046
 *
 * If charset is NULL, message will use defaultMIMECharset that
 * set in ENV like: FASTMAIL_CHARSET=gb2312.
 *
 * @param s         text/plain content data
 * @param charset   content charset, default is defaultMIMECharset.
 */
inline void MimeMessage::setText(FastString &s, const char *charset) 
{
    MimeBodyPart::setText(s, charset); 
}

/**
 * Set the text/plain body text of Message, a message can only contains 
 * one <code>text/plain</code> and one <code>text/html</code> body text
 * part that contained by a Multipart which subtype is "alternative"; 
 * @See RFC2046
 *
 * If charset is NULL, message will use defaultMIMECharset that
 * set in ENV like: FASTMAIL_CHARSET=gb2312.
 *
 * @param s         text/plain content data
 * @param charset   content charset, default is defaultMIMECharset.
 */
inline void MimeMessage::setText(FastString &s, FastString &charset) 
{
    MimeBodyPart::setText(s, charset); 
}

/**
 * Set the text/plain body text of Message, a message can only contains 
 * one <code>text/plain</code> and one <code>text/html</code> body text
 * part that contained by a Multipart which subtype is "alternative"; 
 * @See RFC2046
 *
 * If charset is NULL, message will use defaultMIMECharset that
 * set in ENV like: FASTMAIL_CHARSET=gb2312.
 *
 * @param s         text/plain content data
 * @param charset   content charset, default is defaultMIMECharset.
 * @param stype     text body content-type.
 */
inline void MimeMessage::setText(FastString &s, FastString &charset, FastString &stype) 
{
    MimeBodyPart::setText(s, charset, stype); 
}

/**
 * Set the text/plain body text of Message, a message can only contains 
 * one <code>text/plain</code> and one <code>text/html</code> body text
 * part that contained by a Multipart which subtype is "alternative"; 
 * @See RFC2046
 *
 * If charset is NULL, message will use defaultMIMECharset that
 * set in ENV like: FASTMAIL_CHARSET=gb2312.
 *
 * @param s         text/plain content data
 * @param charset   content charset, default is defaultMIMECharset.
 * @param stype     text body content-type.
 */
inline void MimeMessage::setText(FastString &s, FastString &charset, const char *stype) 
{
    MimeBodyPart::setText(s, charset, stype); 
}

/**
 * Set the text/plain body text of Message, a message can only contains 
 * one <code>text/plain</code> and one <code>text/html</code> body text
 * part that contained by a Multipart which subtype is "alternative"; 
 * @See RFC2046
 *
 * If charset is NULL, message will use defaultMIMECharset that
 * set in ENV like: FASTMAIL_CHARSET=gb2312.
 *
 * @param s         text/plain content data
 * @param charset   content charset, default is defaultMIMECharset.
 * @param stype     text body content-type.
 */
inline void MimeMessage::setText(FastString &s, const char *charset, const char *stype) 
{
    MimeBodyPart::setText(s, charset, stype); 
}

/**
 * Set the text/plain body text of Message, a message can only contains 
 * one <code>text/plain</code> and one <code>text/html</code> body text
 * part that contained by a Multipart which subtype is "alternative"; 
 * @See RFC2046
 *
 * If charset is NULL, message will use defaultMIMECharset that
 * set in ENV like: FASTMAIL_CHARSET=gb2312.
 *
 * @param s         text/plain content data
 * @param charset   content charset, default is defaultMIMECharset.
 */
inline void MimeMessage::setTextPlain(FastString &s, FastString &charset) 
{
    setTextPlainHtml(s, charset, FALSE); 
}

/**
 * Set the text/html body text of Message, a message can only contains 
 * one <code>text/plain</code> and one <code>text/html</code> body text
 * part that contained by a Multipart which subtype is "alternative"; 
 * @See RFC2046
 *
 * If charset is NULL, message will use defaultMIMECharset that
 * set in ENV like: FASTMAIL_CHARSET=gb2312.
 *
 * @param s         text/plain content data
 * @param charset   content charset, default is defaultMIMECharset.
 */
inline void MimeMessage::setTextHtml(FastString &s, FastString &charset) 
{
    setTextPlainHtml(s, charset, TRUE); 
    updateRelatedAttachments(); 
}

/**
 * Set the text/plain body text of Message, a message can only contains 
 * one <code>text/plain</code> and one <code>text/html</code> body text
 * part that contained by a Multipart which subtype is "alternative"; 
 * @See RFC2046
 *
 * If charset is NULL, message will use defaultMIMECharset that
 * set in ENV like: FASTMAIL_CHARSET=gb2312.
 *
 * @param s         text/plain content data
 * @param charset   content charset, default is defaultMIMECharset.
 */
inline void MimeMessage::setTextPlain(FastString &s, const char *charset) 
{
    FastString sCharset(charset); 
    setTextPlain(s, sCharset); 
}

/**
 * Set the text/plain body text of Message, a message can only contains 
 * one <code>text/plain</code> and one <code>text/html</code> body text
 * part that contained by a Multipart which subtype is "alternative"; 
 * @See RFC2046
 *
 * If charset is NULL, message will use defaultMIMECharset that
 * set in ENV like: FASTMAIL_CHARSET=gb2312.
 *
 * @param s         text/plain content data
 * @param charset   content charset, default is defaultMIMECharset.
 */
inline void MimeMessage::setTextPlain(const char *s, const char *charset) 
{
    FastString sText(s), sCharset(charset); 
    setTextPlain(sText, sCharset); 
}

/**
 * Set the text/html body text of Message, a message can only contains 
 * one <code>text/plain</code> and one <code>text/html</code> body text
 * part that contained by a Multipart which subtype is "alternative"; 
 * @See RFC2046
 *
 * If charset is NULL, message will use defaultMIMECharset that
 * set in ENV like: FASTMAIL_CHARSET=gb2312.
 *
 * @param s         text/plain content data
 * @param charset   content charset, default is defaultMIMECharset.
 */
inline void MimeMessage::setTextHtml(FastString &s, const char *charset) 
{
    FastString sCharset(charset); 
    setTextHtml(s, sCharset); 
}

/**
 * Set the text/html body text of Message, a message can only contains 
 * one <code>text/plain</code> and one <code>text/html</code> body text
 * part that contained by a Multipart which subtype is "alternative"; 
 * @See RFC2046
 *
 * If charset is NULL, message will use defaultMIMECharset that
 * set in ENV like: FASTMAIL_CHARSET=gb2312.
 *
 * @param s         text/plain content data
 * @param charset   content charset, default is defaultMIMECharset.
 */
inline void MimeMessage::setTextHtml(const char *s, const char *charset) 
{
    FastString sHtml(s), sCharset(charset); 
    setTextHtml(sHtml, sCharset); 
}

/**
 * Get the <code>text/plain</code> body text part 
 * in current message. 
 *
 * @param s         text/plain main bodytext content
 */
inline void MimeMessage::getTextPlain(FastString &s) 
{
    FastString charset; 
    getTextPlainHtml(s, charset, FALSE); 
}

/**
 * Get the <code>text/plain</code> body text part 
 * in current message. 
 *
 * @param s         text/plain main bodytext content
 * @param charset   test/plain charset in content-type
 */
inline void MimeMessage::getTextPlain(FastString &s, FastString &charset) 
{
    getTextPlainHtml(s, charset, FALSE); 
}

/**
 * Get the <code>text/html</code> body text part 
 * in current message. 
 *
 * @param s         text/html main bodytext content
 */
inline void MimeMessage::getTextHtml(FastString &s) 
{
    FastString charset; 
    getTextPlainHtml(s, charset, TRUE); 
}

/**
 * Get the <code>text/html</code> body text part 
 * in current message. 
 *
 * @param s         text/html main bodytext content
 * @param charset   test/html charset in content-type
 */
inline void MimeMessage::getTextHtml(FastString &s, FastString &charset) 
{
    getTextPlainHtml(s, charset, TRUE); 
}

/**
 * Remove the <code>text/plain</code> body text part 
 * in current message. 
 *
 * NOT ATTACHMENT!
 */
inline void MimeMessage::removeTextPlain() 
{
    setTextPlain(NULL); 
}

/**
 * Remove the <code>text/html</code> body text part 
 * in current message. 
 *
 * NOT ATTACHMENT!
 */
inline void MimeMessage::removeTextHtml() 
{
    setTextHtml(NULL); 
}

/**
 * Add Attachment Part into Message, that attachment content 
 * <code>s</code> will swap data into message. 
 * Attachment data will encoded by BASE64.
 *
 * @param s         attachment content data
 * @param filename  attachment filename
 * @param params    attachment content-type parameter list
 */
inline void MimeMessage::addAttachment(FastString &s, const char *filename, ParameterList &params) 
{
    FastString sFileName(filename); 
    addAttachment(s, sFileName, params); 
}

/**
 * Add Attachment Part into Message, that attachment content 
 * <code>s</code> will swap data into message. 
 * Attachment data will encoded by BASE64.
 *
 * @param s         attachment content data
 * @param filename  attachment filename
 * @param stype     attachment content-type
 */
inline void MimeMessage::addAttachment(FastString &s, FastString &filename, FastString &stype) 
{
    HeaderMap hdrs; 
    addAttachment(s, filename, stype, hdrs); 
}

/**
 * Add Attachment Part into Message, that attachment content 
 * <code>s</code> will swap data into message. 
 * Attachment data will encoded by BASE64.
 *
 * @param s         attachment content data
 * @param filename  attachment filename
 * @param stype     attachment content-type
 */
inline void MimeMessage::addAttachment(FastString &s, FastString &filename, const char *stype) 
{
    FastString sType(stype); 
    addAttachment(s, filename, sType); 
}

/**
 * Add Attachment Part into Message, that attachment content 
 * <code>s</code> will swap data into message. 
 * Attachment data will encoded by BASE64.
 *
 * @param s         attachment content data
 * @param filename  attachment filename
 * @param stype     attachment content-type
 */
inline void MimeMessage::addAttachment(FastString &s, const char *filename, const char *stype) 
{
    FastString sFileName(filename), sType(stype); 
    addAttachment(s, sFileName, sType); 
}

/**
 * Add Attachment Part into Message, that attachment content 
 * <code>s</code> will swap data into message. 
 * Attachment data will encoded by BASE64.
 *
 * @param s         attachment content data
 * @param filename  attachment filename
 */
inline void MimeMessage::addAttachment(FastString &s, FastString &filename) 
{
    addAttachment(s, filename, NULL); 
}

/**
 * Add Attachment Part into Message, that attachment content 
 * <code>s</code> will swap data into message. 
 * Attachment data will encoded by BASE64.
 *
 * @param s         attachment content data
 * @param filename  attachment filename
 */
inline void MimeMessage::addAttachment(FastString &s, const char *filename) 
{
    FastString sFileName(filename); 
    addAttachment(s, sFileName); 
}

/**
 * Add RFC822 Message Attachment Part into Message, that attachment 
 * content <code>s</code> will swap data into message. 
 * Attachment data will encoded like Text/.
 *
 * @param s         rfc822 message content data
 * @param filename  rfc822 message filename
 */
inline void MimeMessage::addRFC822Message(FastString &s, FastString &filename) 
{
    addAttachment(s, filename, "message/rfc822"); 
}

/**
 * Add RFC822 Message Attachment Part into Message, that attachment 
 * content <code>s</code> will swap data into message. 
 * Attachment data will encoded like Text/.
 *
 * @param s         rfc822 message content data
 * @param filename  rfc822 message filename
 */
inline void MimeMessage::addRFC822Message(FastString &s, const char *filename) 
{
    FastString sFileName(filename); 
    addRFC822Message(s, sFileName); 
}

/**
 * Add Attachment Part into related multipart of Message, and update the 
 * <code>text/html</code> main bodytext in alternative multipart of message, 
 * that attachment content <code>s</code> will swap data into message. 
 * Attachment data will encoded by BASE64.
 *
 * @param s         attachment content data
 * @param src       attachment src name in html, include filename
 * @param cid       return attachment content-id
 * @return          count of src in html replaced with this cid
 */
inline int MimeMessage::addRelatedAttachment(
        FastString &s, FastString &src, FastString &cid, FastString &stype) 
{
    HeaderMap hdrs; 
    return addRelatedAttachment(s, src, cid, stype, hdrs); 
}

/**
 * Add Attachment Part into related multipart of Message, and update the 
 * <code>text/html</code> main bodytext in alternative multipart of message, 
 * that attachment content <code>s</code> will swap data into message. 
 * Attachment data will encoded by BASE64.
 *
 * @param s         attachment content data
 * @param src       attachment src name in html, include filename
 * @param cid       return attachment content-id
 * @return          count of src in html replaced with this cid
 */
inline int MimeMessage::addRelatedAttachment(
        FastString &s, FastString &src, FastString &cid, const char *stype) 
{
    FastString sType(stype); 
    return addRelatedAttachment(s, src, cid, sType); 
}

/**
 * Add Attachment Part into related multipart of Message, and update the 
 * <code>text/html</code> main bodytext in alternative multipart of message, 
 * that attachment content <code>s</code> will swap data into message. 
 * Attachment data will encoded by BASE64.
 *
 * @param s         attachment content data
 * @param src       attachment src name in html, include filename
 * @param cid       return attachment content-id
 * @return          count of src in html replaced with this cid
 */
inline int MimeMessage::addRelatedAttachment(
        FastString &s, const char *src, FastString &cid, const char *stype) 
{
    FastString sSrc(src), sType(stype); 
    return addRelatedAttachment(s, sSrc, cid, sType); 
}

/**
 * Add Attachment Part into related multipart of Message, and update the 
 * <code>text/html</code> main bodytext in alternative multipart of message, 
 * that attachment content <code>s</code> will swap data into message. 
 * Attachment data will encoded by BASE64.
 *
 * @param s         attachment content data
 * @param src       attachment src name in html, include filename
 * @return          count of src in html replaced with this cid
 */
inline int MimeMessage::addRelatedAttachment(
        FastString &s, FastString &src, const char *stype) 
{
    FastString sType(stype), cid; 
    return addRelatedAttachment(s, src, cid, sType); 
}

/**
 * Add Attachment Part into related multipart of Message, and update the 
 * <code>text/html</code> main bodytext in alternative multipart of message, 
 * that attachment content <code>s</code> will swap data into message. 
 * Attachment data will encoded by BASE64.
 *
 * @param s         attachment content data
 * @param src       attachment src name in html, include filename
 * @return          count of src in html replaced with this cid
 */
inline int MimeMessage::addRelatedAttachment(
        FastString &s, const char *src, const char *stype) 
{
    FastString sSrc(src), sType(stype), cid; 
    return addRelatedAttachment(s, sSrc, cid, sType); 
}

/**
 * Get the Attachment BodyPart's pointer in current message, 
 * with filename. 
 *
 * @param filename  attachment filename
 * @return          attachments bodypart pointer
 */
inline MimeBodyPart * MimeMessage::getAttachment(const char *filename) 
{
    FastString sFileName(filename); 
    return getAttachment(sFileName); 
}

/**
 * Get the Attachment BodyPart's content in current message, 
 * with filename. 
 *
 * @param filename  attachment filename
 * @param s         attachments bodypart content
 * @return      return 0 if is text else return 1, if failed, return -1
 */
inline int MimeMessage::getAttachment(const char *filename, FastString &s) 
{
    FastString sFileName(filename); 
    return getAttachment(sFileName, s); 
}

/**
 * Get the related Attachment BodyPart's pointer in current message, 
 * with cid. 
 *
 * @param cid   attachment related cid
 * @return      attachment bodypart pointer
 */
inline MimeBodyPart * MimeMessage::getRelatedAttachment(const char *cid) 
{
    FastString sCID(cid); 
    return getRelatedAttachment(sCID); 
}

/**
 * Get the Related Attachment BodyPart's content and filename 
 * in current message, with index. 
 *
 * @param index     attachment index in array
 * @param cid       attachment related cid
 * @param filename  attachment filename
 * @param s         attachment bodypart content
 * @return      return 0 if is text else return 1, if failed, return -1
 */
inline int MimeMessage::getRelatedAttachment(int index, FastString &s, FastString &cid) 
{
    FastString filename; 
    return getRelatedAttachment(index, s, cid); 
}

/**
 * Get the Related Attachment BodyPart's content and filename 
 * in current message, with index. 
 *
 * @param index     attachment index in array
 * @param cid       attachment related cid
 * @param s         attachment bodypart content
 * @return      return 0 if is text else return 1, if failed, return -1
 */
inline int MimeMessage::getRelatedAttachment(int index, FastString &s) 
{
    FastString filename, cid; 
    return getRelatedAttachment(index, s, cid); 
}

/**
 * Get the Related Attachment BodyPart's content and filename
 * in current message, with cid. 
 *
 * @param cid       attachment related cid
 * @param filename  attachment filename
 * @param s         attachment bodypart content
 * @return      return 0 if is text else return 1, if failed, return -1
 */
inline int MimeMessage::getRelatedAttachment(const char *cid, FastString &s, 
                                      FastString &filename) 
{
    FastString sCID(cid); 
    return getRelatedAttachment(sCID, s, filename); 
}

/**
 * Get the Related Attachment BodyPart's content
 * in current message, with cid. 
 *
 * @param cid       attachment related cid
 * @param filename  attachment filename
 * @param s         attachment bodypart content
 * @return      return 0 if is text else return 1, if failed, return -1
 */
inline int MimeMessage::getRelatedAttachment(FastString &cid, FastString &s) 
{
    FastString filename; 
    return getRelatedAttachment(cid, s, filename); 
}

/**
 * Get the Related Attachment BodyPart's content
 * in current message, with cid. 
 *
 * @param cid       attachment related cid
 * @param filename  attachment filename
 * @param s         attachment bodypart content
 * @return      return 0 if is text else return 1, if failed, return -1
 */
inline int MimeMessage::getRelatedAttachment(const char *cid, FastString &s) 
{
    FastString sCID(cid), filename; 
    return getRelatedAttachment(sCID, s, filename); 
}

/**
 * Dump the object's state.
 */
inline void MimeMessage::dump() 
{
    FAST_TRACE_BEGIN("MimeMessage::dump()"); 
    FAST_TRACE("sizeof(MimeMessage) = %d", sizeof(MimeMessage)); 
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
    FAST_TRACE_END("MimeMessage::dump()"); 
}


_FASTMIME_END_NAMESPACE

#endif
