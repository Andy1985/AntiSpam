//=============================================================================
/**
 *  @file    FastBase.h
 *
 *  ver 1.0.0 2004/06/20 Naven Exp, for Fast Common Framework.
 *
 *  @author Naven (Navy Chen) 2004/06/20 created.
 */
//=============================================================================

#ifndef _FAST_COMM_BASE_H
#define _FAST_COMM_BASE_H

#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <string>


#if defined(_WIN32) || defined(__WIN32__) 
#else // UNIX 
    #include <errno.h>
    #define NAVEN_PRAGMA_ONCE
#endif /* _WIN32 */

#if !defined (NAVEN_PRAGMA_ONCE)
# pragma once
#endif /* NAVEN_PRAGMA_ONCE */


#ifndef _FAST_USE_NAMESPACES
#define _FAST_USE_NAMESPACES  using namespace fast;
#endif 

#ifndef _FAST 
#define _FAST fast 
#endif 

#ifndef _FAST_BEGIN_NAMESPACE
#define _FAST_BEGIN_NAMESPACE namespace fast {
#endif 

#ifndef _FAST_END_NAMESPACE
#define _FAST_END_NAMESPACE } 
#endif 


_FAST_BEGIN_NAMESPACE


/**
 *NOTE: This is an internal header file, included by other headers.
 *   You should not attempt to use it directly.
 *
 * @author Naven
 * @see SGI STL & ACE wrappers
 */

/**
 *  This configuration define macro is for all of Microsoft Windows
 *  platforms that FAST runs on.  Based on preprocessor definitions, it
 *  includes other more specific configuration files.
 */
#if defined(_WIN32) || defined(__WIN32__) 
    #define snprintf    _snprintf
    #define vsnprintf   _vsnprintf

/**
 *  This configuration define macro is for all of Unix or Linux
 *  platforms that FAST runs on.  Based on preprocessor definitions, it
 *  includes other more specific configuration files.
 */
#else

#endif /* _WIN32 */


// Forward declarations.

#ifndef SIZET
#define SIZET  size_t
#endif

#ifndef SSIZET
#define SSIZET int
#endif

template<typename CHAR, SIZET BUFSIZE> class FastString_Base; 

template<class T> class FastArray_Iterator;
template<class T> class FastArray;

typedef class FastString_Base<char, 256>        FastString; 
typedef class FastString_Base<char, 32>         ShortString; 

typedef class FastArray<FastString>             FastStringArray; 
typedef class FastArray<ShortString>            ShortStringArray; 

typedef class FastArray_Iterator<FastString>    FastStringArrayIterator; 
typedef class FastArray_Iterator<ShortString>   ShortStringArrayIterator; 

typedef FastStringArray             StringArray; 
typedef FastStringArrayIterator     StringArrayIterator; 



/**
 * Convenience macros to test the versions of glibc and gcc.
 * Use them like this:
 * #if FAST_GNUC_PREREQ (2,8)
 * ... code requiring gcc 2.8 or later ...
 * #endif
 * Note - they won't work for gcc1 or glibc1, since the _MINOR macros
 * were not defined then. 
 */
#if defined __GNUC__ && defined __GNUC_MINOR__
#define FAST_GNUC_PREREQ(maj, min) \
        ((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#else
#define FAST_GNUC_PREREQ(maj, min) 0
#endif


/**
 * Define the default constants for FAST.  Many of these are used for
 * the FAST tests and applications.  You can change these values by
 * defining the macros in your this file.
 */

// Defaults for the Fast Free List
#define FAST_DEFAULT_INIT_CHUNKS    16
#define FAST_DEFAULT_FREE_LIST_PREALLOC 0
#define FAST_DEFAULT_FREE_LIST_LWM  0
#define FAST_DEFAULT_FREE_LIST_HWM  25000
#define FAST_DEFAULT_FREE_LIST_INC  100
#define FAST_MALLOC_ALIGN           8

// This is just a guess.  8k is the normal limit on
// most machines because that's what NFS expects.
#define FAST_MAX_DGRAM_SIZE         8192

#define FAST_DEFAULT_ARGV_BUFSIZ    1024 * 4

// Fast_Cached_Allocator<T> MOST allocate 1 chunk one time. 
// And can also allocate more chunks in series, But if allocate
// larger than this below, allocator will use ::malloc to
// allocate to decrease time.
#define FAST_MAX_ALLOC_CHUNKS_ONETIME 8

// A free list which create more elements when there aren't enough
// elements.
#define FAST_FREE_LIST_WITH_POOL    1

// A simple free list which doen't allocate/deallocate elements.
#define FAST_PURE_FREE_LIST         2

// Default Allocator for FAST contains
#define FAST_ALLOCATOR_TYPE(X)      Fast_New_Allocator<X>
#define FAST_DEFAULT_ALLOCATOR(X)   Fast_Cached_Allocator<X>


/**
 * This stuff must come first to avoid problems with circular headers...
 * The following macro is defined to simply print dump object stat for 
 * debug by define FAST_DEBUG, So you should define it first and pull 
 * that one early..
 * and FAST_LOGGING is to append dump log message to log file, but not 
 * implemented yeat :)
 * 
 * @author Naven
 */

#if defined (_DEBUG) || defined (DEBUG)
#define FAST_DEBUG
#endif /* _DEBUG */ 

#define FAST_TEXT(X) (char *)(X)
#define FAST_TRACE_FILEDES stderr

#if !defined (FAST_DEBUG) 
#define FAST_ASSERT(x)
#define FAST_TRACE_BEGIN(x)
#define FAST_TRACE_END(x)
#define FAST_TRACE fast_trace_none
#else
#define FAST_ASSERT(X) \
    do { if(!(X)) { \
        fprintf(FAST_TRACE_FILEDES, FAST_TEXT("FAST_ASSERT: file %s, line %d assertion failed for '%s'.\n"), \
            FAST_TEXT(__FILE__), __LINE__, FAST_TEXT(X)); \
        fflush(FAST_TRACE_FILEDES); \
    } } while (0)
#define FAST_TRACE_BEGIN(X) \
    do { \
        fprintf(FAST_TRACE_FILEDES, FAST_TEXT("FAST_TRACE_BEGIN: file %s, line %d: %s\n"), \
            FAST_TEXT(__FILE__), __LINE__, FAST_TEXT(X)); \
        fflush(FAST_TRACE_FILEDES); \
    } while (0)
#define FAST_TRACE_END(X) \
    do { \
        fprintf(FAST_TRACE_FILEDES, FAST_TEXT("FAST_TRACE_END: file %s, line %d: %s\n"), \
            FAST_TEXT(__FILE__), __LINE__, FAST_TEXT(X)); \
        fflush(FAST_TRACE_FILEDES); \
    } while (0)
#define FAST_TRACE2(X) \
    do { \
        fprintf(FAST_TRACE_FILEDES, FAST_TEXT("FAST_TRACE: "X)); \
        fprintf(FAST_TRACE_FILEDES, FAST_TEXT("\n")); \
        fflush(FAST_TRACE_FILEDES); \
    } while (0)
#define FAST_TRACE fast_trace
#endif  /* FAST_DEBUG */

inline void fast_trace_none(const char *fmt, ...)
{
    
}

inline void fast_trace(const char *fmt, ...)
{
    char buf[1024];
    va_list ap;
    memset(buf, 0, sizeof(buf)); 
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    fprintf(FAST_TRACE_FILEDES, FAST_TEXT("FAST_TRACE: ")); 
    fprintf(FAST_TRACE_FILEDES, FAST_TEXT(buf)); 
    fprintf(FAST_TRACE_FILEDES, FAST_TEXT("\n")); 
    fflush(FAST_TRACE_FILEDES); 
}


/**
 * ============================================================================
 * FAST_NEW macros
 *
 * A useful abstraction for expressions involving operator new since
 * we can change memory allocation error handling policies (e.g.,
 * depending on whether ANSI/ISO exception handling semantics are
 * being used).
 * ============================================================================
 *
 * Since new() throws exceptions, we need a way to avoid passing
 * exceptions past the call to new because ACE counts on having a 0
 * return value for a failed allocation. Some compilers offer the
 * new (nothrow) version, which does exactly what we want. Others
 * do not. For those that do not, this sets up what exception is thrown,
 * and then below we'll do a try/catch around the new to catch it and
 * return a 0 pointer instead.
 *
 * @see ACE OS_Memory.h
 */

#define FAST_NOTHROW    std::nothrow

#define FAST_NEW_RETURN(POINTER,CONSTRUCTOR,RET_VAL) \
    do { POINTER = new (FAST_NOTHROW) CONSTRUCTOR; \
        if( POINTER == 0 ) { errno = ENOMEM; return RET_VAL; } \
    } while (0)

#define FAST_NEW(POINTER,CONSTRUCTOR) \
    do { POINTER = new (FAST_NOTHROW) CONSTRUCTOR; \
        if( POINTER == 0 ) { errno = ENOMEM; return; } \
    } while (0)

#define FAST_NEW_NORETURN(POINTER,CONSTRUCTOR) \
    do { POINTER = new (FAST_NOTHROW) CONSTRUCTOR; \
        if( POINTER == 0 ) { errno = ENOMEM; } \
    } while (0)

#define FAST_DELETE(POINTER,CLASS) \
    do { if (POINTER) { \
        (POINTER)->~CLASS (); delete (POINTER); POINTER = 0; } \
    } while (0)

#define FAST_DELETE_ARRAY(POINTER) \
    do { if (POINTER) { \
        delete [] (POINTER); POINTER = 0; } \
    } while (0)

#define FAST_FREE(POINTER) \
    do { if (POINTER) { \
        ::free(POINTER); POINTER = 0; } \
    } while (0)

#define FAST_NEW_MALLOC_RETURN(POINTER,ALLOCATOR,CONSTRUCTOR,RET_VAL) \
    do { POINTER = ALLOCATOR; \
        if( POINTER == 0 ) { errno = ENOMEM; return RET_VAL; } \
        else { new (POINTER) CONSTRUCTOR; } \
    } while (0)

#define FAST_NEW_MALLOC(POINTER,ALLOCATOR,CONSTRUCTOR) \
    do { POINTER = ALLOCATOR; \
        if( POINTER == 0 ) { errno = ENOMEM; return; } \
        else { new (POINTER) CONSTRUCTOR; } \
    } while (0)

#define FAST_NEW_MALLOC_NORETURN(POINTER,ALLOCATOR,CONSTRUCTOR) \
    do { POINTER = ALLOCATOR; \
        if( POINTER == 0 ) { errno = ENOMEM;} \
        else { new (POINTER) CONSTRUCTOR; } \
    } while (0)

#define FAST_DES_FREE_TEMPLATE(POINTER,DEALLOCATOR,T_CLASS,T_PARAMETER) \
    do { if (POINTER) { \
        (POINTER)->~T_CLASS (); \
        DEALLOCATOR (POINTER); \
    } } while (0)

#define FAST_DES_FREE_CLASS(POINTER,DEALLOCATOR,T_CLASS) \
    do { if (POINTER) { \
        (POINTER)->~T_CLASS (); \
        DEALLOCATOR (POINTER); \
    } } while (0)


/**
 * ============================================================================
 * Miscellaneous macros
 * ============================================================================
 */

#if defined(_WIN32) || defined(__WIN32__) 
typedef int BOOL; 

#else // UNIX 
//#ifndef BOOL
//#define BOOL    int
//#endif 

typedef int BOOL; 

#endif 

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0 
#endif

#if !defined (ENOTSUP)
#define ENOTSUP ENOSYS  /* Operation not supported. */
#endif

// This is used to indicate that a platform doesn't support a
// particular feature.
#define FAST_NOTSUP_RETURN(FAILVALUE) do { errno = ENOTSUP ; return FAILVALUE; } while (0)
#define FAST_NOTSUP do { errno = ENOTSUP; return; } while (0)


//==================Fast Functions========================

/**
 * Binary search (lower_bound, value list from smaller to bigger).
 * 二分查找法，在一个从小到大排好序的表格中找到接近它的最小数。
 */
template<class T>
const T* fast_lower_bound(const T* first, const T* last, const T& val) 
{
    int len = last - first;
    int half = 0;
    T *middle = NULL, *found = (T*)first;
    while( len > 0 ) 
    {
        half = len >> 1;
        middle = found + half;
        if( *middle < val ) 
        {
            found = middle;
            ++ found;
            len = len - half - 1;
        }
        else
            len = half;
    }
    return found;
}

/**
 * Swap two variables。
 * 
 * @param a  variable a
 * @param b  variable b
 */
template<class T>
inline void fast_swap_value(T& a, T& b) 
{
    if( &a != &b ) 
    {
        T tmp = a;
        a = b;
        b = tmp;
    }
}

template<typename T> inline 
const T & fast_min(const T &left, const T &right) 
{
    return left < right ? left : right; 
}

template<typename T> inline 
const T & fast_max(const T &left, const T &right) 
{
    return left > right ? left : right; 
}

/**
 * ROUND_UP() to make the <nbytes> up to mutiple of 
 * <FAST_MALLOC_ALIGN=8>.
 *
 * @param nbytes    bytes to malloc
 * @return  bytes to actually malloc
 */
inline SIZET fast_round_up(SIZET nbytes) 
{
    return (((nbytes) + FAST_MALLOC_ALIGN - 1) & ~(FAST_MALLOC_ALIGN - 1)); 
}

// Hash function for char* hashing. 
inline SIZET fast_hash_string(const char* s)
{
    unsigned long h = 0; 
    for( ; *s; ++s )
        h = 5*h + *s;
    return SIZET(h);
}

/**
 * construct and destroy.  These functions are not part of the C++ standard,
 * and are provided for backward compatibility with the HP STL.  We also
 * provide internal names _Construct and _Destroy that can be used within
 * the library, so that standard-conforming pieces don't have to rely on
 * non-standard extensions.
 *
 * Internal names
 */

template<class T1, class T2>
inline void fast_construct(T1* p, const T2& value) 
{
    new ((void*) p) T1(value);
}

template<class T1>
inline void fast_construct(T1* p) 
{
    new ((void*) p) T1();
}

template<class T>
inline void fast_destroy(T* pointer) 
{
    pointer->~T();
    // 为了安全性，再把它在stack中占的内存清0
    ::memset(pointer, 0, sizeof(T)); 
}

/**
 * The _Fast_S_eos function is used for those functions that
 * convert to/from C-like strings to detect the end of the string.
 *
 * The end-of-C-string character.
 * This is what the draft standard says it should be.
 */
template<class CHAR>
inline CHAR _Fast_S_eos(CHAR*) { return CHAR(); }

template<class ARG, class RESULT>
struct Fast_Unary_Function 
{
    typedef ARG argument_type;
    typedef RESULT result_type;
};

template<class ARG1, class ARG2, class RESULT>
struct Fast_Binary_Function 
{
    typedef ARG1 first_argument_type;
    typedef ARG2 second_argument_type;
    typedef RESULT result_type;
}; 

template<class T>
struct Fast_Equal_To : public Fast_Binary_Function<T,T,BOOL> 
{
    BOOL operator()(const T& x, const T& y) const { return x == y ? TRUE : FALSE; }
};

template<class T>
struct Fast_Not_Equal_To : public Fast_Binary_Function<T,T,BOOL> 
{
    BOOL operator()(const T& x, const T& y) const { return x != y ? TRUE : FALSE; }
};

// select1st and select2nd are extensions: they are not part of the standard.
template<class PAIR>
struct Fast_Select1st : public Fast_Unary_Function<PAIR, typename PAIR::first_type> 
{
    const typename PAIR::first_type& operator()(const PAIR& x) const 
    {
        return x.first();
    }
};

template<class PAIR>
struct Fast_Select2nd : public Fast_Unary_Function<PAIR, typename PAIR::second_type>
{
    const typename PAIR::second_type& operator()(const PAIR& x) const 
    {
        return x.second();
    }
};


//===================Fast_Pair Classes=========================

/**
 * @class Fast_Pair
 *
 * @brief Defines a pair.
 *
 * Similar to the STL pair.
 */
template<class T1, class T2>
class Fast_Pair 
{
public: 
    typedef T1 first_type;
    typedef T2 second_type;

    // = Initialization and termination methods.
    // Constructor.
    Fast_Pair() : _first(T1()), _second(T2()) {}
    Fast_Pair(const T1& a, const T2& b) : _first(a), _second(b) {}

    // Get/Set first.
    T1& first() { return _first; }
    const T1& first() const { return _first; }
    void first(const T1 &t1) { _first = t1; }

    // Get/Set first.
    T2& second() { return _second; }
    const T2& second() const { return _second; }
    void second(const T2& t2) { _second = t2; }

    // Compare pairs.
    BOOL operator== (const Fast_Pair<T1, T2> &rhs) const 
    { return this == rhs ? TRUE : ((_first == rhs._first && _second == rhs._second) 
                ? TRUE : FALSE); }

protected:
    T1 _first;
    T2 _second;
};

/**
 * @class Fast_Reference_Pair
 *
 * @brief Defines a pair that only hold references.
 *
 * Similar to the STL pair (but restricted to holding references
 * and not copies).
 */
template<class T1, class T2>
class Fast_Reference_Pair 
{
public: 
    typedef T1 first_type;
    typedef T2 second_type;

    // = Initialization and termination methods.
    // Constructor.
    Fast_Reference_Pair(T1& a, T2& b) : _first(a), _second(b) {}

    // Access first.
    T1& first() { return _first; }

    // Access first.
    T2& second() { return _second; }

    // Compare pairs.
    BOOL operator== (const Fast_Reference_Pair<T1, T2> &rhs) const 
    { return this == rhs ? TRUE : ((_first == rhs._first && _second == rhs._second) 
                ? TRUE : FALSE); }

protected:
    T1& _first;
    T2& _second;
};


//===============Fast_Unbounded_Set Classes======================

// Forward declarations.
template<class T> class Fast_Unbounded_Set;
template<class T> class Fast_Unbounded_Set_Iterator;
template<class T> class Fast_Unbounded_Set_Const_Iterator;

/**
 * @class Fast_Node
 *
 * @brief Implementation element in a Queue, Set, and Stack.
 */
template<class T>
class Fast_Node
{
public:
    friend class Fast_Unbounded_Set<T>;
    friend class Fast_Unbounded_Set_Iterator<T>;
    friend class Fast_Unbounded_Set_Const_Iterator<T>;

  // This isn't necessary, but it keeps some compilers happy.
  ~Fast_Node() {}

private:
    // = Initialization methods
    Fast_Node(const T &i, Fast_Node<T> *n) : _next(n), _item(i) {}
    Fast_Node(Fast_Node<T> *n = 0, int = 0) : _next(n) {}
    Fast_Node(const Fast_Node<T> &n) : _next(n._next), _item(n._item) {}

    // Pointer to next element in the list of <Fast_Node>s.
    Fast_Node<T> *_next;

    // Current value of the item in this node.
    T _item;
};

/**
 * @class Fast_Unbounded_Set_Iterator
 *
 * @brief Implement an iterator over an unbounded set.
 */
template<class T>
class Fast_Unbounded_Set_Iterator
{
public:
    // = Initialization method.
    Fast_Unbounded_Set_Iterator(Fast_Unbounded_Set<T> &s, int end = 0);

    // = Iteration methods.

    // Pass back the <next_item> that hasn't been seen in the Set.
    // Returns 0 when all items have been seen, else 1.
    int next(T *&next_item);

    // Move forward by one element in the set.  Returns 0 when all the
    // items in the set have been seen, else 1.
    int advance();

    // Move to the first element in the set.  Returns 0 if the
    // set is empty, else 1.
    int first();

    // Returns 1 when all items have been seen, else 0.
    int done() const;

    // Dump the state of an object.
    void dump() const;

    // = STL styled iteration, compare, and reference functions.

    // Postfix advance.
    Fast_Unbounded_Set_Iterator<T> operator++ (int);

    // Prefix advance.
    Fast_Unbounded_Set_Iterator<T>& operator++ ();

    // Returns a reference to the internal element <this> is pointing to.
    T& operator* ();

    // Check if two iterators point to the same position
    BOOL operator== (const Fast_Unbounded_Set_Iterator<T> &) const;
    BOOL operator!= (const Fast_Unbounded_Set_Iterator<T> &) const;

private:

    // Pointer to the current node in the iteration.
    Fast_Node<T> *_current;

    // Pointer to the set we're iterating over.
    Fast_Unbounded_Set<T> *_set;
};

/**
 * @class Fast_Unbounded_Set_Const_Iterator
 *
 * @brief Implement an const iterator over an unbounded set.
 */
template<class T>
class Fast_Unbounded_Set_Const_Iterator
{
public:
    // = Initialization method.
    Fast_Unbounded_Set_Const_Iterator(const Fast_Unbounded_Set<T> &s, int end = 0);

    // = Iteration methods.

    // Pass back the <next_item> that hasn't been seen in the Set.
    // Returns 0 when all items have been seen, else 1.
    int next(T *&next_item);

    // Move forward by one element in the set.  Returns 0 when all the
    // items in the set have been seen, else 1.
    int advance();

    // Move to the first element in the set.  Returns 0 if the
    // set is empty, else 1.
    int first();

    // Returns 1 when all items have been seen, else 0.
    int done() const;

    // Dump the state of an object.
    void dump() const;

    // = STL styled iteration, compare, and reference functions.

    // Postfix advance.
    Fast_Unbounded_Set_Const_Iterator<T> operator++ (int);

    // Prefix advance.
    Fast_Unbounded_Set_Const_Iterator<T>& operator++ ();

    // Returns a reference to the internal element <this> is pointing to.
    T& operator* ();

    // Check if two iterators point to the same position
    BOOL operator== (const Fast_Unbounded_Set_Const_Iterator<T> &) const;
    BOOL operator!= (const Fast_Unbounded_Set_Const_Iterator<T> &) const;

private:

    // Pointer to the current node in the iteration.
    Fast_Node<T> *_current;

    // Pointer to the set we're iterating over.
    const Fast_Unbounded_Set<T> *_set;
};

/**
 * @class Fast_Unbounded_Set
 *
 * @brief Implement a simple unordered set of <T> of unbounded size.
 *
 * This implementation of an unordered set uses a circular
 * linked list with a dummy node.  This implementation does not
 * allow duplicates, but it maintains FIFO ordering of insertions.
 *
 * <b> Requirements and Performance Characteristics</b>
 *   - Internal Structure
 *       Circular linked list
 *   - Duplicates allowed?
 *       No
 *   - Random access allowed?
 *       No
 *   - Search speed
 *       Linear
 *   - Insert/replace speed
 *       Linear
 *   - Iterator still valid after change to container?
 *       Yes
 *   - Frees memory for removed elements?
 *       Yes
 *   - Items inserted by
 *       Value
 *   - Requirements for contained type
 *       -# Default constructor
 *       -# Copy constructor
 *       -# operator=
 *       -# operator==
 *
 */
template<class T>
class Fast_Unbounded_Set
{
public:
    friend class Fast_Unbounded_Set_Iterator<T>;
    friend class Fast_Unbounded_Set_Const_Iterator<T>;

    // Trait definition.
    typedef Fast_Unbounded_Set_Iterator<T> ITERATOR;
    typedef Fast_Unbounded_Set_Iterator<T> iterator;
    typedef Fast_Unbounded_Set_Const_Iterator<T> CONST_ITERATOR;
    typedef Fast_Unbounded_Set_Const_Iterator<T> const_iterator;

    // = Initialization and termination methods.
    // Constructor. Initialize an empty set
    Fast_Unbounded_Set();

    /**
     * Copy constructor.
     * Initialize this set to be an exact copy of the set provided. 
     */
    Fast_Unbounded_Set(const Fast_Unbounded_Set<T> &);

    /**
     * Assignment operator.
     * Perform a deep copy of the rhs into the lhs. 
     */
    void operator= (const Fast_Unbounded_Set<T> &);

    /**
     * Destructor.
     * Destroy the nodes of the set. 
     */
    ~Fast_Unbounded_Set();

    // = Check boundary conditions.

    /**
     * Returns 1 if the container is empty, otherwise returns 0.
     * Constant time is_empty check. 
     */
    int is_empty() const;

    /** 
     * Always returns 0 since the set can never fill up. 
     */
    int is_full() const;

    // = Classic unordered set operations.

    /**
     * Linear insertion of an item. 
     * Insert <new_item> into the set (doesn't allow duplicates).
     * Returns -1 if failures occur, 1 if item is already present, else
     * 0.
     */
    int insert(const T &new_item);

    /**
     * Insert <item> at the tail of the set (doesn't check for
     * duplicates).
     * Constant time insert at the end of the set. 
     */
    int insert_tail(const T &item);

    /**
     * Linear remove operation. 
     * Remove first occurrence of <item> from the set.  Returns 0 if
     * it removes the item, -1 if it can't find the item, and -1 if a
     * failure occurs.
     */
    int remove(const T &item);

    /**
     * Finds if <item> occurs in the set.  Returns 0 if find succeeds,
     * else -1.
     * Performs a linear find operation. 
     */
    int find(const T &item) const;

    /**
     * Size of the set. Access the size of the set. 
     */
    SIZET size() const;

    // Dump the state of an object.
    void dump() const;

    /**
     * Reset the <Fast_Unbounded_Set> to be empty.
     * Delete the nodes of the set. 
     */
    void reset();

    // = STL-styled unidirectional iterator factory.
    Fast_Unbounded_Set_Iterator<T> begin();
    Fast_Unbounded_Set_Iterator<T> end();

    void swap(Fast_Unbounded_Set<T> &s); 

private:
    // Delete all the nodes in the Set.
    void delete_nodes();

    // Copy nodes into this set.
    void copy_nodes(const Fast_Unbounded_Set<T> &);

    // Head of the linked list of Nodes.
    Fast_Node<T> *_head;

    // Current size of the set.
    SIZET _cur_size;
};

//=============Fast_Unbounded_Set Functions Implements=============

template<class T> inline SIZET
Fast_Unbounded_Set<T>::size() const
{
    return this->_cur_size;
}

template<class T> int
Fast_Unbounded_Set<T>::insert_tail(const T &item)
{
    Fast_Node<T> *temp;

    // Insert <item> into the old dummy node location.
    this->_head->_item = item;

    // Create a new dummy node.
    FAST_NEW_MALLOC_RETURN(temp,
                           static_cast<Fast_Node<T>*> (::malloc(sizeof(Fast_Node<T>))),
                           Fast_Node<T>(this->_head->_next),
                           -1);
    // Link this pointer into the list.
    this->_head->_next = temp;

    // Point the head to the new dummy node.
    this->_head = temp;

    this->_cur_size ++;
    return 0;
}

template<class T> inline void
Fast_Unbounded_Set<T>::reset()
{
    this->delete_nodes();
}

template<class T> void
Fast_Unbounded_Set<T>::copy_nodes(const Fast_Unbounded_Set<T> &us)
{
    for( Fast_Node<T> *curr = us._head->_next;
         curr != us._head;
         curr = curr->_next) 
        this->insert_tail(curr->_item);
}

template<class T> void
Fast_Unbounded_Set<T>::delete_nodes()
{
    if( this->_head == 0 ) 
        return; 

    Fast_Node<T> *curr = this->_head->_next;

    // Keep looking until we've hit the dummy node.

    while( curr != this->_head )
    {
        Fast_Node<T> *temp = curr;
        curr = curr->_next;
        FAST_DES_FREE_CLASS(temp,
                            ::free,
                            Fast_Node<T>);
        this->_cur_size --;
    }

    // Reset the list to be a circular list with just a dummy node.
    this->_head->_next = this->_head;
}

template<class T>
Fast_Unbounded_Set<T>::~Fast_Unbounded_Set()
{
    this->delete_nodes();

    // Delete the dummy node.
    FAST_DES_FREE_CLASS(_head,
                        ::free,
                        Fast_Node<T>);
    this->_head = 0;
}

template<class T>
Fast_Unbounded_Set<T>::Fast_Unbounded_Set()
 : _head(0),
   _cur_size(0)
{
    FAST_NEW_MALLOC(this->_head,
                    (Fast_Node<T>*)::malloc(sizeof(Fast_Node<T>)),
                    Fast_Node<T>);
    // Make the list circular by pointing it back to itself.
    this->_head->_next = this->_head;
}

template<class T>
Fast_Unbounded_Set<T>::Fast_Unbounded_Set(const Fast_Unbounded_Set<T> &us)
 : _head(0),
   _cur_size(0)
{
    FAST_NEW_MALLOC(this->_head,
                    (Fast_Node<T>*)::malloc(sizeof(Fast_Node<T>)),
                    Fast_Node<T>);
    this->_head->_next = this->_head;
    this->copy_nodes(us);
}

template<class T> inline void
Fast_Unbounded_Set<T>::operator= (const Fast_Unbounded_Set<T> &us)
{
    if( this != &us )
    {
        this->delete_nodes();
        this->copy_nodes(us);
    }
}

template<class T> int
Fast_Unbounded_Set<T>::find(const T &item) const
{
    // Set <item> into the dummy node.
    this->_head->_item = item;

    Fast_Node<T> *temp = this->_head->_next;

    // Keep looping until we find the item.
    while( !(temp->_item == item) )
        temp = temp->_next;

    // If we found the dummy node then it's not really there, otherwise,
    // it is there.
    return temp == this->_head ? -1 : 0;
}

template<class T> inline int
Fast_Unbounded_Set<T>::insert(const T &item)
{
    if( this->find(item) == 0 )
        return 1;
    else
        return this->insert_tail(item);
}

template<class T> int
Fast_Unbounded_Set<T>::remove(const T &item)
{
    // Insert the item to be founded into the dummy node.
    this->_head->_item = item;

    Fast_Node<T> *curr = this->_head;

    while( !(curr->_next->_item == item) )
        curr = curr->_next;

    if( curr->_next == this->_head )
        return -1; // Item was not found.
    else
    {
        Fast_Node<T> *temp = curr->_next;
        // Skip over the node that we're deleting.
        curr->_next = temp->_next;
        this->_cur_size --;
        FAST_DES_FREE_CLASS(temp,
                            ::free,
                            Fast_Node<T>);
        return 0;
    }
}

template<class T> inline
Fast_Unbounded_Set_Iterator<T>
Fast_Unbounded_Set<T>::begin()
{
    return Fast_Unbounded_Set_Iterator<T> (*this);
}

template<class T> inline 
Fast_Unbounded_Set_Iterator<T>
Fast_Unbounded_Set<T>::end()
{
    return Fast_Unbounded_Set_Iterator<T> (*this, 1);
}

template<class T> void
Fast_Unbounded_Set_Iterator<T>::dump() const
{
    FAST_TRACE_BEGIN("Fast_Unbounded_Set_Iterator<T>::dump");
    FAST_TRACE_END("Fast_Unbounded_Set_Iterator<T>::dump"); 
}

template<class T>
Fast_Unbounded_Set_Iterator<T>::Fast_Unbounded_Set_Iterator(Fast_Unbounded_Set<T> &s, int end)
 : _current((end == 0 && s._head)? s._head->_next : s._head),
   _set(&s)
{
}

template<class T> inline int
Fast_Unbounded_Set_Iterator<T>::advance()
{
    this->_current = this->_current->_next;
    return this->_current != this->_set->_head;
}

template<class T> inline int
Fast_Unbounded_Set_Iterator<T>::first()
{
    this->_current = this->_set->_head->_next;
    return this->_current != this->_set->_head;
}

template<class T> inline int
Fast_Unbounded_Set_Iterator<T>::done() const
{
    return this->_current == this->_set->_head;
}

template<class T> inline int
Fast_Unbounded_Set_Iterator<T>::next(T *&item)
{
    if( this->_current == this->_set->_head )
        return 0;
    else
    {
        item = &(this->_current->_item);
        return 1;
    }
}

template<class T> inline 
Fast_Unbounded_Set_Iterator<T>
Fast_Unbounded_Set_Iterator<T>::operator++ (int)
{
    Fast_Unbounded_Set_Iterator<T> retv(*this);
    // postfix operator
    this->advance();
    return retv;
}

template<class T> inline
Fast_Unbounded_Set_Iterator<T>&
Fast_Unbounded_Set_Iterator<T>::operator++ ()
{
    // prefix operator
    this->advance();
    return *this;
}

template<class T> inline T&
Fast_Unbounded_Set_Iterator<T>::operator* ()
{
    T *retv = 0;

    int result = this->next(retv);
    FAST_ASSERT(result != 0);

    return *retv;
}

template<class T> inline BOOL
Fast_Unbounded_Set_Iterator<T>::operator== (const Fast_Unbounded_Set_Iterator<T> &rhs) const
{
    return (this->_set == rhs._set && this->_current == rhs._current) ? TRUE : FALSE;
}

template<class T> inline BOOL
Fast_Unbounded_Set_Iterator<T>::operator!= (const Fast_Unbounded_Set_Iterator<T> &rhs) const
{
    return (this->_set != rhs._set || this->_current != rhs._current) ? TRUE : FALSE;
}

template<class T> void
Fast_Unbounded_Set_Const_Iterator<T>::dump() const
{
    FAST_TRACE_BEGIN("Fast_Unbounded_Set_Const_Iterator<T>::dump");
    FAST_TRACE_END("Fast_Unbounded_Set_Const_Iterator<T>::dump"); 
}

template<class T>
Fast_Unbounded_Set_Const_Iterator<T>::Fast_Unbounded_Set_Const_Iterator(const Fast_Unbounded_Set<T> &s, int end)
 : _current(end == 0 ? s._head_->_next : s._head),
   _set(&s)
{
}

template<class T> inline int
Fast_Unbounded_Set_Const_Iterator<T>::advance()
{
    this->_current = this->_current->_next;
    return this->_current != this->_set->_head;
}

template<class T> inline int
Fast_Unbounded_Set_Const_Iterator<T>::first()
{
    this->_current = this->_set->_head->_next;
    return this->_current != this->_set->_head;
}

template<class T> inline int
Fast_Unbounded_Set_Const_Iterator<T>::done() const
{
    return this->_current == this->_set->_head;
}

template<class T> inline int
Fast_Unbounded_Set_Const_Iterator<T>::next(T *&item)
{
    if( this->_current == this->_set->_head )
        return 0;
    else
    {
        item = &(this->_current->_item);
        return 1;
    }
}

template<class T> inline 
Fast_Unbounded_Set_Const_Iterator<T>
Fast_Unbounded_Set_Const_Iterator<T>::operator++ (int)
{
    Fast_Unbounded_Set_Const_Iterator<T> retv (*this);
    // postfix operator
    this->advance();
    return retv;
}

template<class T> inline 
Fast_Unbounded_Set_Const_Iterator<T>&
Fast_Unbounded_Set_Const_Iterator<T>::operator++ ()
{
    // prefix operator
    this->advance();
    return *this;
}

template<class T> inline T&
Fast_Unbounded_Set_Const_Iterator<T>::operator* ()
{
    T *retv = 0;

    int result = this->next(retv);
    FAST_ASSERT(result != 0);

    return *retv;
}

template<class T> inline int
Fast_Unbounded_Set<T>::is_empty() const
{
    return this->_head == this->_head->_next;
}

template<class T> inline int
Fast_Unbounded_Set<T>::is_full() const
{
    return 0; // We should implement a "node of last resort for this..."
}


template<class T> inline void
Fast_Unbounded_Set<T>::swap(Fast_Unbounded_Set<T> &s) 
{
    _FAST::fast_swap_value(_head, s._head); 
    _FAST::fast_swap_value(_cur_size, s._cur_size); 
}

template<class T> void
Fast_Unbounded_Set<T>::dump() const
{
    FAST_TRACE_BEGIN("Fast_Unbounded_Set<T>::dump");
    FAST_TRACE("_head = 0x%08X", this->_head);
    FAST_TRACE("_head->_next = 0x%08X", this->_head->_next);
    FAST_TRACE("_cur_size = %d", this->_cur_size);
#ifdef FAST_DEBUG
    T *item = 0; SIZET count = 1;
    Fast_Unbounded_Set_Iterator<T> iter(*(Fast_Unbounded_Set<T> *)this); 
    for( ; iter.next(item) != 0; iter.advance() ) 
        FAST_TRACE("count = %d, item -> 0x%08X", count++, item);
#endif
    FAST_TRACE_END("Fast_Unbounded_Set<T>::dump");
}


//================Fast_Allocator Classes=====================

/**
 * @class Fast_Allocator
 *
 * @brief Fast_Allocator interface for hashing
 *
 * Interface for a dynamic memory allocator that uses inheritance
 * and dynamic binding to provide extensible mechanisms for
 * allocating and deallocating memory.
 */
class Fast_Allocator
{
public:
    // 
    //virtual  ~Fast_Allocator();
    // Allocate <nbytes>, but don't give them any initial value.
    virtual void *malloc(SIZET nbytes) = 0;

    // Allocate <nbytes>, giving them <initial_value>.
    virtual void *calloc(SIZET nbytes, char initial_value = '\0') = 0;

    // Allocate <n_elem> each of size <elem_size>, giving them
    // <initial_value>.
    virtual void *calloc(SIZET n_elem, SIZET elem_size, char initial_value = '\0') = 0;

    // Free <ptr> (must have been allocated by <Fast_Allocator::malloc>).
    virtual void free(void *ptr) = 0;

    // Free <ptr> with <nbytes> size (must have been allocated by <Fast_Allocator::malloc>).
    virtual void free(void *ptr, SIZET nbytes) = 0;

    // Dump the state of an object.
    virtual void dump() const = 0;
};

/**
 * @class Fast_New_Allocator
 *
 * @brief Fast_New_Allocator interface for hashing
 *
 * Interface for a dynamic memory allocator that uses inheritance
 * and dynamic binding to provide extensible mechanisms for
 * allocating and deallocating memory.
 */
template<class T>
class Fast_New_Allocator : public Fast_Allocator
{
public:
    // Destructor - removes all the elements.
    virtual ~Fast_New_Allocator();

    // Similar to the STL allocator
    // Allocate <code>n</code> size of <code>T</code> elements memory.
    virtual T* allocate(SIZET n) = 0; 

    // Allocate one <code>T</code> elements memory.
    virtual T* allocate() = 0; 

    // Deallocate <code>n</code> size of <code>T</code> elements memory.
    virtual void deallocate(T* p, SIZET n) = 0; 

    // Deallocate <code>T</code> pointer to memory.
    virtual void deallocate(T* p) = 0; 

};

/**
 * @class Fast_Simple_Alloc
 *
 * @brief Fast_Simple_Alloc object for hashing
 */
template<class T>
class Fast_Simple_Alloc : public Fast_New_Allocator<T>
{
public:
    void *malloc(SIZET nbytes) 
        { return (void *) ::malloc(nbytes); }

    void *calloc(SIZET nbytes, char initial_value = '\0')
    {
        void *p = this->malloc(nbytes); 
        ::memset(p, initial_value, nbytes); 
        return p; 
    }

    void *calloc(SIZET n_elem, SIZET elem_size, char initial_value = '\0')
        { return this->calloc(n_elem * elem_size, initial_value); }

    void free(void *ptr) 
        { if( ptr ) ::free(ptr); }

    void free(void *ptr, SIZET nbytes) 
        { this->free(ptr); } 

    // Similar to the STL allocator
    // Allocate <code>n</code> size of <code>T</code> elements memory.
    T* allocate(SIZET n)
        { return 0 == n ? 0 : (T*) this->calloc(n, sizeof(T)); }

    // Allocate one <code>T</code> elements memory.
    T* allocate()
        { return (T*) this->calloc(1, sizeof(T)); }

    // Deallocate <code>n</code> size of <code>T</code> elements memory.
    void deallocate(T* p, SIZET n)
        { this->free(p); }

    // Deallocate <code>T</code> pointer to memory.
    void deallocate(T* p)
        { this->free(p); }

    // Dump the state of an object.
    void dump() const {}
};


//===============Fast_Cached_Allocator Classes=============

// Forward declarations.
template<class T> class Fast_Cached_Mem_Pool_Node;

/**
 * @class Fast_Free_List
 *
 * @brief Implements a free list.
 *
 * This class maintains a free list of nodes of type T.
 */
template<class T>
class Fast_Free_List
{
public:
    // Destructor - removes all the elements from the free_list.
    virtual ~Fast_Free_List();

    // Inserts an element onto the free list (if it isn't past the high
    // water mark).
    virtual void add(T *element) = 0;

    // Takes a element off the freelist and returns it.  It creates
    // <inc> new elements if the size is at or below the low water mark.
    virtual T *remove() = 0;

    // Takes <n> elements off the freelist and returns it.
    // These elements MUST be in series !!!
    virtual T *remove(SIZET n) = 0;

    // Returns the current size of the free list.
    virtual SIZET size() const = 0;

    // Resizes the free list to <newsize>.
    virtual void resize(SIZET newsize) = 0;

    // Dump the state of an object.
    virtual void dump() const = 0;
};

/**
 * @class Fast_Cached_Free_List
 *
 * @brief Implements a free list.
 *
 * This class maintains a free list of nodes of type T.  It
 * depends on the type T having a <get_next> and <set_next>
 * method.  It maintains a mutex so the freelist can be used in
 * a multithreaded program .
 *
 * BUT this ISNOT Thread-Safed, because Thread-Locked not 
 * implemented.
 *
 */
template<class T>
class Fast_Cached_Free_List : public Fast_Free_List<T>
{
public:
    // Traits 

    // = Initialization and termination.
    /**
     * Constructor takes a <mode> (i.e., FAST_FREE_LIST_WITH_POOL or
     * FAST_PURE_FREE_LIST), a count of the number of nodes to
     * <prealloc>, a low and high water mark (<lwm> and <hwm>) that
     * indicate when to allocate more nodes, an increment value (<inc>)
     * that indicates how many nodes to allocate when the list must
     * grow. <chunk_size> is earch element's size in list. 
     */
    Fast_Cached_Free_List(SIZET chuck_size, 
                          int mode = FAST_FREE_LIST_WITH_POOL,
                          SIZET prealloc = FAST_DEFAULT_FREE_LIST_PREALLOC,
                          SIZET lwm = FAST_DEFAULT_FREE_LIST_LWM,
                          SIZET hwm = FAST_DEFAULT_FREE_LIST_HWM,
                          SIZET inc = FAST_DEFAULT_FREE_LIST_INC);

    // Destructor - removes all the elements from the free_list.
    virtual ~Fast_Cached_Free_List();

    // Inserts an element onto the free list (if it isn't past the high
    // water mark).
    virtual void add(T *element);

    // Takes a element off the freelist and returns it.  It creates
    // <inc> new elements if the size is at or below the low water mark.
    virtual T *remove();

    // Takes <n> elements off the freelist and returns it.  It creates <inc> 
    // new elements if we are allowed to do it and the size is at the low 
    // water mark.
    // These elements MUST be in series !!!
    virtual T *remove(SIZET n);

    // Returns the current size of the free list.
    virtual SIZET size() const;

    // Resizes the free list to <newsize>.
    virtual void resize(SIZET newsize);

    // Dump the state of an object.
    void dump() const;

protected:
    // Allocates <n> extra nodes for the freelist.
    virtual void allocate(SIZET n);

    // Removes and frees <n> nodes from the freelist.
    virtual void deallocate(SIZET n);

    // Free list operation mode, either FAST_FREE_LIST_WITH_POOL or
    // FAST_PURE_FREE_LIST.
    int _mode;

    // Pointer to the first node in the freelist.
    T *_free_list;

    // Low water mark.
    SIZET _lwm;

    // High water mark.
    SIZET _hwm;

    // Increment value.
    SIZET _inc;

    // Keeps track of the size of the list.
    SIZET _size;

    // One chunk size in memory list, Cannot changed
    const SIZET _one_chunk_size;

private:
    // = Don't allow these operations for now.
    Fast_Cached_Free_List(const Fast_Cached_Free_List<T> &); 
    void operator= (const Fast_Cached_Free_List<T> &); 
};

/**
 * @class Fast_Cached_Mem_Pool_Node
 *
 * @brief Fast_Cached_Mem_Pool_Node keeps unused memory within a free
 * list.
 *
 * The length of a piece of unused memory must be greater than
 * sizeof (void*).  This makes sense because we'll waste even
 * more memory if we keep them in a separate data structure.
 * This class should really be placed within the Fast_Cached_Allocator
 * class but this can't be done due to C++ compiler portability problems.
 */
template<class T>
class Fast_Cached_Mem_Pool_Node
{
public:
    // Traits
    typedef T node_type; 

    // Return the address of free memory.
    T *addr();

    // Get the next Fast_Cached_Mem_Pool_Node in a list.
    Fast_Cached_Mem_Pool_Node<T> *get_next();

    // Set the next Fast_Cached_Mem_Pool_Node.
    void set_next(Fast_Cached_Mem_Pool_Node<T> *ptr);

private:
    /**
     * Since memory is not used when placed in a free list,
     * we can use it to maintain the structure of free list.
     * I was using union to hide the fact of overlapping memory
     * usage.  However, that cause problem on MSVC.  So, I now turn
     * back to hack this with casting.
     */
    Fast_Cached_Mem_Pool_Node<T> *_next;
};

/**
 * @class Fast_Cached_Allocator
 *
 * @brief A fixed-size allocator that caches items for quicker access.
 *
 * This class enables caching of dynamically allocated,
 * fixed-sized classes.  Notice that the <code>sizeof (TYPE)</code>
 * must be greater than or equal to <code> sizeof (void*) </code> for
 * this to work properly.
 *
 * BUT this ISNOT Thread-Safed, because Thread-Locked not implemented.
 *
 */
template<class T>
class Fast_Cached_Allocator : public Fast_New_Allocator<T>
{
public:
    // Create a cached memory pool with @a n_chunks chunks
    // each with sizeof (TYPE) size.
    Fast_Cached_Allocator(SIZET n_chunks = FAST_DEFAULT_INIT_CHUNKS);

    // Clear things up.
    ~Fast_Cached_Allocator();

    /**
     * Get a chunk of memory from free list cache.  Note that @a nbytes is
     * only checked to make sure that it's less or equal to sizeof T, and is
     * otherwise ignored since @c malloc() always returns a pointer to an
     * item of sizeof (T).
     */
    void *malloc(SIZET nbytes = sizeof(T));

    /**
     * Get a chunk of memory from free list cache, giving them
     * @a initial_value.  Note that @a nbytes is only checked to make sure
     * that it's less or equal to sizeof T, and is otherwise ignored since
     * calloc() always returns a pointer to an item of sizeof (T).
     */
    void *calloc(SIZET nbytes, char initial_value = '\0');

    // This method is a no-op and just returns 0 since the free list
    // only works with fixed sized entities.
    void *calloc(SIZET n_elem, SIZET elem_size, char initial_value = '\0');

    // Return serial chunks of memory back to free list cache.
    void free(void *ptr, SIZET nbytes); 

    // Similar to the STL allocator
    // Allocate <code>n</code> size of <code>T</code> elements memory.
    T* allocate(SIZET n); 

    // Allocate one <code>T</code> elements memory.
    T* allocate(); 

    // Deallocate <code>n</code> size of <code>T</code> elements memory.
    void deallocate(T* p, SIZET n); 

    // Deallocate <code>T</code> pointer to memory.
    void deallocate(T* p); 

    // Dump the state of an object.
    void dump() const;

private:
    // Count allocate chunks count
    SIZET chunks_count(SIZET nbytes); 

    // Count type T's count within nbytes.
    SIZET t_count(SIZET nbytes); 

    // DISABLE: Return a chunk of memory back to free list cache.
    void free(void *ptr); 

    // Allocate new memory pool into chunks with n_chunks size
    void expand_chunks(SIZET n_chunks = 1); 

    // List of memory that we have allocated.
    Fast_Unbounded_Set<char *> _allocated_chunks;

    // Total allocated meory size
    SIZET _chunks_size; 

    // One chunk size in memory list, Cannot changed
    const SIZET _one_chunk_size;

    // Maintain a cached memory free list.
    Fast_Cached_Free_List<Fast_Cached_Mem_Pool_Node<T> > _free_list;
};


//=============Allocator Functions Implements===============

// Empty constructor

template<class T>
Fast_New_Allocator<T>::~Fast_New_Allocator()
{
    // Nothing
}

// Empty constructor

template<class T>
Fast_Free_List<T>::~Fast_Free_List()
{
    // Nothing
}

// Default constructor that takes in a preallocation number
// (<prealloc>), a low and high water mark (<lwm> and <hwm>) and an
// increment value (<inc>)

template<class T>
Fast_Cached_Free_List<T>::Fast_Cached_Free_List(SIZET chuck_size, 
                                                int mode,
                                                SIZET prealloc,
                                                SIZET lwm,
                                                SIZET hwm,
                                                SIZET inc)
  : _mode(mode),
    _free_list(0),
    _lwm(lwm),
    _hwm(hwm),
    _inc(inc),
    _size(0), 
    _one_chunk_size(chuck_size) 
{
    this->allocate(prealloc);
}

// Destructor - removes all the elements from the free_list

template<class T>
Fast_Cached_Free_List<T>::~Fast_Cached_Free_List()
{
    if( this->_mode != FAST_PURE_FREE_LIST ) 
    {
        while( this->_free_list != 0 )
        {
            T *temp = this->_free_list;
            this->_free_list = this->_free_list->get_next();
            delete temp;
        }
    }
}

// Allocates <n> extra nodes for the freelist

template<class T> void
Fast_Cached_Free_List<T>::allocate(SIZET n)
{
    for( ; n > 0; n -- )
    {
        T *temp = 0;
        FAST_NEW(temp, T);
        temp->set_next(this->_free_list);
        this->_free_list = temp;
        this->_size ++;
    }
}

// Removes and frees <n> nodes from the freelist.

template<class T> void
Fast_Cached_Free_List<T>::deallocate(SIZET n)
{
    for( ; this->_free_list != 0 && n > 0; n -- )
    {
        T *temp = this->_free_list;
        this->_free_list = this->_free_list->get_next();
        delete temp;
        this->_size --;
    }
}

// Inserts an element onto the free list (if we are allowed to manage
// elements withing and it pasts the high water mark, delete the
// element)

template<class T> inline void 
Fast_Cached_Free_List<T>::add(T *element)
{
    // Check to see that we not at the high water mark.
    if( this->_mode == FAST_PURE_FREE_LIST || this->_size < this->_hwm )
    {
        element->set_next(this->_free_list);
        this->_free_list = element;
        this->_size ++;
    }
    else
        delete element;
}

// Takes a element off the freelist and returns it.  It creates <inc>
// new elements if we are allowed to do it and the size is at the low
// water mark.

template<class T> T *
Fast_Cached_Free_List<T>::remove()
{
    // If we are at the low water mark, add some nodes
    if( this->_mode != FAST_PURE_FREE_LIST && this->_size <= this->_lwm )
        this->allocate(this->_inc);

    // Remove a node
    T *temp = this->_free_list;

    if( temp != 0 )
    {
        this->_free_list = this->_free_list->get_next();
        this->_size --;
    }

    return temp;
}

// Takes <n> elements off the freelist and returns it.  It creates <inc> 
// new elements if we are allowed to do it and the size is at the low 
// water mark.
// These elements MUST be in series !!!

template<class T> T *
Fast_Cached_Free_List<T>::remove(SIZET n)
{
    // If we are at the low water mark, add some nodes
    if( this->_mode != FAST_PURE_FREE_LIST && this->_size <= this->_lwm )
        this->allocate(this->_inc);

    // Return the first node
    T *first = 0, *temp = 0, *next = 0;
    SIZET n_nodes = 0; 
    int one_chunk_size = (int) _one_chunk_size * (-1); 

    first = temp = this->_free_list; 

    if( first != 0 ) 
    {
        // get first chunk
        n_nodes = 1; 

        // Check list nodes one by one
        while( (next = temp->get_next()) != 0 && n_nodes < n ) 
        {
            // Check if is in series. 
            int between_size = (char *) next - (char *) temp; 
            if( between_size != one_chunk_size ) 
            {
                // Check if they are reverse forward.
                int ps = one_chunk_size * (-1); 
                if( between_size == ps  && n_nodes == 1 ) 
                    one_chunk_size = ps; 
                else 
                {
                    first = temp = next; 
                    n_nodes = 1; 
                }
            }
            temp = next; 
            n_nodes += 1; 
        }

        // Remove these list of nodes.
        if( n_nodes >= n ) 
        {
            this->_free_list = next; 
            this->_size -= n_nodes; 
        }
        else
            first = temp = 0; 
    }

    // Return the start pointer that is smaller
    return temp < first ? temp : first; 
}

// Returns the current size of the free list

template<class T> inline SIZET 
Fast_Cached_Free_List<T>::size() const
{
    return this->_size;
}

// Resizes the free list to <newsize>

template<class T> inline void 
Fast_Cached_Free_List<T>::resize(SIZET newsize)
{
    // Check if we are allowed to resize
    if( this->_mode != FAST_PURE_FREE_LIST ) 
    {
        // Check to see if we grow or shrink
        if( newsize < this->_size )
            this->deallocate(this->_size - newsize);
        else 
            this->allocate(newsize - this->_size);
    }
}

template<class T> void 
Fast_Cached_Free_List<T>::dump() const
{
    FAST_TRACE_BEGIN("Fast_Cached_Free_List<T>::dump()"); 
    FAST_TRACE("_mode = %d, _lwm = %d, _inc = %d, _size = %d", _mode, _lwm, _hwm, _inc, _size); 
    FAST_TRACE("_one_chunk_size = %d", _one_chunk_size); 
    FAST_TRACE(" (first)    --> [0]"); 
#ifdef FAST_DEBUG
    int n = 0; 
    T *ptr = this->_free_list; 
    while( ptr ) {
        FAST_TRACE(" 0x%08X --> [%d]", ptr, ++n); 
        ptr = ptr->get_next(); 
    }
#endif
    FAST_TRACE("_one_chunk_size = %d", _one_chunk_size); 
    FAST_TRACE_END("Fast_Cached_Free_List<T>::dump()"); 
}

template<class T> inline T *
Fast_Cached_Mem_Pool_Node<T>::addr()
{
    // This should be done using a single reinterpret_cast, but Sun/CC
    // (4.2) gets awfully confused when T is a char[20] (and maybe other
    // types).
    return static_cast<T *> (static_cast<void *> (this));
}

template<class T> inline 
Fast_Cached_Mem_Pool_Node<T> *
Fast_Cached_Mem_Pool_Node<T>::get_next()
{
    return this->_next;
}

template<class T> inline void
Fast_Cached_Mem_Pool_Node<T>::set_next(Fast_Cached_Mem_Pool_Node<T> *ptr)
{
    this->_next = ptr;
}

template<class T>
Fast_Cached_Allocator<T>::Fast_Cached_Allocator(SIZET n_chunks)
  : _allocated_chunks(), 
    _chunks_size(0), 
    _one_chunk_size(_FAST::fast_round_up(sizeof(T))), 
    _free_list(_one_chunk_size, FAST_PURE_FREE_LIST) 
{
    expand_chunks(n_chunks); 
}

template<class T>
Fast_Cached_Allocator<T>::~Fast_Cached_Allocator()
{
    //free all momery
    char **item = 0;
    Fast_Unbounded_Set_Iterator<char *> iter(_allocated_chunks); 
    for( ; iter.next(item); iter.advance() ) 
    {
        if( item ) 
            FAST_DELETE_ARRAY(*item);
    }
}

template<class T> void 
Fast_Cached_Allocator<T>::expand_chunks(SIZET n_chunks)
{
    // To maintain alignment requirements, make sure that each element
    // inserted into the free list is aligned properly for the platform.
    // Since the memory is allocated as a char[], the compiler won't help.
    // To make sure enough room is allocated, round up the size so that
    // each element starts aligned.
    //
    // NOTE - this would probably be easier by defining pool as a pointer
    // to T and allocating an array of them (the compiler would probably
    // take care of the alignment for us), but then the FAST_NEW below would
    // require a default constructor on T - a requirement that is not in
    // previous versions
    //
    //SIZET chunk_size = sizeof(T);
    //if( chunk_size % FAST_MALLOC_ALIGN != 0 )
    //    chunk_size += (FAST_MALLOC_ALIGN - (chunk_size % FAST_MALLOC_ALIGN));
    //// USE fast_round_up() to do it.

    // default malloc 2 bigger than previous chunks
    if( n_chunks <= 1 ) 
        n_chunks = (_chunks_size/_one_chunk_size) * 2; 
    if( n_chunks < FAST_DEFAULT_INIT_CHUNKS ) 
        n_chunks = FAST_DEFAULT_INIT_CHUNKS; 

    SIZET new_size = n_chunks * _one_chunk_size; 
    _chunks_size += new_size; 

    char *pool = 0; 
    FAST_NEW(pool, char[new_size]);
    
    _allocated_chunks.insert(pool); 

    for( SIZET c = 0; c < n_chunks; c ++ )
    {
        void* placement = pool + c * _one_chunk_size;
        this->_free_list.add(new (placement) Fast_Cached_Mem_Pool_Node<T>);
    }
    // Put into free list using placement contructor, no real memory
    // allocation in the above <new>.
}

template<class T> inline SIZET
Fast_Cached_Allocator<T>::chunks_count(SIZET nbytes)
{
    // Count allocate chunks
    SIZET n_chunks = nbytes / _one_chunk_size; 
    if( nbytes % _one_chunk_size != 0 ) 
        n_chunks += 1; 
    return n_chunks; 
}

template<class T> inline SIZET
Fast_Cached_Allocator<T>::t_count(SIZET nbytes)
{
    // Must allocate enough memory to contain <n> number of T.
    SIZET n = nbytes / sizeof(T); 
    if( nbytes % sizeof(T) != 0 )
        n += 1; 
    return n; 
}

template<class T> inline void *
Fast_Cached_Allocator<T>::malloc(SIZET nbytes)
{
    return (void *) this->allocate(this->t_count(nbytes)); 
}

template<class T> inline void *
Fast_Cached_Allocator<T>::calloc(SIZET nbytes, char initial_value)
{
    void *ptr = this->malloc(nbytes); 
    ::memset(ptr, initial_value, sizeof(T));
    return ptr;
}

template<class T> inline void *
Fast_Cached_Allocator<T>::calloc(SIZET n_elem, SIZET elem_size, char initial_value)
{
    return this->calloc(n_elem * elem_size, initial_value); 
}

template<class T> inline void
Fast_Cached_Allocator<T>::free(void * ptr)
{
    if( ptr != 0 )
        this->_free_list.add((Fast_Cached_Mem_Pool_Node<T> *) ptr) ;
}

template<class T> void
Fast_Cached_Allocator<T>::free(void * ptr, SIZET nbytes)
{
    // type T's count in nbytes.
    SIZET n = this->t_count(nbytes); 

    // Large memory use ::malloc() and ::free() 
    if( this->chunks_count(n * sizeof(T)) > FAST_MAX_ALLOC_CHUNKS_ONETIME ) 
    {
        if( ptr ) ::free(ptr); 
        return; 
    }

    // This <p> is a list chunks memory, free them one by one.
    char *p = (char *) ptr; 
    int chunk_sizes = (int) nbytes; 
    while( p != 0 && chunk_sizes > 0 ) 
    {
        this->free(p); 
        p += _one_chunk_size; 
        chunk_sizes -= _one_chunk_size; 
    }
}

template<class T> T* 
Fast_Cached_Allocator<T>::allocate(SIZET n)
{
    // Count allocate chunks
    SIZET n_chunks = this->chunks_count(n * sizeof(T)); 

    // Fast_Cached_Allocator<T> MOST allocate 1 chunk one time. 
    // And can also allocate more chunks in series, But if allocate
    // larger than this below, allocator will use ::malloc to
    // allocate to decrease time.
    if( n_chunks > FAST_MAX_ALLOC_CHUNKS_ONETIME ) 
    {
        void *p = ::malloc(n_chunks * _one_chunk_size); 
        return (T*) p; 
    }

    // addr() call is really not absolutely necessary because of the way
    // Fast_Cached_Mem_Pool_Node's internal structure arranged.
    void *ptr = this->_free_list.remove(n_chunks)->addr();

    // No free memory, malloc new
    if( ptr == 0 ) 
    {
        expand_chunks(n); 
        ptr = this->_free_list.remove(n_chunks)->addr();
    }

    //::memset(ptr, 0, sizeof(T));
    return (T*) ptr;
}

template<class T> T* 
Fast_Cached_Allocator<T>::allocate()
{
    // addr() call is really not absolutely necessary because of the way
    // Fast_Cached_Mem_Pool_Node's internal structure arranged.
    void *ptr = this->_free_list.remove()->addr();

    // No free memory, malloc new
    if( ptr == 0 ) 
    {
        expand_chunks(); 
        ptr = this->_free_list.remove()->addr();
    }

    //::memset(ptr, 0, sizeof(T));
    return (T*) ptr;
}

template<class T> void
Fast_Cached_Allocator<T>::deallocate(T* p, SIZET n)
{
    // Count allocate chunks
    SIZET n_chunks = this->chunks_count(n * sizeof(T)); 

    // Large memory use ::malloc() and ::free() 
    if( n_chunks > FAST_MAX_ALLOC_CHUNKS_ONETIME ) 
    {
        if( p ) ::free(p); 
        return; 
    }

    // This <p> is a list chunks memory, free them one by one.
    char *ptr = (char *) p; 
    int n_size = n * sizeof(T); 
    while( ptr && n_size > 0 ) 
    {
        this->free(ptr); 
        ptr += _one_chunk_size; 
        n_size -= _one_chunk_size; 
    }
}

template<class T> inline void 
Fast_Cached_Allocator<T>::deallocate(T* p)
{
    this->free(p); 
}

template<class T> void
Fast_Cached_Allocator<T>::dump() const
{
    FAST_TRACE_BEGIN("Fast_Cached_Allocator<T>::dump()"); 
    FAST_TRACE("sizeof(Fast_Cached_Allocator<T>) = %d", sizeof(Fast_Cached_Allocator<T>)); 
    FAST_TRACE("_chunks_size = %d", _chunks_size); 
    FAST_TRACE("_allocated_chunks.size() = %d", _allocated_chunks.size()); 
    FAST_TRACE("_free_list.size() = %d", _free_list.size()); 
    _free_list.dump(); 
    FAST_TRACE("_chunks_size = %d", _chunks_size); 
    FAST_TRACE("_allocated_chunks.size() = %d", _allocated_chunks.size()); 
    FAST_TRACE("_free_list.size() = %d", _free_list.size()); 
    FAST_TRACE_END("Fast_Cached_Allocator<T>::dump()"); 
}


_FAST_END_NAMESPACE
_FAST_USE_NAMESPACES

#endif
