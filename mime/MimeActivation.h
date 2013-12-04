//=============================================================================
/**
 *  @file    MimeActivation.h
 *
 *  ver 1.0.0 2004/06/16 Naven Exp, for Mime Message Parse Engine.
 *
 *  @author Naven 2004/06/16 created.
 */
//=============================================================================

#ifndef _MIMEACTIVATION_H
#define _MIMEACTIVATION_H

#if defined(_WIN32) || defined(__WIN32__)

#if !defined (NAVEN_PRAGMA_ONCE)
# pragma once
#endif /* NAVEN_PRAGMA_ONCE */

#endif /* _WIN32 */


#if defined(_REENTRANT) || defined(_POSIX_PTHREAD_SEMANTICS) 
    #define MIME_THREADSAFE_ENABLE 
#endif 


#if defined(_WIN32) || defined(__WIN32__) 
    #undef MIME_THREADSAFE_ENABLE
    
#else /* UNIX */
    #include <unistd.h>
    
#ifdef MIME_THREADSAFE_ENABLE
    #include <pthread.h> 
#endif 
    
#endif 

#include <time.h>
#include "MimeBase.h" 


_FASTMIME_BEGIN_NAMESPACE



//=============class ThreadSync define==============

/**
 * Thread safe synchronization for singleton object.
 * use posix thread default.
 */
#ifdef MIME_THREADSAFE_ENABLE

class ThreadMutex
{
private: 
    pthread_mutex_t _mutex; 
public: 
    ThreadMutex() { pthread_mutex_init(&_mutex, NULL); } 
   ~ThreadMutex() { pthread_mutex_destroy(&_mutex); } 
    void lock()   { pthread_mutex_lock(&_mutex); } 
    void unlock() { pthread_mutex_unlock(&_mutex); } 
}; 

class ThreadSync
{
private:
    ThreadMutex &_tmMutex; 
public: 
    ThreadSync(ThreadMutex &mutex) : _tmMutex(mutex) { _tmMutex.lock(); } 
    ~ThreadSync() { _tmMutex.unlock(); } 
};

#define DECLARE_MIME_THREAD_MUTEX()      static ThreadMutex _mutex; 
#define IMPLEMENT_MIME_THREAD_MUTEX(xxx) ThreadMutex xxx::_mutex = ThreadMutex(); 
#define MIME_THREAD_SYNCHRONIZE()        ThreadSync _sync(_mutex); 

/**
 * Thread safe not enable
 */ 
#else

#define DECLARE_MIME_THREAD_MUTEX()      
#define IMPLEMENT_MIME_THREAD_MUTEX(xxx) 
#define MIME_THREAD_SYNCHRONIZE()   

#endif 


//=============class SystemProperty define==============

/**
 * Get all the system environment varibales.
 * This class use Singleton-Pattern to initialize, you cannot use default
 * contructor but only SystemProperty.getInstance() to get/create instance. 
 * For only create one instance in a process.
 * You can use it to get all the properties of system environment by 
 * <p><pre>
 * SytemtProperty.getProperty("name"); 
 * </pre>
 * 
 * @see  java.system
 */
class SystemProperty
{
public:
    // Traits
    typedef FastHashMap<ShortString,FastString> PropertyHashMap; 

private:
    static SystemProperty *m_pInstance; 
    PropertyHashMap m_hmProperties; 
    
    DECLARE_MIME_THREAD_MUTEX()

    SystemProperty() { loadAll(); }
    void loadAll(); 

public:
    static SystemProperty& getInstance(); 
    static void closeSingleton(); 
    const FastString& getProperty(ShortString &name); 
    const FastString& getProperty(const ShortString &name); 
    const FastString& getProperty(ShortString &name, const FastString &def); 
    const FastString& getProperty(const ShortString &name, const FastString &def); 
    const char *getProperty(const char *name); 
    const char *getProperty(const char *name, const char *def); 
    const char *get(const char *name) { return getProperty(name); } 
    const char *get(const char *name, const char *def) { return getProperty(name, def); } 
    void dump(); 
};

//============SystemProperty Functions Implements================

/**
 * Gets the system property indicated by the specified key.
 *
 * @param name  Property name
 * @return  Property value by reference
 */
inline const FastString& 
SystemProperty::getProperty(const ShortString &name) 
{
    ShortString sName(name); 
    return getProperty(sName); 
} 

/**
 * Gets the system property indicated by the specified key.
 *
 * @param name  Property name
 * @return  Property value by reference
 */
inline const char * 
SystemProperty::getProperty(const char *name) 
{
    ShortString sName(name); 
    return getProperty(sName).c_str(); 
} 

/**
 * Gets the system property indicated by the specified key.
 * If not found or empty return the default.
 *
 * @param name  Property name
 * @param def   Property default value
 * @return  Property value by reference
 */
inline const FastString& 
SystemProperty::getProperty(ShortString &name, const FastString &def) 
{
    const FastString &value = getProperty(name); 
    if( value.empty() ) 
        return def; 
    else 
        return value; 
} 

/**
 * Gets the system property indicated by the specified key.
 * If not found or empty return the default.
 *
 * @param name  Property name
 * @param def   Property default value
 * @return  Property value by reference
 */
inline const FastString& 
SystemProperty::getProperty(const ShortString &name, const FastString &def) 
{
    ShortString sName(name); 
    return getProperty(sName, def); 
} 

/**
 * Gets the system property indicated by the specified key.
 * If not found or empty return the default.
 *
 * @param name  Property name
 * @param def   Property default value
 * @return  Property value by reference
 */
inline const char *
SystemProperty::getProperty(const char *name, const char *def) 
{
    ShortString sName(name); 
    const FastString &value = getProperty(sName); 
    if( value.empty() ) 
        return def; 
    else 
        return value.c_str(); 
} 


/**
 * Dump the object state.
 */
inline void SystemProperty::dump() 
{
    FAST_TRACE_BEGIN("SystemProperty::dump()"); 
    FAST_TRACE("sizeof(SystemProperty) = %d", sizeof(SystemProperty)); 
    FAST_TRACE("m_hmProperties.size() = %d", m_hmProperties.size()); 
    FAST_TRACE("m_pInstance -> 0x%08X", m_pInstance); 
#ifdef FAST_DEBUG
    PropertyHashMap::iterator it = m_hmProperties.begin(); 
    while( !it.done() ) {
        FAST_TRACE(" %s = %s", it->first().c_str(), it->second().c_str()); 
        it.advance(); 
    }
#endif
    FAST_TRACE_END("SystemProperty::dump()"); 
}


//=============class MimetypesFileTypeMap define==============

/**
 * This class extends FileTypeMap and provides data typing of files
 * via their file extension. It uses the <code>.mime.types</code> format. <p>
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
 *
 */
class MimetypesFileTypeMap
{
public: 
    // Traits.
    typedef FastHashMap<ShortString,ShortString> TypesHashMap; 
    typedef TypesHashMap::iterator iterator; 

private: 
    /*
     * We manage a collection of databases, searched in order.
     * The default database is shared between all instances
     * of this class.
     * XXX - Can we safely share more databases between instances?
     */
    TypesHashMap m_hmMimeTypesMap; 

    // the singleton instance.
    static MimetypesFileTypeMap *m_pInstance; 
    static const char *DEFAULT_CONTENT_TYPE; 
    
    DECLARE_MIME_THREAD_MUTEX()

    void loadFile(FastString &filename, FastString &filedata); 
    void appendFileTypes(const char *resourcekey, FastString &mimeType, FastString &fileTypes); 
    void appendFileTypes(const char *resourcekey, const char *mimeType, const char *fileTypes); 
    void loadResources(const char *resourcekey, FastString &filename); 
    void loadAllResources(); 
    MimetypesFileTypeMap(); 

public: 
    static MimetypesFileTypeMap& getInstance(); 
    static void closeSingleton(); 
    void getContentType(FastString &filename, FastString &contentType); 
    void getContentType(const char *filename, FastString &contentType); 
    void getFileExtend(FastString &mimetype, FastString &ext); 
    void getFileExtend(const char *mimetype, FastString &ext); 
    void dump(); 

}; 

//============MimetypesFileTypeMap Inline Implements================

/**
 * Default Constructor.
 */
inline MimetypesFileTypeMap::MimetypesFileTypeMap()
{
    loadAllResources(); 
}

/**
 * Make the fileTypes string line to good rule, example:
 *    "     gif   jpeg   bmp  jpg"   ---> " gif jpeg bmp jpg "
 */ 
inline void MimetypesFileTypeMap::appendFileTypes(
        const char *resourcekey, const char *mimeType, const char *fileTypes) 
{
    FastString sMimeType(mimeType), sFileTypes(fileTypes); 
    appendFileTypes(resourcekey, sMimeType, sFileTypes); 
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
inline void MimetypesFileTypeMap::getContentType(const char *filename, FastString &contentType) 
{
    FastString sFileName(filename); 
    getContentType(sFileName, contentType); 
}

/**
 * Return the file extend name with the specified file name.
 *
 * @param mimetype  the file's MIME type
 * @param ext       return the file's extend name
 */
inline void MimetypesFileTypeMap::getFileExtend(const char *mimetype, FastString &ext) 
{
    FastString sMimeType(mimetype); 
    getFileExtend(sMimeType, ext); 
}

/**
 * Dump the object state.
 */
inline void MimetypesFileTypeMap::dump() 
{
    FAST_TRACE_BEGIN("MimetypesFileTypeMap::dump()"); 
    FAST_TRACE("sizeof(MimetypesFileTypeMap) = %d", sizeof(MimetypesFileTypeMap)); 
    FAST_TRACE("m_hmMimeTypesMap.size() = %d", m_hmMimeTypesMap.size()); 
#ifdef FAST_DEBUG
    iterator it = m_hmMimeTypesMap.begin(); 
    while( !it.done() ) {
        FAST_TRACE("MimeTypes: %s = %s", it->first().c_str(), it->second().c_str()); 
        it.advance(); 
    }
#endif 
    FAST_TRACE("m_hmMimeTypesMap.size() = %d", m_hmMimeTypesMap.size()); 
    FAST_TRACE_END("MimetypesFileTypeMap::dump()"); 
}


//=============class MimeInitialization define==============

/**
 * This class use to inilializate all the global static varibales 
 * for MimeMessage by create a global MimeInitialization varibales.
 * It while construct before main().
 *
 * This is a package private class.
 *
 */
class MimeInitialization
{
public:
    MimeInitialization(); 
    ~MimeInitialization(); 
};


//=============MimeInitialization inline functions===============

/**
 * This function inilializate all the global static varibales 
 * for MimeMessage by create a global MimeInitialization varibales.
 * It while construct before main().
 */
inline MimeInitialization::MimeInitialization() 
{
    ::tzset();  // update CRT timezone to default, ENV variable: SET TZ=GST+08:00

    SystemProperty &sysprops = SystemProperty::getInstance(); 

    // ENV_FASTMAIL_CHARSET for MimeUtility::m_sDefaultMIMECharset
    const FastString &charset = sysprops.getProperty(ENV_FASTMAIL_CHARSET); 
    if( !charset.empty() ) 
        MimeUtility::setDefaultMIMECharset(charset); 

    // ENV_FASTMAIL_PART for UniqueValue::PART
    const FastString &part = sysprops.getProperty(ENV_FASTMAIL_PART); 
    if( !part.empty() ) 
        UniqueValue::PART = part.c_str(); 

    // ENV_FASTMAIL_NAME for UniqueValue::FASTMAIL
    const FastString &fastmail = sysprops.getProperty(ENV_FASTMAIL_NAME); 
    if( !part.empty() ) 
        UniqueValue::FASTMAIL = fastmail.c_str(); 

    // Initialize MimeTypes FileTypes
    MimetypesFileTypeMap &map = MimetypesFileTypeMap::getInstance(); 

}

/**
 * This function destruct and free all the global static varibales.
 */
inline MimeInitialization::~MimeInitialization() 
{
    SystemProperty::closeSingleton(); 
    MimetypesFileTypeMap::closeSingleton(); 
}


_FASTMIME_END_NAMESPACE

#endif
