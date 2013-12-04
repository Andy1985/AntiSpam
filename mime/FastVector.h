//=============================================================================
/**
 *  @file    FastVector.h
 *
 *  ver 1.0.0 2004/06/20 Naven Exp, for Fast Common Framework.
 *
 *  @author Naven (Navy Chen) 2004/06/20 created.
 */
//=============================================================================

#ifndef _FAST_COMM_FASTVECTOR_H
#define _FAST_COMM_FASTVECTOR_H

#if defined(_WIN32) || defined(__WIN32__)

#if !defined (NAVEN_PRAGMA_ONCE)
# pragma once
#endif /* NAVEN_PRAGMA_ONCE */

#endif /* _WIN32 */


#include "FastBase.h"
#include "FastArray.h"



_FAST_BEGIN_NAMESPACE

/*
 * Default size for an FastVector.
 */
#define FASTVECTOR_DEFAULT_SIZE     8


// Forward declaration.
template <class T>
class FastVector; 

template <class T> 
class FastVector_Iterator; 

#ifndef Vector 
#define Vector FastVector 
#endif 

#ifndef VectorIterator 
#define VectorIterator FastVector_Iterator 
#endif 


/**
 * @class FastVector
 *
 * @brief Defines an STL-like vector container.
 *
 * This is an STL-like template vector container, a wrapper around
 * FastArray.  It provides at least the basic std::vector look and
 * feel: push_back(), clear(), resize(), capacity().  This template
 * class uses the copy semantic paradigm, though it is okay to use
 * reference counted smart pointers (see FastAutoPtr&lt;T&gt;) with this
 * template class.
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
 *
 * <b> Requirements and Performance Characteristics</b>
 *   - Internal Structure
 *       FastArray
 *   - Duplicates allowed?
 *       Yes
 *   - Random access allowed?
 *       Yes
 *   - Search speed
 *       N/A
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
 *
 * @author Naven
 * @see ACE wrappers
 */
template<class T>
class FastVector
{
public:
    // Traits
    typedef T value_type; 
    typedef T& reference;
    typedef T* pointer;
    typedef FastVector_Iterator<T> Iterator;
    typedef FastVector_Iterator<T> ITERATOR;
    typedef FastVector_Iterator<T> iterator;


    /**
     * General constructor.
     *
     * @param init_size Initial size of the vector with the default
     *                  value of DEFAULT_SIZE
     */
    FastVector(const size_t init_size = FASTVECTOR_DEFAULT_SIZE);

    // Dynamically initialize the entire array to the <default_value>.
    FastVector(const size_t size, const T &default_value);

    /**
     * The copy constructor performs initialization by making an exact
     * copy of the contents of parameter <s>, i.e., *this == s will
     * return true.
     */
    FastVector(const FastVector<T> &s);

    /**
     * Assignment operator performs an assignment by making an exact
     * copy of the contents of parameter <s>, i.e., *this == s will
     * return true.  Note that if the <m_nMaxSize> of <m_pArray> is >= than
     * <s.m_nMaxSize> we can copy it without reallocating.  However, if
     * <m_nMaxSize> is < <s.m_nMaxSize> we must delete the <m_pArray>,
     * reallocate a new <m_pArray>, and then copy the contents of <s>.
     */
    FastVector<T>& operator= (const FastVector<T> &s); 

    /**
     * Destructor.
     */
    virtual ~FastVector();

    // = Set/get methods.

    // Set item in the vector at location <slot>.  Doesn't
    // perform range checking.
    T &operator [] (size_t slot);

    // Get item in the vector at location <slot>.  Doesn't
    // perform range checking.
    const T &operator [] (size_t slot) const;

    // Set an item in the vectpr at location <slot>.  Returns
    // -1 if <slot> is not in range, else returns 0.
    int set(const T &new_item, int slot);

    /**
     * Get an item in the vector at location <slot>.  Returns -1 if
     * <slot> is not in range, else returns 0.  Note that this function
     * copies the item.  If you want to avoid the copy, you can use
     * the const operator [], but then you'll be responsible for range checking.
     */
    int get(T &item, int slot) const;

    /**
     * Sets the component at the specified index of this vector to 
     * be the specified object.
     */
    int setElementAt(const T &item, int index); 

    // Like operator[] but Perform range checking.
    int at(int index, T &item) const; 
    int elementAt(int index, T &item) const; 

    // Returns the first component (the item at index 0) of this vector.
    int firstElement(T &item) const; 

    // Returns the last component of the vector.
    int lastElement(T &item) const; 

    /**
     * Returns the current vector capacity, that is, the currently
     * allocated buffer size.
     *
     * @return Current buffer size of the vector
     */
    size_t capacity() const;

    /**
     * Returns the vector's dynamic size / actual current size of the
     * vector.  Do not confuse it with FastArray::size(), which returns
     * the array's capacity.  Unfortunately, FastXXX is not very consistent
     * with the function names.
     *
     * @return Dynamic size / actual current size of the vector.
     */
    size_t size() const;

    /**
     * Tests if this vector has no components.
     */
    BOOL empty() const; 
    BOOL isEmpty() const; 

    /**
     * Compares the specified Object with this Vector for equality.
     */
    BOOL equals(const FastVector<T> &s) const; 
    BOOL operator==(const FastVector<T> &s) const; 

    /**
     * Clears out the vector.  It does not reallocate the vector's
     * buffer, it is just sets the vector's dynamic size to 0.
     * removeXXX() similar to Java Vector functions.
     */
    void clear();
    void removeAll(); 
    void removeAllElements(); 

    /**
     * Clears out the vector, and deallocate the vector's buffer..
     */
    void release();

    /**
     * Resizes the vector to the new capacity.  If the vector's current
     * capacity is smaller than the size to be specified, then the
     * buffer gets reallocated.  If the new capacity is less than the
     * current capacity of the vector, the buffer size stays the same.
     *
     * @param new_size New capacity of the vector
     * @param t A filler value (of the class T) for initializing the
     *          elements of the vector with.  By default, if this
     *          parameter is not specified, the default value of the
     *          class T will be used (for more detail, see the
     *          initialization clause for this parameter).
     */
    void resize(const size_t new_size, const T& t);

    /**
     * Resizes the vector to the new capacity.  If the vector's current
     * capacity is smaller than the size to be specified, then the
     * buffer gets reallocated.  If the new capacity is less than the
     * current capacity of the vector, the buffer size stays the same.
     *
     * @param new_size New capacity of the vector
     */
    void resize(const size_t new_size);
    void reserve(const size_t new_size);

    // similar to Java Vector functions.
    void setSize(const size_t new_size);

    /**
     * Appends a new element to the vector ("push back").  If the
     * dynamic size of the vector is equal to the capacity of the vector
     * (vector is at capacity), the vector automatically doubles its
     * capacity.
     *
     * @param elem A reference to the new element to be appended.  By
     *             default, this parameters gets initialized with the
     *             default value of the class T.
     */
    void push_back(const T& elem);
    void pushBack(const T& elem);

    /**
     * Push the element to the vector like ("push back"), but this 
     * use T.swap() function to push for not use ASSIGN operator=()
     * that can be used for large object.
     *
     * @param elem A reference to the new element to be appended.  By
     *             default, this parameters gets initialized with the
     *             default value of the class T.
     */
    void swap_push(T& elem);

    /**
     * Deletes the last element from the vector ("pop back").  What this
     * function really does is decrement the dynamic size of the
     * vector.  The vector's buffer does not get reallocated for
     * performance.
     */
    void pop_back();
    void popBack();

    /**
     * Removes the element at the specified position in this Vector.
     * The vector's buffer does not get reallocated for performance.
     * removeXXX() similar to Java Vector functions.
     */
    int erase(const size_t index); 
    int remove(const size_t index); 

    /**
     * Removes from this List all of the elements whose index is between 
     * from_index, inclusive and to_index, exclusive.
     * The vector's buffer does not get reallocated for performance.
     */
    int erase(const size_t from_index, const size_t to_index); 

    // removeXXX() similar to Java Vector functions.
    int removeRange(const size_t from_index, const size_t to_index); 

    /**
     * Inserts an element or a number of elements into the vector at 
     * a specified position.
     */
    int insert(const size_t index, const T &elem, const size_t count = 1); 

    /**
     * Inserts a range of elements into the vector at a specified position.
     * the elements in <code>s</code> array whose index is between 
     * from_index, inclusive and to_index, exclusive. 
     * And from_index = -1 is from first element, to_index = -1 is to end.
     */
    int insert(const size_t index, const FastArray<T> &s, 
               int from_index = -1, int to_index = -1); 

    /**
     * Inserts the specified object as a component in this vector at 
     * the specified index.
     */
    int insertElementAt(const T &elem, const size_t index); 

    /**
     * Inserts the specified element at the specified position in this Vector.
     */
    int add(const size_t index, const T &elem); 

    /**
     * Appends the specified element to the end of this Vector.
     */
    void add(const T& elem); 

    /**
     * Appends all of the elements in the specified Collection to the end of 
     * this Vector, in the order that they are returned by the specified 
     * Collection's Iterator.
     */
    int addAll(const FastArray<T> &s); 

    /**
     * Inserts all of the elements in in the specified Collection into this 
     * Vector at the specified position.
     */
    int addAll(const size_t index, const FastArray<T> &s); 

    /**
     * Adds the specified component to the end of this vector, increasing 
     * its size by one.
     */
    void addElement(const T& elem); 

    /**
     * Searches for the first occurence of the given argument, 
     * testing for equality using the equals method.
     */
    int indexOf(const T &elem) const; 

    /**
     * Searches for the first occurence of the given argument, 
     * beginning the search at index, and testing for equality using 
     * the equals method.
     */
    int indexOf(const T &elem, size_t index) const; 

    /**
     * Returns the index of the last occurrence of the specified 
     * object in this vector.
     */
    int lastIndexOf(const T &elem) const; 

    /**
     * Searches backwards for the specified object, starting from 
     * the specified index, and returns an index to it.
     */
    int lastIndexOf(const T &elem, size_t index) const; 

    /**
     * Copies the components of this vector into the specified array.
     */
    void copyInto(FastArray<T> &s) const; 
    void copyTo(FastArray<T> &s) const; 

    // Swap two variables
    void swap(FastVector<T> &fv); 

    // Dump the state of an object.
    void dump() const;

    // = STL styled iterator factory functions.
    iterator begin() { return iterator(0, *this); }
    iterator end() { return iterator(size(), *this); }

private:
    // Expand vector's capacity to store new item.
    int expand_capacity(const size_t exp_size); 

protected:

    /**
     * Dynamic size (length) of the vector.
     */
    size_t m_nLength;

    /**
     * Current capacity (buffer size) of the vector.
     */
    size_t m_nCurrMaxSize;

    /**
     * Current data array of the vector.
     */
    FastArray<T> m_tArray;
};

/**
 * @class FastVector_Iterator
 *
 * @brief Implement an iterator over an FastVector.
 *
 * This iterator is safe in the face of vector element deletions.
 * But it is NOT safe if the array is resized (via the FastVector
 * assignment operator) during iteration.  That would be very
 * odd, and dangerous.
 */
template <class T>
class FastVector_Iterator
{
public:
    // Define a "trait"
    typedef T TYPE;
    typedef T value_type;
    typedef T& reference;
    typedef T* pointer;
    typedef FastVector_Iterator<T> ITERATOR;
    typedef FastVector_Iterator<T> iterator;

    // = Initialization method.
    FastVector_Iterator(FastVector<T> &);
    FastVector_Iterator(int slot, FastVector<T> &);

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
    reference operator*() const { return m_fvVector[m_nCurrent]; }
    pointer operator->() const { return &(operator*()); }
    iterator& operator++() { advance(); return *this; } 
    iterator operator++(int) { iterator tmp = *this; operator++(); return tmp; } 
    BOOL operator==(const iterator& it) const ;
    BOOL operator!=(const iterator& it) const 
        { return operator==(it) == FALSE ? TRUE : FALSE; }

private:
    // Pointer to the current item in the iteration.
    int m_nCurrent;

    // Pointer to the vector we're iterating over.
    FastVector<T> &m_fvVector;
};


//==============FastVector Functions====================

template <class T> int 
FastVector<T>::expand_capacity(const size_t exp_size)
{
    if( exp_size > m_nCurrMaxSize ) 
    {
        size_t new_size = m_nCurrMaxSize * 2; 

        // for not increase memory so quickly.
        if( new_size > 65536 ) 
            new_size = m_nCurrMaxSize + 8192; 
        else if( new_size < 32 ) 
            new_size = 32; 

        if( new_size < exp_size ) 
            new_size = exp_size; 

        m_tArray.size(new_size);

        // expand array capacity failed, try again use exp_size
        if( m_tArray.max_size() < new_size && new_size > exp_size ) 
            m_tArray.size(exp_size); 

        m_nCurrMaxSize = m_tArray.max_size();

        if( m_nCurrMaxSize < exp_size ) 
            return -1; 
    }
    return 0; 
}

template <class T>
void FastVector<T>::resize(const size_t new_size, const T& t)
{
    this->resize(new_size); 

    if( this->capacity() >= new_size ) 
    {
        for( int i = m_nLength; i < new_size; ++ i ) 
            (*this)[i] = t; 
        m_nLength = new_size; 
    }
}

template <class T> inline
void FastVector<T>::resize(const size_t new_size)
{
    if( this->capacity() < new_size ) 
    {
        if( this->m_tArray.size(new_size) == -1 ) 
            return; 

        m_nCurrMaxSize = m_tArray.max_size();
    }
}

template <class T> int 
FastVector<T>::erase(const size_t index) 
{
    if( index >= 0 && index < this->size() ) 
    {
        int i = index; 
        for( ; i < (int)(this->size() - 1); i ++ ) 
            m_tArray[i] = m_tArray[i+1]; 
        m_tArray[i+1] = T(); 
        m_nLength --; 
        return 0; 
    }
    return -1; 
}

template <class T> int 
FastVector<T>::erase(const size_t from_index, const size_t to_index) 
{
    if( from_index >= 0 && to_index < this->size() && from_index < to_index ) 
    {
        int i = from_index, j = to_index; 
        for( ; i < (int)(this->size() - 1) && j < (int)(this->size() - 1); 
             i ++, j ++ ) 
            m_tArray[i] = m_tArray[j]; 
        for( ; i < (int)(this->size() - 1); i ++ ) 
            m_tArray[i] = T(); 
        m_nLength -= to_index - from_index; 
        return 0; 
    }
    return -1; 
}

template <class T> BOOL 
FastVector<T>::equals(const FastVector<T> &s) const 
{
    if( this == &s ) return TRUE; 
    if( this->size() != s.size() ) return FALSE; 

    for( int i = 0; i < (int)this->size() && i < (int)s.size(); i ++ ) 
        if( !((*this)[i] == s[i]) ) 
            return FALSE; 
    return TRUE; 
}

template <class T> int 
FastVector<T>::indexOf(const T &elem, size_t index) const 
{
    for( int i = index; i < (int)this->size(); i ++ ) 
        if( (*this)[i] == elem ) 
            return i; 
    return -1; 
}

template <class T> int 
FastVector<T>::lastIndexOf(const T &elem, size_t index) const 
{
    for( int i = index; i < (int)this->size() && i >= 0; i -- ) 
        if( (*this)[i] == elem ) 
            return i; 
    return -1; 
}

template <class T> void
FastVector<T>::copyInto(FastArray<T> &s) const
{
    s.size(this->size()); 
    for( int i = 0; i < (int)this->size() && i < (int)s.size(); i ++ ) 
        s[i] = (*this)[i]; 
}

template <class T> int
FastVector<T>::insert(const size_t index, const T &elem, const size_t count) 
{
    if( index <= this->size() && count > 0 ) 
    {
        if( this->expand_capacity(this->size() + count) < 0 ) 
            return -1; 

        for( int i = this->size() - 1; i >= (int)index; i -- ) 
            (*this)[i+count] = (*this)[i]; 
        
        for( int i = index; i < (int)(index + count); i ++ ) 
            (*this)[i] = elem; 

        m_nLength += count; 
        return 0; 
    }
    return -1; 
}

template <class T> int
FastVector<T>::insert(const size_t index, const FastArray<T> &s, int from_index, int to_index)
{
    if( from_index < 0 ) from_index = 0; 
    if( to_index < 0 || to_index > (int)s.size() ) 
        to_index = s.size(); 

    if( from_index >= to_index || index > this->size() ) 
        return -1; 

    size_t count = to_index - from_index; 

    if( this->expand_capacity(this->size() + count) < 0 ) 
        return -1; 

    for( int i = this->size() - 1; i >= (int)index; i -- ) 
        (*this)[i+count] = (*this)[i]; 

    for( int i = index, j = from_index; 
         i < (int)(index + count) && j < (int)to_index; 
         i ++, j ++ ) 
        (*this)[i] = s[j]; 

    m_nLength += count; 
    return 0; 
}


//==============FastVector Inline Functions====================

template <class T> inline
FastVector<T>::FastVector(const size_t init_size)
 : m_tArray(FastArray<T>(init_size == 0 ? FASTVECTOR_DEFAULT_SIZE : init_size))
{
    m_nLength = 0;
    m_nCurrMaxSize = m_tArray.max_size();
}

template <class T> inline
FastVector<T>::FastVector(const size_t init_size, const T &default_value)
 : m_tArray(FastArray<T>((init_size == 0 ? FASTVECTOR_DEFAULT_SIZE : init_size), default_value))
{
    m_nLength = init_size == 0 ? FASTVECTOR_DEFAULT_SIZE : init_size;
    m_nCurrMaxSize = m_tArray.max_size();
}

template <class T> inline
FastVector<T>::FastVector(const FastVector<T> &s)
 : m_tArray(s.m_tArray)
{
    m_nLength = s.m_nLength;
    m_nCurrMaxSize = m_tArray.max_size();
}

template <class T> inline void 
FastVector<T>::push_back(const T& elem)
{
    if( this->expand_capacity(m_nLength + 1) < 0 ) 
        return; 
    m_nLength ++;
    // 调用 operator=() 运算符函数，T必须实现它
    (*this)[m_nLength-1] = elem; 
}

template <class T> inline void 
FastVector<T>::swap_push(T& elem)
{
    if( this->expand_capacity(m_nLength + 1) < 0 ) 
        return; 
    m_nLength ++;
    // 调用 swap() 函数，T必须实现它
    (*this)[m_nLength-1].swap(elem); 
}

template <class T> inline FastVector<T>&
FastVector<T>::operator= (const FastVector<T> &s)
{
    if( this != &s ) 
    {
        this->m_tArray = s.m_tArray; 
        this->m_nCurrMaxSize = s.m_nCurrMaxSize; 
        this->m_nLength = s.m_nLength; 
    }
    return *this; 
}

template <class T> inline
FastVector<T>::~FastVector()
{
}

template <class T> inline
void FastVector<T>::release()
{
    m_tArray.release(); 
    m_tArray.size(FASTVECTOR_DEFAULT_SIZE); 
    m_nLength = 0;
    m_nCurrMaxSize = m_tArray.max_size();
}

template <class T> inline T &
FastVector<T>::operator[] (size_t index)
{
    return this->m_tArray[index];
}

template <class T> inline const T &
FastVector<T>::operator[] (size_t index) const
{
    return this->m_tArray[index];
}

template <class T> inline int
FastVector<T>::at(int index, T &item) const
{
    return this->get(item, index);
}

template <class T> inline int
FastVector<T>::elementAt(int index, T &item) const
{
    return this->get(item, index);
}

template <class T> inline int
FastVector<T>::firstElement(T &item) const
{
    return this->get(item, 0);
}

template <class T> inline int
FastVector<T>::lastElement(T &item) const
{
    return this->get(item, this->size() -1);
}

template <class T> inline int
FastVector<T>::setElementAt(const T &item, int index)
{
    return this->set(item, index); 
}

template <class T> inline int
FastVector<T>::set(const T &new_item, int slot)
{
    if( slot >= 0 && slot < (int)this->size() )
    {
        (*this)[slot] = new_item;
        return 0;
    }
    else
        return -1;
}

template <class T> inline int
FastVector<T>::get(T &item, int slot) const
{
    if( slot >= 0 && slot < this->size() )
    {
        item = (*this)[slot];
        return 0;
    }
    else
        return -1;
}

template <class T> inline size_t 
FastVector<T>::capacity() const
{
    return m_nCurrMaxSize;
}

template <class T> inline size_t 
FastVector<T>::size() const
{
    return m_nLength;
}

template <class T> inline BOOL 
FastVector<T>::empty() const
{
    return m_nLength > 0 ? FALSE : TRUE;
}

template <class T> inline BOOL 
FastVector<T>::isEmpty() const
{
    return this->empty();
}

template <class T> inline BOOL 
FastVector<T>::operator==(const FastVector<T> &s) const 
{
    return this->equals(s); 
}

template <class T> inline void 
FastVector<T>::reserve(const size_t new_size)
{
    this->resize(new_size); 
}

template <class T> inline void 
FastVector<T>::setSize(const size_t new_size)
{
    this->resize(new_size); 
}

template <class T> inline void 
FastVector<T>::clear()
{
    for( size_t i = 0; i < m_nLength; ++ i ) 
        (*this)[i] = T(); 
    m_nLength = 0;
}

template <class T> inline void 
FastVector<T>::removeAll()
{
    this->clear(); 
}

template <class T> inline void 
FastVector<T>::removeAllElements()
{
    this->clear(); 
}

template <class T> inline void 
FastVector<T>::pop_back()
{
    if( m_nLength > 0 )
    {
        -- m_nLength;
        // 这里不能调用析构函数，对象析构只能由m_tArray来做
        (*this)[m_nLength] = T(); 
    }
}

template <class T> inline void 
FastVector<T>::popBack()
{
    this->pop_back(); 
}

template <class T> inline void 
FastVector<T>::pushBack(const T& elem)
{
    this->push_back(elem); 
}

template <class T> inline void 
FastVector<T>::add(const T& elem)
{
    this->push_back(elem); 
}

template <class T> inline int 
FastVector<T>::remove(const size_t index) 
{
    return this->erase(index); 
}

template <class T> inline int 
FastVector<T>::removeRange(const size_t from_index, const size_t to_index) 
{
    return this->erase(from_index, to_index); 
}

template <class T> inline int 
FastVector<T>::insertElementAt(const T &elem, const size_t index) 
{
    return this->insert(index, elem); 
}

template <class T> inline int 
FastVector<T>::add(const size_t index, const T &elem) 
{
    return this->insert(index, elem); 
}

template <class T> inline int 
FastVector<T>::addAll(const FastArray<T> &s) 
{
    return this->insert(this->size(), s); 
}

template <class T> inline int 
FastVector<T>::addAll(const size_t index, const FastArray<T> &s) 
{
    return this->insert(index, s); 
}

template <class T> inline void 
FastVector<T>::addElement(const T& elem) 
{
    this->push_back(elem); 
}

template <class T> inline int 
FastVector<T>::indexOf(const T &elem) const 
{
    return this->indexOf(elem, 0); 
}

template <class T> inline int 
FastVector<T>::lastIndexOf(const T &elem) const 
{
    return this->lastIndexOf(elem, this->size() - 1); 
}

template <class T> inline void
FastVector<T>::copyTo(FastArray<T> &s) const
{
    this->copyInto(s); 
}

// Swap two variables
template <class T> inline void 
FastVector<T>::swap(FastVector<T> &fv) 
{
    m_tArray.swap(fv.m_tArray); 
    _FAST::fast_swap_value<size_t>(m_nLength, fv.m_nLength); 
    _FAST::fast_swap_value<size_t>(m_nCurrMaxSize, fv.m_nCurrMaxSize); 
}


//========FastVector_Iterator functions========

template <class T> BOOL
FastVector_Iterator<T>::next(T *&item)
{
    if( this->done() ) 
    {
        item = 0;
        return FALSE;
    }
    else
    {
        item = &m_fvVector[m_nCurrent++];
        return TRUE;
    }
}

template <class T> BOOL
FastVector_Iterator<T>::prev(T *&item)
{
    if( this->done() ) 
    {
        item = 0;
        return FALSE;
    }
    else
    {
        item = &m_fvVector[m_nCurrent--];
        return TRUE;
    }
}

template <class T> inline void
FastVector_Iterator<T>::first()
{
    this->m_nCurrent = 0;
}

template <class T> inline void
FastVector_Iterator<T>::last()
{
    this->m_nCurrent = m_fvVector.size() - 1; 
}

template <class T> inline
FastVector_Iterator<T>::FastVector_Iterator(FastVector<T> &a)
: m_nCurrent(0),
  m_fvVector(a)
{
    // TRACE ("FastVector_Iterator<T>::FastVector_Iterator");
}

template <class T> inline
FastVector_Iterator<T>::FastVector_Iterator(int slot, FastVector<T> &a)
: m_nCurrent(slot >= 0 && slot <= (int)a.size() ? slot : 0),
  m_fvVector(a)
{
    // TRACE ("FastVector_Iterator<T>::FastVector_Iterator(int slot, FastVector<T> &a)");
}

template <class T> inline BOOL
FastVector_Iterator<T>::advance()
{
    if( this->m_nCurrent < (int) m_fvVector.size() ) 
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
FastVector_Iterator<T>::done() const
{
    return this->m_nCurrent >= (int) this->m_fvVector.size () || this->m_nCurrent < 0 
            ? TRUE : FALSE;
}

template <class T> BOOL
FastVector_Iterator<T>::operator==(const iterator& it) const 
{ 
    if( &m_fvVector != &(it.m_fvVector) ) 
        return FALSE; 
    else if( done() || it.done() ) 
        return done() && it.done() ? TRUE : FALSE; 
    else 
        return &(m_fvVector[m_nCurrent]) == &(it.m_fvVector[it.m_nCurrent]) ? TRUE : FALSE; 
}

template <class T> void 
FastVector<T>::dump() const
{
    FAST_TRACE_BEGIN("FastVector<T>::dump()"); 
    FAST_TRACE("m_tArray.size() = %d", m_tArray.size()); 
    FAST_TRACE("m_nCurrMaxSize = %d", m_nCurrMaxSize); 
    FAST_TRACE("m_nLength = %d", m_nLength); 
    FAST_TRACE_END("FastVector<T>::dump()"); 
}


_FAST_END_NAMESPACE

#endif
