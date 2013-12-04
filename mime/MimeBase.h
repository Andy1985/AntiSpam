//=============================================================================
/**
 *  @file    MimeBase.h
 *
 *  ver 1.0.0 2004/06/16 Naven Exp, for Mime Message Parse Engine.
 *
 *  @author Naven 2004/06/16 created.
 */
//=============================================================================

#ifndef _MIMEBASE_H
#define _MIMEBASE_H

#if defined(_WIN32) || defined(__WIN32__)

#if !defined (NAVEN_PRAGMA_ONCE)
# pragma once
#endif /* NAVEN_PRAGMA_ONCE */

#endif /* _WIN32 */


#include "Array.h"
#include "String.h"
#include "Vector.h"
#include "AutoPtr.h"
#include "HashMap.h" 
#include "CharsetUtils.h"


_FASTMIME_BEGIN_NAMESPACE


// Forward declaration.

class SystemProperty; 
class HeaderTokenizer; 
class MailDateParser; 
class MailDateFormat; 
class MimetypesFileTypeMap; 
class MimeUtility; 

class hdr; 
class InternetHeaders; 
class InternetAddress; 
class ParameterList; 
class ContentType; 

class IMimePart; 
class IMultipart; 
class MimeBodyPart; 
class MimeMultipart; 
class MimeMessage; 

typedef class FastArray<InternetAddress>          InternetAddressArray; 
typedef class FastArray_Iterator<InternetAddress> InternetAddressArrayIterator; 

typedef class FastArray<hdr>            hdrArray; 
typedef class FastArray_Iterator<hdr>   hdrArrayIterator; 

typedef InternetAddressArray            AddressArray; 
typedef InternetAddressArrayIterator    AddressArrayIterator; 

typedef class MimeBodyPart*             AttachmentPtr; 
typedef class FastArray<MimeBodyPart*>  AttachmentPtrArray; 


// MimeMessage used environments name macros

#if defined(_WIN32) || defined(__WIN32__)
#define PATH_SEPARATOR                  "\\\\"
#define FASTMIME_CRT_TIMEZONE           (_timezone*(-1))
#define FASTMIME_CRT_ENVIRON            (environ)

#define ENV_USER_HOMEPATH               "HOME"              // win: HOME       unix: HOME
#define ENV_SYSTEM_ENVPATH              "SystemRoot"        // win: SystemRoot unix: /env

#define DEFAULT_USER_HOME_DIRECTORY     "C:"                // win: HOMEPATH   unix: ~
#define DEFAULT_SYSTEM_ENV_DIRECTORY    "C:"                // win: SystemRoot unix: /env

#else   // UNIX
#define PATH_SEPARATOR                  "/"

#if defined(SOLARIS) || defined(_SOLARIS) || defined(__SOLARIS__)
#define FASTMIME_CRT_TIMEZONE           (timezone*(-1))
//#define FASTMIME_CRT_ENVIRON            (environ)
#else
#define FASTMIME_CRT_TIMEZONE           (__timezone*(-1))
#define FASTMIME_CRT_ENVIRON            (__environ)
#endif

#define ENV_USER_HOMEPATH               "HOME"
#define ENV_SYSTEM_ENVPATH              "SYSTEM_HOME"

#define DEFAULT_USER_HOME_DIRECTORY     "~"                 // win: HOMEPATH   unix: ~
#define DEFAULT_SYSTEM_ENV_DIRECTORY    "/env"              // win: SystemRoot unix: /env

#endif 

#define ENV_FASTMAIL_CHARSET            "FASTMAIL_CHARSET"
#define ENV_FASTMAIL_PART               "FASTMAIL_PART_NAME" 
#define ENV_FASTMAIL_NAME               "FASTMAIL_NAME" 
#define ENV_MIMETYPES_FILE              "MIMETYPES_FILE"

#define MIMETYPES_FILE_IN_USER_HOME     "mime.types" 
#define MIMETYPES_FILE_IN_SYSTEM_ENV    "mime.types.default" 

#define MIMETYPES_RESOURCEKEY_ENV       "env"
#define MIMETYPES_RESOURCEKEY_USER      "user"
#define MIMETYPES_RESOURCEKEY_SYSTEM    "system"

#define DEFAULT_MIME_CHARSET            "gb2312"
#define DEFAULT_MIME_TYPE               "application/octet-stream"

#define DEFAULT_MESSAGE_SUBJECT         "New Message"

#define DEFAULT_ENCODE_LINELENGTH       77

#define MIME_ATTACHMENT                 "attachment"
#define MIME_INLINE                     "inline"

#define DEFAULT_BOUNDARY_PART_NAME      "263Part" 
#define DEFAULT_BOUNDARY_FASTMAIL_NAME  "263Mail" 

#define CRLF                            "\r\n"
#define LINEAR_WHITE_SPACE              " \t\r\n"
#define ADDR_WHITE_SPACE                LINEAR_WHITE_SPACE"<>,;"
#define SPECIALS_NODOT_NOAT             "()<>,;:\\\"[]"
#define SPECIALS_NODOT                  SPECIALS_NODOT_NOAT"@"

#define SPECIALS_RFC822                 "()<>@,;:\\\"\t .[]"
#define SPECIALS_MIME                   "()<>@,;:\\\"\t []/?="


#ifndef LONG_MAX
#define LONG_MAX      2147483647L       /* maximum (signed) long value */
#endif

#ifndef ULONG_MAX
#define ULONG_MAX     0xffffffffUL      /* maximum unsigned long value */
#endif


inline int nonascii(int b) 
{
    return b >= 0177 || (b < 040 && b != '\r' && b != '\n' && b != '\t') ? 1 : 0;
}

static const char *specialsNoDotNoAt    = SPECIALS_NODOT_NOAT;
static const char *specialsNoDot        = SPECIALS_NODOT;



//=================Interface IMimePart Define====================

/**
 * The MimePart interface models an <strong>Entity</strong> as defined
 * by MIME (RFC2045, Section 2.4). <p>
 *
 * MimePart extends the Part interface to add additional RFC822 and MIME
 * specific semantics and attributes. It provides the base interface for
 * the MimeMessage and  MimeBodyPart classes 
 * 
 * <hr> <strong>A note on RFC822 and MIME headers</strong><p>
 *
 * RFC822 and MIME header fields <strong>must</strong> contain only 
 * US-ASCII characters. If a header contains non US-ASCII characters,
 * it must be encoded as per the rules in RFC 2047. The MimeUtility
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
class IMimePart
{
public: 
    // Traits 

    enum{   TO  = 0,               // All RecipientTypes
            CC,
            BCC, 
            NEWSGROUPS, 

            MAX_RECIPIENTTYPE
        };

    enum{   UNKNOWN_MIMETYPE  = 0,  // All Mime Types
            TEXT,                   // Content-Type: text/*;
            TEXT_PLAIN,             // Content-Type: text/plain;
            TEXT_HTML,              // Content-Type: text/html;
            TEXT_ENRICHED,          // Content-Type: text/enriched;
            TEXT_UNRECOGNIZED,      // Content-Type: text/unrecognized;
            //add by yao 20101011 for other text types 
            TEXT_CSS,               // Content-Type: text/css
            TEXT_H323,              // Content-Type: text/h323
            TEXT_ULS,               // Content-Type: text/uls
            TEXT_RTX,               // Content-Type: text/richtext
            TEXT_SCT,               // Content-Type: text/scriptlet
            TEXT_TSV,               // Content-Type:_text/tab-separated-values
            TEXT_HTT,               // Content-Type: text/webviewhtml
            TEXT_HTC,               // Content-Type: text/x-component
            TEXT_ETX,               // Content-Type: text/x-setext
            TEXT_VCF,               // Content-Type: text/x-vcard
            //end add
            MULTIPART,              // Content-Type: mutipart/*;
            MULTIPART_MIXED,        // Content-Type: mutipart/mixed;
            MULTIPART_RELATED,      // Content-Type: mutipart/related;
            MULTIPART_ALTERNATIVE,  // Content-Type: mutipart/alternative;
            MESSAGE,                // Content-Type: message/*;
            MESSAGE_RFC822,         // Content-Type: message/rfc822;
            // add by henh 2012/7/20 16:13:31 for message types
            MESSAGE_DELSTATUS,			// Content-Type: message/delivery-status
            // add end
            IMAGE,                  // Content-Type: image/*;
            IMAGE_JPEG,             // Content-Type: image/jpeg;
            IMAGE_GIF,              // Content-Type: image/gif;
            IMAGE_PNG,              // Content-Type: image/png;
            //add by yao 20101011 for other image types
            IMAGE_BMP,              // Content-Type: image/bmp
            IMAGE_COD,              // Content-Type: image/cis-cod
            IMAGE_IEF,              // Content-Type: image/ief
            IMAGE_PIPEG,            // Content-Type: image/pipeg
            IMAGE_SVG,              // Content-Type: image/svg+xml
            IMAGE_TIFF,             // Content-Type: image/tiff
            IMAGE_RAS,              // Content-Type: image/x-cmu-raster
            IMAGE_CMX,              // Content-Type: image/x-cmx
            IMAGE_ICO,              // Content-Type: image/x-icon
            IMAGE_PNM,              // Content-Type: image/x-portable-anymap
            IMAGE_PBM,              // Content-Type: image/x-portable-bitmap
            IMAGE_PGM,              // Content-Type: image/x-portable-graymap
            IMAGE_PPM,              // Content-Type: image/x-portable-pixmap
            IMAGE_RGB,              // Content-Type: image/x-rgb
            IMAGE_XBM,              // Content-Type: image/x-xbitmap
            IMAGE_XPM,              // Content-Type: image/x-xpixmap
            IMAGE_XWD,              // Content-Type: image/x-xwindowdump
            //end add
            AUDIO,                  // Content-Type: audio/*;
            AUDIO_BASIC,            // Content-Type: audio/basic;
            //add by yao 20101011 for other audio types
            //AUDIO_MP3,            // Content-Type: audio/mp3;
            AUDIO_MP3,              // Content-Type: audio/mpeg;
            AUDIO_MID,              // Content-Type: audio/mid
            AUDIO_AIF,              // Content-Type: audio/aiff
            AUDIO_M3U,              // Content-Type: audio/x-mpegurl
            AUDIO_RA,               // Content-Type: audio/x-pn-realaudio
            AUDIO_WAV,              // Content-Type: audio/x-wav
            //end add
            VIDEO,                  // Content-Type: video/*;
            VIDEO_MPEG,             // Content-Type: video/mpeg;
            //add by yao 20101011 for other video types
            VIDEO_QT,               // Content-Type: video/quicktime
            VIDEO_LSF,              // Content-Type: video/x-la-asf
            VIDEO_ASF,              // Content-Type: video/x-ms-asf
            VIDEO_AVI,              // Content-Type: video/x-msvideo
            VIDEO_MOV,              // Content-Type: video/x-sgi-movie
            //end add
            APPLICATION,            // Content-Type: application/*;
            APPLICATION_OCTET_STREAM, // Content-Type: application/octet-stream;
            APPLICATION_POSTSCRIPT, // Content-Type: application/PostScript;
            //add by yao 20101122 for other application types
            APPLICATION_EVY,        //Content-Type: application/envoy
            APPLICATION_FIF,        //Content-Type: application/fractals
            APPLICATION_SPL,        //Content-Type: application/futuresplash
            APPLICATION_HTA,        //Content-Type: application/hta
            APPLICATION_ACX,        //Content-Type: application/internet-property-stream
            APPLICATION_HQX,        //Content-Type: application/mac-binhex40
            APPLICATION_DOC,        //Content-Type: application/msword
            APPLICATION_PDF,        //Content-Type: application/pdf
            APPLICATION_RTF,        //Content-Type: application/rtf
            APPLICATION_EXCEL,      //Content-Type: application/vnd.ms-excel
            APPLICATION_OUTLOOK,    //Content-Type: application/vnd.ms-outlook
            APPLICATION_SST,        //Content-Type: application/vnd.ms-pkicertstore
            APPLICATION_CAT,        //Content-Type: application/vnd.ms-pkiseccat
            APPLICATION_STL,        //Content-Type: application/vnd.ms-pkistl
            APPLICATION_PPT,        //Content-Type: application/vnd.ms-powerpoint
            APPLICATION_PROJECT,    //Content-Type: application/vnd.ms-project
            APPLICATION_WORKS,      //Content-Type: application/vnd.ms-works
            APPLICATION_CDF,        //Content-Type: application/cdf
            APPLICATION_Z,          //Content-Type: application/x-compress
            APPLICATION_TGZ,        //Content-Type: application/x-compressed
            APPLICATION_CSH,        //Content-Type: application/x-csh
            APPLICATION_DIREC,      //Content-Type: application/x-director
            APPLICATION_DVI,        //Content-Type: application/x-dvi
            APPLICATION_GTAR,       //Content-Type: application/x-gtar
            APPLICATION_GZ,         //Content-Type: application/x-gzip
            APPLICATION_LATEX,      //Content-Type: application/x-latex
            APPLICATION_HDF,        //Content-Type: application/x-hdf
            APPLICATION_JS,         //Content-Type: application/x-javascript
            APPLICATION_MDB,        //Content-Type: application/x-msaccess
            APPLICATION_P12,        //Content-Type: application/x-pkcs12
            APPLICATION_SWF,        //Content-Type: application/x-shockwave-flash
            APPLICATION_TAR,        //Content-Type: application/x-tar
            APPLICATION_TCL,        //Content-Type: application/x-tcl
            APPLICATION_TEX,        //Content-Type: application/x-tex
            APPLICATION_CERT,       //Content-Type: application/x-x509-ca-cert
            APPLICATION_ZIP,        //Content-Type: application/zip
            //end add

            MAX_MIMETYPE
        };

    static const char *ATTACHMENT;  // This part should be presented as an attachment.
    static const char *INLINE;      // This part should be presented inline.

public:
    // Destructor - removes all the elements.
    virtual ~IMimePart(); 

    virtual BOOL isMimeBodyPart() = 0; 
    virtual BOOL isMimeMessage() = 0; 
    virtual BOOL isSetDefaultTextCharset() = 0; 
    virtual MimeMessage *getMessage() = 0; 
    virtual IMultipart *getParent() = 0; 
    virtual void setParent(IMultipart *parent) = 0; 
    virtual const char * getContentBuffer() = 0; 
    virtual int getContentBufferSize() = 0; 
    virtual const char * getHeaderBuffer() = 0; 
    virtual int getHeaderBufferSize() = 0; 
    virtual const char * getBodyBuffer() = 0; 
    virtual int getBodyBufferSize() = 0; 

    virtual int  getSize() = 0; 
    virtual int  getLineCount() = 0; 
    virtual void getContentType(FastString &s) = 0; 
    virtual void setContentType(FastString &s) = 0; 
    virtual void setContentType(const char *s) = 0; 
    virtual BOOL isMimeType(FastString &mimeType) = 0; 
    virtual BOOL isMimeType(const char *mimeType) = 0; 
    virtual int  getMimeType() = 0; 
    virtual void setMimeType(int mimetype) = 0; 
    virtual void getDisposition(FastString &s) = 0; 
    virtual void setDisposition(FastString &s) = 0; 
    virtual void setDisposition(const char *s) = 0; 
    virtual void getDescription(FastString &s) = 0; 
    virtual void setDescription(FastString &s, const char *charset = NULL) = 0; 
    virtual void setDescription(const char *s, const char *charset = NULL) = 0; 
    virtual void getFileName(FastString &s) = 0; 
    virtual void setFileName(FastString &s) = 0; 
    virtual void setFileName(const char *s) = 0; 
    virtual void getEncoding(FastString &s) = 0; 
    virtual void setEncoding(FastString &s) = 0; 
    virtual void setEncoding(const char *s) = 0; 
    virtual void getBodyLines(FastString &s) = 0; 
    virtual void getContentLines(FastString &s) = 0; 
    virtual void getMultipartLines(FastString &s) = 0; 
    virtual void getText(FastString &s) = 0; 
    virtual void getContent(FastString &s) = 0; 
    virtual FastString* getContent() = 0; 
    virtual MimeMultipart* getMultipart() = 0; 
    virtual void setContent(FastString &s, FastString &type) = 0; 
    virtual void setContent(FastString &s, const char *type = 0) = 0; 
    virtual void setContent(IMultipart *mp) = 0; 
    virtual void setContent(MimeMultipart &mp) = 0; 
    virtual void setMultipart(IMultipart *mp) = 0; 
    virtual void setMultipart(MimeMultipart &mp) = 0; 
    virtual void setText(FastString &s, FastString &charset) = 0; 
    virtual void setText(FastString &s, const char *charset = 0) = 0; 
    virtual int  getHeader(FastString &name, FastStringArray &values) = 0; 
    virtual int  getHeader(const char *name, FastStringArray &values) = 0; 
    virtual int  getHeader(FastString &name, FastString &s, const char *sep = NULL) = 0; 
    virtual int  getHeader(const char *name, FastString &s, const char *sep = NULL) = 0; 
    virtual void setHeader(FastString &name, FastString &s) = 0; 
    virtual void setHeader(const char *name, FastString &s) = 0; 
    virtual void addHeader(FastString &name, FastString &s, int index = -1) = 0; 
    virtual void addHeader(const char *name, FastString &s, int index = -1) = 0; 
    virtual void addHeaderLine(FastString &line) = 0; 
    virtual void removeHeader(FastString &name) = 0; 
    virtual void removeHeader(const char *name) = 0; 
    virtual int  getAllHeaders(hdrArray &headers) = 0; 
    virtual int  getAllHeaderLines(FastString &lines) = 0; 
    virtual int  getMatchingHeaders(FastStringArray &names, hdrArray &headers) = 0; 
    virtual int  getMatchingHeaderLines(FastStringArray &names, FastString &lines) = 0; 
    virtual int  getNonMatchingHeaders(FastStringArray &names, hdrArray &headers) = 0; 
    virtual int  getNonMatchingHeaderLines(FastStringArray &names, FastString &lines) = 0; 
    virtual void getContentID(FastString &s) = 0; 
    virtual void setContentID(FastString &s) = 0; 
    virtual void getContentMD5(FastString &s) = 0; 
    virtual void setContentMD5(FastString &s) = 0; 
    virtual void getMessageID(FastString &s) = 0; 
    virtual void appendTo(FastString &s) = 0; 
    virtual void toString(FastString &s) = 0; 
    virtual void release() = 0; 
    virtual void dump() = 0; 

    friend class MimeBodyPart; 
};


//=================Interface IMultipart Define====================

/**
 * IMultipart is a container that holds multiple body parts. IMultipart
 * provides methods to retrieve and set its subparts. <p>
 * 
 * IMultipart also acts as the base class for the content object returned 
 * by most IMultipart DataContentHandlers. For example, invoking getContent()
 * on a DataHandler whose source is a "multipart/signed" data source may
 * return an appropriate subclass of IMultipart. <p>
 *
 * Some messaging systems provide different subtypes of Multiparts. For
 * example, MIME specifies a set of subtypes that include "alternative", 
 * "mixed", "related", "parallel", "signed", etc. <p>
 *
 * IMultipart is an interface class.  Subclasses provide actual implementations.
 *
 */
class IMultipart
{
public: 
    // Destructor - removes all the elements. 
    virtual ~IMultipart(); 

    virtual MimeMessage *getMessage() = 0; 
    virtual IMimePart *getParent() = 0; 
    virtual void setParent(IMimePart *parent) = 0; 
    virtual void getContentType(FastString &s) = 0; 
    virtual int  getCount() = 0; 
    virtual MimeBodyPart* getBodyPart(int index) = 0; 
    virtual BOOL removeBodyPart(int index) = 0; 
    virtual void addBodyPart(MimeBodyPart &part) = 0; 
    virtual void addBodyPart(MimeBodyPart &part, int index) = 0; 
    virtual void appendTo(FastString &s) = 0; 
    virtual void toString(FastString &s) = 0; 

};


//=================Inline Functions Implements================

/**
 * Destructor - removes all the elements. 
 */
inline IMimePart::~IMimePart() 
{
    // Nothing
}

/**
 * Destructor - removes all the elements. 
 */
inline IMultipart::~IMultipart() 
{
    // Nothing
}



_FASTMIME_END_NAMESPACE

#endif
