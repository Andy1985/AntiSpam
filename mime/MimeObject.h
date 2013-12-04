//=============================================================================
/**
 *  @file    MimeObject.h
 *
 *  ver 1.0.0 2004/06/16 Naven Exp, for Mime Message Parse Engine.
 *
 *  @author Naven 2004/06/16 created.
 */
//=============================================================================

#ifndef _MIMEOBJECT_H
#define _MIMEOBJECT_H

#if defined(_WIN32) || defined(__WIN32__)

#if !defined (NAVEN_PRAGMA_ONCE)
# pragma once
#endif /* NAVEN_PRAGMA_ONCE */

#endif /* _WIN32 */


#include "MimeBase.h" 
#include "MimeUtility.h"


_FASTMIME_BEGIN_NAMESPACE



//=============Class ParameterList define===================

// Hash functions define
template<class CHART> struct Fast_HashIgnoreCase { };

// Hash functions ignore case only for FastString
template<typename CHAR, size_t BUFSIZE> 
struct Fast_HashIgnoreCase<_FAST::FastString_Base<CHAR,BUFSIZE> > {
    size_t operator()(const _FAST::FastString_Base<CHAR,BUFSIZE> &s) const { 
        _FAST::FastString_Base<CHAR,BUFSIZE> s2(s); s2.toLowerCase(); 
        return fast_hash_string(s2.c_str()); 
    }
};
template<typename CHAR, size_t BUFSIZE> 
struct Fast_HashIgnoreCase<const _FAST::FastString_Base<CHAR,BUFSIZE> > {
    size_t operator()(const _FAST::FastString_Base<CHAR,BUFSIZE> &s) const { 
        _FAST::FastString_Base<CHAR,BUFSIZE> s2(s); s2.toLowerCase(); 
        return fast_hash_string(s2.c_str()); 
    }
};

// HashMap Key equal function ( ignore case ) only for ParameterList 
template<class T>
struct Fast_EqualIgnoreCase_To : public Fast_Binary_Function<T,T,BOOL> 
{
    BOOL operator()(const T& x, const T& y) const { return x.equalsIgnoreCase(y) ? TRUE : FALSE; }
};

/**
 * This class holds MIME parameters (attribute-value pairs).
 *
 */
class ParameterList 
{
public: 
    // Traits 
    typedef SmallString NameString; 
    typedef ShortString ValueString; 
    typedef FastArray<NameString> NameArray; 
    typedef FastHashMap<NameString, ValueString, 
                        Fast_HashIgnoreCase<NameString>, 
                        Fast_EqualIgnoreCase_To<NameString> > 
            ParamHashMap; 
    typedef ParamHashMap::iterator iterator; 

private: 
    ParamHashMap m_hmList;                          // internal hashtable
    int initialize(FastString &s); 
    void quote(FastString &word); 

public:
    ParameterList() {}                              // No-arg Constructor.
    ParameterList(ParameterList &list) : m_hmList(list.m_hmList) {} 
    ParameterList(FastString &s) { initialize(s); } // Constructor that takes a parameter-list string.
    ParameterList(const char *s, int length = 0); 
    void load(FastString &s) { initialize(s); }  
    void load(const char *s, int length = -1); 
    size_t size() { return m_hmList.size(); }       // the number of parameters in this list.
    iterator begin() { return m_hmList.begin(); } 
    iterator end() { return m_hmList.end(); } 
    const ValueString& get(FastString &name); 
    const ValueString& get(const char *name); 
    void get(FastString &name, FastString &value); 
    void get(const char *name, FastString &value); 
    void set(FastString &name, FastString &value); 
    void set(const char *name, FastString &value); 
    void set(const char *name, const char *value); 
    void remove(FastString &name); 
    void remove(const char *name); 
    int  getNames(NameArray &names); 
    void toString(FastString &s, int used = 0); 
    void dump(); 
};

//=============ParameterList inline functions============

/**
 * Constructor that takes a parameter-list string.
 */
inline ParameterList::ParameterList(const char *s, int length) 
{ 
    load(s, length); 
} 

/**
 * Load that takes a parameter-list string.
 */
inline void ParameterList::load(const char *s, int length) 
{ 
    if( length <= 0 ) 
        length = s ? ::strlen(s) : 0; 
    FastString str(s, length); 
    initialize(str); 
} 

/**
 * Quote a parameter value token if required.
 */
inline void ParameterList::quote(FastString &word) 
{
    MimeUtility::quote(word, HeaderTokenizer::MIME);
}

/**
 * Returns the value of the specified parameter. Note that 
 * parameter names are case-insensitive.
 *
 * @param name    parameter name.
 * @return        Value of the parameter. Returns 
 *            <code>empty</code> if the parameter is not 
 *            present.
 */
inline const ParameterList::ValueString& ParameterList::get(FastString &name) 
{
    NameString sName(name.c_str(), name.length()); 
    sName.trim(); 
    return m_hmList.get(sName);
}

/**
 * Returns the value of the specified parameter. Note that 
 * parameter names are case-insensitive.
 *
 * @param name    parameter name.
 * @return        Value of the parameter. Returns 
 *            <code>empty</code> if the parameter is not 
 *            present.
 */
inline const ParameterList::ValueString& ParameterList::get(const char *name) 
{
    NameString sName(name); 
    sName.trim(); 
    return m_hmList.get(sName);
}

/**
 * Returns the value of the specified parameter. Note that 
 * parameter names are case-insensitive.
 *
 * @param name    parameter name.
 * @param value   Value of the parameter. Returns 
 *            <code>empty</code> if the parameter is not 
 *            present.
 */
inline void ParameterList::get(FastString &name, FastString &value) 
{
    const ValueString &val = this->get(name); 
    value.set(val.c_str(), val.length());
}

/**
 * Returns the value of the specified parameter. Note that 
 * parameter names are case-insensitive.
 *
 * @param name    parameter name.
 * @param value   Value of the parameter. Returns 
 *            <code>empty</code> if the parameter is not 
 *            present.
 */
inline void ParameterList::get(const char *name, FastString &value) 
{
    const ValueString &val = this->get(name); 
    value.set(val.c_str(), val.length());
}

/**
* Set a parameter. If this parameter already exists, it is
* replaced by this new value.
*
* @param    name     name of the parameter.
* @param    value    value of the parameter.
*/
inline void ParameterList::set(FastString &name, FastString &value) 
{
    NameString  sName(name.c_str(), name.length()); 
    ValueString sValue(value.c_str(), value.length()); 
    m_hmList.put(sName, sValue);
}

/**
* Set a parameter. If this parameter already exists, it is
* replaced by this new value.
*
* @param    name     name of the parameter.
* @param    value    value of the parameter.
*/
inline void ParameterList::set(const char *name, FastString &value) 
{
    NameString  sName(name); 
    ValueString sValue(value.c_str(), value.length()); 
    m_hmList.put(sName, sValue);
}

/**
* Set a parameter. If this parameter already exists, it is
* replaced by this new value.
*
* @param    name     name of the parameter.
* @param    value    value of the parameter.
*/
inline void ParameterList::set(const char *name, const char *value) 
{
    NameString  sName(name); 
    ValueString sValue(value); 
    m_hmList.put(sName, sValue);
}

/**
 * Removes the specified parameter from this ParameterList.
 * This method does nothing if the parameter is not present.
 *
 * @param    name    name of the parameter.
 */
inline void ParameterList::remove(FastString &name) 
{
    NameString sName(name.c_str(), name.length()); 
    sName.trim(); 
    m_hmList.remove(sName);
}

/**
 * Removes the specified parameter from this ParameterList.
 * This method does nothing if the parameter is not present.
 *
 * @param    name    name of the parameter.
 */
inline void ParameterList::remove(const char *name) 
{
    NameString sName(name); 
    sName.trim(); 
    m_hmList.remove(sName);
}

/**
 * Dump the object state.
 */
inline void ParameterList::dump() 
{
    FAST_TRACE_BEGIN("ParameterList::dump()"); 
    FAST_TRACE("sizeof(ParameterList) = %d", sizeof(ParameterList)); 
    FAST_TRACE("m_hmList.size() = %d", m_hmList.size()); 
#ifdef FAST_DEBUG
    iterator it = this->begin(); 
    while( !it.done() ) {
        FAST_TRACE("ParameterList: %s = %s", it->first().c_str(), it->second().c_str()); 
        it.advance(); 
    }
#endif 
    FAST_TRACE("sizeof(ParameterList) = %d", sizeof(ParameterList)); 
    FAST_TRACE("m_hmList.size() = %d", m_hmList.size()); 
    FAST_TRACE_END("HeaderTokenizer::dump()"); 
}


//=============class ContentType define=================

/**
 * This class represents a MIME ContentType value. It provides
 * methods to parse a ContentType string into individual components
 * and to generate a MIME style ContentType string.
 *
 */
class ContentType 
{
public: 
    // Traits 
    typedef SmallString PrimaryString; 
    typedef SmallString SubtypeString; 

protected: 
    PrimaryString   m_sPrimaryType;     // primary type
    SubtypeString   m_sSubType;         // subtype
    ParameterList   m_pList;            // parameter list

    int initialize(FastString &s); 

public:
    ContentType() {}                    // No-arg Constructor.
    ContentType(FastString &primaryType, FastString &subType); 
    ContentType(const char *primaryType, const char *subType); 
    ContentType(FastString &primaryType, FastString &subType, ParameterList &list); 
    ContentType(const char *primaryType, const char *subType, ParameterList &list); 
    ContentType(FastString &s) { initialize(s); } 
    ContentType(const char *s, int length = -1); 
    void load(FastString &s) { initialize(s); } 
    void load(const char *s, int length = -1); 
    const char *primaryType() { return m_sPrimaryType.c_str(); } 
    const PrimaryString& getPrimaryType(); 
    void getPrimaryType(FastString &s); 
    void setPrimaryType(FastString &s); 
    void setPrimaryType(const char *s); 
    const char *subType() { return m_sSubType.c_str(); } 
    const SubtypeString& getSubType(); 
    void getSubType(FastString &s); 
    void setSubType(FastString &s); 
    void setSubType(const char *s); 
    FastString getBaseType(); 
    void getBaseType(FastString &s); 
    const ParameterList::ValueString& getParameter(FastString &name); 
    void getParameter(FastString &name, FastString &value); 
    void getParameter(const char *name, FastString &value); 
    void setParameter(FastString &name, FastString &value); 
    void setParameter(const char *name, const char *value); 
    ParameterList& getParameterList(); 
    void getParameterList(ParameterList &list); 
    void setParameterList(ParameterList &list); 
    BOOL match(ContentType &cType); 
    BOOL match(FastString &s); 
    BOOL match(const char *s, int length = -1); 
    void toString(FastString &s); 
    void dump(); 
};

//=============ContentType Inline Implements=============

/**
 * Constructor.
 *
 * @param primaryType   primary type
 * @param subType       subType
 */
inline ContentType::ContentType(FastString &primaryType, FastString &subType) 
 : m_sPrimaryType(PrimaryString(primaryType.c_str(), primaryType.length())), 
   m_sSubType(SubtypeString(subType.c_str(), subType.length())), 
   m_pList() 
{
}

/**
 * Constructor.
 *
 * @param primaryType   primary type
 * @param subType       subType
 */
inline ContentType::ContentType(const char *primaryType, const char *subType) 
 : m_sPrimaryType(PrimaryString(primaryType)), 
   m_sSubType(SubtypeString(subType)), 
   m_pList() 
{
}

/**
 * Constructor.
 *
 * @param primaryType   primary type
 * @param subType       subType
 * @param list          ParameterList
 */
inline ContentType::ContentType(FastString &primaryType, FastString &subType, 
                                ParameterList &list) 
 : m_sPrimaryType(PrimaryString(primaryType.c_str(), primaryType.length())), 
   m_sSubType(SubtypeString(subType.c_str(), subType.length())), 
   m_pList(list) 
{
}

/**
 * Constructor.
 *
 * @param primaryType   primary type
 * @param subType       subType
 * @param list          ParameterList
 */
inline ContentType::ContentType(const char *primaryType, const char *subType, 
                                ParameterList &list) 
 : m_sPrimaryType(PrimaryString(primaryType)), 
   m_sSubType(SubtypeString(subType)), 
   m_pList(list) 
{
}

/**
 * Constructor.
 *
 * @param s   content string
 * @param length  content string length
 */
inline ContentType::ContentType(const char *s, int length) 
{
    this->load(s, length); 
}

/**
 * Load initialize.
 *
 * @param s   content string
 * @param length  content string length
 */
inline void ContentType::load(const char *s, int length) 
{
    if( length <= 0 ) 
        length = s ? ::strlen(s) : 0; 
    FastString str(s, length); 
    initialize(str); 
}

/**
 * Return the primary type.
 *
 * @return the primary type
 */
inline const ContentType::PrimaryString& ContentType::getPrimaryType() 
{
    return m_sPrimaryType;
}

/**
 * Get the primary type.
 *
 * @s the primary type
 */
inline void ContentType::getPrimaryType(FastString &s) 
{
    s.set(m_sPrimaryType.c_str(), m_sPrimaryType.length());
}

/**
 * Set the primary type. Overrides existing primary type.
 * 
 * @param s  primary type
 */
inline void ContentType::setPrimaryType(FastString &s) 
{
    m_sPrimaryType.set(s.c_str(), s.length());
}

/**
 * Set the primary type. Overrides existing primary type.
 * 
 * @param s  primary type
 */
inline void ContentType::setPrimaryType(const char *s) 
{
    m_sPrimaryType.set(s);
}

/**
 * Return the subType.
 *
 * @return the subType
 */
inline const ContentType::SubtypeString& ContentType::getSubType() 
{
    return m_sSubType;
}

/**
 * Get the subType.
 *
 * @s the subType
 */
inline void ContentType::getSubType(FastString &s) 
{
    s.set(m_sSubType.c_str(), m_sSubType.length());
}

/**
 * Set the subType.  Replaces the existing subType.
 *
 * @param s  the subType
 */
inline void ContentType::setSubType(FastString &s) 
{
    m_sSubType.set(s.c_str(), s.length()); 
}

/**
 * Set the subType.  Replaces the existing subType.
 *
 * @param s  the subType
 */
inline void ContentType::setSubType(const char *s) 
{
    m_sSubType.set(s); 
}

/**
 * Return the MIME type string, without the parameters.
 * The returned value is basically the concatenation of
 * the primaryType, the '/' character and the secondaryType.
 *
 * @s the type
 */
inline void ContentType::getBaseType(FastString &s) 
{
    s.clear(); 
    s.append(m_sPrimaryType.c_str(), m_sPrimaryType.length()); 
    s.append('/'); 
    s.append(m_sSubType.c_str(), m_sSubType.length());
}

/**
 * Return the specified parameter value. Returns <code>empty</code>
 * if this parameter is absent.
 * 
 * @return    parameter value
 */
inline const ParameterList::ValueString& ContentType::getParameter(FastString &name) 
{
    return m_pList.get(name);
}

/**
 * Get the specified parameter value. Returns <code>empty</code>
 * if this parameter is absent.
 * 
 * @param    name    parameter name
 * @param    value   parameter value
 */
inline void ContentType::getParameter(FastString &name, FastString &value) 
{
    m_pList.get(name, value);
}

/**
 * Get the specified parameter value. Returns <code>empty</code>
 * if this parameter is absent.
 * 
 * @param    name    parameter name
 * @param    value   parameter value
 */
inline void ContentType::getParameter(const char *name, FastString &value) 
{
    m_pList.get(name, value);
}

/**
 * Set the specified parameter. If this parameter already exists,
 * it is replaced by this new value.
 *
 * @param    name    parameter name
 * @param    value   parameter value
 */
inline void ContentType::setParameter(FastString &name, FastString &value) 
{
    m_pList.set(name, value);
}

/**
 * Set the specified parameter. If this parameter already exists,
 * it is replaced by this new value.
 *
 * @param    name    parameter name
 * @param    value   parameter value
 */
inline void ContentType::setParameter(const char *name, const char *value) 
{
    m_pList.set(name, value);
}

/**
 * Return a ParameterList object that holds all the available 
 * parameters. Returns empty if no parameters are available.
 *
 * @return    ParameterList
 */
inline ParameterList& ContentType::getParameterList() 
{
    return m_pList;
}

/**
 * Get a ParameterList object that holds all the available 
 * parameters. Returns empty if no parameters are available.
 *
 * @param list    ParameterList
 */
inline void ContentType::getParameterList(ParameterList &list) 
{
    list = m_pList;
}

/**
 * Set a ParameterList object that holds all the available 
 * parameters. 
 *
 * @param list    ParameterList
 */
inline void ContentType::setParameterList(ParameterList &list) 
{
    m_pList = list;
}

/**
 * Match with the specified content-type string. This method
 * compares <strong>only the <code>primaryType</code> and 
 * <code>subType</code> </strong>.
 * The parameters of both operands are ignored. <p>
 *
 * For example, this method will return <code>true</code> when
 * comparing the ContentType for <strong>"text/plain"</strong>
 * with <strong>"text/plain; charset=foobar"</strong>.
 *
 * If the <code>subType</code> of either operand is the special 
 * character '*', then the subtype is ignored during the match. 
 * For example, this method will return <code>true</code> when 
 * comparing the ContentType for <strong>"text/plain"</strong> 
 * with <strong>"text/" </strong>
 */
inline BOOL ContentType::match(FastString &s) 
{
    ContentType cType(s); 
    return this->match(cType); 
}

/**
 * Match with the specified content-type string, 
 * like match(FastString &s).
 */

inline BOOL ContentType::match(const char *s, int length) 
{
    ContentType cType(s, length); 
    return this->match(cType); 
}

/**
 * Dump the object state.
 */
inline void ContentType::dump() 
{
    FAST_TRACE_BEGIN("ContentType::dump()"); 
    FAST_TRACE("sizeof(ContentType) = %d", sizeof(ContentType)); 
    FAST_TRACE("m_sPrimaryType = %s", m_sPrimaryType.c_str()); 
    FAST_TRACE("m_sSubType = %s", m_sSubType.c_str()); 
    FAST_TRACE("m_pList.size() = %d", m_pList.size()); 
#ifdef FAST_DEBUG
    ParameterList::iterator it = m_pList.begin(); 
    while( !it.done() ) {
        FAST_TRACE("ContentType: %s = %s", it->first().c_str(), it->second().c_str()); 
        it.advance(); 
    }
#endif 
}


//=============class MimeType define=================
/**
 * A Multipurpose Internet Mail Extension (MIME) type, as defined
 * in RFC 2045 and 2046.
 */
class MimeType : public ContentType 
{
public: 
    MimeType() {}                    // No-arg Constructor.
    MimeType(FastString &primaryType, FastString &subType); 
    MimeType(const char *primaryType, const char *subType); 
    MimeType(FastString &primaryType, FastString &subType, ParameterList &list); 
    MimeType(const char *primaryType, const char *subType, ParameterList &list); 
    MimeType(FastString &s) { ContentType::initialize(s); } 
    MimeType(const char *s, int length = -1); 
    void dump(); 

};

//=============MimeType Inline Implements=============

/**
 * Constructor.
 *
 * @param primaryType   primary type
 * @param subType       subType
 */
inline MimeType::MimeType(FastString &primaryType, FastString &subType) 
 : ContentType(primaryType, subType) 
{
}

/**
 * Constructor.
 *
 * @param primaryType   primary type
 * @param subType       subType
 */
inline MimeType::MimeType(const char *primaryType, const char *subType) 
 : ContentType(primaryType, subType)  
{
}

/**
 * Constructor.
 *
 * @param primaryType   primary type
 * @param subType       subType
 * @param list          ParameterList
 */
inline MimeType::MimeType(FastString &primaryType, FastString &subType, 
                                ParameterList &list) 
 : ContentType(primaryType, subType, list) 
{
}

/**
 * Constructor.
 *
 * @param primaryType   primary type
 * @param subType       subType
 * @param list          ParameterList
 */
inline MimeType::MimeType(const char *primaryType, const char *subType, 
                                ParameterList &list) 
 : ContentType(primaryType, subType, list) 
{
}

/**
 * Constructor.
 *
 * @param s   content string
 * @param length  content string length
 */
inline MimeType::MimeType(const char *s, int length) 
 : ContentType(s, length) 
{
}

/**
 * Dump the object state.
 */
inline void MimeType::dump() 
{
    FAST_TRACE_BEGIN("MimeType::dump()"); 
    FAST_TRACE("sizeof(MimeType) = %d", sizeof(MimeType)); 
    FAST_TRACE("m_sPrimaryType = %s", m_sPrimaryType.c_str()); 
    FAST_TRACE("m_sSubType = %s", m_sSubType.c_str()); 
    FAST_TRACE("m_pList.size() = %d", m_pList.size()); 
#ifdef FAST_DEBUG
    ParameterList::iterator it = m_pList.begin(); 
    while( !it.done() ) {
        FAST_TRACE("ContentType: %s = %s", it->first().c_str(), it->second().c_str()); 
        it.advance(); 
    }
#endif 
    FAST_TRACE_END("MimeType::dump()"); 
}


//=============class ContentDisposition define=================

/**
 * This class represents a MIME ContentDisposition value. It provides
 * methods to parse a ContentDisposition string into individual components
 * and to generate a MIME style ContentDisposition string.
 *
 */

class ContentDisposition 
{
public:
    // Traits 
    typedef ShortString DispositionString; 

protected: 
    DispositionString m_sDisposition;   // disposition
    ParameterList     m_pList;          // parameter list

    int initialize(FastString &s); 

public: 
    ContentDisposition() { }            // No-arg Constructor.
    ContentDisposition(FastString &disp, ParameterList &list); 
    ContentDisposition(const char *disp, ParameterList &list); 
    ContentDisposition(FastString &s) { initialize(s); } 
    ContentDisposition(const char *s); 
    DispositionString& getDisposition(); 
    void getDisposition(FastString &s); 
    void setDisposition(FastString &s); 
    void setDisposition(const char *s); 
    const ParameterList::ValueString& getParameter(FastString &name); 
    void getParameter(FastString &name, FastString &value); 
    void getParameter(const char *name, FastString &value); 
    void setParameter(FastString &name, FastString &value); 
    void setParameter(const char *name, const char *value); 
    ParameterList& getParameterList(); 
    void getParameterList(ParameterList &list); 
    void setParameterList(ParameterList &list); 
    void toString(FastString &s); 
    void dump(); 
};

//=============ContentDisposition Inline Implements=============

/**
 * Constructor.
 *
 * @param disposition   disposition
 * @param list          ParameterList
 */
inline ContentDisposition::ContentDisposition(FastString &disp, ParameterList &list) 
 : m_sDisposition(DispositionString(disp.c_str(), disp.length())), 
   m_pList(list) 
{
}

/**
 * Constructor.
 *
 * @param disposition   disposition
 * @param list          ParameterList
 */
inline ContentDisposition::ContentDisposition(const char *disp, ParameterList &list) 
: m_sDisposition(DispositionString(disp)), 
  m_pList(list) 
{
}

/**
 * Constructor that takes a ContentDisposition string. The String
 * is parsed into its constituents: dispostion and parameters. 
 * A ParseException is thrown if the parse fails. 
 *
 * @param s    the ContentDisposition string.
 */
inline ContentDisposition::ContentDisposition(const char *s) 
{
    FastString str(s); 
    initialize(str); 
}

/**
 * Return the disposition value.
 *
 * @return the disposition
 */
inline ContentDisposition::DispositionString& ContentDisposition::getDisposition() 
{
    return m_sDisposition;
}

/**
 * Get the disposition value.
 *
 * @param disp   the disposition
 */
inline void ContentDisposition::getDisposition(FastString &s) 
{
    s.set(m_sDisposition.c_str(), m_sDisposition.length());
}

/**
 * Set the disposition.  Replaces the existing disposition.
 *
 * @param disp   the disposition
 */
inline void ContentDisposition::setDisposition(FastString &s) 
{
    m_sDisposition.set(s.c_str(), s.length());
}

/**
 * Set the disposition.  Replaces the existing disposition.
 *
 * @param disp   the disposition
 */
inline void ContentDisposition::setDisposition(const char *s) 
{
    m_sDisposition.set(s);
}

/**
 * Return the specified parameter value. Returns <code>empty</code>
 * if this parameter is absent.
 * 
 * @return    parameter value
 */
inline const ParameterList::ValueString& ContentDisposition::getParameter(FastString &name) 
{
    return m_pList.get(name);
}

/**
 * Get the specified parameter value. Returns <code>empty</code>
 * if this parameter is absent.
 * 
 * @param    name    parameter name
 * @param    value    parameter value
 */
inline void ContentDisposition::getParameter(FastString &name, FastString &value) 
{
    m_pList.get(name, value);
}

/**
 * Get the specified parameter value. Returns <code>empty</code>
 * if this parameter is absent.
 * 
 * @param    name    parameter name
 * @param    value   parameter value
 */
inline void ContentDisposition::getParameter(const char *name, FastString &value) 
{
    m_pList.get(name, value);
}

/**
 * Set the specified parameter. If this parameter already exists,
 * it is replaced by this new value.
 *
 * @param    name    parameter name
 * @param    value    parameter value
 */
inline void ContentDisposition::setParameter(FastString &name, FastString &value) 
{
    m_pList.set(name, value);
}

/**
 * Set the specified parameter. If this parameter already exists,
 * it is replaced by this new value.
 *
 * @param    name    parameter name
 * @param    value    parameter value
 */
inline void ContentDisposition::setParameter(const char *name, const char *value) 
{
    m_pList.set(name, value);
}

/**
 * Return a ParameterList object that holds all the available 
 * parameters. Returns empty if no parameters are available.
 *
 * @return    ParameterList
 */
inline ParameterList& ContentDisposition::getParameterList() 
{
    return m_pList;
}

/**
 * Get a ParameterList object that holds all the available 
 * parameters. Returns empty if no parameters are available.
 *
 * @param list    ParameterList
 */
inline void ContentDisposition::getParameterList(ParameterList &list) 
{
    list = m_pList;
}

/**
 * Set a ParameterList object that holds all the available 
 * parameters. 
 *
 * @param list    ParameterList
 */
inline void ContentDisposition::setParameterList(ParameterList &list) 
{
    m_pList = list;
}

/**
 * Dump the object state.
 */
inline void ContentDisposition::dump() 
{
    FAST_TRACE_BEGIN("ContentDisposition::dump()"); 
    FAST_TRACE("sizeof(ContentDisposition) = %d", sizeof(ContentDisposition)); 
    FAST_TRACE("m_sDisposition = %s", m_sDisposition.c_str()); 
    FAST_TRACE("m_pList.size() = %d", m_pList.size()); 
#ifdef FAST_DEBUG
    ParameterList::iterator it = m_pList.begin(); 
    while( !it.done() ) {
        FAST_TRACE("Parameter: %s = %s", it->first().c_str(), it->second().c_str()); 
        it.advance(); 
    }
#endif 
    FAST_TRACE_END("ContentDisposition::dump()"); 
}


//=============class ContentID define=================

/**
 * This class represents a MIME ContentID value. It provides
 * methods to parse a ContentID string into individual components
 * and to generate a MIME style ContentID string.
 *
 * @see RFC2392
 *
 *   In limited circumstances (e.g., within multipart/alternate), a single
 *   message may contain several body parts that have the same Content-ID.
 *   That occurs, for example, when identical data can be accessed through
 *   different methods.  In those cases, conforming implementations are
 *   required to use the rules of the containing MIME entity (e.g.,
 *   multipart/alternate) to select the body part to which the Content-ID
 *   refers.
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
 *   A "mid" URL is converted to a Message-ID or Message-ID/Content-ID
 *   pair in a similar fashion.
 *
 *   Both message-id and content-id are required to be globally unique.
 *   That is, no two different messages will ever have the same Message-ID
 *   addr-spec; no different body parts will ever have the same Content-ID
 *   addr-spec.  A common technique used by many message systems is to use
 *   a time and date stamp along with the local host's domain name, e.g.,
 *   950124.162336@XIson.com.
 *
 */

class ContentID 
{
public:
    // Traits 
    typedef ShortString CIDString; 

protected: 
    CIDString       m_sCID;     // cid

    int initialize(FastString &s); 

public: 
    ContentID();                // No-arg Constructor.
    ContentID(FastString &s) { initialize(s); } 
    ContentID(const char *s); 
    CIDString& getContentID(); 
    const char * contentID(); 
    const char * CID(); 
    void getContentID(FastString &s); 
    void setContentID(FastString &s); 
    void setContentID(const char *s); 
    void toCIDURL(FastString &s); 
    void toString(FastString &s); 
    void dump(); 
};

//=============ContentID inline functions==============

/**
 * Default Constructor
 *
 */
inline ContentID::ContentID() 
{
    FastString s; 
    UniqueValue::getUniqueContentIDValue(s); 
    setContentID(s); 
}

/**
 * Constructor that takes a ContentID string. The String
 * is parsed into its constituents: 
 *
 * @param s    the ContentID string.
 */
inline ContentID::ContentID(const char *s) 
{
    FastString str(s); 
    initialize(str); 
}

/**
 * Return the Content-ID value.
 *
 * @return the Content-ID
 */
inline ContentID::CIDString& ContentID::getContentID() 
{
    return m_sCID;
}

/**
 * Return the Content-ID value.
 *
 * @return the Content-ID
 */
inline const char * ContentID::contentID() 
{
    return m_sCID.c_str();
}

/**
 * Return the Content-ID value.
 *
 * @return the Content-ID
 */
inline const char * ContentID::CID() 
{
    return m_sCID.c_str();
}

/**
 * Get the Content-ID value.
 *
 * @param s   the Content-ID
 */
inline void ContentID::getContentID(FastString &s) 
{
    s.set(m_sCID.c_str(), m_sCID.length());
}

/**
 * Set the Content-ID.  Replaces the existing Content-ID.
 *
 * @param s   the Content-ID
 */
inline void ContentID::setContentID(FastString &s) 
{
    m_sCID.set(s.c_str(), s.length());
}

/**
 * Set the Content-ID.  Replaces the existing Content-ID.
 *
 * @param s   the Content-ID
 */
inline void ContentID::setContentID(const char *s) 
{
    m_sCID.set(s);
}

/**
 * Dump the object state.
 */
inline void ContentID::dump() 
{
    FAST_TRACE_BEGIN("ContentID::dump()"); 
    FAST_TRACE("sizeof(ContentID) = %d", sizeof(ContentID)); 
    FAST_TRACE("m_sCID = %s", m_sCID.c_str()); 
    FAST_TRACE_END("ContentID::dump()"); 
}


//==============InternetAddress define==================

/**
 * This class represents an Internet email address using the syntax
 * of <a href="http://www.ietf.org/rfc/rfc822.txt">RFC822</a>.
 * Typical address syntax is of the form "user@host.domain" or
 * "Personal Name <user@host.domain>".
 * 
 * The RFC 2047 encoded version of the personal name. <p>
 *
 * This field and the <code>personal</code> field track each
 * other, so if a subclass sets one of these fields directly, it
 * should set the other to <code>empty</code>, so that it is
 * suitably recomputed.
 *
 * @see    MimeUtility
 * @author Naven
 */
class InternetAddress
{
public:
    // Traits 
    typedef FastArray<InternetAddress>          array_type; 
    typedef FastArray_Iterator<InternetAddress> array_iterator; 

protected: 
    FastString64    m_sAddress; 
    FastString16    m_sPersonal; 
    FastString8     m_sGroup; 
    FastString64    m_sLine; 

    void initialize(FastString &address); 

public:
    InternetAddress() {}; 
    virtual ~InternetAddress() {}; 
    InternetAddress(const InternetAddress &address); 
    InternetAddress(FastString &address); 
    InternetAddress(const char *address); 
    void operator= (const InternetAddress &address); 
    void setAddress(FastString &address); 
    void setPersonal(FastString &personal); 
    void setGroup(FastString &group); 
    void setLine(FastString &line); 
    void setAddress(const char *address, size_t len = 0); 
    void setPersonal(const char *personal, size_t len = 0); 
    void setGroup(const char *group, size_t len = 0); 
    void setLine(const char *line, size_t len = 0); 
    void getAddress(FastString &address); 
    void getPersonal(FastString &personal); 
    void getGroup(FastString &group); 
    void getLine(FastString &line); 
    int  equalsAddress(FastString &address); 
    int  equalsPersonal(FastString &personal); 
    int  equalsGroup(FastString &group); 
    int  equalsLine(FastString &line); 
    const size_t addressLength() const; 
    const size_t personalLength() const; 
    const size_t groupLength() const; 
    const size_t lineLength() const; 
    const char *address() const; 
    const char *personal() const; 
    const char *group() const; 
    const char *line() const; 
    void clear(); 
    void toString(FastString &s); 
    void toUnicodeString(FastString &s); 
    void dump(); 
};


//================InternetAddress Inline Functions==============

/**
 * Constructor that specify another hdr.
 *
 * @param header   another hdr
 */
inline InternetAddress::InternetAddress(const InternetAddress &address) 
{
    *this = address; 
}

/**
 * Constructor that specify address string line.
 *
 * @param address   string line include address
 */
inline InternetAddress::InternetAddress(FastString &address) 
{
    initialize(address); 
}

/**
 * Constructor that specify address string line.
 *
 * @param address   string line include address
 */
inline InternetAddress::InternetAddress(const char *address) 
{
    FastString sAddress(address); 
    initialize(sAddress); 
}

/**
 * Constructor that specify address string line.
 *
 * @param address   string line include address
 */
inline void InternetAddress::initialize(FastString &address) 
{
    InternetAddressArray addrs; 
    int count = MimeUtility::parseHeaderAddress(addrs, address); 
    if( addrs.size() > 0 ) 
        *this = addrs[0]; 
}

/**
 * Assignment operator (performs assignment).
 *
 * @param header   another hdr
 */
inline void InternetAddress::operator= (const InternetAddress &address) 
{
    // Check for "self-assignment".
    if( this != &address ) 
    {
        this->m_sAddress  = address.m_sAddress; 
        this->m_sPersonal = address.m_sPersonal; 
        this->m_sGroup    = address.m_sGroup; 
        this->m_sLine     = address.m_sLine; 
    }
}

/**
 * Set the email address.
 *
 * @param   address email address
 */
inline void InternetAddress::setAddress(FastString &address) 
{
    this->m_sAddress.set(address.c_str(), address.length()); 
}

inline void InternetAddress::setAddress(const char *address, size_t len) 
{
    this->m_sAddress.clear(); 
    if( address ) 
    {
        if( len == 0 ) len = strlen(address); 
        this->m_sAddress.append(address, len); 
    }
}

inline void InternetAddress::getAddress(FastString &address) 
{
    address.set(m_sAddress.c_str(), m_sAddress.length()); 
}

inline int InternetAddress::equalsAddress(FastString &address) 
{
    return this->m_sAddress.equalsIgnoreCase(address.c_str(), address.length()); 
}

inline const size_t InternetAddress::addressLength() const 
{
    return this->m_sAddress.length(); 
}

inline const char * InternetAddress::address() const
{
    return this->m_sAddress.c_str(); 
}

/**
 * Set the personal name. If the name contains non US-ASCII
 * characters, then the name will be encoded using the platform's 
 * default charset. If the name contains only US-ASCII characters,
 * no encoding is done and the name is used as is. <p>
 *
 * @param name  personal name
 */
inline void InternetAddress::setPersonal(FastString &personal) 
{
    this->m_sPersonal.set(personal.c_str(), personal.length()); 
}

inline void InternetAddress::setPersonal(const char *personal, size_t len) 
{
    this->m_sPersonal.clear(); 
    if( personal ) 
    {
        if( len == 0 ) len = strlen(personal); 
        this->m_sPersonal.append(personal, len); 
    }
}

inline void InternetAddress::getPersonal(FastString &personal) 
{
    personal.set(m_sPersonal.c_str(), m_sPersonal.length()); 
}

inline int InternetAddress::equalsPersonal(FastString &personal) 
{
    return this->m_sPersonal.equals(personal.c_str(), personal.length()); 
}

inline const size_t InternetAddress::personalLength() const 
{
    return this->m_sPersonal.length(); 
}

inline const char * InternetAddress::personal() const
{
    return this->m_sPersonal.c_str(); 
}

inline void InternetAddress::setGroup(FastString &group) 
{
    this->m_sGroup.set(group.c_str(), group.length()); 
}

inline void InternetAddress::setGroup(const char *group, size_t len) 
{
    this->m_sGroup.clear(); 
    if( group ) 
    {
        if( len == 0 ) len = strlen(group); 
        this->m_sGroup.append(group, len); 
    }
}

inline void InternetAddress::getGroup(FastString &group) 
{
    group.set(m_sGroup.c_str(), m_sGroup.length()); 
}

inline int InternetAddress::equalsGroup(FastString &group) 
{
    return this->m_sGroup.equals(group.c_str(), group.length()); 
}

inline const size_t InternetAddress::groupLength() const 
{
    return this->m_sGroup.length(); 
}

inline const char * InternetAddress::group() const
{
    return this->m_sGroup.c_str(); 
}

inline void InternetAddress::setLine(FastString &line) 
{
    this->m_sLine.set(line.c_str(), line.length()); 
}

inline void InternetAddress::setLine(const char *line, size_t len) 
{
    this->m_sLine.clear(); 
    if( line ) 
    {
        if( len == 0 ) len = strlen(line); 
        this->m_sLine.append(line, len); 
    }
}

inline void InternetAddress::getLine(FastString &line) 
{
    line.set(m_sLine.c_str(), m_sLine.length()); 
}

inline int InternetAddress::equalsLine(FastString &line) 
{
    return this->m_sLine.equals(line.c_str(), line.length()); 
}

inline const size_t InternetAddress::lineLength() const 
{
    return this->m_sLine.length(); 
}

inline const char * InternetAddress::line() const
{
    return this->m_sLine.c_str(); 
}

/**
 * Clear all the string buffer.
 */
inline void InternetAddress::clear() 
{
    this->m_sAddress.clear(); 
    this->m_sPersonal.clear(); 
    this->m_sGroup.clear(); 
    this->m_sLine.clear(); 
}


//=============class hdr define===================

/*
 * A private utility class to represent an individual header.
 * A hdr object with a null line field is used as a "place holder"
 * for headers of that name, to preserve order of headers.
 */
class hdr 
{
public: 
    // Traits 
    typedef FastArray<hdr>            array_type; 
    typedef FastArray_Iterator<hdr>   array_iterator; 

protected:
    FastString16 m_sName; 
    FastString   m_sValue; 
    InternetAddressArray m_arAddresses; 
    size_t m_nAddressCount; 

public:
    hdr(): m_nAddressCount(0) {}; 
    virtual ~hdr() {}; 
    hdr(const char *line, size_t len = 0); 
    hdr(const char *name, const char *value); 
    hdr(FastString &name, FastString &value); 
    hdr(const FastString &s); 
    hdr(const hdr &header); 
    hdr& set(const char *name, const char *value); 
    void setLine(const char *line, size_t len = 0); 
    void operator= (const hdr &header); 
    FastString getName() const; 
    FastString getValue() const; 
    int getAddress(InternetAddressArray &addrs); 
    void getName(FastString &s) const; 
    void getValue(FastString &s) const; 
    const char * name() const; 
    const char * value() const; 
    void setAddress(InternetAddressArray &addrs); 
    void setName(const FastString &s); 
    void setValue(const FastString &s); 
    int equalsName(const FastString &s) const; 
    int equalsValue(const FastString &s) const; 
    size_t nameLength() const; 
    size_t valueLength() const; 
    void clear(); 
    void clearAddress(); 
    void parseAddressHeader(int strict = 1); 
    void updateAddressHeader(); 
    void setAddressHeader(InternetAddressArray &addrs); 
    void addAddress(InternetAddressArray &addrs); 
    void removeAddress(InternetAddressArray &addrs); 
    void removeAddress(FastStringArray &addrs); 
    void removeAddressGroup(FastString &group); 
    void dump(); 
};


//=============hdr inline functions==============

/**
 * Constructor that takes a line and splits out
 * the header name. Line can be mutilines.
 *
 * @param line     line string
 * @param len      line length
 */
inline hdr::hdr(const char *line, size_t len) 
{
    if( line ) 
    {
        if( len == 0 ) len = ::strlen(line); 
        this->setLine(line, len); 
    }
}

/**
 * Constructor that specify the name and value.
 *
 * @param name     name string
 * @param value    value string
 */
inline hdr::hdr(const char *name, const char *value) 
{
    this->m_sName  = name; 
    this->m_sValue = value; 
}

inline hdr::hdr(FastString &name, FastString &value) 
{
    this->m_sName.set(name.c_str(), name.length()); 
    this->m_sValue = value; 
}

/**
 * Constructor that takes a line and splits out
 * the header name. Line can be mutilines.
 *
 * @param line     line string
 * @param len      line length
 */
inline hdr::hdr(const FastString &s) 
{
    this->setLine(s.c_str(), s.length()); 
}

/**
 * Constructor that specify another hdr.
 *
 * @param header   another hdr
 */
inline hdr::hdr(const hdr &header) 
{
    *this = header; 
}

/**
 * Assignment operator (performs assignment).
 *
 * @param header   another hdr
 */
inline void hdr::operator= (const hdr &header) 
{
    // Check for "self-assignment".
    if( this != &header ) 
    {
        this->m_sName  = header.m_sName; 
        this->m_sValue = header.m_sValue; 
        this->m_arAddresses = header.m_arAddresses; 
        this->m_nAddressCount = header.m_nAddressCount; 
    }
}

/**
 * Reset hdr that specify the name and value.
 *
 * @param name     name string
 * @param value    value string
 */
inline hdr& hdr::set(const char *name, const char *value) 
{
    this->m_sName  = name; 
    this->m_sValue = value; 
    return *this; 
}

/**
 * Return the "name" part of the header line.
 */
inline FastString hdr::getName() const
{
    return FastString(m_sName.c_str(), m_sName.length()); 
}

/**
 * Return the "value" part of the header line.
 */
inline FastString hdr::getValue() const
{
    return this->m_sValue; 
}

/**
 * Return the "name" part of the header line.
 */
inline const char * hdr::name() const
{
    return this->m_sName.c_str(); 
}

/**
 * Return the "value" part of the header line.
 */
inline const char * hdr::value() const
{
    return this->m_sValue.c_str(); 
}

/**
 * Return the "name" part of the header line.
 */
inline void hdr::getName(FastString &s) const
{
    s.set(m_sName.c_str(), m_sName.length()); 
}

/**
 * Return the "value" part of the header line.
 */
inline void hdr::getValue(FastString &s) const
{
    s = this->m_sValue; 
}

/**
 * Update the "name" part of the header line.
 */
inline void hdr::setAddress(InternetAddressArray &addrs) 
{
    this->setAddressHeader(addrs); 
}

/**
 * Update the "name" part of the header line.
 */
inline void hdr::setName(const FastString &s)
{
    this->m_sName.set(s.c_str(), s.length()); 
}

/**
 * Update the "value" part of the header line.
 */
inline void hdr::setValue(const FastString &s)
{
    this->m_sValue = s; 
}

/**
 * Check equalsIgnoreCase the "name" part of the header line.
 */
inline int hdr::equalsName(const FastString &s) const 
{
    return this->m_sName.equalsIgnoreCase(s.c_str(), s.length()); 
}

/**
 * Check equalsIgnoreCase the "value" part of the header line.
 */
inline int hdr::equalsValue(const FastString &s) const 
{
    return this->m_sValue.equalsIgnoreCase(s); 
}

/**
 * Return the "name" part's length.
 */
inline size_t hdr::nameLength() const 
{
    return this->m_sName.length(); 
}

/**
 * Return the "value" part's length.
 */
inline size_t hdr::valueLength() const 
{
    return this->m_sValue.length(); 
}

/**
 * Clear the name and value.
 */
inline void hdr::clear()  
{
    this->m_sName.clear(); 
    this->m_sValue.clear(); 
    this->clearAddress(); 
}

/**
 * Reparse the array of addresses when modified value.
 */
inline void hdr::parseAddressHeader(int strict) 
{
    this->m_nAddressCount = 
        MimeUtility::parseHeaderAddress(this->m_arAddresses, this->m_sValue, 
        strict); 
}

/**
 * Update the header line with the array of addresses.
 */
inline void hdr::updateAddressHeader() 
{
    MimeUtility::getAddressString(this->m_arAddresses, this->m_sValue); 
}


//===========class InternetHeaders define=================

/**
 * InternetHeaders is a utility class that manages RFC822 style
 * headers. Given an RFC822 format message stream, it reads lines
 * until the blank line that indicates end of header. The input stream
 * is positioned at the start of the body. The lines are stored 
 * within the object and can be extracted as either Strings or
 * {@link javax.mail.Header} objects. <p>
 *
 * This class is mostly intended for service providers. MimeMessage
 * and MimeBody use this class for holding their headers. <p>
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
 * @see    MimeUtility
 * @author Naven
 */
class InternetHeaders 
{
protected:
    hdrArray m_arHeaders; 
    size_t   m_nHeaderCount; 
    int m_bStrict;  // =1 to enforce RFC822 address parse or =0 not.

private:
    void initHeaders(); 

public:
    InternetHeaders(); 
    InternetHeaders(InternetHeaders &ih); 
    InternetHeaders(const char *header, size_t len = 0); 
    virtual ~InternetHeaders(); 
    InternetHeaders& operator=(const InternetHeaders &ih); 
    void swap(InternetHeaders &ih); 
    void release(); 
    void dump(); 

    void setStrict(int strict = 1); 
    void load(const char *header, size_t len = 0); 
    int getHeader(FastString &name, FastString &s, const char *sep = NULL); 
    int getHeader(const char *name, FastString &s, const char *sep = NULL); 
    int getHeader(FastString &name, FastStringArray &values); 
    int getHeader(const char *name, FastStringArray &values); 
    int getMatchingHeaders(FastStringArray &names, hdrArray &headers, int nonmatch = 0); 
    int getNonMatchingHeaders(FastStringArray &names, hdrArray &headers); 
    int getAllHeaders(hdrArray &headers); 
    int getMatchingHeaderLines(FastStringArray &names, FastString &lines); 
    int getNonMatchingHeaderLines(FastStringArray &names, FastString &lines); 
    int getAllHeaderLines(FastString &lines); 
    void setHeader(hdr &header); 
    void setHeader(FastString &name, FastString &s); 
    void setHeader(const char *name, FastString &s); 
    void addHeader(hdr &header, int index = -1); 
    void addHeader(FastString &name, FastString &s, int index = -1); 
    void addHeader(const char *name, FastString &s, int index = -1); 
    void addHeaderBehind(FastString &field, const hdr &header, int before = 0); 
    void addHeaderBehind(FastString &field, FastString &name, FastString &s, int before = 0); 
    void addHeaderBehind(const char *field, FastString &name, FastString &s); 
    void addHeaderBehind(const char *field, const char *name, FastString &s); 
    void addHeaderBehind(FastString &field, const char *name, FastString &s); 
    void addHeaderBefore(FastString &field, const hdr &header); 
    void addHeaderBefore(FastString &field, FastString &name, FastString &s); 
    void addHeaderBefore(const char *field, FastString &name, FastString &s); 
    void addHeaderBefore(const char *field, const char *name, FastString &s); 
    void addHeaderBefore(FastString &field, const char *name, FastString &s); 
    void addHeaderLine(FastString &line); 
    void addHeaderLine(const char *line, int len = -1); 
    void removeHeader(FastString &name); 
    void removeHeader(const char *name); 
    void removeAllHeaders(); 
    hdrArrayIterator getHeaderIterator(); 
    size_t getHeaderCount() const; 
    size_t getHeaderCount(FastString &name); 
    size_t getHeaderCount(const char *name); 
    int  getAddressHeader(FastString &name, InternetAddressArray &addrs); 
    void setAddressHeader(FastString &name, InternetAddressArray &addrs, FastString &group); 
    void setAddressHeader(FastString &name, InternetAddressArray &addrs); 
    void setAddressHeader(FastString &name, InternetAddress &addr); 
    void addAddressHeader(FastString &name, InternetAddressArray &addrs, FastString &group); 
    void addAddressHeader(FastString &name, InternetAddressArray &addrs); 
    void addAddressHeader(FastString &name, InternetAddress &addr, FastString &group); 
    void addAddressHeader(FastString &name, InternetAddress &addr); 
    void removeAddress(FastString &name, InternetAddressArray &addrs); 
    void removeAddress(FastString &name, FastStringArray &addrs); 
    void removeAddressGroup(FastString &name, FastString &group); 

    friend class hdr;
};


//===========InternetHeaders inline functions========

/**
 * Constructor default.
 */
inline InternetHeaders::InternetHeaders() 
: m_arHeaders(), 
  m_nHeaderCount(0), 
  m_bStrict(1) 
{
    this->initHeaders(); 
}

/**
 * Assign Constructor.
 */
inline InternetHeaders::InternetHeaders(InternetHeaders &ih) 
: m_arHeaders(ih.m_arHeaders), 
  m_nHeaderCount(ih.m_nHeaderCount), 
  m_bStrict(ih.m_bStrict) 
{
    this->initHeaders(); 
}

/**
 * Constructor that takes the header buffer to parse all values. 
 *
 * @param header    pointer to header buffer.
 * @param len       header buffer length.
 */
inline InternetHeaders::InternetHeaders(const char *header, size_t len) 
: m_arHeaders(), 
  m_nHeaderCount(0), 
  m_bStrict(1) 
{
    this->load(header, len); 
}

/**
 * Assign operator =.
 */
inline InternetHeaders& InternetHeaders::operator=(const InternetHeaders &ih) 
{
    release(); 
    m_arHeaders     = ih.m_arHeaders; 
    m_nHeaderCount  = ih.m_nHeaderCount; 
    m_bStrict       = ih.m_bStrict; 

    return *this; 
}

/**
 * Destructor ~
 */
inline InternetHeaders::~InternetHeaders()
{
    release(); 
}

/**
 * Release all memory.
 */
inline void InternetHeaders::release()
{

}

/**
 * Swap two variables, only swap their points and reference.
 * used for swap two large objects. 
 */
inline void InternetHeaders::swap(InternetHeaders &ih)
{
    fast_swap_value(m_nHeaderCount, ih.m_nHeaderCount); 
    fast_swap_value(m_bStrict,      ih.m_bStrict); 

    m_arHeaders.swap(ih.m_arHeaders); 
}

inline void InternetHeaders::setStrict(int strict) 
{
    this->m_bStrict = strict; 
}

/**
 * Get all the headers for this header name, returned as a single
 * String, with headers separated by the delimiter. If the
 * delimiter is <code>empty</code>, only the first header is 
 * returned.  Returns <code>0</code>
 * if no headers with the specified name exist. 
 * reference to RFC 822.
 *
 * @param name     header field name.
 * @param s        header field value.
 * @param sep      delimiter
 * @return         count of specified header.
 */
inline int InternetHeaders::getHeader(
    const char *name, FastString &s, const char *sep) 
{
    FastString faName(name); 
    return this->getHeader(faName, s, sep); 
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
inline int InternetHeaders::getHeader(
    const char *name, FastStringArray &values) 
{
    FastString faName(name); 
    return this->getHeader(faName, values); 
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
inline void InternetHeaders::setHeader(
    const char *name, FastString &s) 
{
    FastString faName(name); 
    this->setHeader(faName, s); 
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
inline void InternetHeaders::addHeader(
    const char *name, FastString &s, int index) 
{
    FastString faName(name); 
    this->addHeader(faName, s, index); 
}

/**
 * Add a header with the specified name and value to the header list. <p>
 *
 * Note that RFC822 headers can only contain US-ASCII characters.
 *
 * @param name      header name
 * @param value     header value
 * @param field     header item that add behind
 */ 
inline void InternetHeaders::addHeaderBehind(
    const char *field, FastString &name, FastString &s) 
{
    FastString faField(field); 
    this->addHeaderBehind(faField, name, s); 
}

inline void InternetHeaders::addHeaderBehind(
    const char *field, const char *name, FastString &s) 
{
    FastString faField(field), faName(name); 
    this->addHeaderBehind(faField, faName, s); 
}

inline void InternetHeaders::addHeaderBehind(
    FastString &field, const char *name, FastString &s) 
{
    FastString faName(name); 
    this->addHeaderBehind(field, faName, s); 
}

/**
 * Add a header with the specified name and value to the header list. <p>
 *
 * Note that RFC822 headers can only contain US-ASCII characters.
 *
 * @param name      header name
 * @param value     header value
 * @param field     header item that add before
 */ 
inline void InternetHeaders::addHeaderBefore(
    FastString &field, FastString &name, FastString &s) 
{
    this->addHeaderBehind(field, name, s, 1); 
}

inline void InternetHeaders::addHeaderBefore(
    FastString &field, const hdr &header) 
{
    this->addHeaderBehind(field, header, 1); 
}

inline void InternetHeaders::addHeaderBefore(
    const char *field, FastString &name, FastString &s) 
{
    FastString faField(field); 
    this->addHeaderBefore(faField, name, s); 
}

inline void InternetHeaders::addHeaderBefore(
    const char *field, const char *name, FastString &s) 
{
    FastString faField(field), faName(name); 
    this->addHeaderBefore(faField, faName, s); 
}

inline void InternetHeaders::addHeaderBefore(
    FastString &field, const char *name, FastString &s) 
{
    FastString faName(name); 
    this->addHeaderBefore(field, faName, s); 
}

/**
 * Add an RFC822 header line to the header store.
 * If the line starts with a space or tab (a continuation line),
 * add it to the last header line in the list. <p>
 *
 * Note that RFC822 headers can only contain US-ASCII characters
 *
 * @param line  raw RFC822 header line
 * @param line  raw RFC822 header line length
 */
inline void InternetHeaders::addHeaderLine(const char *line, int len)
{
    if( !line ) return; 
    FastString faLine; 
    if( len <= 0 ) len = strlen(line); 
    faLine.append(line, len); 
    this->addHeaderLine(faLine); 
}

/**
 * Remove all header entries that match the given name
 * @param name  header name
 */
inline void InternetHeaders::removeHeader(const char *name) 
{
    FastString faName(name); 
    this->removeHeader(faName); 
}

/**
 * Return the header lines array's iterator.
 */
inline hdrArrayIterator InternetHeaders::getHeaderIterator() 
{
    return hdrArrayIterator(this->m_arHeaders); 
}

/**
 * Return the header lines count.
 */
inline size_t InternetHeaders::getHeaderCount() const 
{
    return this->m_nHeaderCount; 
}

/**
 * Return the matching header lines count.
 */
inline size_t InternetHeaders::getHeaderCount(const char *name) 
{
    FastString faName(name); 
    return this->getHeaderCount(faName); 
}

/**
 * Return all non-matching header objects.
 *
 * @param names      header names
 * @param headers    non-matching header objects
 * @return  matching Header objects count.
 */
inline int InternetHeaders::getNonMatchingHeaders(
        FastStringArray &names, hdrArray &headers) 
{
    return this->getMatchingHeaders(names, headers, 1); 
}

// Convenience method to set addresses
inline void InternetHeaders::setAddressHeader(
    FastString &name, InternetAddressArray &addrs) 
{
    FastString group; 
    this->setAddressHeader(name, addrs, group); 
}

inline void InternetHeaders::setAddressHeader(
    FastString &name, InternetAddress &addr) 
{
    FastString group; 
    InternetAddressArray addrs(1); 
    addrs.set(addr, 0); 
    this->setAddressHeader(name, addrs, group); 
}

// Convenience method to add addresses
inline void InternetHeaders::addAddressHeader(
    FastString &name, InternetAddressArray &addrs) 
{
    FastString group; 
    this->addAddressHeader(name, addrs, group); 
}

inline void InternetHeaders::addAddressHeader(
    FastString &name, InternetAddress &addr, FastString &group) 
{
    InternetAddressArray addrs(1); 
    addrs.set(addr, 0); 
    this->addAddressHeader(name, addrs, group); 
}

inline void InternetHeaders::addAddressHeader(
    FastString &name, InternetAddress &addr) 
{
    FastString group; 
    InternetAddressArray addrs(1); 
    addrs.set(addr, 0); 
    this->addAddressHeader(name, addrs, group); 
}



/**
 * Dump the object's state.
 */
inline void InternetAddress::dump() 
{
    FAST_TRACE_BEGIN("InternetAddress::dump()"); 
    FAST_TRACE("sizeof(InternetAddress) = %d", sizeof(InternetAddress)); 
    FAST_TRACE("m_sAddress = \"%s\"", m_sAddress.c_str()); 
    FAST_TRACE("m_sPersonal = \"%s\"", m_sPersonal.c_str()); 
    FAST_TRACE("m_sGroup = \"%s\"", m_sGroup.c_str()); 
    FAST_TRACE("m_sLine = \"%s\"", m_sLine.c_str()); 
    FAST_TRACE_END("InternetAddress::dump()"); 
}

/**
 * Dump the object's state.
 */
inline void hdr::dump()
{
    FAST_TRACE_BEGIN("hdr::dump()"); 
    FAST_TRACE("sizeof(hdr) = %d", sizeof(hdr)); 
    FAST_TRACE("m_sName = \"%s\"", m_sName.c_str()); 
    FAST_TRACE("m_sValue = \"%s\"", m_sValue.c_str()); 
    FAST_TRACE("m_arAddresses.size() = %d", m_arAddresses.size()); 
    FAST_TRACE("m_nAddressCount = %d", m_nAddressCount); 
#ifdef FAST_DEBUG
    InternetAddressArray::iterator it(m_arAddresses); 
    InternetAddress *paddr = NULL; 
    while( it.next(paddr) && paddr ) {
        paddr->dump(); 
    }
#endif
    FAST_TRACE("sizeof(hdr) = %d", sizeof(hdr)); 
    FAST_TRACE("m_sName = \"%s\"", m_sName.c_str()); 
    FAST_TRACE("m_sValue = \"%s\"", m_sValue.c_str()); 
    FAST_TRACE("m_arAddresses.size() = %d", m_arAddresses.size()); 
    FAST_TRACE("m_nAddressCount = %d", m_nAddressCount); 
    FAST_TRACE_END("hdr::dump()"); 
}

/**
 * Dump the object's state.
 */
inline void InternetHeaders::dump() 
{
    FAST_TRACE_BEGIN("InternetHeaders::dump()"); 
    FAST_TRACE("sizeof(InternetHeaders) = %d", sizeof(InternetHeaders)); 
    FAST_TRACE("m_arHeaders.size() = %d", m_arHeaders.size()); 
    FAST_TRACE("m_nHeaderCount = %d", m_nHeaderCount); 
    FAST_TRACE("m_bStrict = %d", m_bStrict); 
#ifdef FAST_DEBUG
    hdrArray::iterator it(m_arHeaders); 
    hdr *phdr = NULL; 
    while( it.next(phdr) && phdr ) {
        phdr->dump(); 
    }
#endif
    FAST_TRACE("sizeof(InternetHeaders) = %d", sizeof(InternetHeaders)); 
    FAST_TRACE("m_arHeaders.size() = %d", m_arHeaders.size()); 
    FAST_TRACE("m_nHeaderCount = %d", m_nHeaderCount); 
    FAST_TRACE("m_bStrict = %d", m_bStrict); 
    FAST_TRACE_END("InternetHeaders::dump()"); 
}


_FASTMIME_END_NAMESPACE

#endif
