//=============================================================================
/**
 *  @file    MimeContainer.h
 *
 *  ver 1.0.0 2004/06/16 Naven Exp, for Mime Message Parse Engine.
 *
 *  @author Naven 2004/06/16 created.
 */
//=============================================================================

#ifndef _MIMECONTAINER_H
#define _MIMECONTAINER_H

#if defined(_WIN32) || defined(__WIN32__)

#if !defined (NAVEN_PRAGMA_ONCE)
# pragma once
#endif /* NAVEN_PRAGMA_ONCE */

#endif /* _WIN32 */


#include "MimeBase.h" 
#include "MimeUtility.h"
#include "MimeObject.h"
#include "MimeEntity.h"


_FASTMIME_BEGIN_NAMESPACE



//=================Class MimeMultipart Define==================

/**
 * The MimeMultipart class is an implementation of the interface IMultipart
 * class that uses MIME conventions for the multipart data. <p>
 *
 * A MimeMultipart is obtained from a MimePart whose primary type
 * is "multipart" (by invoking the part's <code>getContent()</code> method)
 * or it can be created by a client as part of creating a new MimeMessage. <p>
 *
 * The default multipart subtype is "mixed".  The other multipart
 * subtypes, such as "alternative", "related", and so on, can be
 * implemented as subclasses of MimeMultipart with additional methods
 * to implement the additional semantics of that type of multipart
 * content. The intent is that service providers, mail JavaBean writers
 * and mail clients will write many such subclasses and their Command
 * Beans, and will install them into the JavaBeans Activation
 * Framework, so that any JavaMail implementation and its clients can
 * transparently find and use these classes. Thus, a MIME multipart
 * handler is treated just like any other type handler, thereby
 * decoupling the process of providing multipart handlers from the
 * JavaMail API. Lacking these additional MimeMultipart subclasses,
 * all subtypes of MIME multipart data appear as MimeMultipart objects. <p>
 *
 * An application can directly construct a MIME multipart object of any
 * subtype by using the <code>MimeMultipart(String subtype)</code>
 * constructor.  For example, to create a "multipart/alternative" object,
 * use <code>new MimeMultipart("alternative")</code>.
 *
 */
class MimeMultipart : public IMultipart
{
public:
    // Traits 
    typedef FastVector<MimeBodyPart> container_type; 

    static const char *MIXED;        // Content-Type: mutipart/mixed;
    static const char *RELATED;      // Content-Type: mutipart/related;
    static const char *ALTERNATIVE;  // Content-Type: mutipart/alternative;
    static const char *DIGEST;       // Content-Type: mutipart/digest;

protected:
    /**
     * Pointer to the buffer that holds the bytes of 
     * the content of this Part.
     */
    char *m_psContentBuffer; 
    size_t m_nContentBufferSize;

    /**
     * Vector of BodyPart objects.
     */
    container_type m_vParts; // Holds BodyParts

    /**
     * This field specifies the content-type of this multipart
     * object. It defaults to "multipart/mixed".
     */
    ShortString m_sContentType; // Content-Type

    /**
     * The <code>Part</code> containing this <code>Multipart</code>,
     * if known.
     */
    IMimePart *m_pParent; 

    /**
     * Have we parsed the data from our InputStream yet?
     * Defaults to true; set to false when our constructor is
     * given a DataSource with an InputStream that we need to
     * parse.
     */
    BOOL m_bParsed; 

protected: 
    MimeMultipart(const char *subtype, char *content);
    MimeMultipart(const char *subtype, char *content, size_t len);
    MimeMultipart(IMimePart *parent);
    void init(FastString &subtype); 
    void parse(); 
    void updateHeaders(); 
    void updateParent(); 

public:
    MimeMultipart(); 
    MimeMultipart(MimeMultipart &part); 
    MimeMultipart(FastString &subtype);
    MimeMultipart(const char *subtype);
    ~MimeMultipart(); 
    void swap(MimeMultipart &mp); 
    void release(); 
    void dump(); 

    MimeMessage *getMessage(); 
    IMimePart *getParent(); 
    void setParent(IMimePart *parent); 
    void getContentType(FastString &s); 
    int  getCount(); 
    MimeBodyPart* getBodyPart(int index); 
    MimeBodyPart* getBodyPart(FastString &cid); 
    MimeBodyPart* getBodyPart(const char *cid); 
    BOOL removeBodyPart(int index); 
    BOOL removeBodyPart(MimeBodyPart *bp); 
    void moveToFirst(int index); 
    void moveFromTo(int from, int to); 
    void swapPosition(int index1, int index2); 
    void addBodyPart(MimeBodyPart &part); 
    void addBodyPart(MimeBodyPart &part, int index); 
    void addTextBodyPart(MimeBodyPart &part); 
    void setSubType(FastString &s); 
    void setSubType(const char *s); 
    void appendTo(FastString &s); 
    void toString(FastString &s); 

    friend class MimeBodyPart; 
};


//=============MimeMultipart Inline functions==================

/**
 * Default constructor. An empty MimeMultipart object
 * is created. Its content type is set to "multipart/mixed".
 * A unique boundary string is generated and this string is
 * setup as the "boundary" parameter for the 
 * <code>contentType</code> field. <p>
 *
 * MimeBodyParts may be added later.
 */
inline MimeMultipart::MimeMultipart()
: m_psContentBuffer(0), 
  m_nContentBufferSize(0), 
  m_vParts(container_type(4)), 
  m_sContentType("multipart/mixed"), 
  m_pParent(0), 
  m_bParsed(FALSE) 
{
    FastString subtype("mixed"); 
    init(subtype); 
}

/**
 * Assign constructor. 
 */
inline MimeMultipart::MimeMultipart(MimeMultipart &part)
: m_psContentBuffer(part.m_psContentBuffer), 
  m_nContentBufferSize(part.m_nContentBufferSize), 
  m_vParts(container_type(part.m_vParts)), 
  m_sContentType(part.m_sContentType), 
  m_pParent(part.m_pParent), 
  m_bParsed(part.m_bParsed) 
{

}

/**
 * Construct a MimeMultipart object of the given subtype.
 * A unique boundary string is generated and this string is
 * setup as the "boundary" parameter for the 
 * <code>contentType</code> field. <p>
 *
 * MimeBodyParts may be added later.
 */
inline MimeMultipart::MimeMultipart(FastString &subtype) 
: m_psContentBuffer(0), 
  m_nContentBufferSize(0), 
  m_vParts(container_type(4)), 
  m_sContentType("multipart/mixed"), 
  m_pParent(0), 
  m_bParsed(FALSE) 
{
    init(subtype); 
}

/**
 * Construct a MimeMultipart object of the given subtype.
 * A unique boundary string is generated and this string is
 * setup as the "boundary" parameter for the 
 * <code>contentType</code> field. <p>
 *
 * MimeBodyParts may be added later.
 */
inline MimeMultipart::MimeMultipart(const char *subtype) 
: m_psContentBuffer(0), 
  m_nContentBufferSize(0), 
  m_vParts(container_type(4)), 
  m_sContentType("multipart/mixed"), 
  m_pParent(0), 
  m_bParsed(FALSE) 
{
    FastString subType(subtype); 
    init(subType); 
}

/**
 * Constructs a MimeMultipart object and its bodyparts from the 
 * given content string. <p>
 *
 * The <code>parsed</code> flag is set to false.  When
 * the data for the body parts are needed, the parser extracts the
 * "boundary" parameter from the content type of this content,
 * skips the 'preamble' and reads bytes till the terminating
 * boundary and creates MimeBodyParts for each part of the string.
 *
 * @param subtype is default content type for this multipart, but
 *      will be updated by parse() later if content string include
 *      Content-Type.
 * MimeBodyParts may be added later.
 */
inline MimeMultipart::MimeMultipart(const char *subtype, char *content) 
: m_psContentBuffer(content), 
  m_nContentBufferSize(content ? ::strlen(content) : 0), 
  m_vParts(container_type(4)), 
  m_sContentType("multipart/mixed"), 
  m_pParent(0), 
  m_bParsed(FALSE) 
{
    if( subtype )
    {
        FastString subType(subtype); 
        init(subType); 
    }
}

/**
 * Constructs a MimeMultipart object and its bodyparts from the 
 * given content string. <p>
 *
 * The <code>parsed</code> flag is set to false.  When
 * the data for the body parts are needed, the parser extracts the
 * "boundary" parameter from the content type of this content,
 * skips the 'preamble' and reads bytes till the terminating
 * boundary and creates MimeBodyParts for each part of the string.
 *
 * @param subtype is default content type for this multipart, but
 *      will be updated by parse() later if content string include
 *      Content-Type.
 * MimeBodyParts may be added later.
 */
inline MimeMultipart::MimeMultipart(const char *subtype, 
                                    char *content, size_t len) 
: m_psContentBuffer(content), 
  m_nContentBufferSize(content && len == 0 ? ::strlen(content) : len), 
  m_vParts(container_type(4)), 
  m_sContentType("multipart/mixed"), 
  m_pParent(0), 
  m_bParsed(FALSE) 
{
    if( subtype )
    {
        FastString subType(subtype); 
        init(subType); 
    }
}

/**
 * Constructs a MimeMultipart object and its bodyparts from the 
 * given MimePart. <p>
 *
 * The <code>parsed</code> flag is set to false.  When
 * the data for the body parts are needed, the parser extracts the
 * "boundary" parameter from the content type of this content,
 * skips the 'preamble' and reads bytes till the terminating
 * boundary and creates MimeBodyParts for each part of the string.
 *
 * MimeBodyParts is added to this parent.
 */
inline MimeMultipart::MimeMultipart(IMimePart *parent) 
: m_psContentBuffer(0), 
  m_nContentBufferSize(0), 
  m_vParts(container_type(4)), 
  m_sContentType("multipart/mixed"), 
  m_pParent(parent), 
  m_bParsed(FALSE) 
{
    if( m_pParent ) 
    {
        m_psContentBuffer = (char *) m_pParent->getBodyBuffer(); 
        m_nContentBufferSize = m_pParent->getBodyBufferSize(); 
        FastString s; 
        m_pParent->getContentType(s); 
        m_sContentType.set(s.c_str(), s.length()); 
    }
}

inline void MimeMultipart::init(FastString &subtype) 
{
    /*
     * Compute a boundary string.
     */
    FastString boundary; 
    UniqueValue::getUniqueBoundaryValue(boundary); 

    if( subtype.empty() ) 
        subtype = "mixed"; 

    ContentType cType("multipart", subtype);
    cType.setParameter("boundary", boundary);

    FastString contentType; 
    cType.toString(contentType);
    m_sContentType.set(contentType.c_str(), contentType.length()); 
}

/**
 *  Destructor ~
 */ 
inline MimeMultipart::~MimeMultipart() 
{
    release(); 
}

/**
 * Swap two variables, only swap their points and reference.
 * used for swap two large objects. 
 */
inline void MimeMultipart::swap(MimeMultipart &mp)
{
    fast_swap_value(m_psContentBuffer,      mp.m_psContentBuffer); 
    fast_swap_value(m_nContentBufferSize,   mp.m_nContentBufferSize); 
    fast_swap_value(m_pParent,              mp.m_pParent); 
    fast_swap_value(m_bParsed,              mp.m_bParsed); 

    m_vParts.swap(mp.m_vParts); 
    m_sContentType.swap(mp.m_sContentType); 
}

/**
 * Release all the memory of this object.
 */
inline void MimeMultipart::release()
{

}

/**
 * Return the <code>Part</code> that contains this <code>Multipart</code>
 * object, or <code>null</code> if not known.
 */
inline IMimePart* MimeMultipart::getParent() 
{
    return m_pParent;
}

/**
 * Set the parent of this <code>Multipart</code> to be the specified
 * <code>Part</code>.  Normally called by the <code>Message</code>
 * or <code>BodyPart</code> <code>setContent(Multipart)</code> method.
 * <code>parent</code> may be <code>null</code> if the
 * <code>Multipart</code> is being removed from its containing
 * <code>Part</code>.
 */
inline void MimeMultipart::setParent(IMimePart *parent) 
{
    m_pParent = parent;
}

/**
 * Return the content-type of this Multipart. <p>
 *
 * This implementation just returns the value of the
 * <code>contentType</code> field.
 *
 * @param s     content-type
 */
inline void MimeMultipart::getContentType(FastString &s) 
{
    s.set(m_sContentType.c_str(), m_sContentType.length()); 
}

/**
 * Set the subtype. This method should be invoked only on a new
 * MimeMultipart object created by the client. The default subtype
 * of such a multipart object is "mixed". <p>
 *
 * @param subtype   Subtype
 */
inline void MimeMultipart::setSubType(FastString &subtype) 
{
    if( !subtype.empty() ) 
    {
        FastString s(m_sContentType.c_str(), m_sContentType.length()); 
        ContentType cType(s);	
        cType.setSubType(subtype);
        cType.toString(s); 
        m_sContentType.set(s.c_str(), s.length()); 
    }
}

/**
 * Set the subtype. This method should be invoked only on a new
 * MimeMultipart object created by the client. The default subtype
 * of such a multipart object is "mixed". <p>
 *
 * @param subtype   Subtype
 */
inline void MimeMultipart::setSubType(const char *subtype) 
{
    FastString subType(subtype); 
    setSubType(subType); 
}

/**
 * Return the number of enclosed BodyPart objects. <p>
 *
 * @return  number of parts
 */
inline int MimeMultipart::getCount() 
{
    parse(); 
    return m_vParts.size();
}

/**
 * Get the specified Part.  Parts are numbered starting at 0.
 *
 * @param index the index of the desired Part
 * @return      the Part
 */
inline MimeBodyPart* MimeMultipart::getBodyPart(int index) 
{
    parse(); 
    if( index >= 0 && index < (int) m_vParts.size() ) 
        return &(m_vParts[index]); 
    else
        return NULL; 
}

/**
 * Move this part to the first position of the part list.
 *
 * @param index  index of The Part
 */
inline void MimeMultipart::moveToFirst(int index) 
{
    moveFromTo(index, 0); 
}

/**
 * Move this part from <code>from</code> to the <code>to</code> 
 * position of the part list.
 *
 * @param from  from index of The Part
 * @param to    to index of The Part
 */
inline void MimeMultipart::moveFromTo(int from, int to) 
{
    swapPosition(from, to); 
}

/**
 * Swap the <code>index1</code> and <code>index2</code> part's
 * position of the part list.
 *
 * @param index1  one Part's index
 * @param index2  other Part's index
 */
inline void MimeMultipart::swapPosition(int index1, int index2) 
{
    if( (index1 >= 0 && index1 < (int) m_vParts.size()) && 
        (index2 >= 0 && index2 < (int) m_vParts.size()) && 
        index1 != index2 ) 
    {
        m_vParts[index1].swap(m_vParts[index2]); 
    }
}

/**
 * Adds a Part to the multipart.  The BodyPart is appended to 
 * the list of existing Parts.
 *
 * @param part  The Part to be appended
 */
inline void MimeMultipart::addBodyPart(MimeBodyPart &part) 
{
    part.setParent(this);
    m_vParts.swap_push(part); 
}

/**
 * Adds a MAIN text Part to the multipart.  The BodyPart is appended to 
 * the first index of existing Parts.
 *
 * @param part  The Part to be appended
 */
inline void MimeMultipart::addTextBodyPart(MimeBodyPart &part) 
{
    addBodyPart(part, 0); 
}

/**
 * Get the MimeBodyPart referred to by the given ContentID (CID). 
 * Returns null if the part is not found.
 *
 * @param cid   the ContentID of the desired part
 * @return      the point to the Part, NULL if not found.
 */
inline MimeBodyPart* MimeMultipart::getBodyPart(const char *cid) 
{
    FastString CID(cid); 
    return getBodyPart(CID); 
}

/**
 * Convert this multipart into a RFC 822 / RFC 2047 encoded string.
 * The resulting string contains only US-ASCII characters, and
 * hence is mail-safe.
 *
 * @param s     possibly encoded mime string
 */
inline void MimeMultipart::toString(FastString &s) 
{
    s.clear(); 
    appendTo(s); 
}

/**
 * Dump the object's state.
 */
inline void MimeMultipart::dump() 
{
    FAST_TRACE_BEGIN("MimeMultipart::dump()"); 
    FAST_TRACE("sizeof(MimeMultipart) = %d", sizeof(MimeMultipart)); 
    FAST_TRACE("this -> 0x%08X", this); 
    FAST_TRACE("m_pParent -> 0x%08X", m_pParent); 
    FAST_TRACE("m_psContentBuffer -> 0x%08X", m_psContentBuffer); 
    FAST_TRACE("m_nContentBufferSize = %d", m_nContentBufferSize); 
    FAST_TRACE("m_bParsed = %d", m_bParsed); 
    FAST_TRACE("m_vParts.size() = %d", m_vParts.size()); 
#ifdef FAST_DEBUG
    FastString s(m_sContentType.c_str(), m_sContentType.length()); 
    s.removeChars("\r\n"); 
    FAST_TRACE("m_sContentType = %s", s.c_str()); 
#endif 
    FAST_TRACE_END("MimeMultipart::dump()"); 
}


_FASTMIME_END_NAMESPACE

#endif
