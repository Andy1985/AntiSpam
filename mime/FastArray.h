//=============================================================================
/**
 *  @file    FastArray.h
 *
 *  ver 1.0.0 2004/06/20 Naven Exp, for Fast Common Framework.
 *
 *  @author Naven (Navy Chen) 2004/06/20 created.
 */
//=============================================================================

#ifndef _FAST_COMM_FASTARRAY_H
#define _FAST_COMM_FASTARRAY_H

#if defined(_WIN32) || defined(__WIN32__)

#if !defined (NAVEN_PRAGMA_ONCE)
# pragma once
#endif /* NAVEN_PRAGMA_ONCE */

#endif /* _WIN32 */


#include <new>
#include "FastBase.h"



_FAST_BEGIN_NAMESPACE


// Forward declaration.
template<class T>
class FastArray; 

template <class T> 
class FastArray_Iterator; 


#ifndef Array 
#define Array FastArray 
#endif 

#ifndef ArrayIterator 
#define ArrayIterator FastArray_Iterator 
#endif 


/**
 * @class FastArray
 *
 * @brief Implement a simple dynamic array
 *
 * This parametric class implements a simple dynamic array;
 * resizing must be controlled by the user. No comparison or find
 * operations are implemented. 
 *
 * <p>
 * Class T must implemented default constructor and destructor, 
 * and assign operator function (operator=), and copy constructor. 
 *
 * 类T必须实现确省构造函数和析构函数，operator=()符值运算符函数
 * 和copy拷贝构造函数，即以下形式：<p><pre>
 * class T {
 * public:
 *      T(); 
 *      T(const T& t); 
 *      ~T(); 
 *      T& operator=(const T& t); 
 * };
 * </pre>
 */
template<class T>
class FastArray
{
public:
    // Define a "trait"
    typedef T TYPE;
    typedef T value_type;
    typedef T& reference;
    typedef T* pointer;
    typedef FastArray_Iterator<T> ITERATOR;
    typedef FastArray_Iterator<T> iterator;

    // = Initialization and termination methods.

    // Dynamically create an uninitialized array.
    FastArray(size_t size = 0);

    // Dynamically initialize the entire array to the <default_value>.
    FastArray(size_t size, const T &default_value);

    /**
     * The copy constructor performs initialization by making an exact
     * copy of the contents of parameter <s>, i.e., *this == s will
     * return true.
     */
    FastArray(const FastArray<T> &s);

    /**
     * Assignment operator performs an assignment by making an exact
     * copy of the contents of parameter <s>, i.e., *this == s will
     * return true.  Note that if the <m_nMaxSize> of <m_pArray> is >= than
     * <s.m_nMaxSize> we can copy it without reallocating.  However, if
     * <m_nMaxSize> is < <s.m_nMaxSize> we must delete the <m_pArray>,
     * reallocate a new <m_pArray>, and then copy the contents of <s>.
     */
    void operator= (const FastArray<T> &s); 

    /**
     * Add size of the items in array <s>, if size = -1, add all. 
     */
    void add(const FastArray<T> &s, int size = -1); 

    /**
     * Copy size of the items in array <s>, if size = -1, copy all. 
     */
    void copyFrom(const FastArray<T> &s, int size = -1); 

    /**
     * Copy size of the this items to array <s>, if size = -1, copy all. 
     */
    void copyTo(FastArray<T> &s, int size = -1) const; 

    // Clean up the array (e.g., delete dynamically allocated memory).
    virtual ~FastArray();

    /**
     * Clears out the array, and deallocate the array's buffer.
     */
    void release();

    // = Set/get methods.

    // Set item in the array at location <slot>.  Doesn't
    // perform range checking.
    T &operator [] (size_t slot);

    // Get item in the array at location <slot>.  Doesn't
    // perform range checking.
    const T &operator [] (size_t slot) const;

    // Set an item in the array at location <slot>.  Returns
    // -1 if <slot> is not in range, else returns 0.
    int set(const T &new_item, size_t slot);

    /**
     * Get an item in the array at location <slot>.  Returns -1 if
     * <slot> is not in range, else returns 0.  Note that this function
     * copies the item.  If you want to avoid the copy, you can use
     * the const operator [], but then you'll be responsible for range checking.
     */
    int get(T &item, size_t slot) const;

    // Returns the <m_nCurSize> of the array.
    size_t size() const;

    /**
     * Changes the size of the array to match <new_size>.
     * It copies the old contents into the new array.
     * Return -1 on failure.
     */
    int size(size_t new_size);
    int resize(size_t new_size);

    // Returns the <m_nMaxSize> of the array.
    size_t max_size() const;

    /**
     * Changes the size of the array to match <new_size>.
     * It copies the old contents into the new array.
     * Return -1 on failure.
     * It does not affect new_size
     */
    int max_size(size_t new_size);

    // Swap two array variables
    void swap(FastArray<T> &fa); 

    // = STL styled iterator factory functions.
    iterator begin() { return iterator(0, *this); }
    iterator end() { return iterator(size(), *this); }

private:
    // Returns 1 if <slot> is within range, i.e., 0 >= <slot> <
    // <m_nCurSize>, else returns 0.
    BOOL in_range(size_t slot) const;

    // Maximum size of the array, i.e., the total number of <T> elements
    // in <m_pArray>.
    size_t m_nMaxSize;

    /**
     * Current size of the array.  This starts out being == to
     * <m_nMaxSize>.  However, if we are assigned a smaller array, then
     * <m_nCurSize> will become less than <m_nMaxSize>.  The purpose of
     * keeping track of both sizes is to avoid reallocating memory if we
     * don't have to.
     */
    size_t m_nCurSize;

    // Pointer to the array's storage buffer.
    T *m_pArray;

    friend class FastArray_Iterator<T>;
};

// ****************************************************************

/**
 * @class FastArray_Iterator
 *
 * @brief Implement an iterator over an FastArray.
 *
 * This iterator is safe in the face of array element deletions.
 * But it is NOT safe if the array is resized (via the FastArray
 * assignment operator) during iteration.  That would be very
 * odd, and dangerous.
 */
template <class T>
class FastArray_Iterator
{
public:
    // Define a "trait"
    typedef T TYPE;
    typedef T value_type;
    typedef T& reference;
    typedef T* pointer;
    typedef FastArray_Iterator<T> ITERATOR;
    typedef FastArray_Iterator<T> iterator;

    // = Initialization method.
    FastArray_Iterator(FastArray<T> &);
    FastArray_Iterator(int slot, FastArray<T> &);

    // = Iteration methods.

    // Move to first or last item. 
    // Returns 0 when all items have been seen, else 1.
    void first(); 
    void last(); 

    // Pass back the <next_item> that hasn't been seen in the Array.
    // Returns 0 when all items have been seen, else 1.
    BOOL next(T *&next_item);

    // Pass back the <prev_item> that hasn't been seen in the Array.
    // Returns 0 when all items have been seen, else 1.
    BOOL prev(T *&prev_item);

    // Move forward by one element in the Array.  Returns 0 when all the
    // items in the Array have been seen, else 1.
    BOOL advance();

    // Returns 1 when all items have been seen, else 0.
    BOOL done() const;

    // = STL styled iterator factory functions.
    reference operator*() const { return m_faArray[m_nCurrent]; }
    pointer operator->() const { return &(operator*()); }
    iterator& operator++() { advance(); return *this; } 
    iterator operator++(int) { iterator tmp = *this; operator++(); return tmp; } 
    BOOL operator==(const iterator& it) const ;
    BOOL operator!=(const iterator& it) const 
        { return operator==(it) == FALSE ? TRUE : FALSE; }

private:
    // Pointer to the current item in the iteration.
    int m_nCurrent;

    // Pointer to the Array we're iterating over.
    FastArray<T> &m_faArray;
};


//===============FastArray Implement=========================

// Array free macro define
#define FASTARRAY_FREE(POINTER,SIZE,DEALLOCATOR,CLASS) \
   do { \
        if (POINTER) \
          { \
            for (size_t i = 0; \
                 i < SIZE; \
                 ++i) \
            { \
              (&(POINTER)[i])->~CLASS (); \
            } \
            DEALLOCATOR (POINTER); \
          } \
      } \
   while (0)

#define FASTARRAY_NOFREE(POINTER,SIZE,CLASS) \
   do { \
        if (POINTER) \
          { \
            for (size_t i = 0; \
                 i < SIZE; \
                 ++i) \
            { \
              (&(POINTER)[i])->~CLASS (); \
            } \
          } \
      } \
   while (0)


// Dynamically initialize an array.

template <class T>
FastArray<T>::FastArray(size_t size)
: m_nMaxSize(size),
  m_nCurSize(size)
{
    if( size > 0 ) 
    {
        this->m_pArray = (T *) calloc(size, sizeof(T)); 
        if( this->m_pArray == NULL ) 
        {
            this->m_nMaxSize = this->m_nCurSize = 0; 
            return; 
        }

        for( size_t i = 0; i < size; i++ )
            new (&m_pArray[i]) T (); // Placement new
    }
    else
        this->m_pArray = 0;
}

template <class T>
FastArray<T>::FastArray(size_t size, const T &default_value)
: m_nMaxSize(size),
  m_nCurSize(size)
{
    if( size > 0 ) 
    {
        this->m_pArray = (T *) calloc(size, sizeof(T)); 
        if( this->m_pArray == NULL ) 
        {
            this->m_nMaxSize = this->m_nCurSize = 0; 
            return; 
        }

        for( size_t i = 0; i < size; i++ )
            new (&this->m_pArray[i]) T (default_value); 
    }
    else
        this->m_pArray = 0;
}


// The copy constructor (performs initialization).

template <class T>
FastArray<T>::FastArray(const FastArray<T> &s)
: m_nMaxSize(s.size()),
  m_nCurSize(s.size())
{
    if( s.size() > 0 ) 
    {
        this->m_pArray = (T *) calloc(s.size(), sizeof(T)); 
        if( this->m_pArray == NULL ) 
        {
            this->m_nMaxSize = this->m_nCurSize = 0; 
            return; 
        }

        for( size_t i = 0; i < this->size(); i++ )
            new (&this->m_pArray[i]) T (s.m_pArray[i]); 
    }
    else
        this->m_pArray = 0;
}


// Assignment operator (performs assignment).

template <class T> void
FastArray<T>::operator= (const FastArray<T> &s)
{
    // Check for "self-assignment".
    if( this != &s ) 
    {
        if( this->m_nMaxSize < s.size () ) 
        {
            FASTARRAY_FREE(this->m_pArray,
                           this->m_nMaxSize,
                           free,
                           T);

            this->m_pArray = (T *) calloc(s.size(), sizeof(T)); 
            if( this->m_pArray == NULL ) 
            {
                this->m_nMaxSize = this->m_nCurSize = 0; 
                return; 
            }
            
            this->m_nMaxSize = s.size();
        }
        else
        {
            FASTARRAY_NOFREE(this->m_pArray,
                             s.size(),
                             T);
        }

        this->m_nCurSize = s.size();

        for( size_t i = 0; i < this->size(); i++ ) 
            new (&this->m_pArray[i]) T (s.m_pArray[i]);
    }
}


// Add size of the items in array <s>, if size = -1, add all. 

template <class T> void 
FastArray<T>::add(const FastArray<T> &s, int size) 
{
    // Check for "self-assignment".
    if( this != &s ) 
    {
        size_t pre_size = this->size(); 

        if( size <= 0 || size > (int) s.size() ) 
            size = s.size(); 

        size_t new_size = size + pre_size; 
        this->max_size(new_size); 

        if( this->size() != new_size ) 
            return; 

        for( size_t i = pre_size, j = 0; i < new_size && (int) j < size; i++, j++ ) 
            new (&this->m_pArray[i]) T (s.m_pArray[j]);
    }
}


// Copy size of the items in array <s>, if size = -1, copy all. 

template <class T> void 
FastArray<T>::copyFrom(const FastArray<T> &s, int size) 
{
    // Check for "self-assignment".
    if( this != &s ) 
    {
        size_t new_size = s.size(); 

        if( size >= 0 && size <= (int) s.size() ) 
            new_size = size; 

        FASTARRAY_FREE(this->m_pArray,
                       this->m_nMaxSize,
                       free,
                       T);

        this->m_nMaxSize = new_size;
        this->m_nCurSize = new_size;

        if( new_size > 0 ) 
        {
            this->m_pArray = (T *) calloc(new_size, sizeof(T)); 
            if( this->m_pArray == NULL ) 
            {
                this->m_nMaxSize = this->m_nCurSize = 0; 
                return; 
            }

            for( size_t i = 0; i < this->size(); i++ ) 
                new (&this->m_pArray[i]) T (s.m_pArray[i]);
        }
    }
}


// Set an item in the array at location slot.

template <class T> int
FastArray<T>::set(const T &new_item, size_t slot)
{
    if( this->in_range(slot) )
    {
        this->m_pArray[slot] = new_item;
        return 0;
    }
    else
        return -1;
}


// Get an item in the array at location slot.

template <class T> int
FastArray<T>::get(T &item, size_t slot) const
{
    if( this->in_range(slot) )
    {
        // Copies the item.  If you don't want to copy, use operator []
        // instead (but then you'll be responsible for range checking).
        item = this->m_pArray[slot];
        return 0;
    }
    else
        return -1;
}


template<class T> int
FastArray<T>::max_size(size_t new_size)
{
    if( new_size > this->m_nMaxSize )
    {
        T *tmp = (T *) calloc(new_size, sizeof(T)); 
        if( tmp == NULL ) 
            return -1; 

        for( size_t i = 0; i < this->m_nCurSize; i++ )
            new (&tmp[i]) T (this->m_pArray[i]);

        // Initialize the new portion of the array that exceeds the
        // previously allocated section.
        for( size_t j = this->m_nCurSize; j < new_size; j++ )
            new (&tmp[j]) T;

        FASTARRAY_FREE(this->m_pArray,
                       this->m_nMaxSize,
                       free,
                       T);
        
        this->m_pArray   = tmp;
        this->m_nMaxSize = new_size;
        this->m_nCurSize = new_size;
    }

    return 0;
}


//========FastArray_Iterator functions========

template <class T> BOOL
FastArray_Iterator<T>::next(T *&item)
{
    if( this->done() ) 
    {
        item = 0;
        return FALSE;
    }
    else
    {
        item = &m_faArray[m_nCurrent++];
        return TRUE;
    }
}

template <class T> BOOL
FastArray_Iterator<T>::prev(T *&item)
{
    if( this->done() ) 
    {
        item = 0;
        return FALSE;
    }
    else
    {
        item = &m_faArray[m_nCurrent--];
        return TRUE;
    }
}


//===============FastArray inline functions==================

// Clean up the array (e.g., delete dynamically allocated memory).

template <class T> inline
FastArray<T>::~FastArray()
{
    FASTARRAY_FREE(this->m_pArray,
                   this->m_nMaxSize,
                   free,
                   T);
}

template <class T> inline void
FastArray<T>::release()
{
    FASTARRAY_FREE(this->m_pArray,
                   this->m_nMaxSize,
                   free,
                   T);
    m_pArray = 0; 
    m_nCurSize = 0; 
    m_nMaxSize = 0; 
}

template<class T> inline int
FastArray<T>::size(size_t new_size)
{
    int r = this->max_size(new_size);
    if( r != 0 )
        return r;
    this->m_nCurSize = new_size;
    return 0;
}

template <class T> inline size_t
FastArray<T>::size() const
{
    return this->m_nCurSize;
}

template <class T> inline int
FastArray<T>::resize(size_t new_size) 
{
    return this->max_size(new_size);
}

template <class T> inline size_t
FastArray<T>::max_size() const
{
    return this->m_nMaxSize;
}

template <class T> inline BOOL
FastArray<T>::in_range(size_t index) const
{
    return index < this->m_nCurSize ? TRUE : FALSE;
}

template <class T> inline T &
FastArray<T>::operator[] (size_t index)
{
    return this->m_pArray[index];
}

template <class T> inline const T &
FastArray<T>::operator[] (size_t index) const
{
    return this->m_pArray[index];
}

template <class T> inline void 
FastArray<T>::copyTo(FastArray<T> &s, int size) const
{
    s.copyFrom(*this, size); 
}

// Swap two array variables
template <class T> inline void 
FastArray<T>::swap(FastArray<T> &fa) 
{ 
    _FAST::fast_swap_value<T*>(this->m_pArray, fa.m_pArray); 
    _FAST::fast_swap_value<size_t>(this->m_nCurSize, fa.m_nCurSize); 
    _FAST::fast_swap_value<size_t>(this->m_nMaxSize, fa.m_nMaxSize); 
} 


//==========FastArray_Iterator inline functions======

template <class T> inline void
FastArray_Iterator<T>::first()
{
    this->m_nCurrent = 0;
}

template <class T> inline void
FastArray_Iterator<T>::last()
{
    this->m_nCurrent = m_faArray.size() - 1; 
}

template <class T> inline
FastArray_Iterator<T>::FastArray_Iterator(FastArray<T> &a)
: m_nCurrent(0),
  m_faArray(a)
{
    // TRACE ("FastArray_Iterator<T>::FastArray_Iterator(FastArray<T> &a)");
}

template <class T> inline
FastArray_Iterator<T>::FastArray_Iterator(int slot, FastArray<T> &a)
: m_nCurrent(slot >= 0 && slot <= (int)a.size() ? slot : 0),
  m_faArray(a)
{
    // TRACE ("FastArray_Iterator<T>::FastArray_Iterator(int slot, FastArray<T> &a)");
}

template <class T> inline BOOL
FastArray_Iterator<T>::advance()
{
    // TRACE ("FastArray_Iterator<T>::advance");

    if( this->m_nCurrent < (int) m_faArray.size() ) 
    {
        ++this->m_nCurrent;
        return TRUE;
    }
    else
    {
        // Already finished iterating.
        return FALSE;
    }
}

template <class T> inline BOOL
FastArray_Iterator<T>::done() const
{
    // TRACE ("FastArray_Iterator<T>::done");

    return this->m_nCurrent >= (int) this->m_faArray.size () || this->m_nCurrent < 0 
        ? TRUE : FALSE;
}

template <class T> BOOL
FastArray_Iterator<T>::operator==(const iterator& it) const 
{ 
    if( &m_faArray != &(it.m_faArray) ) 
        return FALSE; 
    else if( done() || it.done() ) 
        return done() && it.done() ? TRUE : FALSE; 
    else 
        return &(m_faArray[m_nCurrent]) == &(it.m_faArray[it.m_nCurrent]) ? TRUE : FALSE; 
}


_FAST_END_NAMESPACE

#endif
