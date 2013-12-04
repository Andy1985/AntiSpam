//=============================================================================
/**
 *  @file    MimeMessage.cpp
 *
 *  ver 1.0.0 2004/06/16 Naven Exp, for Mime Message Parse Engine.
 *
 *  @author Naven 2004/06/16 created.
 */
//=============================================================================

#include "MimeMessage.h"


_FASTMIME_BEGIN_NAMESPACE


//=============MimeMessage Functions Implement================

/**
 * Check current message if is a standard RFC822 message.
 * If not is, must update the content buffer pointer. 
 *
 */
void MimeMessage::checkRFC822() 
{
    if( !m_psContentBuffer || m_nContentBufferSize == 0 ) 
    {
        setDefaultHeaders(); 
        return; 
    }

    if( isMimeMessage() ) 
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

        if( count == 0 ) 
        {
            removeAllHeaders(); 

            m_psHeaderBuffer    = 0; 
            m_nHeaderBufferSize = 0; 
            m_psBodyBuffer      = m_psContentBuffer; 
            m_nBodyBufferSize   = m_nContentBufferSize; 
        }
    }
}

/**
 * Called by the <code>saveChanges</code> method to actually
 * update the MIME headers.  The implementation here sets the
 * <code>Content-Transfer-Encoding</code> header (if needed
 * and not already set), the <code>Mime-Version</code> header
 * and the <code>Message-ID</code> header. Also, if the content
 * of this message is a <code>MimeMultipart</code>, it's 
 * <code>updateHeaders</code> method is called.
 */
void MimeMessage::updateHeaders() 
{
    MimeBodyPart::updateHeaders(*((IMimePart*)this)); 
    setHeader("Mime-Version", "1.0");
    FastString s, messageid; 
    UniqueValue::getUniqueMessageIDValue(messageid); 
    s.append("<"); 
    s.append(messageid); 
    s.append(">"); 
    setHeader("Message-ID", s);
}

/**
 * Set <code>text/plain</code> or <code>text/html</code> bodypart content
 * in a bodypart or MimeMessage.
 *
 * If this bodypart is the same <code>text/</code> type as the new 
 * <code>text/</code> content, this function will overwrite it with the new
 * content data <code>s</code>.
 *
 * If they are not the same text type, this function will make it to a 
 * alternatice multipart which contains both the original text bodypart and 
 * the new text bodypart. 
 *
 * @see RFC2046
 *
 * @param bp        bodypart pointer
 * @param s         text bodypart content data
 * @param isHtml    TRUE is <code>text/html</code> else is 
 *                  <code>text/plain</code>
 */
static BOOL setTextPlainHtmlInTextBodyPart(
                MimeBodyPart *bp, FastString &s, FastString &charset, BOOL isHtml) 
{
    if( bp == NULL || s.empty() ) 
        return FALSE; 

    if( bp->isText() ) 
    {
        if( (isHtml == TRUE  && bp->isTextPlain()) || 
            (isHtml == FALSE && bp->isTextHtml()) ) 
        {
            //    Make it to:
            // +----- multipart/alternative ------+
            // |  +------------+  +------------+  |
            // |  | text/plain |  | text/html  |  |
            // |  +------------+  +------------+  |
            // +----------------------------------+

            FastString content, contentType; 
            bp->getContentType(contentType); 
            bp->getContent(content); 

            MimeBodyPart part1, part2; 
            part1.setContent(content, contentType); 

            if( isHtml == TRUE ) 
                part2.setText(s, charset, "text/html"); 
            else
                part2.setText(s, charset, "text/plain"); 

            MimeMultipart mpart(MimeMultipart::ALTERNATIVE); 
            mpart.addBodyPart(part1); 
            mpart.addBodyPart(part2); 

            bp->setMultipart(mpart); 
        }
        else
        {
            if( isHtml == TRUE ) 
                bp->setText(s, charset, "text/html"); 
            else
                bp->setText(s, charset, "text/plain"); 
        }
    }
    else
    {
        //    Multiparts Like This:
        // +----- multipart/mixed ------------+
        // |  +------------+  +------------+  |
        // |  | text/      |  | attachment |  |
        // |  +------------+  +------------+  |
        // +----------------------------------+

        FastString content, contentType; 
        bp->getContentType(contentType); 
        bp->getContent(content); 

        MimeBodyPart part1, part2; 
        part1.setContentType(contentType); 
        part1.setContent(content); 

        if( isHtml == TRUE ) 
            part2.setText(s, charset, "text/html"); 
        else
            part2.setText(s, charset, "text/plain"); 

        MimeMultipart mpart;        // mixed
        mpart.addBodyPart(part1); 
        mpart.addBodyPart(part2); 

        bp->setMultipart(mpart); 
    }

    return TRUE; 
}

/**
 * Set <code>text/plain</code> or <code>text/html</code> bodypart content
 * in a multipart.
 *
 * This function only called by <code>setTextPlainHtmlInMultipartRelated()</code>
 * and <code>setTextPlainHtmlInMultipartMixed()</code> when the multipart 
 * only contains a text bodypart and other attachmen bodyparts.
 *
 * @see RFC2046
 *
 * @param mp        multipart pointer
 * @param s         text bodypart content data
 * @param isHtml    TRUE is <code>text/html</code> else is 
 *                  <code>text/plain</code>
 */
static BOOL setTextPlainHtmlInMultipart(
                MimeMultipart *mp, FastString &s, FastString &charset, BOOL isHtml) 
{
    if( mp == NULL ) 
        return FALSE; 

    for( int i = 0; i < mp->getCount(); i ++ ) 
    {
        MimeBodyPart *bp = mp->getBodyPart(i); 
        if( bp == NULL )
            continue; 
        
        if( bp->isText() ) 
        {
            FastString filename; 
            bp->getFileName(filename); 

            // This BodyPart is a BodyText not a text/  attachment.
            // Only find the first main text part, for multipart should
            // have only one main text part.
            if( filename.empty() ) 
            {
                if( setTextPlainHtmlInTextBodyPart(bp, s, charset, isHtml) ) 
                    return TRUE; 
            }
        }
    }

    MimeBodyPart part; 

    if( isHtml == TRUE ) 
        part.setText(s, charset, "text/html"); 
    else
        part.setText(s, charset, "text/plain"); 

    mp->addBodyPart(part); 

    return TRUE; 
}

/**
 * Set <code>text/plain</code> or <code>text/html</code> bodypart content
 * in a <code>multipart/alternative</code> multipart.
 *
 * This multipart should has two <code>text/ </code> bodypart, one is 
 * <code>text/plain</code> and other is <code>text/html</code>, but maybe 
 * has only one <code>text/ </code> bodypart.
 *
 * Alternatice multipart must be email text bodypart contains main text.
 * @see RFC2046
 *
 * @param mp        multipart pointer
 * @param s         text bodypart content data
 * @param isHtml    TRUE is <code>text/html</code> else is 
 *                  <code>text/plain</code>
 */
static BOOL setTextPlainHtmlInMultipartAlternative(
                MimeMultipart *mp, FastString &s, FastString &charset, BOOL isHtml) 
{
    if( mp == NULL ) 
        return FALSE; 

    //    Multiparts Like This:
    // +----- multipart/alternative ------+
    // |  +------------+  +------------+  |
    // |  | text/plain |  | text/html  |  |
    // |  +------------+  +------------+  |
    // +----------------------------------+

    for( int i = 0; i < mp->getCount(); i ++ ) 
    {
        MimeBodyPart *bp = mp->getBodyPart(i); 
        if( bp != NULL ) 
        {
            if( (isHtml == TRUE && bp->isTextHtml()) || 
                (isHtml == FALSE && bp->isTextPlain()) ) 
            {
                if( isHtml == TRUE ) 
                    bp->setText(s, charset, "text/html"); 
                else
                    bp->setText(s, charset, "text/plain"); 
                return TRUE; 
            }
        }
    }

    if( mp->getCount() <= 1 ) 
    {
        MimeBodyPart part; 

        if( isHtml == TRUE ) 
            part.setText(s, charset, "text/html"); 
        else
            part.setText(s, charset, "text/plain"); 

        mp->addBodyPart(part); 
    }
    else
    {
        MimeBodyPart *bp = mp->getBodyPart(0); 
        if( bp != NULL ) 
        {
            if( isHtml == TRUE ) 
                bp->setText(s, charset, "text/html"); 
            else
                bp->setText(s, charset, "text/plain"); 
        }
    }

    return TRUE; 
}

/**
 * Set <code>text/plain</code> or <code>text/html</code> bodypart content
 * in a <code>multipart/related</code> multipart.
 *
 * This multipart may contains a <code>multipart/alternative</code> 
 * multipart or only contains the main text bodypart and other related 
 * attachment bodypart. So it will first find the alternatice multipart 
 * and do as <code>setTextPlainHtmlInMultipartAlternative()</code>, 
 * else will do as <code>setTextPlainHtmlInMultipart()</code>.
 *
 * @see RFC2046
 *
 * @param mp        multipart pointer
 * @param s         text bodypart content data
 * @param isHtml    TRUE is <code>text/html</code> else is 
 *                  <code>text/plain</code>
 */
static BOOL setTextPlainHtmlInMultipartRelated(
                MimeMultipart *mp, FastString &s, FastString &charset, BOOL isHtml) 
{
    if( mp == NULL ) 
        return FALSE; 

    //    Multiparts May Like This:
    // +----------------- multipart/related ------------------+
    // |  +----- multipart/alternative ------+  +----------+  |
    // |  |                                  |  | rlt-attr |  |
    // |  |  +------------+  +------------+  |  +----------+  |
    // |  |  | text/plain |  | text/html  |  |                |
    // |  |  +------------+  +------------+  |  +----------+  |
    // |  |                                  |  | rlt-attr |  |
    // |  +----------------------------------+  +----------+  |
    // +------------------------------------------------------+

    for( int i = 0; i < mp->getCount(); i ++ ) 
    {
        MimeBodyPart *bp = mp->getBodyPart(i); 
        if( bp == NULL )
            continue; 
        
        if( bp->isMultipartAlternative() ) 
        {
            if( setTextPlainHtmlInMultipartAlternative(
                    bp->getMultipart(), s, charset, isHtml) == TRUE ) 
                return TRUE; 
        }
    }

    return setTextPlainHtmlInMultipart(mp, s, charset, isHtml); 
}

/**
 * Set <code>text/plain</code> or <code>text/html</code> bodypart content
 * in a <code>multipart/mixed</code> multipart.
 *
 * This multipart may contains a <code>multipart/alternative</code> 
 * multipart or a <code></code> multipart or only contains the main text 
 * bodypart and other related attachment bodypart. So it will first find 
 * the alternatice or related multipart and do as 
 * <code>setTextPlainHtmlInMultipartAlternative()</code> or 
 * <code>setTextPlainHtmlInMultipartRelated()</code>, 
 * else will do as <code>setTextPlainHtmlInMultipart()</code>.
 *
 * @see RFC2046
 *
 * @param mp        multipart pointer
 * @param s         text bodypart content data
 * @param isHtml    TRUE is <code>text/html</code> else is 
 *                  <code>text/plain</code>
 */
static BOOL setTextPlainHtmlInMultipartMixed(
                MimeMultipart *mp, FastString &s, FastString &charset, BOOL isHtml) 
{
    if( mp == NULL ) 
        return FALSE; 

    //    Multiparts May Like This:
    // +------------------------- multipart/mixed ----------------------------+
    // |  +----------------- multipart/related ------------------+            |
    // |  |  +----- multipart/alternative ------+  +----------+  |  +------+  |
    // |  |  |                                  |  | rlt-attr |  |  | attr |  |
    // |  |  |  +------------+  +------------+  |  +----------+  |  +------+  |
    // |  |  |  | text/plain |  | text/html  |  |                |            |
    // |  |  |  +------------+  +------------+  |  +----------+  |  +------+  |
    // |  |  |                                  |  | rlt-attr |  |  | attr |  |
    // |  |  +----------------------------------+  +----------+  |  +------+  |
    // |  +------------------------------------------------------+            |
    // +----------------------------------------------------------------------+

    for( int i = 0; i < mp->getCount(); i ++ ) 
    {
        MimeBodyPart *bp = mp->getBodyPart(i); 
        if( bp == NULL ) 
            continue; 
        
        if( bp->isMultipartAlternative() ) 
        {
            if( setTextPlainHtmlInMultipartAlternative(
                    bp->getMultipart(), s, charset, isHtml) == TRUE ) 
                return TRUE; 
        }
        else if( bp->isMultipartRelated() ) 
        {
            if( setTextPlainHtmlInMultipartRelated(
                    bp->getMultipart(), s, charset, isHtml) == TRUE ) 
                return TRUE; 
        }
    }

    return setTextPlainHtmlInMultipart(mp, s, charset, isHtml); 
}

/**
 * Set the text/  body text of Message, a message can only contains 
 * one <code>text/plain</code> and one <code>text/html</code> body text
 * part that contained by a Multipart which subtype is "alternative"; 
 * @See RFC2046
 *
 * If charset is NULL, message will use defaultMIMECharset that
 * set in ENV like: FASTMAIL_CHARSET=gb2312.
 *
 * @param s         text/  content data
 * @param charset   content charset, default is defaultMIMECharset.
 */
void MimeMessage::setTextPlainHtml(FastString &s, FastString &charset, BOOL isHtml) 
{
    if( isMultipart() ) 
    {
        if( isMultipartAlternative() ) 
        {
            setTextPlainHtmlInMultipartAlternative(getMultipart(), s, charset, isHtml); 
        }
        else if( isMultipartRelated() ) 
        {
            setTextPlainHtmlInMultipartRelated(getMultipart(), s, charset, isHtml); 
        }
        else if( isMultipartMixed() ) 
        {
            setTextPlainHtmlInMultipartMixed(getMultipart(), s, charset, isHtml); 
        }
        else
        {
            setTextPlainHtmlInMultipart(getMultipart(), s, charset, isHtml); 
        }
    }
    else
    {
        setTextPlainHtmlInTextBodyPart((MimeBodyPart*)this, s, charset, isHtml); 
    }

}

/**
 * Get the <code>text/ </code> body text part 
 * in current message. 
 *
 * @param s         text/  main bodytext content
 * @param charset   text/  charset in content-type
 * @param isHtml    TRUE get text/html else get text/ 
 */
static BOOL getTextPlainHtmlInBodyPart(
                MimeBodyPart *bp, FastString &s, FastString &charset, BOOL isHtml) 
{
    if( bp == NULL ) return FALSE; 

    if( (isHtml == TRUE  && bp->isTextHtml()) || 
        (isHtml == FALSE && bp->isTextPlain()) ) 
    {
        bp->getContent(s); 

        FastString stype; 
        bp->getContentType(stype); 

        ContentType cType(stype); 
        cType.getParameter("charset", charset); 

        return TRUE; 
    }

    return FALSE; 
}

static BOOL getTextPlainHtmlInMultipart(
                MimeMultipart *mp, FastString &s, FastString &charset, BOOL isHtml) 
{
    if( mp == NULL ) return FALSE; 

    for( int i = 0; i < mp->getCount(); i ++ ) 
    {
        MimeBodyPart *bp = mp->getBodyPart(i); 
        if( bp == NULL ) 
            continue; 

        if( bp->isText() ) 
        {
            FastString filename; 
            bp->getFileName(filename); 

            if( filename.empty() ) 
            {
                if( getTextPlainHtmlInBodyPart(bp, s, charset, isHtml) == TRUE ) 
                    return TRUE; 
            }
        }
        else
            return getTextPlainHtmlInMultipart(bp->getMultipart(), s, charset, isHtml);
    }

    return FALSE; 
}

static BOOL getTextPlainHtmlInMultipartAlternative(
                MimeMultipart *mp, FastString &s, FastString &charset, BOOL isHtml) 
{
    if( mp == NULL ) return FALSE; 

    for( int i = 0; i < mp->getCount(); i ++ ) 
    {
        MimeBodyPart *bp = mp->getBodyPart(i); 
        if( bp == NULL ) 
            continue; 

        if( getTextPlainHtmlInBodyPart(bp, s, charset, isHtml) == TRUE ) 
            return TRUE; 
    }

    return FALSE; 
}

static BOOL getTextPlainHtmlInMultipartRelated(
                MimeMultipart *mp, FastString &s, FastString &charset, BOOL isHtml) 
{
    if( mp == NULL ) return FALSE; 

    for( int i = 0; i < mp->getCount(); i ++ ) 
    {
        MimeBodyPart *bp = mp->getBodyPart(i); 
        if( bp == NULL ) 
            continue; 

        if( bp->isMultipartAlternative() ) 
        {
            BOOL got = getTextPlainHtmlInMultipartAlternative(
                            bp->getMultipart(), s, charset, isHtml); 

            if( got == TRUE ) 
                return TRUE; 
        }
    }

    return getTextPlainHtmlInMultipart(mp, s, charset, isHtml); 
}

static BOOL getTextPlainHtmlInMultipartMixed(
                MimeMultipart *mp, FastString &s, FastString &charset, BOOL isHtml) 
{
    if( mp == NULL ) return FALSE; 

    for( int i = 0; i < mp->getCount(); i ++ ) 
    {
        MimeBodyPart *bp = mp->getBodyPart(i); 
        if( bp == NULL ) 
            continue; 

        BOOL got = FALSE; 

        if( bp->isMultipartAlternative() ) 
        {
            got = getTextPlainHtmlInMultipartAlternative(
                        bp->getMultipart(), s, charset, isHtml); 
        }
        else if( bp->isMultipartRelated() ) 
        {
            got = getTextPlainHtmlInMultipartRelated(
                        bp->getMultipart(), s, charset, isHtml); 
        }

        if( got == TRUE ) 
            return TRUE; 
    }

    return getTextPlainHtmlInMultipart(mp, s, charset, isHtml); 
}

/**
 * Get the <code>text/ </code> body text part 
 * in current message. 
 *
 * @param s         text/  main bodytext content
 * @param charset   text/  charset in content-type
 * @param isHtml    TRUE get text/html else get text/ 
 */
void MimeMessage::getTextPlainHtml(FastString &s, FastString &charset, BOOL isHtml) 
{
    s.clear(); 
    charset.clear(); 

    if( isMultipart() ) 
    {
        MimeMultipart *mp = getMultipart(); 
        if( mp != NULL ) 
        {
            if( isMultipartAlternative() ) 
            {
                getTextPlainHtmlInMultipartAlternative(mp, s, charset, isHtml); 
            }
            else if( isMultipartRelated() ) 
            {
                getTextPlainHtmlInMultipartRelated(mp, s, charset, isHtml); 
            }
            else if( isMultipartMixed() ) 
            {
                getTextPlainHtmlInMultipartMixed(mp, s, charset, isHtml); 
            }
            else // found any text/  without filename
            {
                getTextPlainHtmlInMultipart(mp, s, charset, isHtml); 
            }
        }
    }
    else 
    {
        getTextPlainHtmlInBodyPart((MimeBodyPart*)this, s, charset, isHtml); 
    }
}

/**
 * Extract out filename in src url in html.. 
 *
 * @param src       attachment src name in html, include filename
 * @param filename  attachment filename
 */
static void extractFileName(FastString &src, FastString &filename) 
{
    filename.clear(); 

    if( src.empty() ) 
        return; 

    int pos1 = src.lastIndexOf('/'); 
    int pos2 = src.lastIndexOf('\\'); 
    int pos = -1; 
    if( pos1 >= 0 && pos2 >= 0 ) 
        pos = pos1 > pos2 ? pos1 : pos2; 
    else
        pos = pos1 > 0 ? pos1 : pos2; 
    if( pos >= 0 ) 
        src.substring(filename, pos + 1); 
    
    if( filename.empty() ) 
        filename = src; 
}

static int updateRelatedTextHtml(
                MimeBodyPart *bp, FastString &src, FastString &cid) 
{
    if( bp == NULL || src.empty() ) return -1; 

    int count = -1; 

    if( bp->isTextHtml() ) 
    {
        FastString cidurl; 
        ContentID CID;  // new cid
        CID.getContentID(cid); 
        CID.toCIDURL(cidurl); 

        FastString html; 
        bp->getContent(html); 

        // update html code with cidurl
        count = MimeUtility::replaceSrcWithCIDURL(html, src, cidurl); 
        if( count <= 0 ) 
        {
            cid.clear(); 
            return count; 
        }

        // update new html content
        bp->setContent(html); 
    }

    return count; 
}

static void newRelatedBodyPart(
                MimeBodyPart &part, FastString &s, FastString &src, FastString &cid, 
                FastString &stype, HeaderMap &hdrs) 
{
    if( s.empty() || src.empty() || cid.empty() ) 
        return; 

    HeaderMap::iterator it(hdrs); 
    while( !it.done() ) 
    {
        FastString name  = it->first(); 
        FastString value = it->second(); 
        
        if( !name.empty() ) 
            part.setHeader(name, value); 
        
        it.advance(); 
    }

    /**
     * Get the Content-Type of this new bodypart.
     */
    FastString filename; 
    extractFileName(src, filename); 

    if( stype.empty() ) 
    {
        MimetypesFileTypeMap &map = MimetypesFileTypeMap::getInstance(); 
        map.getContentType(filename, stype); 
    }

    ContentType cType(stype); 
    cType.setParameter("name", filename); 
    cType.toString(stype); 

    // new attachment bodypart
    part.setContent(s, stype); 
    part.setContentID(cid); 

}

static int addRelatedAttachmentInBodyPart(
                MimeBodyPart *bp, FastString &s, FastString &src, FastString &cid, 
                FastString &stype, HeaderMap &hdrs) 
{
    if( bp == NULL || s.empty() || src.empty() ) 
        return -1; 

    int count = -1; 

    if( bp->isTextHtml() ) 
    {
        FastString filename; 
        bp->getFileName(filename); 

        if( filename.empty() ) 
        {
            count = updateRelatedTextHtml(bp, src, cid); 
            if( count <= 0 ) 
                return count; 

            MimeBodyPart part; 
            newRelatedBodyPart(part, s, src, cid, stype, hdrs); 

            FastString content, contentType; 
            bp->getContentType(contentType); 
            bp->getContent(content); 

            //    Multiparts Like This:
            // +----- multipart/related ----------+
            // |  +------------+  +------------+  |
            // |  | text/html  |  | cid-attr   |  |
            // |  +------------+  +------------+  |
            // +----------------------------------+

            MimeBodyPart part1; 
            part1.setContent(content, contentType); 

            MimeMultipart mpart(MimeMultipart::RELATED); 
            mpart.addBodyPart(part1); 
            mpart.addBodyPart(part); 

            bp->setMultipart(mpart); 
        }
    }

    return count; 
}

static int addRelatedAttachmentInMultipart(
                MimeMultipart *mp, FastString &s, FastString &src, FastString &cid, 
                FastString &stype, HeaderMap &hdrs) 
{
    if( mp == NULL || s.empty() || src.empty() ) 
        return -1; 

    int count = -1; 

    for( int i = 0; i < mp->getCount(); i ++ ) 
    {
        MimeBodyPart *bp = mp->getBodyPart(i); 
        if( bp == NULL ) 
            continue; 

        if( bp->isTextHtml() ) 
        {
            count = addRelatedAttachmentInBodyPart(bp, s, src, cid, stype, hdrs); 
            if( count >= 0 ) 
                break; 
        }
    }

    return count; 
}

static int addRelatedAttachmentInMultipartAlternative(
                MimeBodyPart *bp, FastString &s, FastString &src, FastString &cid, 
                FastString &stype, HeaderMap &hdrs) 
{
    if( bp == NULL || s.empty() || src.empty() ) 
        return -1; 

    MimeMultipart *mp = bp->getMultipart(); 
    if( mp == NULL || !bp->isMultipartAlternative() ) 
        return -1; 

    int count = -1; 

    for( int i = 0; i < mp->getCount(); i ++ ) 
    {
        MimeBodyPart *bpa = mp->getBodyPart(i); 
        if( bpa == NULL ) 
            continue; 

        if( bpa->isTextHtml() ) 
        {
            count = updateRelatedTextHtml(bpa, src, cid); 
            break; 
        }
    }

    if( count <= 0 ) 
        return count; 

    if( cid.empty() ) 
        return -1; 

    MimeBodyPart part; 
    newRelatedBodyPart(part, s, src, cid, stype, hdrs); 

    MimeBodyPart part1; 
    part1.setMultipart(*mp); 

    MimeMultipart mpart(MimeMultipart::RELATED); 
    mpart.addBodyPart(part1); 
    mpart.addBodyPart(part); 

    bp->setMultipart(mpart); 

    return count; 
}

static int addRelatedAttachmentInMultipartRelated(
                MimeBodyPart *bp, FastString &s, FastString &src, FastString &cid, 
                FastString &stype, HeaderMap &hdrs) 
{
    if( bp == NULL || s.empty() || src.empty() ) 
        return -1; 

    MimeMultipart *mp = bp->getMultipart(); 
    if( mp == NULL || !bp->isMultipartRelated() ) 
        return -1; 

    int count = -1; 

    for( int i = 0; i < mp->getCount(); i ++ ) 
    {
        MimeBodyPart *bpr = mp->getBodyPart(i); 
        if( bpr == NULL ) 
            continue; 

        if( bpr->isMultipartAlternative() ) 
        {
            MimeMultipart *mpa = bpr->getMultipart(); 
            if( mpa == NULL ) 
                continue; 

            for( int j = 0; j < mpa->getCount(); j ++ ) 
            {
                MimeBodyPart *bpa = mpa->getBodyPart(j); 
                if( bpa == NULL ) 
                    continue; 

                if( bpa->isTextHtml() ) 
                {
                    count = updateRelatedTextHtml(bpa, src, cid); 
                    break; 
                }
            }
            break; 
        }
    }

    if( count < 0 ) 
    {
        for( int i = 0; i < mp->getCount(); i ++ ) 
        {
            MimeBodyPart *bpr = mp->getBodyPart(i); 
            if( bpr == NULL ) 
                continue; 

            if( bpr->isTextHtml() ) 
            {
                count = updateRelatedTextHtml(bpr, src, cid); 
                break; 
            }
        }
    }

    if( count <= 0 ) 
        return count; 

    if( cid.empty() ) 
        return -1; 

    MimeBodyPart part; 
    newRelatedBodyPart(part, s, src, cid, stype, hdrs); 

    mp->addBodyPart(part); 

    return count; 
}

static int addRelatedAttachmentInMultipartMixed(
                MimeBodyPart *bp, FastString &s, FastString &src, FastString &cid, 
                FastString &stype, HeaderMap &hdrs) 
{
    if( bp == NULL || s.empty() || src.empty() ) 
        return -1; 

    MimeMultipart *mp = bp->getMultipart(); 
    if( mp == NULL || !bp->isMultipartMixed() ) 
        return -1; 

    int count = -1; 

    for( int i = 0; i < mp->getCount(); i ++ ) 
    {
        MimeBodyPart *bpm = mp->getBodyPart(i); 
        if( bpm == NULL ) 
            continue; 

        if( bpm->isMultipartRelated() ) 
        {
            count = addRelatedAttachmentInMultipartRelated(bpm, s, src, cid, stype, hdrs); 
            break; 
        }
        else if( bpm->isMultipartAlternative() ) 
        {
            count = addRelatedAttachmentInMultipartAlternative(bpm, s, src, cid, stype, hdrs); 
            break; 
        }
    }

    if( count < 0 ) 
    {
        count = addRelatedAttachmentInMultipart(mp, s, src, cid, stype, hdrs); 
    }

    return count; 
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
 * @param stype     attachment mime-type, if empty it will search in configure.
 * @param hdrs       attachment bodypart headers map
 * @return          count of src in html replaced with this cid
 */
int MimeMessage::addRelatedAttachment(
        FastString &s, FastString &src, FastString &cid, 
        FastString &stype, HeaderMap &hdrs) 
{
    if( s.empty() || src.empty() ) 
        return -1; 

    int count = 0; 
    MimeBodyPart *bp = (MimeBodyPart *) this; 

    if( isMultipart() ) 
    {
        if( isMultipartAlternative() ) 
        {
            count = addRelatedAttachmentInMultipartAlternative(bp, s, src, cid, stype, hdrs); 
        }
        else if( isMultipartRelated() ) 
        {
            count = addRelatedAttachmentInMultipartRelated(bp, s, src, cid, stype, hdrs); 
        }
        else if( isMultipartMixed() ) 
        {
            count = addRelatedAttachmentInMultipartMixed(bp, s, src, cid, stype, hdrs); 
        }
    }
    else if( isTextHtml() )     // cid only in html
    {
        count = addRelatedAttachmentInBodyPart(bp, s, src, cid, stype, hdrs); 
    }

    return count; 
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
void MimeMessage::addAttachment(FastString &s, FastString &filename, ParameterList &params) 
{
    FastString sType; 
    MimetypesFileTypeMap &map = MimetypesFileTypeMap::getInstance(); 
    map.getContentType(filename, sType); 

    ContentType cType(sType); 

    ParameterList::iterator it = params.begin(); 
    while( !it.done() ) 
    {
        cType.setParameter(it->first(), it->second()); 
        it.advance(); 
    }

    cType.toString(sType); 

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
 * @param hdrs       attachment bodypart headers map
 */
// add by ljh, 增加参数charset，以便设置正确的编码
void MimeMessage::addAttachment(FastString &s, FastString &filename, 
                                FastString &stype, HeaderMap &hdrs, const char *charset) 
{
    if( s.empty() ) 
        return; 

    if( stype.empty() ) 
    {
        MimetypesFileTypeMap &map = MimetypesFileTypeMap::getInstance(); 
        map.getContentType(filename, stype); 
    }

    MimeBodyPart part; 
    
    HeaderMap::iterator it(hdrs); 
    while( !it.done() ) 
    {
        FastString name  = it->first(); 
        FastString value = it->second(); 
        
        if( !name.empty() ) 
            part.setHeader(name, value); 
        
        it.advance(); 
    }
    
    part.setContent(s, stype); 
// modify by ljh begin
//    part.setFileName(filename); 
    part.setFileName(filename,charset);
// modify by ljh end

    if( isMultipart() ) 
    {
        MimeMultipart *mp = getMultipart(); 
        if( mp != NULL ) 
        {
            BOOL added = FALSE; 

            if( isMultipartAlternative() ) 
            {
                MimeBodyPart part1; 
                part1.setMultipart(*mp); 

                MimeMultipart mpart;        // mixed
                mpart.addBodyPart(part1); 
                mpart.addBodyPart(part); 

                setMultipart(mpart); 
                added = TRUE; 
            }
            else if( isMultipartRelated() ) 
            {
                MimeBodyPart part1; 
                part1.setMultipart(*mp); 

                MimeMultipart mpart;        // mixed
                mpart.addBodyPart(part1); 
                mpart.addBodyPart(part); 

                setMultipart(mpart); 
                added = TRUE; 
            }
            else if( isMultipartMixed() ) 
            {
                // do as bellow...
            }

            if( added == FALSE ) 
            {
                mp->addBodyPart(part); 
            }
        }
    }
    else
    {
        FastString content, contentType; 
        getContentType(contentType); 
        getContent(content); 

        //    Multiparts Like This:
        // +----- multipart/mixed ------------+
        // |  +------------+  +------------+  |
        // |  | text/      |  | attachment |  |
        // |  +------------+  +------------+  |
        // +----------------------------------+

        MimeBodyPart part1; 
        part1.setContent(content, contentType); 

        MimeMultipart mpart;        // mixed
        mpart.addBodyPart(part1); 
        mpart.addBodyPart(part); 

        setMultipart(mpart); 
    }
}

/**
 * Updates the appropriate header fields of this message to be
 * consistent with the message's contents. If this message is
 * contained in a Folder, any changes made to this message are
 * committed to the containing folder. <p>
 *
 * If any part of a message's headers or contents are changed,
 * <code>saveChanges</code> must be called to ensure that those
 * changes are permanent. Otherwise, any such modifications may or 
 * may not be saved, depending on the folder implementation. <p>
 *
 * Messages obtained from folders opened READ_ONLY should not be
 * modified and saveChanges should not be called on such messages. <p>
 *
 * This method sets the <code>modified</code> flag to true, the
 * <code>save</code> flag to true, and then calls the
 * <code>updateHeaders<code> method.
 *
 */
void MimeMessage::saveChanges(BOOL forceUpdate) 
{
    if( m_bSaved && forceUpdate == FALSE ) 
        return; 

    m_bSaved = TRUE; 

    updateHeaders(); 
    updateRelatedAttachments(); 

    // Move the main text bodypart to the first position of the parts list
    if( isMultipart() ) 
    {
        MimeMultipart *mp = getMultipart(); 
        if( mp != NULL ) 
        {
            if( moveAlternativeRelatedToFirst(mp) == FALSE ) 
                moveTextToFirst(mp); 
        }
    }
}

/**
 * Move the <code>multipart/alternative</code> and <code>multipart/related</code> 
 * bodypart to the first in the multipart's parts list. 
 *
 * @param mp    the multipart pointer.
 */
BOOL MimeMessage::moveAlternativeRelatedToFirst(MimeMultipart *mp) 
{
    if( mp == NULL ) 
        return FALSE; 

    BOOL moved = FALSE; 
    BOOL found = FALSE; 

    for( int i = 0; i < mp->getCount(); i ++ ) 
    {
        MimeBodyPart *bp = mp->getBodyPart(i); 
        if( bp == NULL ) 
            continue; 
        
        if( bp->isMultipartAlternative() ) 
        {
            MimeMultipart *mpa = bp->getMultipart(); 
            if( mpa != NULL ) 
            {
                moveTextToFirst(mpa); 
                found = TRUE; 
            }
        }
        else if( bp->isMultipartRelated() ) 
        {
            MimeMultipart *mpr = bp->getMultipart(); 
            if( mpr != NULL ) 
            {
                BOOL alternativeMoved = FALSE; 

                for( int j = 0; j < mpr->getCount(); j ++ ) 
                {
                    MimeBodyPart *bpr = mpr->getBodyPart(i); 
                    if( bpr == NULL ) 
                        continue; 

                    if( bpr->isMultipartAlternative() ) 
                    {
                        MimeMultipart *mpa = bpr->getMultipart(); 
                        if( mpa != NULL ) 
                            moveTextToFirst(mpa); 

                        mpr->moveToFirst(j); 
                        alternativeMoved = TRUE; 
                        break; 
                    }
                }

                if( alternativeMoved == FALSE ) 
                    moveTextToFirst(mpr); 

                found = TRUE; 
            }
        }

        if( found == TRUE ) 
        {
            mp->moveToFirst(i); 
            moved = TRUE; 
            break; 
        }
    }

    return moved; 
}

/**
 * Move the <code>text/plain</code> bodypart to the first in 
 * the multipart's parts list. 
 *
 * @param mp    the multipart pointer.
 */
BOOL MimeMessage::moveTextToFirst(MimeMultipart *mp) 
{
    if( mp == NULL ) 
        return FALSE; 

    BOOL moved = FALSE; 
    int plainlastindex = 0; 
    int lastindex = 0; 
    FastString filename; 

    for( int i = 0; i < mp->getCount(); i ++ ) 
    {
        MimeBodyPart *bp = mp->getBodyPart(i); 
        if( bp == NULL ) 
            continue; 

        if( bp->isText() ) 
        {
            bp->getFileName(filename); 
            if( filename.empty() ) 
            {
                if( bp->isTextPlain() ) 
                {
                    mp->moveFromTo(i, lastindex); 
                    if( lastindex > plainlastindex ) 
                        mp->moveFromTo(lastindex, plainlastindex); 
                    plainlastindex ++; 
                }
                else
                    mp->moveFromTo(i, lastindex); 

                lastindex ++; 
                moved = TRUE; 
            }
        }
    }

    return moved; 
}

/**
 * Convert this bodypart into a RFC 822 / RFC 2047 encoded string, 
 * and append it to a existed string buffer. 
 * The resulting string contains only US-ASCII characters, and
 * hence is mail-safe.
 *
 * @param s     possibly encoded mime string that appended
 */
void MimeMessage::appendTo(FastString &s) 
{
    // safely check if s is current contentbuffer
    if( s.c_str() >= m_psContentBuffer && 
        s.c_str() <= m_psContentBuffer + m_nContentBufferSize ) 
        return; 

    setSentDate(::time(NULL)); 

    saveChanges(TRUE); 
    MimeBodyPart::appendTo(s); 
}

static int getAllRelatedAttachmentsInMultipartRelated(
                MimeBodyPart *bp, AttachmentPtrArray &attachments) 
{
    if( bp == NULL ) 
        return -1; 

    if( !bp->isMultipartRelated() ) 
        return 0; 

    MimeMultipart *mp = bp->getMultipart(); 
    if( mp == NULL ) 
        return -1; 

    FastVector<AttachmentPtr> vParts; 
    FastString cid; 

    for( int i = 0; i < mp->getCount(); i ++ ) 
    {
        MimeBodyPart *bpr = mp->getBodyPart(i); 
        if( bpr == NULL ) 
            continue;
        
        bpr->getContentID(cid); 
        
        // add by henh 2012/7/20 16:13:31 for filename empty
        FastString filename;
        bpr->getFileName(filename); 
        if( bp->isText() && filename.empty() && !cid.empty()) 
        {
            if( bpr->isTextHtml() ) 
                UniqueValue::getUniqueAttachmentTextHtmlValue(filename); 
            else
                UniqueValue::getUniqueAttachmentTextPlainValue(filename); 
            bpr->setFileName(filename); 
        }
        else if (filename.empty() && !cid.empty())
        {
                if(bpr->isMessageRFC822())
              {
                  FastString contents;
                  bpr->getBodyLines(contents);
                  MimeMessage email(contents);
                  FastString subject;
                  email.getSubject(subject);
                  if(!subject.empty())
                  {
                      filename = subject;
                      filename.append(".eml");
                      bpr->setFileName(filename);                      
                  }
              }
              if (filename.empty())
              {
                    FastString contentType; 
                  bpr->getContentType(contentType); 
                  UniqueValue::getUniqueAttachmentValue(filename, contentType); 
                  if(bpr->isMessageDELStatus())
                  {
                      filename.replace(FastString("ATT"),FastString("delivery-report"));
                  }
                  bpr->setFileName(filename);
                }
        }
        // add end

        if( !cid.empty() ) 
            vParts.push_back(bpr); 
    }

    int count = vParts.size(); 
    if( count > 0 ) 
        vParts.copyTo(attachments); 
    
    return count; 
}

/**
 * Get all Rleated Attachment Part in current message. 
 *
 * @param attachments   attachments bodypart pointer array
 * @return              attachments count
 */
int MimeMessage::getAllRelatedAttachments(AttachmentPtrArray &attachments) 
{
    if( isMultipart() ) 
    {
            MimeMultipart *mp = getMultipart();
            if( isMultipartRelated() ) 
        {
            return getAllRelatedAttachmentsInMultipartRelated(
                        (MimeBodyPart*)this, attachments); 
        }
        else
        {
            // add by henh 2012/7/20 16:13:31 for IOS system to send mail with attachments
            for( int i = 0; ( mp != NULL && i < mp->getCount()); i ++ ) 
            {
                MimeBodyPart *bp = mp->getBodyPart(i); 
                if( bp == NULL ) 
                    continue;                    
                
                if( bp->isMultipart() )
                {
                        if( bp->isMultipartRelated() ) 
                        {
                            return getAllRelatedAttachmentsInMultipartRelated(
                                bp, attachments); 
                        }
                        else
                        {
                                FastString filename;
                                FastString cid;
                                                MimeMultipart *mpp = bp->getMultipart();
                                                if( mpp==NULL )
                                                        continue;
                                                for( int ii=0; ii<mpp->getCount(); ii++ )
                                                {
                                                        MimeBodyPart *bpr = mpp->getBodyPart(ii);
                                                        if( bpr == NULL ) 
                                            continue;
                                                                                            
                                        if( bpr->isMultipartRelated() ) //最多3层
                                        {
                                            return getAllRelatedAttachmentsInMultipartRelated(
                                                bpr, attachments); 
                                        }
                                                }
                        }
                              }
            }
            // add end
            
        }
    }

    return 0; 
}

/**
 * Get all Related Attachment Part's cid in current message. 
 *
 * @param cids      attachments cids array
 * @return          attachments count
 */
int MimeMessage::getAllRelatedAttachmentCIDs(FastStringArray &cids) 
{
    AttachmentPtrArray attachments; 
    getAllRelatedAttachments(attachments); 

    if( attachments.size() == 0 ) 
        return 0; 

    FastVector<FastString> vCIDs; 

    for( size_t i = 0; i < attachments.size(); i ++ ) 
    {
        MimeBodyPart *part = attachments[i]; 
        if( part == NULL ) 
            continue; 

        FastString cid; 
        part->getContentID(cid); 
        vCIDs.push_back(cid); 
    }

    int count = vCIDs.size(); 
    vCIDs.copyTo(cids); 

    return count; 
}

/**
 * Get the index related Attachment BodyPart's pointer 
 * in current message. start from 0. 
 *
 * @param index     attachment index
 * @return          attachment bodypart pointer
 */
MimeBodyPart * MimeMessage::getRelatedAttachment(int index) 
{
    if( index < 0 ) 
        return (MimeBodyPart *)(NULL); 

    AttachmentPtrArray attachments; 
    int count = getAllRelatedAttachments(attachments); 

    if( index >= 0 && index < (int) attachments.size() ) 
        return attachments[index]; 
    
    return (MimeBodyPart *)(NULL); 
}

/**
 * Get the related Attachment BodyPart's pointer in current message, 
 * with cid. 
 *
 * @param cid   attachment related cid
 * @return      attachment bodypart pointer
 */
MimeBodyPart * MimeMessage::getRelatedAttachment(FastString &cid) 
{
    if( cid.empty() ) 
        return NULL; 

    AttachmentPtrArray attachments; 
    getAllRelatedAttachments(attachments); 

    for( size_t i = 0; i < attachments.size(); i ++ ) 
    {
        MimeBodyPart *part = attachments[i]; 
        if( part == NULL ) 
            continue; 

        FastString contentID; 
        part->getContentID(contentID); 
        
        if( contentID.equals(cid) ) 
            return part; 
    }
    
    return NULL; 
}

/**
 * Get the Related Attachment BodyPart's content and filename and cid
 * in current message, with index. 
 *
 * @param index     attachment index in array
 * @param cid       attachment related cid
 * @param filename  attachment filename
 * @param s         attachment bodypart content
 * @return      return 0 if is text else return 1, if failed, return -1
 */
int MimeMessage::getRelatedAttachment(int index, FastString &s, 
                                      FastString &cid, FastString &filename) 
{
    s.clear(); cid.clear(); 
    filename.clear(); 
    MimeBodyPart *part = getRelatedAttachment(index); 
    if( part != NULL ) 
    {
        part->getContentID(cid); 
        part->getFileName(filename); 
        part->getContent(s); 

        if( part->isText() || part->isMessage() ) 
            return 0; 
        else
            return 1; 
    }
    return -1; 
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
int MimeMessage::getRelatedAttachment(FastString &cid, FastString &s, 
                                      FastString &filename) 
{
    s.clear(); 
    filename.clear(); 
    MimeBodyPart *part = getRelatedAttachment(cid); 
    if( part != NULL ) 
    {
        part->getFileName(filename); 
        part->getContent(s); 

        if( part->isText() || part->isMessage() ) 
            return 0; 
        else
            return 1; 
    }
    return -1; 
}

/**
 * Get all Attachment Part in current message. 
 *
 * @param attachments   attachments bodypart pointer array
 * @return              attachments count
 */
int MimeMessage::getAllAttachments(AttachmentPtrArray &attachments) 
{
    FastVector<AttachmentPtr> vParts; 

    if( isMultipart() ) 
    {
        MimeMultipart *mp = getMultipart();
        if( isMultipartRelated() ) 
        {
            // no attachment in here
        }
        else
        {
            // add by henh 2012/7/20 16:13:31 for IOS system to send mail with attachments
            for( int i = 0; (mp != NULL && i < mp->getCount()); i ++ ) 
            {
                MimeBodyPart *bp = mp->getBodyPart(i); 
                if( bp == NULL ) 
                    continue;                    

                if( bp->isMultipart() )
                {
                    if( bp->isMultipartRelated() ) 
                    {
                        // no attachment in here
                    }
                    else
                    {
                        MimeMultipart *mpp = bp->getMultipart();
                        if( mpp==NULL )
                            continue;
                        for( int ii=0; ii<mpp->getCount(); ii++ )
                        {
                            MimeBodyPart *bpr = mpp->getBodyPart(ii);
                            if( bpr == NULL )
                                continue;

                            if ( bpr->isMultipart() )
                            {
                                //最多3层
                            }
                            else if( !bpr->isText() )
                            {            
                                FastString filename;            
                                bpr->getFileName(filename);
                                if( filename.empty() ) 
                                {
                                    if(bpr->isMessageRFC822())
                                    {
                                        FastString contents;
                                        bpr->getBodyLines(contents);
                                        MimeMessage email(contents);
                                        FastString subject;
                                        email.getSubject(subject);
                                        if(!subject.empty())
                                        {
                                            filename = subject;
                                            filename.append(".eml");
                                            bpr->setFileName(filename);
                                            vParts.push_back(bpr);                   
                                        }
                                    }
                                    if( filename.empty() && !bpr->isMessage()) 
                                    {
                                        FastString contentType; 
                                        bpr->getContentType(contentType); 
                                        UniqueValue::getUniqueAttachmentValue(filename, contentType);
                                        if(bpr->isMessageDELStatus())
                                        {
                                            filename.replace(FastString("ATT"),FastString("delivery-report"));
                                        }
                                        bpr->setFileName(filename); 
                                        vParts.push_back(bpr);
                                    }
                                }
                                else
                                    vParts.push_back(bpr);
                            }
                            else
                            {
                                FastString filename;
                                bpr->getFileName(filename); 
                                if( !filename.empty() ) 
                                    vParts.push_back(bpr);
                            }
                        }
                    }
                }
                else if( !bp->isText() )
                {
                    FastString filename;                                                        
                    bp->getFileName(filename);
                    if( filename.empty() ) 
                    {
                        if(bp->isMessageRFC822())
                        {
                            FastString contents;
                            bp->getBodyLines(contents);
                            MimeMessage email(contents);
                            FastString subject;
                            email.getSubject(subject);
                            if(!subject.empty())
                            {
                                filename = subject;
                                filename.append(".eml");
                                bp->setFileName(filename);
                                vParts.push_back(bp);                  
                            }
                        }
                        if( filename.empty() && !bp->isMessage()) 
                        {
                            FastString contentType; 
                            bp->getContentType(contentType); 
                            UniqueValue::getUniqueAttachmentValue(filename, contentType); 
                            if(bp->isMessageDELStatus())
                            {
                                filename.replace(FastString("ATT"),FastString("delivery-report"));
                            }
                            bp->setFileName(filename); 
                            vParts.push_back(bp);
                        }
                    }
                    else
                        vParts.push_back(bp);

                }
                else
                {
                    FastString filename;                                                        
                    bp->getFileName(filename);
                    if( !filename.empty() ) 
                        vParts.push_back(bp); 
                }
            }
            // add end

        }
    }
    else if( !isText() )
    {
        FastString filename;
        getFileName(filename);
        if( filename.empty() ) 
        {
            if(isMessageRFC822())
            {
                FastString contents;
                getBodyLines(contents);
                MimeMessage email(contents);
                FastString subject;
                email.getSubject(subject);
                if(!subject.empty())
                {
                    filename = subject;
                    filename.append(".eml");
                    setFileName(filename);
                    vParts.push_back(this);                     
                }
            }
            if( filename.empty() && !isMessage())
            {
                FastString contentType; 
                getContentType(contentType); 
                UniqueValue::getUniqueAttachmentValue(filename, contentType);
                if(isMessageDELStatus())
                {
                    filename.replace(FastString("ATT"),FastString("delivery-report"));
                } 
                setFileName(filename);
                vParts.push_back(this);
            }
        }
        else
            vParts.push_back(this);
    }
    else
    {
        FastString filename;
        getFileName(filename);
        if( !filename.empty() ) 
            vParts.push_back(this);
    }

    int count = vParts.size(); 
    if( count > 0 ) 
        vParts.copyTo(attachments); 

    return count; 
}

/**
 * Get all Attachment Part's filename in current message. 
 *
 * @param names     attachments filenames array
 * @return          attachments count
 */
int MimeMessage::getAllAttachmentFilenames(FastStringArray &names) 
{
    AttachmentPtrArray attachments; 
    getAllAttachments(attachments); 

    if( attachments.size() == 0 ) 
        return 0; 

    FastVector<FastString> vNames; 

    for( size_t i = 0; i < attachments.size(); i ++ ) 
    {
        MimeBodyPart *part = attachments[i]; 
        if( part == NULL ) 
            continue; 

        FastString filename; 
        part->getFileName(filename); 
        vNames.push_back(filename); 
    }

    int count = vNames.size(); 
    vNames.copyTo(names); 

    return count; 
}

/**
 * Get the index Attachment BodyPart's pointer in current message. 
 * start from 0. 
 *
 * @param index     attachment index
 * @return          attachment bodypart pointer
 */
MimeBodyPart * MimeMessage::getAttachment(int index) 
{
    if( index < 0 ) 
        return (MimeBodyPart *)(NULL); 

    AttachmentPtrArray attachments; 
    int count = getAllAttachments(attachments); 

    if( index >= 0 && index < (int) attachments.size() ) 
        return attachments[index]; 

    return (MimeBodyPart *)(NULL); 
}

/**
 * Get the Attachment BodyPart's pointer in current message, 
 * with filename. 
 *
 * @param filename  attachment filename
 * @return          attachment bodypart pointer
 */
MimeBodyPart * MimeMessage::getAttachment(FastString &filename) 
{
    if( filename.empty() ) 
        return NULL; 

    AttachmentPtrArray attachments; 
    getAllAttachments(attachments); 

    for( size_t i = 0; i < attachments.size(); i ++ ) 
    {
        MimeBodyPart *part = attachments[i]; 
        if( part == NULL ) 
            continue; 

        FastString name; 
        part->getFileName(name); 
        
        if( name.equals(filename) ) 
            return part; 
    }
    
    return NULL; 
}

/**
 * Get the Attachment BodyPart's content in current message, 
 * with filename. 
 *
 * @param filename  attachment filename
 * @param s         attachment bodypart content
 * @return      return 0 if is text else return 1, if failed, return -1
 */
int MimeMessage::getAttachment(FastString &filename, FastString &s) 
{
    s.clear(); 
    MimeBodyPart *part = getAttachment(filename); 
    if( part != NULL ) 
    {
        part->getContent(s); 
        if( part->isText() || part->isMessage() ) 
            return 0; 
        else
            return 1; 
    }
    return -1; 
}

/**
 * Get the Attachment BodyPart's content and filename in current message, 
 * with index. 
 *
 * @param filename  attachment filename
 * @param s         attachment bodypart content
 * @return      return 0 if is text else return 1, if failed, return -1
 */
int MimeMessage::getAttachment(int index, FastString &s, FastString &filename) 
{
    s.clear(); filename.clear(); 
    MimeBodyPart *part = getAttachment(index); 
    if( part != NULL ) 
    {
        part->getFileName(filename); 
        part->getContent(s); 

        if( part->isText() || part->isMessage() ) 
            return 0; 
        else
            return 1; 
    }
    return -1; 
}


/**
 * Get the Attachment BodyPart's content and filename in current message, 
 * with index. 
 *
 * @param filename  attachment filename
 * @param s         attachment bodypart content
 * @param charset   attachment filename charset
 * @return      return 0 if is text else return 1, if failed, return -1
 */
int MimeMessage::getAttachment(int index, FastString &s, FastString &filename, FastString &charset) 
{
    s.clear(); filename.clear(); 
    MimeBodyPart *part = getAttachment(index); 
    if( part != NULL ) 
    {
        part->getFileName(filename,charset); 
        part->getContent(s); 

        if( part->isText() || part->isMessage() ) 
            return 0; 
        else
            return 1; 
    }
    return -1; 
}

/**
 * Remove the Attachment Part's cid in current message. 
 * by index. 
 *
 * @param index     attachments index in array
 * @return   remove success return TRUE else failed return FALSE.
 */
BOOL MimeMessage::removeAttachment(int index) 
{
    if( index < 0 ) 
        return FALSE; 

    MimeBodyPart *bp = getAttachment(index); 
    if( bp == NULL ) 
        return FALSE; 

    // maybe bp is MimeMessage
    if( bp->isMimeMessage() ) 
    {
        FastString s; 
        bp->setContent(s); 
        return TRUE; 
    }

    MimeMultipart *mp = (MimeMultipart *) bp->getParent(); 
    if( mp == NULL ) 
        return FALSE; 

    return mp->removeBodyPart(bp); 
}

/**
 * Remove Attachment Part specified filename in current message. 
 *
 * @param filename  attachment filename
 * @return   remove success return TRUE else failed return FALSE.
 */
BOOL MimeMessage::removeAttachment(FastString &filename) 
{
    if( filename.empty() ) 
        return FALSE; 

    MimeBodyPart *bp = getAttachment(filename); 
    if( bp == NULL ) 
        return FALSE; 

    // maybe bp is MimeMessage
    if( bp->isMimeMessage() ) 
    {
        FastString s; 
        bp->setContent(s); 
        return TRUE; 
    }

    MimeMultipart *mp = (MimeMultipart *) bp->getParent(); 
    if( mp == NULL ) 
        return FALSE; 

    return mp->removeBodyPart(bp); 
}

/**
 * Remove the Related Attachment Part's cid in current message. 
 * by index. 
 *
 * @param index     attachments index in array
 * @return   remove success return TRUE else failed return FALSE.
 */
BOOL MimeMessage::removeRelatedAttachment(int index) 
{
    if( index < 0 ) 
        return FALSE; 

    MimeBodyPart *bp = getRelatedAttachment(index); 
    if( bp == NULL ) 
        return FALSE; 

    MimeMultipart *mp = (MimeMultipart *) bp->getParent(); 
    if( mp == NULL ) 
        return FALSE; 

    return mp->removeBodyPart(bp); 
}

/**
 * Remove the specified cid related attachment bodypart 
 * in current message. 
 *
 * @param cid     attachments cid
 * @return   remove success return TRUE else failed return FALSE.
 */
BOOL MimeMessage::removeRelatedAttachment(FastString &cid) 
{
    if( cid.empty() ) 
        return FALSE; 

    MimeBodyPart *bp = getRelatedAttachment(cid); 
    if( bp == NULL ) 
        return FALSE; 

    MimeMultipart *mp = (MimeMultipart *) bp->getParent(); 
    if( mp == NULL ) 
        return FALSE; 

    return mp->removeBodyPart(bp); 
}

/**
 * Get the attachment bodyparts 's count
 * in current message. 
 *
 */
int MimeMessage::getAttachmentCount() 
{
    AttachmentPtrArray attachments; 
    getAllAttachments(attachments); 
    return attachments.size(); 
}

/**
 * Get the related attachment bodyparts 's count
 * in current message. 
 *
 */
int MimeMessage::getRelatedAttachmentCount() 
{
    AttachmentPtrArray attachments; 
    getAllRelatedAttachments(attachments); 
    return attachments.size(); 
}

/**
 * Remove all the attachment bodyparts 
 * in current message. 
 *
 */
void MimeMessage::removeAllAttachment() 
{
    int index = getAttachmentCount() - 1; 
    while( index >= 0 ) 
    {
        removeAttachment(index); 
        index --; 
    }
}

/**
 * Remove all the cid related attachment bodyparts 
 * in current message. 
 *
 */
void MimeMessage::removeAllRelatedAttachment() 
{
    int index = getRelatedAttachmentCount() - 1; 
    while( index >= 0 ) 
    {
        removeRelatedAttachment(index); 
        index --; 
    }
}

/**
 * Update the related attachment bodypart in <code>multipart/related</code>
 * when message main html bodytext changed. 
 * This will remove the related attachment bodypart that <code>text/html</code> 
 * bodytext never referenced.
 *
 */
void MimeMessage::updateRelatedAttachments() 
{
    FastString html; 
    FastStringArray cids; 

    getTextHtml(html); 
    getAllRelatedAttachmentCIDs(cids); 

    for( size_t i = 0; i < cids.size(); i ++ ) 
    {
        if( html.indexOf(cids[i]) < 0 ) 
            removeRelatedAttachment(cids[i]); 
    }
}


_FASTMIME_END_NAMESPACE
