//=============================================================================
/**
 *  @file    MimeContainer.cpp
 *
 *  ver 1.0.0 2004/06/16 Naven Exp, for Mime Message Parse Engine.
 *
 *  @author Naven 2004/06/16 created.
 */
//=============================================================================

#include "MimeContainer.h"


_FASTMIME_BEGIN_NAMESPACE


// Initialize Class Static Members

const char * MimeMultipart::MIXED           = "mixed";          // Content-Type: mutipart/mixed;
const char * MimeMultipart::RELATED         = "related";        // Content-Type: mutipart/related;
const char * MimeMultipart::ALTERNATIVE     = "alternative";    // Content-Type: mutipart/alternative;
const char * MimeMultipart::DIGEST          = "digest";         // Content-Type: mutipart/digest;


//===========MimeMultipart Functions Implement============

/**
 * Return the Message that contains the content.
 * Follows the parent chain up through containing Multipart
 * objects until it comes to a Message object, or null.
 *
 * @return	the containing Message, or null if not known
 */
MimeMessage * MimeMultipart::getMessage() 
{
    IMimePart *p = getParent(); 
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
 * Update all Multiparts and BodyParts's parent pointer.
 *
 */
void MimeMultipart::updateParent() 
{
    for( int i = 0; i < getCount(); i ++ ) 
    {
        MimeBodyPart *part = getBodyPart(i); 
        if( part ) 
        {
            part->setParent(this); 
            part->updateParent(); 
        }
    }
}

/**
 * Get the MimeBodyPart referred to by the given ContentID (CID). 
 * Returns null if the part is not found.
 *
 * @param cid   the ContentID of the desired part
 * @return      the point to the Part, NULL if not found.
 */
MimeBodyPart* MimeMultipart::getBodyPart(FastString &cid) 
{
    parse();
    if( cid.empty() ) 
        return NULL; 

    int count = getCount();
    for( int i = 0; i < count; i++ ) 
    {
        MimeBodyPart *part = getBodyPart(i); 
        if( part != NULL ) 
        {
            FastString s; 
            part->getContentID(s);
            if( s.equals(cid) )
                return part; 
        }
    }
    return NULL;
}

/**
 * Adds a BodyPart at position <code>index</code>.
 * If <code>index</code> is not the last one in the list,
 * the subsequent parts are shifted up. If <code>index</code>
 * is larger than the number of parts present, the
 * BodyPart is appended to the end.
 *
 * @param part  The BodyPart to be inserted
 * @param index Location where to insert the part
 */
void MimeMultipart::addBodyPart(MimeBodyPart &part, int index) 
{
    if( index >= 0 && index < (int)m_vParts.size() ) 
    {
        part.setParent(this); 

        //m_vParts.insert(index, MimeBodyPart()); 
        //m_vParts[index].swap(part); 

        // use swap is better
        addBodyPart(part); 
        moveFromTo(m_vParts.size()-1, index); 
    }
    else
        addBodyPart(part); 
}

/**
 * Remove the part at specified location (starting from 0).
 * Shifts all the parts after the removed part down one.
 *
 * @param index  Index of the part to remove
 */
BOOL MimeMultipart::removeBodyPart(int index) 
{
    parse(); 
    if( index >= 0 && index < (int) m_vParts.size() ) 
    {
        MimeBodyPart &part = m_vParts[index]; 
        part.release(); 

        // use swap() not assign operator is better quickly
        int i = index; 
        for( ; i < (int) m_vParts.size() - 1; i ++ ) 
            m_vParts[i].swap(m_vParts[i+1]); 

        m_vParts.remove(i); 
        return TRUE; 
    }
    else
        return FALSE; 
}

/**
 * Remove the part with specified bodypart pointer.
 *
 * @param bp  pointer of the part to remove
 */
BOOL MimeMultipart::removeBodyPart(MimeBodyPart *bp) 
{
    if( bp == NULL ) 
        return FALSE; 

    for( int i = 0; i < (int) m_vParts.size(); i ++ ) 
    {
        MimeBodyPart *bpm = &(m_vParts[i]); 
        if( bp == bpm ) 
            return removeBodyPart(i); 
    }

    return FALSE; 
}

/**
 * Update headers. The default implementation here just
 * calls the <code>updateHeaders</code> method on each of its
 * children BodyParts. <p>
 *
 * Note that the boundary parameter is already set up when
 * a new and empty MimeMultipart object is created. <p>
 *
 * This method is called when the <code>saveChanges</code>
 * method is invoked on the Message object containing this
 * Multipart. This is typically done as part of the Message
 * send process, however note that a client is free to call
 * it any number of times. So if the header updating process is 
 * expensive for a specific MimeMultipart subclass, then it
 * might itself want to track whether its internal state actually
 * did change, and do the header updating only if necessary.
 */
void MimeMultipart::updateHeaders()
{
    for( size_t i = 0; i < m_vParts.size(); i++ ) 
    {
        MimeBodyPart &part = m_vParts[i]; 
        part.updateHeaders();
    }
}

/**
 * Iterates through all the parts and outputs each Mime part
 * separated by a boundary.
 */
void MimeMultipart::appendTo(FastString &s)
{
    parse();

    FastString tmp, boundary("--"); 
    ContentType cType(m_sContentType); 
    cType.getParameter("boundary", tmp);
    boundary.append(tmp); 

    for( size_t i = 0; i < m_vParts.size(); i ++ ) 
    {
        s.append("\r\n"); 
        s.append(boundary); // put out boundary
        s.append("\r\n"); 

        MimeBodyPart &part = m_vParts[i]; 
        part.appendTo(s);

        s.append("\r\n");  // put out empty line
    }

    // put out last boundary
    s.append("\r\n"); 
    s.append(boundary); 
    s.append("--\r\n");
}

/**
 * Parse the InputStream from our DataSource, constructing the
 * appropriate MimeBodyParts.  The <code>parsed</code> flag is
 * set to true, and if true on entry nothing is done.  This
 * method is called by all other methods that need data for
 * the body parts, to make sure the data has been parsed.
 *
 */
void MimeMultipart::parse() 
{
    if( m_bParsed )
        return;

    m_bParsed = TRUE; 

    if( m_psContentBuffer == 0 || m_nContentBufferSize == 0 ) 
        return; 

    ContentType cType(m_sContentType);
    FastString bdr, end_bdr; 
    cType.getParameter("boundary", end_bdr); 
    bdr.append("--"); 
    bdr.append(end_bdr); 
    end_bdr.clear(); 
    end_bdr.append(bdr); 
    end_bdr.append("--"); 

    char *bodypart_start = 0, *bodypart_end = 0; 
    char *line_start = 0, *line_end = 0, *buf_end = 0; 

    char *bdr_start = (char *) bdr.c_str(); 
    char *bdr_end = bdr_start + bdr.length(); 

    char *end_bdr_start = (char *) end_bdr.c_str(); 
    char *end_bdr_end = end_bdr_start + end_bdr.length(); 

    line_start = MimeUtility::findStartLine(m_psContentBuffer, m_nContentBufferSize); 
    buf_end  = m_psContentBuffer + m_nContentBufferSize; 

    while( (line_end = MimeUtility::findEndLine(line_start, buf_end - line_start)) > line_start ) 
    {
        /*
         * Strip trailing whitespace.  Can't use trim method
         * because it's too aggressive.  Some bogus MIME
         * messages will include control characters in the
         * boundary string.
         */
        char *tmp_end = MimeUtility::ignoreCharsBackward(line_start, line_end, " \t\r\n"); 
        if( MimeUtility::equalsRegion(line_start, tmp_end, bdr_start, bdr_end) ) 
        {
            bodypart_start = MimeUtility::findStartLine(line_end, buf_end - line_end); 
            break; 
        }
        line_start = MimeUtility::findStartLine(line_end, buf_end - line_end); 
    }

    if( bodypart_start == 0 ) 
        // throw new MessagingException("Missing start boundary");
        return; 

    /*
     * Read and process body parts until we see the
     * terminating boundary line (or EOF).
     */
    while( bodypart_start && bodypart_start < buf_end ) 
    {
        line_start = bodypart_start; 
        bodypart_end = 0; 

        while( (line_end = MimeUtility::findEndLine(line_start, buf_end - line_start)) > line_start ) 
        {
            /*
            * Strip trailing whitespace.  Can't use trim method
            * because it's too aggressive.  Some bogus MIME
            * messages will include control characters in the
            * boundary string.
            */
            char *tmp_end = MimeUtility::ignoreCharsBackward(line_start, line_end, " \t\r\n"); 
            if( MimeUtility::equalsRegion(line_start, tmp_end, bdr_start, bdr_end) ) 
            {
                bodypart_end = MimeUtility::ignoreCharsBackward(bodypart_start, line_start, " \t\r\n"); 
                break; 
            }
            line_start = MimeUtility::findStartLine(line_end, buf_end - line_end); 
        }

        if( line_end == 0 || line_end > buf_end ) 
            // throw new MessagingException("Missing next boundary");
            return; 

        if( bodypart_end == 0 || bodypart_end <= bodypart_start ) 
            // throw new MessagingException("Missing next boundary");
            return; 

        MimeBodyPart part(bodypart_start, bodypart_end - bodypart_start); 
        addBodyPart(part); 

        bodypart_start = MimeUtility::findStartLine(line_end, buf_end - line_end); 
    }

    m_bParsed = TRUE; 
}


_FASTMIME_END_NAMESPACE
