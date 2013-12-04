//=============================================================================
/**
 *  @file    MimeActivation.cpp
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
#endif

#include "MimeObject.h"
#include "MimeUtility.h" 
#include "MimeActivation.h"


_FASTMIME_BEGIN_NAMESPACE


// Initialize Class Static Members

SystemProperty* SystemProperty::m_pInstance = 0; 

MimetypesFileTypeMap *MimetypesFileTypeMap::m_pInstance = NULL; 
const char *MimetypesFileTypeMap::DEFAULT_CONTENT_TYPE  = DEFAULT_MIME_TYPE; 

IMPLEMENT_MIME_THREAD_MUTEX(SystemProperty)
IMPLEMENT_MIME_THREAD_MUTEX(MimetypesFileTypeMap)


// Globale Variables

MimeInitialization _global_mime_initialization; 



//===========SystemProperty Functions Implements=============

/**
 * Get the singleton instance, if not initialize, create it.
 */
SystemProperty& SystemProperty::getInstance() 
{
    MIME_THREAD_SYNCHRONIZE() 
    
    if( m_pInstance == 0 ) 
        m_pInstance = new SystemProperty(); 
    return *m_pInstance; 
}

/**
 * Delete the singleton instance.
 */
void SystemProperty::closeSingleton() 
{
    MIME_THREAD_SYNCHRONIZE() 
    
    if( m_pInstance != 0 ) 
    {
        delete m_pInstance; 
        m_pInstance = NULL; 
    }
}

/**
 * Reload the object state.
 */
void SystemProperty::loadAll() 
{
    m_hmProperties.clear(); 
#ifdef FASTMIME_CRT_ENVIRON
    char **environ2 = FASTMIME_CRT_ENVIRON;
    while( environ2 && *environ2 && (*environ2)[0] ) 
    {
        FastString line = *environ2; 
        int pos = line.indexOf('='); 
        if( pos <= 0 ) continue; 
        FastString value; 
        line.substring(value, 0, pos); 
        ShortString name(value, value.length()); 
        line.substring(value, pos + 1); 
        name.toUpperCase(); 
        m_hmProperties.put(name, value); 
        environ2 ++; 
    }
#endif
}

/**
 * Gets the system property indicated by the specified key.
 *
 * @param name  Property name
 * @return  Property value by reference
 */
const FastString& 
SystemProperty::getProperty(ShortString &name) 
{
    MIME_THREAD_SYNCHRONIZE() 
    
    name.toUpperCase(); 
    FastString &value = m_hmProperties.get(name); 
    if( value.empty() && !name.empty() ) 
    {
        char *tmp = ::getenv(name.c_str()); 
        if( tmp ) 
            value.set(tmp); 
    }
    return value; 
} 


//===========MimetypesFileTypeMap Functions Implements=============

/**
 * Get the singleton instance, if not initialize, create it.
 */
MimetypesFileTypeMap& MimetypesFileTypeMap::getInstance() 
{
    MIME_THREAD_SYNCHRONIZE() 
    
    if( m_pInstance == 0 ) 
        m_pInstance = new MimetypesFileTypeMap(); 
    return *m_pInstance; 
}

/**
 * Delete the singleton instance.
 */
void MimetypesFileTypeMap::closeSingleton() 
{
    MIME_THREAD_SYNCHRONIZE() 
    
    if( m_pInstance != 0 ) 
    {
        delete m_pInstance; 
        m_pInstance = NULL; 
    }
}

/**
 * Load all of the resources. 
 * 
 * <b>MIME types file search order:</b><p>
 * The MimetypesFileTypeMap looks in various places in the user's
 * system for MIME types file entries. When requests are made
 * to search for MIME types in the MimetypesFileTypeMap, it searches  
 * MIME types files in the following order:
 * <p>
 * <ol>
 * <li> The file that the enviroments <code>MIMETYPES_FILE</code>.
 * <li> The file <code>.mime.types</code> in the user's home directory.
 * <li> The file <code>mime.types.default</code> in the system "/etc" directory.
 * <li> The Mimetypes default defined in the code.
 * </ol>
 * <p>
 */
void MimetypesFileTypeMap::loadAllResources() 
{
    SystemProperty &sysprops = SystemProperty::getInstance(); 

    /**
     * Load The file that the enviroments <code>MIMETYPES_FILE</code>.
     */ 
    FastString typeFile(sysprops.getProperty(ENV_MIMETYPES_FILE)); 
    if( !typeFile.empty() ) 
        loadResources(MIMETYPES_RESOURCEKEY_ENV, typeFile); 

    /**
     * Load The file <code>.mime.types</code> in the user's home directory.
     */ 
#if defined(_WIN32) || defined(__WIN32__)
    typeFile.clear(); 
    typeFile = sysprops.getProperty(ENV_USER_HOMEPATH); 
    typeFile.append(PATH_SEPARATOR); 
    typeFile.append(MIMETYPES_FILE_IN_USER_HOME); 
#else
    typeFile.clear(); 
    typeFile.append(DEFAULT_USER_HOME_DIRECTORY); 
    typeFile.append(PATH_SEPARATOR); 
    typeFile.append(MIMETYPES_FILE_IN_USER_HOME); 
#endif 
    loadResources(MIMETYPES_RESOURCEKEY_USER, typeFile); 

    /**
     * Load The file <code>mime.types.default</code> in the system "/etc" directory.
     */ 
#if defined(_WIN32) || defined(__WIN32__)
    typeFile = sysprops.getProperty(ENV_SYSTEM_ENVPATH); 
    typeFile.append(PATH_SEPARATOR); 
    typeFile.append(MIMETYPES_FILE_IN_SYSTEM_ENV); 
#else
    typeFile.clear(); 
    typeFile.append(DEFAULT_SYSTEM_ENV_DIRECTORY); 
    typeFile.append(PATH_SEPARATOR); 
    typeFile.append(MIMETYPES_FILE_IN_SYSTEM_ENV); 
#endif 
    loadResources(MIMETYPES_RESOURCEKEY_SYSTEM, typeFile); 

    /**
     * Append The default mimetypes.
     */ 
    appendFileTypes(NULL, "message/rfc822",             " eml ");
    // add by henh 2012/7/20 16:13:31 for message types
    appendFileTypes(NULL, "message/delivery-status",             " txt ");
    // and end
    appendFileTypes(NULL, "application/andrew-inset",   " ez "); 
    appendFileTypes(NULL, "application/mac-binhex40",   " hqx "); 
    appendFileTypes(NULL, "application/mac-compactpro", " cpt "); 
    appendFileTypes(NULL, "application/msword",         " doc "); 
    appendFileTypes(NULL, "application/excel",          " xls "); 
    appendFileTypes(NULL, "application/octet-stream",   " exe com "); 
    appendFileTypes(NULL, "application/octet-stream",   " bin dms lha lzh class "); 
    appendFileTypes(NULL, "application/oda",            " oda "); 
    appendFileTypes(NULL, "application/pdf",            " pdf "); 
    appendFileTypes(NULL, "application/postscript",     " ai eps ps "); 
    appendFileTypes(NULL, "application/rtf",            " rtf "); 
    appendFileTypes(NULL, "application/smil",           " smi smil "); 
    appendFileTypes(NULL, "application/x-bcpio",        " bcpio "); 
    appendFileTypes(NULL, "application/x-bzip2",        " bz2 "); 
    appendFileTypes(NULL, "application/x-cdlink",       " vcd "); 
    appendFileTypes(NULL, "application/x-chess-pgn",    " pgn "); 
    appendFileTypes(NULL, "application/x-cpio",         " cpio "); 
    appendFileTypes(NULL, "application/x-csh",          " csh "); 
    appendFileTypes(NULL, "application/x-director",     " dcr dir dxr "); 
    appendFileTypes(NULL, "application/x-dvi",          " dvi "); 
    appendFileTypes(NULL, "application/x-futuresplash", " spl "); 
    appendFileTypes(NULL, "application/x-gtar",         " gtar "); 
    appendFileTypes(NULL, "application/x-gzip",         " gz tgz "); 
    appendFileTypes(NULL, "application/x-hdf",          " hdf "); 
    appendFileTypes(NULL, "application/x-javascript",   " js "); 
    appendFileTypes(NULL, "application/x-koan",         " skp skd skt skm "); 
    appendFileTypes(NULL, "application/x-latex",        " latex "); 
    appendFileTypes(NULL, "application/x-netcdf",       " nc cdf "); 
    appendFileTypes(NULL, "application/x-rpm",          " rpm "); 
    appendFileTypes(NULL, "application/x-sh",           " sh "); 
    appendFileTypes(NULL, "application/x-shar",         " shar "); 
    appendFileTypes(NULL, "application/x-shockwave-flash"," swf "); 
    appendFileTypes(NULL, "application/x-stuffit",      " sit "); 
    appendFileTypes(NULL, "application/x-sv4cpio",      " sv4cpio "); 
    appendFileTypes(NULL, "application/x-sv4crc",       " sv4crc "); 
    appendFileTypes(NULL, "application/x-tar",          " tar "); 
    appendFileTypes(NULL, "application/x-tcl",          " tcl "); 
    appendFileTypes(NULL, "application/x-tex",          " tex "); 
    appendFileTypes(NULL, "application/x-texinfo",      " texinfo texi "); 
    appendFileTypes(NULL, "application/x-troff",        " t tr roff "); 
    appendFileTypes(NULL, "application/x-troff-man",    " man "); 
    appendFileTypes(NULL, "application/x-troff-me",     " me "); 
    appendFileTypes(NULL, "application/x-troff-ms",     " ms "); 
    appendFileTypes(NULL, "application/x-ustar",        " ustar "); 
    appendFileTypes(NULL, "application/x-wais-source",  " src "); 
    appendFileTypes(NULL, "application/zip",            " zip "); 
    appendFileTypes(NULL, "application/rar",            " rar "); 
    appendFileTypes(NULL, "audio/basic",                " au snd "); 
    appendFileTypes(NULL, "audio/midi",                 " mid midi kar "); 
    appendFileTypes(NULL, "audio/mpeg",                 " mpga mp2 mp3 "); 
    appendFileTypes(NULL, "audio/x-aiff",               " aif aiff aifc "); 
    appendFileTypes(NULL, "audio/x-pn-realaudio",       " ram rm au "); 
    appendFileTypes(NULL, "audio/x-realaudio",          " ra "); 
    appendFileTypes(NULL, "audio/x-wav",                " wav "); 
    appendFileTypes(NULL, "audio/x-ms-wma",             " wma "); 
    appendFileTypes(NULL, "chemical/x-pdb",             " pdb xyz "); 
    appendFileTypes(NULL, "image/gif",                  " gif "); 
    appendFileTypes(NULL, "image/ief",                  " ief "); 
    appendFileTypes(NULL, "image/jpeg",                 " jpeg jpg jpe jfif "); 
    appendFileTypes(NULL, "image/png",                  " png "); 
    appendFileTypes(NULL, "image/tiff",                 " tiff tif ");  
    appendFileTypes(NULL, "image/x-cmu-raster",         " ras "); 
    appendFileTypes(NULL, "image/x-portable-anymap",    " pnm "); 
    appendFileTypes(NULL, "image/x-portable-bitmap",    " pbm "); 
    appendFileTypes(NULL, "image/x-portable-graymap",   " pgm "); 
    appendFileTypes(NULL, "image/x-portable-pixmap",    " ppm "); 
    appendFileTypes(NULL, "image/x-rgb",                " rgb "); 
    appendFileTypes(NULL, "image/x-xbitmap",            " xbm "); 
    appendFileTypes(NULL, "image/x-xwindowdump",        " xwd "); 
    // add by henh 2012/7/20 16:13:31 for image types
    appendFileTypes(NULL, "image/bmp",                  " bmp did ");
    appendFileTypes(NULL, "image/cis-cod",        " cod "); 
    appendFileTypes(NULL, "image/pipeg",        " pipeg "); 
    appendFileTypes(NULL, "image/svg+xml",        " svg "); 
    appendFileTypes(NULL, "image/x-cmx",        " cmx "); 
    appendFileTypes(NULL, "image/x-icon",        " icon "); 
    appendFileTypes(NULL, "image/x-xpixmap",        " xpm "); 
    // add end
    appendFileTypes(NULL, "model/iges",                 " igs iges "); 
    appendFileTypes(NULL, "model/mesh",                 " msh mesh silo "); 
    appendFileTypes(NULL, "model/vrml",                 " wrl vrml "); 
    appendFileTypes(NULL, "text/css",                   " css "); 
    appendFileTypes(NULL, "text/plain",                 " txt asc "); 
    appendFileTypes(NULL, "text/richtext",              " rtx "); 
    appendFileTypes(NULL, "text/rtf",                   " rtf "); 
    appendFileTypes(NULL, "text/sgml",                  " sgml sgm "); 
    appendFileTypes(NULL, "text/tab-separated-values",  " tsv "); 
    appendFileTypes(NULL, "text/x-setext",              " etx "); 
    appendFileTypes(NULL, "text/xml",                   " xml "); 
    appendFileTypes(NULL, "video/mpeg",                 " mpeg mpg mpe "); 
    appendFileTypes(NULL, "video/quicktime",            " qt mov "); 
    appendFileTypes(NULL, "video/x-msvideo",            " avi "); 
    appendFileTypes(NULL, "video/x-sgi-movie",          " movie "); 
    appendFileTypes(NULL, "video/x-ms-wmv",             " wmv "); 
    appendFileTypes(NULL, "video/x-ms-asf",             " asf "); 
    appendFileTypes(NULL, "video/vnd.rn-realvideo",     " rv "); 
    appendFileTypes(NULL, "x-conference/x-cooltalk",    " ice "); 
    appendFileTypes(NULL, "text/html",                  " html htm "); 
}

/**
 * Load the named file.
 */
void MimetypesFileTypeMap::loadFile(FastString &filename, FastString &filedata) 
{
    filedata.clear(); 
    if( filename.empty() ) 
        return; 

    struct stat statbuf; 
#if defined(_WIN32) || defined(__WIN32__)
    int infd = open(filename.c_str(), _O_RDONLY); 
#else
    int infd = open(filename.c_str(), O_RDONLY); 
#endif
    if( infd > 0 ) 
    {
        fstat(infd, &statbuf); 
        int filesize = statbuf.st_size; 
        
        char *pdata = (char *) malloc(filesize+1); 
        memset(pdata, 0, filesize); 
        
        read(infd, pdata, filesize); 
        filedata.set(pdata, filesize); 

        free(pdata); 

        close(infd); 
    }
}

/**
 * Make the fileTypes string line to good rule, example:
 *    "     gif   jpeg   bmp  jpg"   ---> " gif jpeg bmp jpg "
 */ 
void MimetypesFileTypeMap::appendFileTypes(
        const char *resourcekey, FastString &mimeType, FastString &fileTypes) 
{
    BOOL first = TRUE; 
    ShortString s, key; 
    ShortString mimeTypeValue(mimeType.c_str(), mimeType.length()); 
    mimeTypeValue.toLowerCase(); 

    for( size_t i = 0; i < fileTypes.length(); i ++ ) 
    {
        char c = fileTypes.charAt(i); 
        if( c == ' ' || c == '\t' || c == '\r' || c == '\n' ) 
        {
            if( !s.empty() ) 
            {
                key.clear(); 
                if( resourcekey != NULL && resourcekey[0] != '\0' ) 
                {
                    key.append(resourcekey); 
                    key.append('.'); 
                }
                s.toLowerCase(); 
                key.append(s); 

                if( first == TRUE ) 
                {
                    m_hmMimeTypesMap.put(mimeTypeValue, s); 
                    first = FALSE; 
                }

                m_hmMimeTypesMap.put(key, mimeTypeValue); 

                s.clear(); 
            }
            continue; 
        }

        s.append(c); 
    }

    if( !s.empty() ) 
    {
        key.clear(); 
        if( resourcekey != NULL && resourcekey[0] != '\0' ) 
        {
            key.append(resourcekey); 
            key.append('.'); 
        }
        s.toLowerCase(); 
        key.append(s); 
        s.clear(); 

        m_hmMimeTypesMap.put(key, mimeTypeValue); 
    }
}

/**
 * Load the mimetypes resource file. 
 * 
 * <b>MIME types file format:</b><p>
 *
 * <code>
 * # comments begin with a '#'<br>
 * # the format is &lt;mime type> &lt;space separated file extensions><br>
 * # for example:<br>
 * text/plain    txt text TXT<br>
 * # this would map file.txt, file.text, and file.TXT to<br>
 * # the mime type "text/plain"<br>
 * </code>
 */
void MimetypesFileTypeMap::loadResources(const char *resourcekey, FastString &filename) 
{
    FastString filedata; 
    loadFile(filename, filedata); 

    if( !filedata.empty() ) 
    {
        FastString line; 
        char *line_end = NULL; 
        char *line_start = (char *) MimeUtility::findStartLine(filedata.c_str(), filedata.length()); 
        char *buf_end    = (char *) (filedata.c_str() + filedata.length()); 

        while( (line_end = MimeUtility::findEndLine(line_start, buf_end - line_start)) > line_start ) 
        {
            line.set(line_start, line_end - line_start); 
            line.replaceChars("\t", ' '); 
            line.trim(); 

            if( line.length() > 0 && line.charAt(0) != '#' ) 
            {
                int pos = line.indexOf(' '); 
                if( pos > 0 ) 
                {
                    FastString sMimeType, sFileTypes; 
                    line.substring(sMimeType, 0, pos); 
                    line.substring(sFileTypes, pos); 

                    appendFileTypes(resourcekey, sMimeType, sFileTypes); 
                }
            }

            line_start = MimeUtility::findStartLine(line_end, buf_end - line_end); 
        }
    }
}

/**
 * Return the MIME type based on the specified file name.
 * The MIME type entries are searched as described above under
 * <i>MIME types file search order</i>.
 * If no entry is found, the type "application/octet-stream" is returned.
 *
 * @param filename      the file name
 * @param contentType   return he file's MIME type
 */
void MimetypesFileTypeMap::getContentType(FastString &filename, FastString &contentType) 
{
    MIME_THREAD_SYNCHRONIZE() 
    
    int pos = filename.lastIndexOf('.'); 

    if( filename.empty() || pos < 0 ) 
    {
        contentType = MimetypesFileTypeMap::DEFAULT_CONTENT_TYPE; 
        return; 
    }

    FastString fileExt; 
    filename.substring(fileExt, pos + 1); 
    fileExt.toLowerCase(); 

    if( fileExt.empty() ) 
    {
        contentType = MimetypesFileTypeMap::DEFAULT_CONTENT_TYPE; 
        return; 
    }

    ShortString key; 
    key.set(MIMETYPES_RESOURCEKEY_ENV); 
    key.append('.'); 
    key.append(fileExt.c_str(), fileExt.length()); 

    ShortString mimeType1 = m_hmMimeTypesMap.get(key); 
    if( !mimeType1.empty() ) 
    {
        contentType.set(mimeType1.c_str(), mimeType1.length()); 
        return; 
    }

    key.clear(); 
    key.set(MIMETYPES_RESOURCEKEY_USER); 
    key.append('.'); 
    key.append(fileExt.c_str(), fileExt.length()); 

    ShortString mimeType2 = m_hmMimeTypesMap.get(key); 
    if( !mimeType2.empty() ) 
    {
        contentType.set(mimeType2.c_str(), mimeType2.length()); 
        return; 
    }

    key.clear(); 
    key.set(MIMETYPES_RESOURCEKEY_SYSTEM); 
    key.append('.'); 
    key.append(fileExt.c_str(), fileExt.length()); 

    ShortString mimeType3 = m_hmMimeTypesMap.get(key); 
    if( !mimeType3.empty() ) 
    {
        contentType.set(mimeType3.c_str(), mimeType3.length()); 
        return; 
    }

    key.clear(); 
    key.append(fileExt.c_str(), fileExt.length()); 

    ShortString mimeType4 = m_hmMimeTypesMap.get(key); 
    if( !mimeType4.empty() ) 
    {
        contentType.set(mimeType4.c_str(), mimeType4.length()); 
        return; 
    }

    contentType = MimetypesFileTypeMap::DEFAULT_CONTENT_TYPE; 
    return; 
}

/**
 * Return the file extend name with the specified file name.
 *
 * @param mimetype      the file's MIME type
 * @param ext           return the file's extend name
 */
void MimetypesFileTypeMap::getFileExtend(FastString &mimetype, FastString &ext) 
{
    MIME_THREAD_SYNCHRONIZE() 
    
    ext.clear(); 
    if( mimetype.empty() ) 
        return; 

    ShortString key(mimetype.c_str(), mimetype.length()); 
    ShortString sFileExt = m_hmMimeTypesMap.get(key); 

    if( !sFileExt.empty() ) 
    {
        ext.set(sFileExt.c_str(), sFileExt.length()); 
    }
}



_FASTMIME_END_NAMESPACE
