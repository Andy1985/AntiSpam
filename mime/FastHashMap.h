//=============================================================================
/**
 *  @file    FastHashMap.h
 *
 *  ver 1.0.0 2004/06/20 Naven Exp, for Fast Common Framework.
 *
 *  @author Naven (Navy Chen) 2004/06/20 created.
 */
//=============================================================================

#ifndef _FAST_COMM_FASTHASHMAP_H
#define _FAST_COMM_FASTHASHMAP_H

#if defined(_WIN32) || defined(__WIN32__)

#if !defined (NAVEN_PRAGMA_ONCE)
# pragma once
#endif /* NAVEN_PRAGMA_ONCE */

#endif /* _WIN32 */


#include "FastBase.h"
#include "FastVector.h"
#include "FastString.h"



_FAST_BEGIN_NAMESPACE

/**
 * ͨ��ѧϰSGI STL��Դ�롢ACE�����Դ���jjhou�ġ�STLԴ��������
 * ��HashMap������STL�а��룬���ۺϸ�������ŵ㣬��һ�����Ż���
 * �򻯴���ʹ�����ʹ�ú����Ч�ʡ�
 *
 * <p>
 * Ϊ�˰�ȫ��ͳһ���������õ�������������������������
 *
 * Class T must implemented default constructor and destructor, 
 * and assign operator function (operator=), and copy constructor. 
 *
 * ��T����ʵ��ȷʡ���캯��������������operator=()��ֵ���������
 * ��copy�������캯������������ʽ��<p><pre>
 * class T {
 * public:
 *      T(); 
 *      T(const T& t); 
 *      ~T(); 
 *      T& operator=(const T& t); 
 * };
 * </pre>
 *
 * @author Naven
 * @see SGI STL & ACE wrappers & JJHOU TAOSS-02
 */

// Forward declaration of functions for hashing

/**
 * 28���������������
 * Note: assumes long is at least 32 bits.
 */
enum { FASTMAP_NUM_PRIMES = 28 };

static const unsigned long FASTMAP_PRIME_LIST[FASTMAP_NUM_PRIMES] =
{
    53ul,         97ul,         193ul,       389ul,       769ul,
    1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
    49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
    1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
    50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul, 
    1610612741ul, 3221225473ul, 4294967291ul
};

/**
 * �ҳ�����28������֮�У���ӽ�������n���Ǹ�������
 * 
 * @param n  input number
 * @return a smallest prime number bigger than n
 */
inline unsigned long FastMap_Next_Prime(unsigned long n)
{
    const unsigned long* first = FASTMAP_PRIME_LIST;
    const unsigned long* last  = FASTMAP_PRIME_LIST + (int)FASTMAP_NUM_PRIMES;
    const unsigned long* pos   = _FAST::fast_lower_bound<unsigned long>(first, last, n);
    return pos == last ? *(last - 1) : *pos;
}

// Hash functions should probably be revisited later:
template<class CHART> struct Fast_Hash { };

/**
 * Hash functions 
 * ���char *, int, long�������ػ��ĺ������󣨷º�����
 */
template<> struct Fast_Hash<char*> {
    size_t operator()(const char* s) const { return fast_hash_string(s); }
};
template<> struct Fast_Hash<const char*> {
    size_t operator()(const char* s) const { return fast_hash_string(s); }
};
template<> struct Fast_Hash<char[]> {
    size_t operator()(const char s[]) const { return fast_hash_string((const char *)s); }
};
template<> struct Fast_Hash<const char[]> {
    size_t operator()(const char s[]) const { return fast_hash_string((const char *)s); }
};
template<> struct Fast_Hash<char> {
    size_t operator()(char x) const { return x; }
};
template<> struct Fast_Hash<unsigned char> {
    size_t operator()(unsigned char x) const { return x; }
};
template<> struct Fast_Hash<signed char> {
    size_t operator()(unsigned char x) const { return x; }
};
template<> struct Fast_Hash<short> {
    size_t operator()(short x) const { return x; }
};
template<> struct Fast_Hash<unsigned short> {
    size_t operator()(unsigned short x) const { return x; }
};
template<> struct Fast_Hash<int> {
    size_t operator()(int x) const { return x; }
};
template<> struct Fast_Hash<unsigned int> {
    size_t operator()(unsigned int x) const { return x; }
};
template<> struct Fast_Hash<long> {
    size_t operator()(long x) const { return x; }
};
template<> struct Fast_Hash<unsigned long> {
    size_t operator()(unsigned long x) const { return x; }
};
// �Զ���� hash functions �ػ��汾
//template<> struct Fast_Hash<std::string> {
//    size_t operator()(const std::string &s) const { return fast_hash_string(s.c_str()); }
//};
//template<> struct Fast_Hash<const std::string> {
//    size_t operator()(const std::string &s) const { return fast_hash_string(s.c_str()); }
//};
template<typename CHAR, size_t BUFSIZE> 
struct Fast_Hash<_FAST::FastString_Base<CHAR,BUFSIZE> > {
    size_t operator()(const _FAST::FastString_Base<CHAR,BUFSIZE> &s) const 
        { return fast_hash_string(s.c_str()); }
};
template<typename CHAR, size_t BUFSIZE> 
struct Fast_Hash<const _FAST::FastString_Base<CHAR,BUFSIZE> > {
    size_t operator()(const _FAST::FastString_Base<CHAR,BUFSIZE> &s) const 
        { return fast_hash_string(s.c_str()); }
};


// put functions for hashmap.put() �����汾
template<class T> struct Fast_Hash_Put_Value { 
    void operator()( T &s1, T &s2 ) { s1 = s2; }
};

// put functions for hashmap.put() �ػ��汾
//template<> struct Fast_Hash_Put_Value<std::string> {
//    void operator()( std::string &s1, std::string &s2 ) { s1.clear(); s1.swap(s2); }
//};
template<typename CHAR, size_t BUFSIZE> 
struct Fast_Hash_Put_Value<_FAST::FastString_Base<CHAR,BUFSIZE> > {
    void operator()( _FAST::FastString_Base<CHAR,BUFSIZE> &s1, _FAST::FastString_Base<CHAR,BUFSIZE> &s2 ) 
        { s1.release(); s1.swap(s2); }
};


// Forward declaration of equality operator; needed for friend declaration.

template<class KEY, class TP,
         class HASHFUNC = Fast_Hash<KEY>,
         class EQUALKEY = Fast_Equal_To<KEY> >
class Fast_HashMap_Iterator; 

template<class KEY, class TP,
         class HASHFUNC = Fast_Hash<KEY>,
         class EQUALKEY = Fast_Equal_To<KEY> >
class Fast_HashMap;

#ifndef FastHashMap
#define FastHashMap Fast_HashMap
#endif 

#ifndef FastHashMapIterator 
#define FastHashMapIterator Fast_HashMap_Iterator 
#endif 

#ifndef HashMap 
#define HashMap Fast_HashMap 
#endif 

#ifndef HashMapIterator 
#define HashMapIterator Fast_HashMap_Iterator 
#endif 

template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY>
class Fast_Hashtable_Iterator; 

template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY>
class Fast_Hashtable; 


/**
 * @class Fast_Hashtable_Node
 *
 * @brief Fast_Hashtable node object for hashing
 */
template<class VALUE>
struct Fast_Hashtable_Node
{
    Fast_Hashtable_Node* _next;
    VALUE _val;
};  

/**
 * @class Fast_Hashtable_Iterator
 *
 * @brief Fast_Hashtable iterator object for hashing
 *
 * hashtable ������������Զάϵ�������� buckets vector �Ĺ�ϵ������¼Ŀǰ��ָ�Ľڵ�
 * ��ǰ�����������ȳ��Դ�Ŀǰ��ָ�Ľڵ������ǰ��һ��λ��/�ڵ㣬���ڽڵ㱻������list
 * �ڣ��������ýڵ��nextָ�뼴���������ǰ�����������Ŀǰ�ڵ�������list��β�ˣ���
 * ������һ��bucket���ϣ�������ָ����һ��list��ͷ���ڵ㡣
 *
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY>
class Fast_Hashtable_Iterator 
{
public: 
    // Traits.
    typedef Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>
            hashtable;
    typedef Fast_Hashtable_Iterator<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>
            iterator;
    typedef Fast_Hashtable_Node<VALUE> hash_node;

    //typedef forward_iterator_tag iterator_category;
    typedef VALUE value_type;
    //typedef ptrdiff_t difference_type;
    typedef size_t size_type;
    typedef VALUE& reference;
    typedef VALUE* pointer;

    //friend class hashtable; 
    friend class Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>;

protected: 
    hash_node* _cur;    // ������Ŀǰ��ָ�ڵ�
    hashtable* _ht;     // ���ֶ����������ӹ�ϵ����Ϊ�п�����Ҫ��bucket����bucket

public: 
    // Initialize a Fast_Hashtable_Iterator with 
    // @n hashtable node @tab hashtable.
    Fast_Hashtable_Iterator(hash_node* n, hashtable* tab) : _cur(n), _ht(tab) {}

    // = Initialization method.
    Fast_Hashtable_Iterator(hashtable &ht) : _cur(0), _ht(&ht) { this->first(); }

    // Initialize a Fast_Hashtable_Iterator with default
    Fast_Hashtable_Iterator() : _cur(0), _ht(0) {}

    // = Assign constructor.
    Fast_Hashtable_Iterator(iterator &it) : _cur(it._cur), _ht(it._ht) {}
    Fast_Hashtable_Iterator(const iterator &it) : _cur(it._cur), _ht(it._ht) {}

    // = Iteration methods.

    // Move to first item. 
    // Returns 0 when all items have been seen, else 1.
    void first(); 

    // Pass back the <next_item> that hasn't been seen in the map.
    // Returns 0 when all items have been seen, else 1.
    BOOL next(value_type *&next_item);

    // Move forward by one element in the map.  Returns 0 when all the
    // items in the map have been seen, else 1.
    BOOL advance();

    // Returns 1 when all items have been seen, else 0.
    BOOL done() const;

    // = STL styled iterator factory functions.
    reference operator*() const { return _cur->_val; }
    pointer operator->() const { return &(operator*()); }
    iterator& operator++();
    iterator operator++(int);
    BOOL operator==(const iterator& it) const { return _cur == it._cur ? TRUE : FALSE; }
    BOOL operator!=(const iterator& it) const { return _cur != it._cur ? TRUE : FALSE; }

private:
    // DIABLED functions unimplements.

    // Move to last item. 
    void last(); 

    // Pass back the <prev_item> that hasn't been seen in the map.
    // Returns 0 when all items have been seen, else 1.
    BOOL prev(value_type *&prev_item);

    // Move to previous item.
    iterator& operator--();
    iterator operator--(int);
};

/**
 * @class Fast_Hashtable
 *
 * @brief Fast_Hashtable object for hashing
 *
 * hashtable��ʵ��hashmap�Ļ����࣬ʹ�ÿ�����separate chaining����ʵ��hash������
 * ������������ÿһ�����Ԫ����ά��һ��list��hash functionΪ���Ƿ���ĳһ��list��
 * Ȼ���������Ǹ�list����ִ��Ԫ�صĲ��롢��Ѱ��ɾ���Ȳ�������Ȼ���list�����е�
 * ��Ѱֻ����һ�����Բ����������list���̣��ٶȻ��ǹ��졣
 *
 * ģ������������£�
 * VALUE        �ڵ��ʵֵ����
 * KEY          �ڵ�ļ�ֵ����
 * HASHFUNC     hash function�ĺ������ͣ���������/�º�����
 * EXTRACTKEY   �ӽڵ���ȡ����ֵ�ķ�������������/�º�����
 * EQUALKEY     �жϼ�ֵ��ͬ���ķ�������������/�º�����
 *
 * @see TASS02
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY>
class Fast_Hashtable 
{
public:
    // Traits 
    typedef KEY                 key_type;
    typedef VALUE               value_type;
    typedef HASHFUNC            hasher;
    typedef EQUALKEY            key_equal;

    typedef size_t              size_type;
    //typedef ptrdiff_t         difference_type;
    typedef value_type*         pointer;
    typedef const value_type*   const_pointer;
    typedef value_type&         reference;
    typedef const value_type&   const_reference;

    hasher hash_funct() const { return _hash; }
    key_equal key_eq() const { return _equals; }

private:
    // �ڵ�����
    typedef Fast_Hashtable_Node<VALUE> hash_node;

public:
    // ����ʹ�õ�ȱʡ���ڴ������
    typedef FAST_ALLOCATOR_TYPE(hash_node) allocator_type;
    typedef FAST_DEFAULT_ALLOCATOR(hash_node) default_allocator;
    allocator_type* get_allocator() const { return _allocator; }
private:
    // ������ͷŽڵ��ڴ�ĺ���
    typedef FAST_ALLOCATOR_TYPE(hash_node) _node_allocator_type;
    hash_node* _get_node() { return (hash_node*)_allocator->allocate(); }
    void _put_node(hash_node* p) { _allocator->deallocate(p); }

private:
    allocator_type*        _allocator;          // �ڴ������
    BOOL                   _release_allocator;  // ����ʱ�Ƿ���Ҫ�ͷ��ڴ������
    hasher                 _hash;               // HASH�㷨������
    key_equal              _equals;             // �Ƚ�KEY������
    EXTRACTKEY             _get_key;            // ��ȡKEY������
    FastVector<hash_node*> _buckets;            // �洢 buckets ������
    size_type              _num_elements;       // ��ǰԪ�ؽڵ����Ŀ

public:
    // ��������Ŀǰֻ֧����һ�֣��㹻��
    typedef Fast_Hashtable_Iterator<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>
            iterator;
    friend class Fast_Hashtable_Iterator<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>;

public:
    // ���캯�� Constructor
    Fast_Hashtable(size_type    n,              // ��ʼ�� buckets ����Ŀ
            const HASHFUNC&     hf,             // HASH�㷨������
            const EQUALKEY&     eql,            // �Ƚ�KEY������
            const EXTRACTKEY&   ext,            // ��ȡKEY������
            allocator_type*     a = 0);         // �ڴ������ =0 �Լ�����

    // ���캯�� Constructor
    Fast_Hashtable(size_type    n,              // ��ʼ�� buckets ����Ŀ
            const HASHFUNC&     hf,             // HASH�㷨������
            const EQUALKEY&     eql,            // �Ƚ�KEY������
            allocator_type*     a = 0);         // �ڴ������ =0 �Լ�����

    // ASSIGN���캯�� Constructor
    Fast_Hashtable(const Fast_Hashtable& ht,    // ��һ��HASH����
            allocator_type*     a = 0);         // �ڴ������ =0 �Լ�����

    // ASSIGN���������
    Fast_Hashtable& operator= (const Fast_Hashtable& ht); 

    // �������� Desctructor
    ~Fast_Hashtable(); 

    // ��ȡԪ�ظ���
    size_type size() const { return _num_elements; }

    // ���Ԫ�ظ�����ʵ���� -1 ��ʾ�����
    size_type max_size() const { return size_type(-1); }

    // �ж������Ƿ�Ϊ��
    BOOL empty() const { return size() == 0 ? TRUE : FALSE; }

    // ���������������������Ԫ��
    void swap(Fast_Hashtable& ht); 

    // ��ȡ��ʼ������
    iterator begin(); 

    // ��ȡ�������������Կյĵ�������ʾ��β
    iterator end() { return iterator(0, this); }

    //friend int operator==(const hashtable&, const hashtable&);

public:

    // ����� buckets ����Ŀ
    size_type bucket_count() const { return _buckets.size(); }

    // ������������ buckets ����Ŀ
    size_type max_bucket_count() const
        { return FASTMAP_PRIME_LIST[(int)FASTMAP_NUM_PRIMES - 1]; } 

    // ��ȡĳ�� bucket ��Ԫ�صĸ�������
    size_type elems_in_bucket(size_type bucket) const; 

    // ������Ԫ�أ��������ظ�
    Fast_Pair<iterator, BOOL> insert_unique(const value_type& obj); 

    // ������Ԫ�أ������ظ�
    iterator insert_equal(const value_type& obj); 

    // ������Ԫ�أ��������ظ������ؽ����
    Fast_Pair<iterator, BOOL> insert_unique_noresize(const value_type& obj);

    // ������Ԫ�أ������ظ������ؽ����
    iterator insert_equal_noresize(const value_type& obj);
 
    // ����һ����Ԫ�أ��������ظ�
    void insert_unique(const value_type* f, const value_type* l); 

    // ����һ����Ԫ�أ������ظ�
    void insert_equal(const value_type* f, const value_type* l); 

    // ����Ԫ�أ���������ھͲ���
    reference find_or_insert(const value_type& obj);

    // ����Ԫ��
    iterator find(const key_type& key); 

    // ͳ����ͬ��ֵ��Ԫ�ظ���
    size_type count(const key_type& key) const; 

    // �������Ԫ�ص���ʼλ��
    Fast_Pair<iterator, iterator> equal_range(const key_type& key);

    // ɾ��ָ����ֵԪ��
    size_type erase(const key_type& key);

    // ɾ��������ָ���Ԫ��
    void erase(const iterator& it);

    // ɾ����ʼ������ָ���һ��Ԫ��
    void erase(iterator first, iterator last);

    // �ؽ��������ʱʹ��
    void resize(size_type num_elements_hint);

    // ����������Ԫ�أ������ֱ���С
    void clear();

    // Dump the state of an object.
    void dump() const;

private:
    // ��ȡ��һ����ӽ������������ݲ��ؽ����
    size_type _next_size(size_type n) const
        { return FastMap_Next_Prime(n); }

    // ��ʼ�� buckets vector
    void _initialize_buckets(size_type n); 

    // ���¼������������ж�Ԫ�ص���Ŵ�
    // ������hash functions��ȡ��һ������ִ��modulus��ȡģ���������ֵ
    // ��ЩԪ�������޷�ֱ��������hashtable�Ĵ�С����ȡģ���㣬�����ַ���const char *
    // ��ʱ����Ҫ��һЩת����ʵ��������Fast_Hash��������ʵ�֣����ģ����
    // �Լ��ֳ��������������ػ�����ʵ�ֻ�ȡ��hashֵ��

    // �ж�Ԫ�ص���Ŵ� �汾1 ֻ���ܽ�ֵ
    size_type _bkt_num_key(const key_type& key) const
        { return _bkt_num_key(key, _buckets.size()); }

    // �ж�Ԫ�ص���Ŵ� �汾2 ֻ����ʵֵ value
    size_type _bkt_num(const value_type& obj) const
        { return _bkt_num_key(_get_key(obj)); }

    // �ж�Ԫ�ص���Ŵ� �汾3 ���ܽ�ֵ �� buckets ����
    size_type _bkt_num_key(const key_type& key, size_t n) const
        { return _hash(key) % n; }

    // �ж�Ԫ�ص���Ŵ� �汾4 ����ʵֵ value �� buckets ����
    size_type _bkt_num(const value_type& obj, size_t n) const
        { return _bkt_num_key(_get_key(obj), n); }

    // �ڵ������� ����һ���ڵ�
    hash_node* _new_node(const value_type& obj); 

    // �ڵ������� �ͷ�һ���ڵ�
    void _delete_node(hash_node* n); 

    // ɾ�� bucket �� first �� last ָ��Χ֮���Ԫ��
    void _erase_bucket(const size_type n, hash_node* first, hash_node* last);

    // ɾ�� bucket ����ʼ�� last ָ��Χ֮���Ԫ��
    void _erase_bucket(const size_type n, hash_node* last);

    // ����һ��������������Ԫ�ع���
    void _copy_from(const Fast_Hashtable& ht);

};

/**
 * @class Fast_HashMap_Iterator
 *
 * @brief Fast_HashMap iterator object for hashing.
 *
 */
template<class KEY, class TP, class HASHFUNC, class EQUALKEY>
class Fast_HashMap_Iterator : 
    public Fast_Hashtable_Iterator<Fast_Pair<const KEY,TP>,KEY,HASHFUNC,
                    Fast_Select1st<Fast_Pair<const KEY,TP> >,EQUALKEY>
{
public: 
    // Traits.
    typedef Fast_HashMap<KEY,TP,HASHFUNC,EQUALKEY> hashmap;
    typedef Fast_HashMap_Iterator<KEY,TP,HASHFUNC,EQUALKEY> iterator; 
    typedef Fast_Hashtable_Iterator<Fast_Pair<const KEY,TP>,KEY,HASHFUNC,
                        Fast_Select1st<Fast_Pair<const KEY,TP> >,EQUALKEY>
            hashtable_iterator; 

    //friend class hashmap; 
    friend class Fast_HashMap<KEY,TP,HASHFUNC,EQUALKEY>;

public: 
    // Initialize a Fast_Hashtable_Iterator with hashmap
    Fast_HashMap_Iterator(hashmap &hm) : hashtable_iterator(hm._ht) { this->first(); }

    // = Assign constructor.
    Fast_HashMap_Iterator(iterator &it) : hashtable_iterator(it._cur, it._ht) {}
    Fast_HashMap_Iterator(const iterator &it) : hashtable_iterator(it._cur, it._ht) {}

    // Initialize with hashtable_iterator
    Fast_HashMap_Iterator(hashtable_iterator &it) : hashtable_iterator(it) {}
    Fast_HashMap_Iterator(const hashtable_iterator &it) : hashtable_iterator(it) {}
};

/**
 * @class Fast_HashMap
 *
 * @brief Defines a map interface, Implementation to be provided by subclasses.
 *
 * This implementation provides constant-time performance for the basic operations 
 * (get and put), assuming the hash function disperses the elements properly among 
 * the buckets. Iteration over collection views requires time proportional to the 
 * "capacity" of the HashMap instance (the number of buckets) plus its size (the 
 * number of key-value mappings). Thus, it's very important not to set the initial 
 * capacity too high (or the load factor too low) if iteration performance is 
 * important. 
 * 
 * An instance of HashMap has two parameters that affect its performance: initial 
 * capacity and allocator factor. The capacity is the number of buckets in the hash 
 * table, and the initial capacity is simply the capacity at the time the hash table 
 * is created. The load factor is a measure of how full the hash table is allowed 
 * to get before its capacity is automatically increased. When the number of entries 
 * in the hash table exceeds the product of the load factor and the current capacity, 
 * the capacity is roughly doubled by calling the rehash method.
 *
 * @see Java HashMap
 */
template<class KEY, class TP, class HASHFUNC, class EQUALKEY>
class Fast_HashMap
{
private:
    typedef Fast_Hashtable<Fast_Pair<const KEY,TP>,KEY,HASHFUNC,
                Fast_Select1st<Fast_Pair<const KEY,TP> >,EQUALKEY> 
            Hashtable;
    Hashtable _ht;  // �ײ������ hash table ���

    typedef Fast_Hash_Put_Value<TP> put_valuer; 
    put_valuer _put_value; 

    friend class Fast_HashMap_Iterator<KEY,TP,HASHFUNC,EQUALKEY>;

public:
    // Traits
    typedef typename Hashtable::key_type key_type;
    typedef TP data_type;
    typedef TP mapped_type;
    typedef typename Hashtable::value_type value_type;
    typedef typename Hashtable::hasher hasher;
    typedef typename Hashtable::key_equal key_equal;
  
    typedef typename Hashtable::size_type size_type;
    //typedef typename Hashtable::difference_type difference_type;
    typedef typename Hashtable::pointer pointer;
    typedef typename Hashtable::const_pointer const_pointer;
    typedef typename Hashtable::reference reference;
    typedef typename Hashtable::const_reference const_reference;

    typedef typename Hashtable::allocator_type allocator_type;

    typedef typename Hashtable::iterator hashtable_iterator;
    typedef Fast_HashMap_Iterator<KEY,TP,HASHFUNC,EQUALKEY> iterator;

    hasher hash_funct() const { return _ht.hash_funct(); }
    key_equal key_eq() const { return _ht.key_eq(); }
    allocator_type* get_allocator() const { return _ht.get_allocator(); }

public:
    // ȱʡʹ�ô�СΪ100�ı��,����hash table����Ϊ��ӽ��ҽϴ�֮����
    Fast_HashMap() : _ht(100, hasher(), key_equal()) {}
    explicit Fast_HashMap(size_type n) : _ht(n, hasher(), key_equal()) {}
    Fast_HashMap(size_type n, const hasher& hf) : _ht(n, hf, key_equal()) {}
    Fast_HashMap(size_type n, const hasher& hf, const key_equal& eql)
        : _ht(n, hf, eql) {}

    // ����,�������ȫ��ʹ�� insert_unique(),��������ظ�
    Fast_HashMap(const value_type* f, const value_type* l)
        : _ht(100, hasher(), key_equal())
        { _ht.insert_unique(f, l); }
    Fast_HashMap(const value_type* f, const value_type* l, size_type n)
        : _ht(n, hasher(), key_equal())
        { _ht.insert_unique(f, l); }
    Fast_HashMap(const value_type* f, const value_type* l, size_type n, const hasher& hf)
        : _ht(n, hf, key_equal())
        { _ht.insert_unique(f, l); }
    Fast_HashMap(const value_type* f, const value_type* l, size_type n,
           const hasher& hf, const key_equal& eql, allocator_type*     a = 0)
        : _ht(n, hf, eql, a)
        { _ht.insert_unique(f, l); }

public:
    // ���в�����������hash table��Ӧ�汾,���ݵ��þ���
    size_type size() const { return _ht.size(); }
    size_type max_size() const { return _ht.max_size(); }
    BOOL empty() const { return _ht.empty(); }
    BOOL isEmpty() const { return _ht.empty(); }
    void swap(Fast_HashMap& hs) { _ht.swap(hs._ht); _FAST::fast_swap_value(_put_value, hs._put_value); }

    iterator begin() { return iterator(_ht.begin()); }
    iterator end() { return iterator(_ht.end()); }

public:
    Fast_Pair<iterator,BOOL> insert(const value_type& obj)
    { 
        Fast_Pair<hashtable_iterator, BOOL> ret = _ht.insert_unique(obj); 
        return Fast_Pair<iterator,BOOL>(ret.first(), ret.second()); 
    }

    void insert(const value_type* f, const value_type* l) 
        { _ht.insert_unique(f, l); } 

    Fast_Pair<iterator,BOOL> insert_noresize(const value_type& obj)
    { 
        Fast_Pair<hashtable_iterator, BOOL> ret = _ht.insert_unique_noresize(obj); 
        return Fast_Pair<iterator,BOOL>(ret.first(), ret.second()); 
    } 

    iterator find(const key_type& key) { return iterator(_ht.find(key)); }

    // Returns true if this map contains a mapping for the specified key.
    BOOL containsKey(const key_type& key) 
        { return ( _ht.find(key) != this->end() ) ? TRUE : FALSE; }

    // Returns true if this map maps one or more keys to the specified value.
    BOOL containsValue(const TP& value); 

    // Get/Set <value> with specified <key>
    TP& operator[] (const key_type& key) 
        { return _ht.find_or_insert(value_type(key, TP())).second(); } 

    // Get <value> with specified <key>, found return 0 else return -1
    int get(const key_type& key, TP& value); 

    // Set <value> with specified <key>, Success return 0 else return -1
    int set(const key_type& key, TP& value) 
        { this->operator[](key) = value; return 0; } 

    // Returns the value to which the specified key is mapped in this 
    // identity hash map, or null value if the map contains no mapping 
    // for this key.
    TP& get(const key_type& key) { return this->operator[](key); }; 

    // Like set()
    void put(const key_type& key, TP& value) 
        { set(key, value); }

    // Like Put(), but value data while be swapped into map, not assign
    // to, always use to put large data.
    // Associates the specified value with the specified key in this map.
    void swap_put(const key_type& key, TP& value) 
        {  _put_value(_ht.find_or_insert(value_type(key, TP())).second(), value); } 

    size_type count(const key_type& key) const { return _ht.count(key); }
  
    Fast_Pair<iterator, iterator> equal_range(const key_type& key)
    { 
        Fast_Pair<hashtable_iterator, hashtable_iterator> ret = _ht.equal_range(key); 
        return Fast_Pair<iterator, iterator>(ret.first(), ret.second()); 
    }

    // Removes the mapping for this key from this map if present.
    size_type erase(const key_type& key) { return _ht.erase(key); }
    size_type remove(const key_type& key) { return _ht.erase(key); }

    void erase(iterator it) { _ht.erase(it); }
    void erase(iterator f, iterator l) { _ht.erase(f, l); }
    void clear() { _ht.clear(); }

    void resize(size_type hint) { _ht.resize(hint); }
    size_type bucket_count() const { return _ht.bucket_count(); }
    size_type max_bucket_count() const { return _ht.max_bucket_count(); }
    size_type elems_in_bucket(size_type n) const
        { return _ht.elems_in_bucket(n); }

    void dump() const { _ht.dump(); }
};


//=========Fast_Hashtable_Iterator Functions Implements=============

/**
 * ������ǰ������ operator++()
 * ��ǰ�����������ȳ��Դ�Ŀǰ��ָ�Ľڵ������ǰ��һ��λ��/�ڵ㣬���ڽڵ㱻������list
 * �ڣ��������ýڵ��nextָ�뼴���������ǰ�����������Ŀǰ�ڵ�������list��β�ˣ���
 * ������һ��bucket���ϣ�������ָ����һ��list��ͷ���ڵ㡣
 *
 * @return      ָ����һ��Ԫ�صĵ�����
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY>
Fast_Hashtable_Iterator<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>&
Fast_Hashtable_Iterator<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::operator++()
{
    const hash_node* old = _cur; 
    // ������ڣ�������������������� if ����
    _cur = _cur->_next;
    if( !_cur ) 
    {
        // ����Ԫ��ֵ����λ����һ�� bucket������ͷ������Ŀ�ĵ�
        size_type bucket = _ht->_bkt_num(old->_val);
        while( !_cur && ++bucket < _ht->_buckets.size() ) 
            _cur = _ht->_buckets[bucket];
    }
    return *this;
}

/**
 * ������ǰ������ operator++(int)
 * ʵ���ǵ��������ǰ����������operator++()
 *
 * @return      ָ����һ��Ԫ�صĵ�����
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> inline 
Fast_Hashtable_Iterator<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>
Fast_Hashtable_Iterator<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::operator++(int)
{
    iterator tmp = *this;
    ++ *this; // call operator++()
    return tmp;
}

/**
 * ��������ʼ���� first()
 * ��������ָ�������е�һ��Ԫ��
 *
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> inline void 
Fast_Hashtable_Iterator<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::first()
{
    if( _ht != 0 ) 
    {
        iterator it = _ht->begin(); 
        _cur = it._cur; 
    }
}

/**
 * ������ȡֵ���� next()
 * ���ж��Ƿ���ָ��ĩβ���������ֱ�ӷ��� FALSE ��ʾ�������ѱ�����ȫ��Ԫ�ء�
 * �����ȡ��ǰ������ָ���Ԫ�أ�����������ָ����һ��Ԫ��
 *
 * @param next_item     ��ȡ��ǰԪ��ֵ��ֻ�Ǵ�����
 * @return              �Ƿ����ߵ�ĩβ
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> inline BOOL 
Fast_Hashtable_Iterator<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::next(value_type *&next_item)
{
    if( this->done() ) 
        return FALSE; 
    next_item = &(_cur->_val); 
    this->advance(); 
    return TRUE; 
}

/**
 * ������ǰ������ advance()
 * �൱�ڵ��������ǰ����������operator++()����ͬ�������˺ܶ��жϴ���
 * ���Ƿ����ߵ�ĩβ���������Ƿ�յȣ�����ȫ
 *
 * @return      �Ƿ����ߵ�ĩβ
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> inline BOOL 
Fast_Hashtable_Iterator<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::advance()
{
    if( this->done() ) 
        return FALSE; 

    if( _ht != 0 ) 
    {
        if( _cur == 0 ) this->first(); 
        this->operator++(); 
    }
    return TRUE; 
}

/**
 * ������ָ��ĩλ�жϺ��� done()
 * �жϵ�ǰ�������Ƿ����ߵ�ĩβ���Ƿ��� TRUE�����򷵻� FALSE
 *
 * @return      �Ƿ����ߵ�ĩβ
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> inline BOOL 
Fast_Hashtable_Iterator<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::done() const
{
    if( _ht != 0 ) 
    {
        if( *this == _ht->end() )
            return TRUE; 
        else
            return FALSE; 
    }
    return TRUE; 
}


//=================FastHashMap Functions Implements=================

/**
 * ���캯�� Fast_Hashtable()
 *
 * @param n     ��ʼ�� buckets �����Ĵ�С
 * @param hf    HASH�㷨������
 * @param eql   �Ƚ�KEY������
 * @param ext   ��ȡKEY������
 * @param a     �ڴ������ allocator
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> inline
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>
    ::Fast_Hashtable(size_type    n,
              const HASHFUNC&     hf,
              const EQUALKEY&     eql,
              const EXTRACTKEY&   ext,
              allocator_type*     a)
: _allocator(a),                    // ȱʡʹ���Լ��� allocator
  _release_allocator(FALSE),        // ��ʼ�������ڲ���Ա
  _hash(hf),
  _equals(eql),
  _get_key(ext),
  _buckets(),
  _num_elements(0)
{
    if( _allocator == 0 ) 
    {
        // ȱʡ�Լ������ڴ������ allocator
        _allocator = (allocator_type *) new default_allocator(); 
        _release_allocator = TRUE; 
    }
    _initialize_buckets(n);         // ��ʼ�� n �� buckets
}

/**
 * ���캯�� Fast_Hashtable()
 *
 * @param n     ��ʼ�� buckets �����Ĵ�С
 * @param hf    HASH�㷨������
 * @param eql   �Ƚ�KEY������
 * @param a     �ڴ������ allocator
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> inline
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>
    ::Fast_Hashtable(size_type    n,
              const HASHFUNC&     hf,
              const EQUALKEY&     eql,
              allocator_type*     a)
: _allocator(a),                    // ȱʡʹ���Լ��� allocator
  _release_allocator(FALSE),        // ��ʼ�������ڲ���Ա
  _hash(hf),
  _equals(eql),
  _get_key(EXTRACTKEY()),
  _buckets(),
  _num_elements(0)
{
    if( _allocator == 0 ) 
    {
        // ȱʡ�Լ������ڴ������ allocator
        _allocator = (allocator_type *) new default_allocator(); 
        _release_allocator = TRUE; 
    }
    _initialize_buckets(n);         // ��ʼ�� n �� buckets
}

/**
 * ASSIGN���캯�� Fast_Hashtable()
 *
 * @param ht    ��һ��HASH����
 * @param a     �ڴ������ allocator
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> inline
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>
    ::Fast_Hashtable(const Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>& ht, 
                     allocator_type* a)
: _allocator(a),                    // ȱʡʹ���Լ��� allocator
  _release_allocator(FALSE),        // ��ʼ�������ڲ���Ա
  _hash(ht._hash),
  _equals(ht._equals),
  _get_key(ht._get_key),
  _buckets(),
  _num_elements(0)
{
    if( _allocator == 0 ) 
    {
        // ȱʡ�Լ������ڴ������ allocator
        _allocator = (allocator_type *) new default_allocator(); 
        _release_allocator = TRUE; 
    }
    _copy_from(ht);               // �� ht ������Ԫ�ظ��ƹ���
}

/**
 * ASSIGN��������� operator=()
 *
 * @param ht    ��һ��HASH����
 * @return      �����Լ�
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> inline
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>& 
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>
    ::operator= (const Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>& ht)
{
    if( &ht != this )               // �������Լ���ֵ���Լ�
    {
        clear();                    // ���������Ԫ�ؽڵ㣬������ _buckets �Ĵ�С
        _hash = ht._hash;           // HASH �㷨������
        _equals = ht._equals;       // �Ƚ�KEY������
        _get_key = ht._get_key;     // ��ȡKEY������
        _copy_from(ht);             // �� ht ������Ԫ�ظ��ƹ���
    }
    return *this;
}

/**
 * �������� ~Fast_Hashtable()
 * ɾ������Ԫ�ؽڵ㣬�ͷ������ڴ档ע�� allocator ���ͷ�
 *
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> inline
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::~Fast_Hashtable() 
{ 
    this->clear(); 
    if( _release_allocator == TRUE ) 
        FAST_DELETE(_allocator, allocator_type); 
    // �����Ҫ�ͷ��ڴ��������Ҫ�ͷ� allocator
}

/**
 * ��ȡ��ʼ���������� begin()
 * ��Ҫ���� buckets ���ҵ���һ��Ԫ�ص� bucket
 *
 * @return      ָ���һ��Ԫ�صĵ�����
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> inline
typename Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::iterator 
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::begin()
{ 
    // �ӱ���е�һ�� bucket �ҳ���һ�����нڵ�� bucket
    for( size_type n = 0; n < _buckets.size(); ++n )
    {
        // ��� bucket ��Ϊ 0 ��Ϊ�գ���Ϊ��һ���ڵ�
        if( _buckets[n] ) 
            return iterator(_buckets[n], this);
    }
    // �Ҳ�������ָ��ĩβ�Ŀյ�����
    return end();
}

/**
 * ��ȡĳ�� bucket ��Ԫ�صĸ������� begin()
 * ��Ҫ���� bucket ָ��������Ի�� bucket ��Ԫ�صĸ���
 *
 * @param bucket    �ڼ��� bucket
 * @return          Ԫ�صĸ���
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> inline
typename Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::size_type 
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::elems_in_bucket(size_type bucket) const
{
    size_type result = 0;
    for( hash_node* cur = _buckets[bucket]; cur; cur = cur->_next)
        result += 1;    // �߹� bucket ����������Ԫ�أ������ۼ�
    return result;
}

/**
 * ������Ԫ�أ��������ظ� insert_unique() 
 *
 * @param obj   �������ݶ���
 * @return      ָ����Ԫ�صĵ�����
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> inline
Fast_Pair<typename Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::iterator, BOOL> 
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::insert_unique(const value_type& obj)
{
    // �ж��Ƿ���Ҫ�ؽ��������Ҫ������
    resize(_num_elements + 1);
    return insert_unique_noresize(obj);
}

/**
 * ������Ԫ�أ������ظ� insert_equal()
 *
 * @param obj   �������ݶ���
 * @return      ָ����Ԫ�صĵ�����
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> inline
typename Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::iterator 
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::insert_equal(const value_type& obj)
{
    // �ж��Ƿ���Ҫ�ؽ��������Ҫ������
    resize(_num_elements + 1);
    return insert_equal_noresize(obj);
}

/**
 * ���벻�ظ���һ����Ԫ�غ��� insert_unique()
 * ����һ����Ԫ�أ���ֵ�������ظ�
 *
 * @param f     ָ����ʼ��Ԫ�ص�ָ��
 * @param l     ָ���β��Ԫ�ص�ָ��
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> inline
void Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>
    ::insert_unique(const value_type* f, const value_type* l)
{
    // ������Ԫ����Ŀ
    size_type n = l - f;
    // �ж��Ƿ���Ҫ�ؽ��������Ҫ������
    resize(_num_elements + n);
    // ���β���
    for( ; n > 0; --n, ++f )
        insert_unique_noresize(*f);
}

/**
 * ������ظ���һ����Ԫ�غ��� insert_equal()
 * ����һ����Ԫ�أ���ֵ�����ظ�
 *
 * @param f     ָ����ʼ��Ԫ�ص�ָ��
 * @param l     ָ���β��Ԫ�ص�ָ��
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> inline
void Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>
    ::insert_equal(const value_type* f, const value_type* l)
{
    // ������Ԫ����Ŀ
    size_type n = l - f;
    // �ж��Ƿ���Ҫ�ؽ��������Ҫ������
    resize(_num_elements + n);
    // ���β���
    for( ; n > 0; --n, ++f )
        insert_equal_noresize(*f);
}

/**
 * ����Ԫ�غ��� find()
 *
 * @param key   Ԫ�صļ�ֵ
 * @return      ָ��Ԫ�صĵ�����
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> 
typename Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::iterator 
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::find(const key_type& key) 
{
    // ����Ѱ��������һ�� bucket ��
    size_type n = _bkt_num_key(key);
    hash_node* first = NULL;
    // ���£��� bucket list ��ͷ��ʼ��һһ�Ƚ�ÿһ��Ԫ�صļ�ֵ
    // ��Ⱦ�����
    for( first = _buckets[n];
            first && !_equals(_get_key(first->_val), key);
            first = first->_next)
    {}
    return iterator(first, this);
} 

/**
 * ͳ����ͬ��ֵ��Ԫ�ظ������� count()
 *
 * @param key   Ԫ�صļ�ֵ
 * @return      ָ��Ԫ�صĵ�����
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> 
typename Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::size_type 
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::count(const key_type& key) const
{
    // ����Ѱ��������һ�� bucket ��
    const size_type n = _bkt_num_key(key);
    size_type result = 0;
    // ���£��� bucket list ��ͷ��ʼ��һһ�Ƚ�ÿһ��Ԫ�صļ�ֵ
    // ��Ⱦ��ۼ� 1
    for( const hash_node* cur = _buckets[n]; cur; cur = cur->_next )
    {
        if( _equals(_get_key(cur->_val), key) )
            ++ result;
    }
    return result;
}

/**
 * ��ʼ�� buckets vector
 *
 * @param n     ����� buckets ����Ŀ
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> inline
void Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::_initialize_buckets(size_type n)
{
    // ��ȡ����ָ����Ŀn����С����
    const size_type n_buckets = _next_size(n);
    // ���� buckets vector��������С
    _buckets.resize(n_buckets);
    // ������õ�һ�����ǲ����λ�ã����� buckets ���һ��λ�ò���
    // �ڶ��������ǲ���Ԫ�صĸ����������������ǿ�ֵ���������ǳ�ʼ�� buckets
    _buckets.insert(_buckets.size(), (hash_node*) 0, n_buckets);
    _num_elements = 0;
}

/**
 * �ڵ������� ����һ���ڵ�
 *
 * @param obj   ��ʼ���ڵ��ֵ
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> inline
typename Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::hash_node* 
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::_new_node(const value_type& obj)
{
    hash_node* n = _get_node();                         // �����ڴ�
    n->_next = 0;
    _FAST::fast_construct<value_type>(&n->_val, obj);   // ����VALUE������
    return n;
}

/**
 * �ڵ������� �ͷ�һ���ڵ�
 *
 * @param n     ָ��ڵ�ָ��
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> inline
void Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::_delete_node(hash_node* n)
{
    _FAST::fast_destroy<value_type>(&n->_val);          // ����VALUE������
    _put_node(n);                                       // �ͷ�ָ��
}

/**
 * ����Ψһ����Ԫ�غ��� insert_unique_noresize()
 * �ڲ���Ҫ�ؽ���������²����½ڵ㣬��ֵ�������ظ�
 *
 * @param obj   ��Ԫ�ض���
 * @return      ָ����Ԫ�صĵ�����
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY>
Fast_Pair<typename Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::iterator, BOOL> 
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::insert_unique_noresize(const value_type& obj)
{
    // ��λ obj Ӧλ�� #n bucket
    const size_type n = _bkt_num(obj);
    // �� first ָ�� bucket ��Ӧ֮����ͷ��
    hash_node* first = _buckets[n];

    // ��� buckets[n] �ѱ�ռ�ã���ʱfirst����Ϊ0�����ǽ�������ѭ��
    // �߹�bucket����Ӧ����������
    for( hash_node* cur = first; cur; cur = cur->_next ) 
    {
        if( _equals(_get_key(cur->_val), _get_key(obj)) ) 
            // ��������������е�ĳ��ֵ��ͬ���Ͳ����룬��������
            return Fast_Pair<iterator, BOOL>(iterator(cur, this), FALSE);
    }

    // �뿪����ѭ�������δ����ѭ��ʱ��firstָ��bucket��ָ�������ͷ���ڵ�
    hash_node* tmp = _new_node(obj);    // �����½ڵ�
    tmp->_next = first;
    _buckets[n] = tmp;                  // ���½ڵ��Ϊ����ĵ�һ���ڵ�
    ++_num_elements;                    // �ڵ�����ۼ� 1

    return Fast_Pair<iterator, BOOL>(iterator(tmp, this), TRUE);
}

/**
 * ���������ظ�����Ԫ�غ��� insert_equal_noresize()
 * �ڲ���Ҫ�ؽ���������²����½ڵ㣬��ֵ�����ظ�
 *
 * @param obj   ��Ԫ�ض���
 * @return      ָ����Ԫ�صĵ�����
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY>
typename Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::iterator 
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::insert_equal_noresize(const value_type& obj)
{
    // ��λ obj Ӧλ�� #n bucket
    const size_type n = _bkt_num(obj);
    // �� first ָ�� bucket ��Ӧ֮����ͷ��
    hash_node* first = _buckets[n];

    // ��� buckets[n] �ѱ�ռ�ã���ʱfirst����Ϊ0�����ǽ�������ѭ��
    // �߹�bucket����Ӧ����������
    for( hash_node* cur = first; cur; cur = cur->_next ) 
    {
        if( _equals(_get_key(cur->_val), _get_key(obj)) ) 
        {
            // ��������������е�ĳ��ֵ��ͬ�������ϲ��룬Ȼ�󷵻�
            hash_node* tmp = _new_node(obj);    // �����½ڵ�
            tmp->_next = cur->_next;            // ���½ڵ����Ŀǰλ��
            cur->_next = tmp;
            ++ _num_elements;                   // �ڵ�����ۼ� 1
            return iterator(tmp, this);         // ����һ����������ָ�������ڵ�
        }
    }

    // ���е��ˣ���ʾû�з����ظ��ļ�ֵ
    hash_node* tmp = _new_node(obj);            // �����½ڵ�
    tmp->_next = first;                         // ���½ڵ��������ͷ��
    _buckets[n] = tmp;
    ++ _num_elements;                           // �ڵ�����ۼ� 1
    return iterator(tmp, this);                 // ����һ����������ָ�������ڵ�
}

/**
 * ���һ������Ԫ�غ��� find_or_insert()
 * �Ȳ���ָ����ֵ��Ԫ�أ���������򷵻ض�Ӧ��VALUEֵ�������������������ֵ
 *
 * @param obj   ����Ԫ�ض���
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY>
typename Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::reference 
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::find_or_insert(const value_type& obj)
{
    // ���� buckets vector��������СΪԭ������1�Բ�����ֵ
    resize(_num_elements + 1);

    // ��λ obj Ӧλ�� #n bucket
    size_type n = _bkt_num(obj);
    // �� first ָ��bucket ��Ӧ֮����ͷ��
    hash_node* first = _buckets[n];

    // ��� buckets[n] �ѱ�ռ�ã���ʱfirst����Ϊ0�����ǽ�������ѭ��
    // �߹�bucket����Ӧ����������
    for( hash_node* cur = first; cur; cur = cur->_next )
    {
        // ��������������е�ĳ��ֵ��ͬ�������Ϸ������Ӧֵ
        if( _equals(_get_key(cur->_val), _get_key(obj)) )
            return cur->_val;
    }

    // ���е��ˣ���ʾû�з�����ȵļ�ֵ
    hash_node* tmp = _new_node(obj);        // �����½ڵ�
    tmp->_next = first;                     // ���½ڵ��������ͷ��
    _buckets[n] = tmp;
    ++ _num_elements;                       // �ڵ�����ۼ� 1
    return tmp->_val;                       // ���������ڵ��Ӧ��ֵ
}

/**
 * �������Ԫ�ص���ʼλ�ú��� equal_range()
 * �Ȳ���ָ����ֵ��Ԫ�أ��ٲ�����֮��ȵ����һ��Ԫ�ص�λ��
 * ���ڲ��Ҽ�ֵ���ظ���Ԫ�ط�Χ(begin, end]��������ʼλ��begin
 * ������������λ��end
 *
 * @param key   ����Ԫ�صļ�ֵ
 * @return      ����ָ����ȵ�����Ԫ�ط�Χ����ʼλ�õĵ�����
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY>
Fast_Pair<typename Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::iterator,
          typename Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::iterator> 
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::equal_range(const key_type& key)
{
    // �����������
    typedef Fast_Pair<iterator, iterator> Pii;
    // ��λ key Ӧλ�� #n bucket
    const size_type n = _bkt_num_key(key);

    // �߹�bucket����Ӧ�������������Ҽ�ֵ���� key ��Ԫ��
    for( hash_node* first = _buckets[n]; first; first = first->_next )
    {
        if( _equals(_get_key(first->_val), key) ) 
        {
            // �����ȣ���˵�ǰԪ�� cur ����ʼλ�ã������������һ��
            // �ҵ���ֵ������ key ��Ԫ�أ���Ϊ����λ��
            for( hash_node* cur = first->_next; cur; cur = cur->_next ) 
                if( !_equals(_get_key(cur->_val), key) )
                    return Pii(iterator(first, this), iterator(cur, this));

            // ������������û�ҵ�������ʼλ�ú�ȫ����ȵ�Ԫ�ػ���ֻ��һ��
            // �����������һ�� bucket �ĵ�һ��Ԫ�أ���Ϊ����λ��
            for( size_type m = n + 1; m < _buckets.size(); ++ m )
                if( _buckets[m] )
                    return Pii(iterator(first, this), iterator(_buckets[m], this));

            // û�ҵ���һ�� bucket���� end() Ϊ����λ��
            return Pii(iterator(first, this), end());
        }
    }

    // ʲôҲû�ҵ�
    return Pii(end(), end());
}

/**
 * ɾ��ָ����ֵ������Ԫ�غ��� erase()
 * �Ȳ���ָ����ֵ��Ԫ�أ��ٲ�����֮��ȵ���һ��Ԫ�أ�����ɾ��
 *
 * @param key   ɾ��Ԫ�صļ�ֵ
 * @return      ����ɾ����Ԫ����Ŀ
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY>
typename Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::size_type 
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::erase(const key_type& key)
{
    // ��λ key Ӧλ�� #n bucket
    const size_type n = _bkt_num_key(key);
    hash_node* first = _buckets[n];         // ָ�� bucket �������һ��Ԫ��
    size_type erased = 0;

    if( first ) 
    {
        // ��������������Ԫ��
        hash_node* cur = first;
        hash_node* next = cur->_next;
        while( next ) 
        {
            // �ҳ���ֵ���� key ��Ԫ�أ�ɾ��֮
            if( _equals(_get_key(next->_val), key) ) 
            {
                cur->_next = next->_next;   // �Ͽ�ָ���Ԫ��
                _delete_node(next);         // �ͷŽڵ�ռ�õ��ڴ�
                next = cur->_next;          // ָ����һ���ڵ㣬�����Ƚ�
                ++ erased;                  // ɾ�������ۼ� 1
                -- _num_elements;           // Ԫ�ؼ����� 1
            }
            else 
            {
                cur = next;
                next = cur->_next;          // ָ����һ���ڵ㣬�����Ƚ�
            }
        }
        // ���ڴ� bucket �е�һ���ڵ㻹δ�Ƚϣ����Ի�����һ�� first
        if( _equals(_get_key(first->_val), key) ) 
        {
            _buckets[n] = first->_next;     // bucket ָ����һ���ԶϿ� first
            _delete_node(first);            // �ͷŽڵ�ռ�õ��ڴ�
            ++ erased;                      // ɾ�������ۼ� 1
            -- _num_elements;               // Ԫ�ؼ����� 1
        }
    }

    return erased;
}

/**
 * ɾ��������ָ���Ԫ�غ��� erase()
 * ���ҵ�������ָ���Ԫ�����ڵ� bucket���ٲ������������Ӧ��Ԫ�ز�ɾ��
 *
 * @param it    ɾ��Ԫ�صĵ�����
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY>
void Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>
    ::erase(const iterator& it)
{
    // ��ȡָ��ڵ��ָ��
    hash_node* p = it._cur;
    if( p ) 
    {
        // ��λ p Ӧλ�� #n bucket
        const size_type n = _bkt_num(p->_val);
        hash_node* cur = _buckets[n];

        // ����� bucket �������еĵ�һ���ڵ���ǣ�ɾ��֮
        if( cur == p ) 
        {
            _buckets[n] = cur->_next;           // bucket ָ����һ���ԶϿ���
            _delete_node(cur);                  // �ͷŽڵ�ռ�õ��ڴ�
            -- _num_elements;                   // Ԫ�ؼ����� 1
        }
        else 
        {
            // ������ǣ����α������µĽڵ�
            hash_node* next = cur->_next;
            while( next ) 
            {
                if( next == p )                 // �ж��Ƿ�Ŀ��ڵ㣺�ǣ�ɾ��֮
                {
                    cur->_next = next->_next;   // ָ����һ���ԶϿ���
                    _delete_node(next);         // �ͷŽڵ�ռ�õ��ڴ�
                    -- _num_elements;           // Ԫ�ؼ����� 1
                    break;
                }
                else                            // ���ǣ�����..
                {
                    cur = next;
                    next = cur->_next;
                }
            }
        }
    }
}

/**
 * ɾ����������ָ��Χ֮���Ԫ�غ��� erase()
 * ���ҵ� first ������ָ���Ԫ�ؽڵ㣬����ɾ���� last ������ָ��Ľڵ�
 *
 * @param first   ��ʼԪ�صĵ�����
 * @param last    ����Ԫ�صĵ�����
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY>
void Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>
    ::erase(iterator first, iterator last)
{
    // ��ȡ first �� last ָ��� bucket #n�����ֵΪ����ָ�������Ŀ� bucket
    size_type f_bucket = first._cur ? _bkt_num(first._cur->_val) : _buckets.size();
    size_type l_bucket = last._cur  ? _bkt_num(last._cur->_val)  : _buckets.size();

    // �������������Ȼ������������ڣ�ֱ�ӷ��ز�������
    if( first._cur == last._cur )
        return;
    // �����������ָ��Ԫ�ؽڵ���ͬһ bucket ��
    else if( f_bucket == l_bucket ) 
        // ɾ�� bucket �������� first �� last ֮��Ľڵ�
        _erase_bucket(f_bucket, first._cur, last._cur);
    else 
    {
        // �����������ָ��Ԫ�ؽڵ��ڲ�ͬ bucket ��
        // ɾ�� first bucket �������� first �� ĩβ ֮��Ľڵ�
        _erase_bucket(f_bucket, first._cur, 0);
        // ���η��� fist bucket �� last bucket ֮��� buckets����������last��
        for( size_type n = f_bucket + 1; n < l_bucket; ++ n ) 
            // ɾ�� bucket �����нڵ�
            _erase_bucket(n, 0);
        // ��� last bucket ����
        if( l_bucket != _buckets.size() )
            // ɾ�� last bucket �������� ��ʼ �� last ֮��Ľڵ�
            _erase_bucket(l_bucket, last._cur);
    }
}

/**
 * �ؽ������ resize()
 * ���º����ж��Ƿ���Ҫ�ؽ�����������Ҫ���������ء������Ҫ���Ͷ��֡���
 *
 * @param num_elements_hint ������������ı����Ԫ�صĸ���
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY>
void Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>
    ::resize(size_type num_elements_hint)
{
    // ���£�������ؽ���񡱵��ж�ԭ������Ԫ�ظ�����������Ԫ�ؼ����
    // �� bucket vector �Ĵ�С�ȣ����ǰ�ߴ��ں��ߣ����ؽ����
    // ÿ�� bucket��list�� �����������buckets vector�Ĵ�С��ͬ
    const size_type old_n = _buckets.size();

    // ȷ�������Ҫ��������
    if( num_elements_hint > old_n ) 
    {
        // �ҳ���һ������ 
        const size_type n = _next_size(num_elements_hint);
        if( n > old_n ) 
        {
            // �����µ�buckets
            FastVector<hash_node*> tmp(n, (hash_node*)(0));

            // ���´���ÿһ���ɵ�bucket
            for( size_type bucket = 0; bucket < old_n; ++ bucket ) 
            {
                // ָ��ڵ�����Ӧ֮���е���ʼ�ڵ�
                hash_node* first = _buckets[bucket];
                // ���´���ÿһ����bucket���������У���ÿһ���ڵ㣬����ָ�����
                while( first )  // ���л�û�н���ʱ
                {
                    // �����ҳ��ڵ�������һ����bucket��
                    size_type new_bucket = _bkt_num(first->_val, n); 
                    // ���bucketָ��������Ӧ֮���е���һ���ڵ㣬�Ա��������
                    _buckets[bucket] = first->_next;
                    // ����ǰ�ڵ���뵽��bucket�ڣ���Ϊ���Ӧ���еĵ�һ���ڵ�
                    first->_next = tmp[new_bucket];
                    tmp[new_bucket] = first;
                    // �ص���bucket��ָ�Ĵ������У�׼��������һ�ڵ�
                    first = _buckets[bucket];          
                }
            }
            // FastVector::swap �¾�����buckets�Ե�
            _buckets.swap(tmp);
            // ע���Ե����������С��ͬ����Ļ��С��С�Ļ���
            // ���ǽ���vector�ڲ�Ԫ�ص�ֵ�������¸��ƹ�������൱��ָ���л���Ч�ʺܸ�
            // �뿪ʱ�ͷ�local tmp���ڴ�
        }
    }
}

/**
 * ɾ��bucket��ָ��Χ֮���Ԫ�غ��� _erase_bucket()
 * ���ҵ� first ָ���Ԫ�ؽڵ㣬����ɾ���� last ָ��Ľڵ� �������� last��
 *
 * @param n       �ڼ��� bucket
 * @param first   ��ʼԪ�ؽڵ��ָ��
 * @param last    ����Ԫ�ؽڵ��ָ��
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY>
void Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>
    ::_erase_bucket(const size_type n, hash_node* first, hash_node* last)
{
    hash_node* cur = _buckets[n];           // ��ȡ bucket �����е���ʼָ��
    if( cur == first )
        _erase_bucket(n, last);             // ���ָ����ȣ�ɾ������ʼλ�õ�last�Ľڵ�
    else
    {
        hash_node* next = 0;
        for( next = cur->_next; next != first; cur = next, next = cur->_next )
            ;                               // ���α���ֱ�� first �ڵ�
        while( next != last )               // ����ɾ���ڵ��� last �ڵ� ����ɾ�� last��
        {
            cur->_next = next->_next;       // ָ����һ���ԶϿ���
            _delete_node(next);             // �ͷŽڵ�ռ�õ��ڴ�
            next = cur->_next;              // ָ����һ��������
            -- _num_elements;               // Ԫ�ؼ����� 1
        }
    }
}

/**
 * ɾ��bucket��ָ��Χ֮���Ԫ�غ��� _erase_bucket()
 * ����ɾ�� bucket ����ʼ�ڵ��� last ָ��Ľڵ� �������� last��
 *
 * @param n       �ڼ��� bucket
 * @param last    ����Ԫ�ؽڵ��ָ��
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY>
void Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>
    ::_erase_bucket(const size_type n, hash_node* last)
{
    hash_node* cur = _buckets[n];
    while( cur != last )                    // ����ɾ���ڵ��� last �ڵ� ����ɾ�� last��
    {
        hash_node* next = cur->_next;       // ָ����һ���ԶϿ���
        _delete_node(cur);                  // �ͷŽڵ�ռ�õ��ڴ�
        cur = next;                         // ָ����һ��������
        _buckets[n] = cur;                  // �� bucket ָ����µ���ʼ�ڵ�
        -- _num_elements;                   // Ԫ�ؼ����� 1
    }
}

/**
 * ����ɾ����������нڵ㺯�� clear()
 * ��������hashtable �� vector �� linked-list ��϶��ɣ���ˣ����ƺ�����ɾ��
 * ����Ҫ�ر�ע���ڴ���ͷ�����
 *
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY>
void Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::clear()
{
    // ���ÿһ�� bucket
    for( size_type i = 0; i < _buckets.size(); ++ i ) 
    {
        hash_node* cur = _buckets[i];
        // �� bucket list �е�ÿһ���ڵ�ɾ����
        while( cur != 0 ) 
        {
            hash_node* next = cur->_next;   // ָ����һ���ԶϿ���
            _delete_node(cur);              // �ͷŽڵ�ռ�õ��ڴ�
            cur = next;                     // ָ����һ��������
        }
        _buckets[i] = 0;                    // �� bucket ����Ϊ null ָ��
    }
    _num_elements = 0;                      // ���ܽڵ����Ϊ 0
    // ע��buckets vector ��δ�ͷŵ��ռ䣬�Ա���ԭ����С
}

/**
 * ���ƺ��� copy_from()
 * ��������hashtable �� vector �� linked-list ��϶��ɣ���ˣ����ƺ�����ɾ��
 * ����Ҫ�ر�ע���ڴ���ͷ�����
 *
 * @param ht    �����Ƶ�hashtable����
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY>
void Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>
    ::_copy_from(const Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>& ht)
{
    // ����������� buckets vector��������ǵ���vector::clear���������Ԫ��Ϊ 0
    _buckets.clear();

    // Ϊ������ buckets vector �����ռ䣬ʹ��Է���ͬ
    // ��������ռ���ڶԷ����Ͳ�������������ռ�С�ڶԷ����ͻ�����
    // ��Щ���� vector ����Ĵ���ʽ
    _buckets.reserve(ht._buckets.size());

    // ������õ�һ�����ǲ����λ�ã����� buckets�Ѿ�����ˣ������� 0������
    // ��һ��λ�ò��룬�ڶ��������ǲ���Ԫ�صĸ�����ʵ�ʼ����ƵĶ���� buckets
    // ��Ԫ���ĸ����������������ǿ�ֵ���������ǳ�ʼ�� buckets
    _buckets.insert(_buckets.size(), (hash_node*)(0), ht._buckets.size());

    // ��� buckets vector
    for( size_type i = 0; i < ht._buckets.size(); ++ i ) 
    {
        // ���� vector ��ÿһ��Ԫ��,�Ǹ�ָ�룬ָ�� hashtable �ڵ�
        const hash_node* cur = ht._buckets[i];
        if( cur ) 
        {
            hash_node* copy = _new_node(cur->_val);
            _buckets[i] = copy;

            // ���ͬһ�� bucket list������ÿһ���ڵ�
            for( hash_node* next = cur->_next; next; 
                 cur = next, next = cur->_next ) 
            {
                copy->_next = _new_node(next->_val);
                copy = copy->_next;
            }
        }
    }
    // ���¸��½ڵ������hashtable�Ĵ�С��
    _num_elements = ht._num_elements;
}

/**
 * ����hashmap����� swap()
 * ����һ��ȫ��swap�������أ���ʵ�ֽ���������ͬģ��������͵�����
 * ʵ���ǵ���hashmap.swap()ʵ�֣���������������������ݶ��Ե���ʵ��
 * ������ָ�����Ч�ʺܸߣ����ô����޴����ʱ��������
 *
 * @param ht   �ڶ���hashmap������
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY>
void Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::swap(Fast_Hashtable& ht)
{
    _FAST::fast_swap_value<allocator_type*>(_allocator, ht._allocator);
    _FAST::fast_swap_value<BOOL>(_release_allocator, ht._release_allocator);
    _FAST::fast_swap_value<hasher>(_hash, ht._hash);
    _FAST::fast_swap_value<key_equal>(_equals, ht._equals);
    _FAST::fast_swap_value<EXTRACTKEY>(_get_key, ht._get_key);
    _buckets.swap(ht._buckets);
    _FAST::fast_swap_value<size_type>(_num_elements, ht._num_elements);
}

/**
 * ����hashmap����� swap()
 * ����һ��ȫ��swap�������أ���ʵ�ֽ���������ͬģ��������͵�����
 * ʵ���ǵ���hashmap.swap()ʵ�֣���������������������ݶ��Ե���ʵ��
 * ������ָ�����Ч�ʺܸߣ����ô����޴����ʱ��������
 *
 * @param hm1   ��һ��hashmap������
 * @param hm2   �ڶ���hashmap������
 */
template<class KEY, class TP, class HASHFUNC, class EQUALKEY>
inline void swap(Fast_HashMap<KEY,TP,HASHFUNC,EQUALKEY>& hm1,
                 Fast_HashMap<KEY,TP,HASHFUNC,EQUALKEY>& hm2)
{
    hm1.swap(hm2);
}

/**
 * ���ٶ������ݺ��� dump()
 * ��hash table�� buckets����ָ��ڵ������ӡ���������ڵ��Ը���
 *
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY>
void Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::dump() const
{
    FAST_TRACE_BEGIN("Fast_Hashtable::dump()");
    FAST_TRACE("sizeof(Fast_Hashtable) = %d", sizeof(*this)); 
    FAST_TRACE("sizeof(allocator_type) = %d", sizeof(allocator_type)); 
    FAST_TRACE("_num_elements = %d, _buckets.size = %d", 
        _num_elements, _buckets.size()); 
#ifdef FAST_DEBUG
    int empty_count = 0, maxemptycount = 0, totalemptycount = 0; 
    int totalnodecount = 0, maxnodecount = 0; 
    for( int i = 0; i < (int) _buckets.size(); i ++ ) 
    {
        hash_node* cur = _buckets[i];
        if( !cur ) {
            empty_count ++; totalemptycount ++; 
            continue; 
        }
        if( empty_count > 0 ) 
            FAST_TRACE("......%d empty buckets", empty_count); 
        if( empty_count > maxemptycount ) 
            maxemptycount = empty_count; 
        empty_count = 0; 

        FAST_TRACE("_buckets[%d] -> _cur = 0x%08X", i, cur); 
        FAST_TRACE("    |-> _cur -> 0x%08X, _bkt_num = %d", 
            cur, _bkt_num_key(cur->_val.first())); 
        FAST_TRACE("        _cur._key -> 0x%08X, _cur._val -> 0x%08X", 
            &(cur->_val.first()), &(cur->_val.second())); 

        int nodecount = 1; 
        totalnodecount ++; 

        while( (cur = cur->_next) ) {
            totalnodecount ++; nodecount ++; 
            FAST_TRACE("    |-> _cur -> 0x%08X, _bkt_num = %d", 
                cur, _bkt_num_key(cur->_val.first())); 
            FAST_TRACE("        _cur._key -> 0x%08X, _cur._val -> 0x%08X", 
                &(cur->_val.first()), &(cur->_val.second())); 
        }

        if( nodecount > maxnodecount ) 
            maxnodecount = nodecount; 
    }
    FAST_TRACE("total_empty_buckets_count = %d", totalemptycount); 
    FAST_TRACE("max_empty_buckets_count = %d", maxemptycount); 
    FAST_TRACE("total_nodecount = %d", totalnodecount); 
    FAST_TRACE("max_nodecount_in_bucket = %d", maxnodecount); 
    FAST_TRACE("_num_elements = %d, _buckets.size = %d", 
        _num_elements, _buckets.size()); 
#endif
    FAST_TRACE_END("Fast_Hashtable::dump()");
}


//===========FastHashMap Functions Implements===============
/**
 * ��ȡָ����ֵ��Ԫ��ֵ���� get()
 * �Ȳ���ָ����ֵ��Ԫ�أ�����ҵ�ȡ�����ݣ����� 0�����򷵻� -1
 *
 * @param key   ��ֵ
 * @param value ��õ�Ԫ��ֵ
 * @return      �ɹ���񣬳ɹ����� 0�����򷵻� -1
 */
template<class KEY, class TP, class HASHFUNC, class EQUALKEY> inline int
Fast_HashMap<KEY,TP,HASHFUNC,EQUALKEY>::get(const key_type& key, TP& value) 
{ 
    iterator it = _ht.find(key); 
    if( it != this->end() ) 
    { 
        value = it->second(); 
        return 0; 
    } 
    else 
    {
        value = TP(); 
        return -1; 
    } 
}

/**
 * ���Ԫ��ֵ�Ƿ���ں��� containsValue()
 * �������ĵ�һ��Ԫ�� begin() ���α��������һ��Ԫ�� end()��������ж��Ƿ����
 * ����ҵ�ֵ���Ԫ�أ��������� TRUE�����ȫ�������꼴û�ҵ����� FALSE��
 *
 * @param value Ԫ��ֵ
 * @return      �ҵ���񣬳ɹ����� TRUE�����򷵻� FALSE
 */
template<class KEY, class TP, class HASHFUNC, class EQUALKEY> BOOL 
Fast_HashMap<KEY,TP,HASHFUNC,EQUALKEY>::containsValue(const TP& value) 
{ 
    iterator it = this->begin(); 
    while( it != this->end() ) 
    { 
        if( it->second() == value ) 
            return TRUE; 
        it ++; 
    } 
    return FALSE; 
}


_FAST_END_NAMESPACE

#endif
