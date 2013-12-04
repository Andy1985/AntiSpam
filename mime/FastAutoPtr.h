//=============================================================================
/**
 *  @file    FastAutoPtr.h
 *
 *  ver 1.0.0 2004/06/20 Naven Exp, for Fast Common Framework.
 *
 *  @author Naven (Navy Chen) 2004/06/20 created.
 */
//=============================================================================

#ifndef _FAST_COMM_FASTAUTOPTR_H
#define _FAST_COMM_FASTAUTOPTR_H

#if defined(_WIN32) || defined(__WIN32__)

#if !defined (NAVEN_PRAGMA_ONCE)
# pragma once
#endif /* NAVEN_PRAGMA_ONCE */

#endif /* _WIN32 */


#include <new>



_FAST_BEGIN_NAMESPACE


#define FastAutoPtr         FastAuto_Ptr
#define AutoPtr             Auto_Ptr
#define FastAutoArrayPtr    FastAuto_Array_Ptr
#define AutoArrayPtr        FastAuto_Array_Ptr


//================class Auto_Ptr define=================

/**
 *  A wrapper class to provide auto_ptr with reference semantics.  For
 *  example, an auto_ptr can be assigned (or constructed from) the result of
 *  a function which returns an auto_ptr by value.
 *
 *  All the auto_ptr_ref stuff should happen behind the scenes.
 */
template<typename T>
struct Auto_Ptr_Ref
{
   T* _ptr;

   explicit
   Auto_Ptr_Ref(T* p)
    : _ptr(p) 
   {}
};

/**
 *  @brief  A simple smart pointer providing strict ownership semantics.
 *
 *  The Standard says:
 *  <pre>
 *  An @c auto_ptr owns the object it holds a pointer to.  Copying an
 *  @c auto_ptr copies the pointer and transfers ownership to the destination.
 *  If more than one @c auto_ptr owns the same object at the same time the
 *  behavior of the program is undefined.
 *
 *  The uses of @c auto_ptr include providing temporary exception-safety for
 *  dynamically allocated memory, passing ownership of dynamically allocated
 *  memory to a function, and returning dynamically allocated memory from a
 *  function.  @c auto_ptr does not meet the CopyConstructible and Assignable
 *  requirements for Standard Library <a href="tables.html#65">container</a>
 *  elements and thus instantiating a Standard Library container with an
 *  @c auto_ptr results in undefined behavior.
 *  </pre>
 *  Quoted from [20.4.5]/3.
 *
 *  Good examples of what can and cannot be done with auto_ptr can be found
 *  in the libstdc++ testsuite.
 *
 *  @if maint
 *  _GLIBCPP_RESOLVE_LIB_DEFECTS
 *  127.  auto_ptr<> conversion issues
 *  These resolutions have all been incorporated.
 *  @endif
 */
template<typename T>
class Auto_Ptr
{
private:
    T* _ptr;

public:
    /// The pointed-to type.
    typedef T element_type;
    
    /**
     *  @brief  An %auto_ptr is usually constructed from a raw pointer.
     *  @param  p  A pointer (defaults to NULL).
     *
     *  This object now @e owns the object pointed to by @a p.
     */
    explicit
    Auto_Ptr(element_type *p = 0) 
     : _ptr( p ) 
    { }
    
    /**
     *  @brief  An %auto_ptr can be constructed from another %auto_ptr.
     *  @param  a  Another %auto_ptr of the same type.
     *
     *  This object now @e owns the object previously owned by @a a, which has
     *  given up ownsership.
     */
    Auto_Ptr(Auto_Ptr &a) 
     : _ptr( a.release() ) 
    { }
    
    /**
     *  @brief  An %auto_ptr can be constructed from another %auto_ptr.
     *  @param  a  Another %auto_ptr of a different but related type.
     *
     *  A pointer-to-Tp1 must be convertible to a pointer-to-Tp/element_type.
     *
     *  This object now @e owns the object previously owned by @a a, which has
     *  given up ownsership.
     */
    template<typename _Tp1>
    Auto_Ptr(Auto_Ptr<_Tp1> &a) 
     : _ptr( a.release() ) 
    { }

    /**
     *  @brief  %auto_ptr assignment operator.
     *  @param  a  Another %auto_ptr of the same type.
     *
     *  This object now @e owns the object previously owned by @a a, which has
     *  given up ownsership.  The object that this one @e used to own and
     *  track has been deleted.
     */
    Auto_Ptr&
    operator=(Auto_Ptr &a) 
    {
        reset(a.release());
        return *this;
    }
    
    /**
     *  @brief  %auto_ptr assignment operator.
     *  @param  a  Another %auto_ptr of a different but related type.
     *
     *  A pointer-to-Tp1 must be convertible to a pointer-to-Tp/element_type.
     *
     *  This object now @e owns the object previously owned by @a a, which has
     *  given up ownsership.  The object that this one @e used to own and
     *  track has been deleted.
     */
    template<typename _Tp1>
    Auto_Ptr&
    operator=(Auto_Ptr<_Tp1> &a) 
    {
        reset(a.release());
        return *this;
    }

    /**
     *  When the %auto_ptr goes out of scope, the object it owns is deleted.
     *  If it no longer owns anything (i.e., @c get() is @c NULL), then this
     *  has no effect.
     *
     *  @if maint
     *  The C++ standard says there is supposed to be an empty throw
     *  specification here, but omitting it is standard conforming.  Its
     *  presence can be detected only if _Tp::~_Tp() throws, but this is
     *  prohibited.  [17.4.3.6]/2
     *  @end maint
     */
    ~Auto_Ptr() { if( _ptr ) delete _ptr; }

    /**
     *  @brief  Smart pointer dereferencing.
     *
     *  If this %auto_ptr no longer owns anything, then this operation will
     *  crash.  (For a smart pointer, "no longer owns anything" is the same as
     *  being a null pointer, and you know what happens when you dereference
     *  one of those...)
     */
    element_type&
    operator*() const { return *_ptr; } 

    /**
     *  @brief  Smart pointer dereferencing.
     *
     *  This returns the pointer itself, which the language then will
     *  automatically cause to be dereferenced.
     */
    element_type*
    operator->() const { return _ptr; }

    /**
     *  @brief  Bypassing the smart pointer.
     *  @return  The raw pointer being managed.
     *
     *  You can get a copy of the pointer that this object owns, for
     *  situations such as passing to a function which only accepts a raw
     *  pointer.
     *
     *  @note  This %auto_ptr still owns the memory.
     */
    element_type*
    get() const { return _ptr; }

    /**
     *  @brief  Bypassing the smart pointer.
     *  @return  The raw pointer being managed.
     *
     *  You can get a copy of the pointer that this object owns, for
     *  situations such as passing to a function which only accepts a raw
     *  pointer.
     *
     *  @note  This %auto_ptr no longer owns the memory.  When this object
     *  goes out of scope, nothing will happen.
     */
    element_type*
    release() 
    {
        element_type* tmp = _ptr;
        _ptr = 0;
        return tmp;
    }

    /**
     *  @brief  Forcibly deletes the managed object.
     *  @param  p  A pointer (defaults to NULL).
     *
     *  This object now @e owns the object pointed to by @a p.  The previous
     *  object has been deleted.
     */
    void
    reset(element_type* p = 0) 
    {
        if( p != _ptr )
        {
            if( _ptr ) delete _ptr; 
            _ptr = p;
        }
    }

    /** 
     *  @brief  Automatic conversions
     *
     *  These operations convert an %auto_ptr into and from an auto_ptr_ref
     *  automatically as needed.  This allows constructs such as
     *  @code
     *    auto_ptr<Derived>  func_returning_auto_ptr(.....);
     *    ...
     *    auto_ptr<Base> ptr = func_returning_auto_ptr(.....);
     *  @endcode
     */
    Auto_Ptr(Auto_Ptr_Ref<element_type> ref) 
     : _ptr(ref._ptr) 
    {}
    
    Auto_Ptr&
    operator=(Auto_Ptr_Ref<element_type> ref) 
    {
        if( ref._ptr != this->get() ) 
        {
            if( _ptr ) delete _ptr;
            _ptr = ref._ptr;
        }
        return *this;
    }
    
    template<typename _Tp1>
    operator Auto_Ptr_Ref<_Tp1>() 
    { 
        return Auto_Ptr_Ref<_Tp1>(this->release()); 
    }
    
    template<typename _Tp1>
    operator Auto_Ptr<_Tp1>() 
    { 
        return Auto_Ptr<_Tp1>(this->release()); 
    }

};


//================class FastAuto_Ptr define=================

/**
 * @class FastAuto_Basic_Ptr
 *
 * @brief Implements the draft C++ standard auto_ptr abstraction.
 * This class allows one to work on non-object (basic) types
 *
 * @author Naven
 * @see ACE wrappers
 */
template <class X>
class FastAuto_Basic_Ptr
{
public:
    // = Initialization and termination methods
    FastAuto_Basic_Ptr(X *p = 0) : m_pPtr(p) {}

    FastAuto_Basic_Ptr(FastAuto_Basic_Ptr<X> &ap);
    FastAuto_Basic_Ptr<X> &operator= (FastAuto_Basic_Ptr<X> &rhs);
    ~FastAuto_Basic_Ptr(void);

    // = Accessor methods.
    X &operator *() const;
    X *get(void) const;
    X *release(void);
    X *detach(void);
    void reset(X *p = 0);

protected:
    X *m_pPtr;
};


/**
 * @class FastAuto_Ptr
 *
 * @brief Implements the draft C++ standard auto_ptr abstraction.
 */
template <class X>
class FastAuto_Ptr : public FastAuto_Basic_Ptr<X>
{
public:
    // = Initialization and termination methods
    FastAuto_Ptr(X *p = 0) : FastAuto_Basic_Ptr<X>(p) {}

    X *operator-> () const;
};


/**
 * @class FastAuto_Basic_Array_Ptr
 *
 * @brief Implements an extension to the draft C++ standard auto_ptr
 * abstraction.  This class allows one to work on non-object
 * (basic) types that must be treated as an array, e.g.,
 * deallocated via "delete [] foo".
 */
template<class X>
class FastAuto_Basic_Array_Ptr
{
public:
    // = Initialization and termination methods.
    FastAuto_Basic_Array_Ptr(X *p = 0) : m_pPtr(p) {}

    FastAuto_Basic_Array_Ptr(FastAuto_Basic_Array_Ptr<X> &ap);
    FastAuto_Basic_Array_Ptr<X> &operator= (FastAuto_Basic_Array_Ptr<X> &rhs);
    ~FastAuto_Basic_Array_Ptr(void);

    // = Accessor methods.
    X &operator* () const;
    X &operator[] (int i) const;
    X *get(void) const;
    X *release(void);
    X *detach(void);
    void reset(X *p = 0);

protected:
    X *m_pPtr;
};


/**
 * @class FastAuto_Array_Ptr
 *
 * @brief Implements an extension to the draft C++ standard auto_ptr
 * abstraction.
 */
template<class X>
class FastAuto_Array_Ptr : public FastAuto_Basic_Array_Ptr<X>
{
public:
    // = Initialization and termination methods.
    FastAuto_Array_Ptr(X *p = 0)
        : FastAuto_Array_Ptr<X> (p) {}

    X *operator-> () const;
};


// Some platforms have an older version of auto_ptr
// support, which lacks reset, and cannot be disabled
// easily.  Portability to these platforms requires
// use of the following FASTAUTO_PTR_RESET macro.
# if defined (FASTAUTO_PTR_LACKS_RESET)
#   define FASTAUTO_PTR_RESET(X,Y,Z) \
      do { \
        if( Y != X.get() ) \
          { \
            X.release(); \
            X = auto_ptr<Z> (Y); \
          } \
      } while (0)
# else /* ! FASTAUTO_PTR_LACKS_RESET */
#   define FASTAUTO_PTR_RESET(X,Y,Z) \
      do { \
         X.reset(Y); \
      } while(0)
# endif /* FASTAUTO_PTR_LACKS_RESET */



//============FastAutoPtr Inline Functions===============

template<class X> inline
FastAuto_Basic_Ptr<X>::FastAuto_Basic_Ptr(FastAuto_Basic_Ptr<X> &rhs)
 : m_pPtr(rhs.release())
{
    // TRACE ("FastAuto_Basic_Ptr<X>::FastAuto_Basic_Ptr");
}

template<class X> inline X *
FastAuto_Basic_Ptr<X>::get(void) const
{
    // TRACE ("ACE_Auto_Basic_Ptr<X>::get");
    return this->m_pPtr;
}

template<class X> inline X *
FastAuto_Basic_Ptr<X>::release(void)
{
    // TRACE ("FastAuto_Basic_Ptr<X>::release");
    X *old = this->m_pPtr;
    this->m_pPtr = 0;
    return old;
}

template<class X> inline X *
FastAuto_Basic_Ptr<X>::detach(void)
{
    return this->release();
}

template<class X> inline void
FastAuto_Basic_Ptr<X>::reset(X *p)
{
    // TRACE ("FastAuto_Basic_Ptr<X>::reset");
    if( this->get() != p ) 
        delete this->get();
    this->m_pPtr = p;
}

template<class X> inline FastAuto_Basic_Ptr<X> &
FastAuto_Basic_Ptr<X>::operator= (FastAuto_Basic_Ptr<X> &rhs)
{
    // TRACE ("FastAuto_Basic_Ptr<X>::operator=");
    if( this != &rhs )
    {
        this->reset(rhs.release());
    }
    return *this;
}

template<class X> inline
FastAuto_Basic_Ptr<X>::~FastAuto_Basic_Ptr(void)
{
    // TRACE ("FastAuto_Basic_Ptr<X>::~FastAuto_Basic_Ptr");
    delete this->get();
}

template<class X> inline X &
FastAuto_Basic_Ptr<X>::operator *() const
{
    // TRACE ("FastAuto_Basic_Ptr<X>::operator *()");
    return *this->get();
}

template<class X> inline X *
FastAuto_Ptr<X>::operator->() const
{
    // TRACE ("auto_ptr<X>::operator->");
    return this->get();
}

template<class X> inline X *
FastAuto_Basic_Array_Ptr<X>::get(void) const
{
    // TRACE ("FastAuto_Basic_Array_Ptr<X>::get");
    return this->m_pPtr;
}

template<class X> inline X *
FastAuto_Basic_Array_Ptr<X>::release(void)
{
    // TRACE ("FastAuto_Basic_Array_Ptr<X>::release");
    X *old = this->m_pPtr;
    this->m_pPtr = 0;
    return old;
}

template<class X> inline X *
FastAuto_Basic_Array_Ptr<X>::detach(void)
{
    return this->release();
}

template<class X> inline void
FastAuto_Basic_Array_Ptr<X>::reset(X *p)
{
    // TRACE ("FastAuto_Basic_Array_Ptr<X>::reset");
    if( this->get() != p )
        delete [] this->get();
    this->m_pPtr = p;
}

template<class X> inline
FastAuto_Basic_Array_Ptr<X>::FastAuto_Basic_Array_Ptr(FastAuto_Basic_Array_Ptr<X> &rhs)
 : m_pPtr(rhs.release())
{
    // TRACE ("FastAuto_Basic_Array_Ptr<X>::FastAuto_Basic_Array_Ptr");
}

template<class X> inline FastAuto_Basic_Array_Ptr<X> &
FastAuto_Basic_Array_Ptr<X>::operator= (FastAuto_Basic_Array_Ptr<X> &rhs)
{
    // TRACE ("FastAuto_Basic_Array_Ptr<X>::operator=");
    if( this != &rhs ) 
    {
        this->reset(rhs.release());
    }
    return *this;
}

template<class X> inline
FastAuto_Basic_Array_Ptr<X>::~FastAuto_Basic_Array_Ptr(void)
{
    // TRACE ("FastAuto_Basic_Array_Ptr<X>::~FastAuto_Basic_Array_Ptr");
    delete [] this->get();
}

template<class X> inline X &
FastAuto_Basic_Array_Ptr<X>::operator *() const
{
    return *this->get();
}

template<class X> inline X &
FastAuto_Basic_Array_Ptr<X>::operator[](int i) const
{
    X *array = this->get();
    return array[i];
}

template<class X> inline X *
FastAuto_Array_Ptr<X>::operator->() const
{
    return this->get();
}


_FAST_END_NAMESPACE

#endif
