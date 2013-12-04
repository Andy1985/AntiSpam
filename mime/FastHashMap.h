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
 * 通过学习SGI STL的源码、ACE网络库源码和jjhou的《STL源码剖析》
 * 将HashMap容器从STL中剥离，并综合各个库的优点，做一定的优化和
 * 简化处理，使其更易使用和提高效率。
 *
 * <p>
 * 为了安全和统一，本容器用到的类对象必须满足以下条件：
 *
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
 * @author Naven
 * @see SGI STL & ACE wrappers & JJHOU TAOSS-02
 */

// Forward declaration of functions for hashing

/**
 * 28个连续的质数表格
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
 * 找出上述28个质数之中，最接近并大于n的那个质数。
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
 * 针对char *, int, long等类型特化的函数对象（仿函数）
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
// 自定义的 hash functions 特化版本
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


// put functions for hashmap.put() 泛化版本
template<class T> struct Fast_Hash_Put_Value { 
    void operator()( T &s1, T &s2 ) { s1 = s2; }
};

// put functions for hashmap.put() 特化版本
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
 * hashtable 迭代器必须永远维系着与整个 buckets vector 的关系，并记录目前所指的节点
 * 其前进操作是首先尝试从目前所指的节点出发，前进一个位置/节点，由于节点被安置于list
 * 内，所以利用节点的next指针即可轻易完成前进操作。如果目前节点正好是list的尾端，就
 * 跳至下一个bucket身上，那正是指向下一个list的头部节点。
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
    hash_node* _cur;    // 迭代器目前所指节点
    hashtable* _ht;     // 保持对容器的连接关系，因为有可能需要从bucket跳到bucket

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
 * hashtable是实现hashmap的基础类，使用开链（separate chaining）法实现hash容器。
 * 这种做法是在每一个表格元素中维护一个list；hash function为我们分配某一个list，
 * 然后我们在那个list身上执行元素的插入、搜寻、删除等操作，虽然针对list而进行的
 * 搜寻只能是一种线性操作，但如果list够短，速度还是够快。
 *
 * 模板参数定义如下：
 * VALUE        节点的实值类型
 * KEY          节点的键值类型
 * HASHFUNC     hash function的函数类型（函数对象/仿函数）
 * EXTRACTKEY   从节点中取出键值的方法（函数对象/仿函数）
 * EQUALKEY     判断键值相同与否的方法（函数对象/仿函数）
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
    // 节点类型
    typedef Fast_Hashtable_Node<VALUE> hash_node;

public:
    // 容器使用的缺省的内存分配器
    typedef FAST_ALLOCATOR_TYPE(hash_node) allocator_type;
    typedef FAST_DEFAULT_ALLOCATOR(hash_node) default_allocator;
    allocator_type* get_allocator() const { return _allocator; }
private:
    // 分配和释放节点内存的函数
    typedef FAST_ALLOCATOR_TYPE(hash_node) _node_allocator_type;
    hash_node* _get_node() { return (hash_node*)_allocator->allocate(); }
    void _put_node(hash_node* p) { _allocator->deallocate(p); }

private:
    allocator_type*        _allocator;          // 内存分配器
    BOOL                   _release_allocator;  // 析构时是否需要释放内存分配器
    hasher                 _hash;               // HASH算法函数类
    key_equal              _equals;             // 比较KEY函数类
    EXTRACTKEY             _get_key;            // 获取KEY函数类
    FastVector<hash_node*> _buckets;            // 存储 buckets 的容器
    size_type              _num_elements;       // 当前元素节点的数目

public:
    // 迭代器，目前只支持这一种，足够了
    typedef Fast_Hashtable_Iterator<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>
            iterator;
    friend class Fast_Hashtable_Iterator<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>;

public:
    // 构造函数 Constructor
    Fast_Hashtable(size_type    n,              // 初始化 buckets 的数目
            const HASHFUNC&     hf,             // HASH算法函数类
            const EQUALKEY&     eql,            // 比较KEY函数类
            const EXTRACTKEY&   ext,            // 获取KEY函数类
            allocator_type*     a = 0);         // 内存分配器 =0 自己创建

    // 构造函数 Constructor
    Fast_Hashtable(size_type    n,              // 初始化 buckets 的数目
            const HASHFUNC&     hf,             // HASH算法函数类
            const EQUALKEY&     eql,            // 比较KEY函数类
            allocator_type*     a = 0);         // 内存分配器 =0 自己创建

    // ASSIGN构造函数 Constructor
    Fast_Hashtable(const Fast_Hashtable& ht,    // 另一个HASH容器
            allocator_type*     a = 0);         // 内存分配器 =0 自己创建

    // ASSIGN运算符函数
    Fast_Hashtable& operator= (const Fast_Hashtable& ht); 

    // 析构函数 Desctructor
    ~Fast_Hashtable(); 

    // 获取元素个数
    size_type size() const { return _num_elements; }

    // 最大元素个数，实际是 -1 表示无穷大
    size_type max_size() const { return size_type(-1); }

    // 判断容器是否为空
    BOOL empty() const { return size() == 0 ? TRUE : FALSE; }

    // 交换两个容器对象的数据元素
    void swap(Fast_Hashtable& ht); 

    // 获取起始迭代器
    iterator begin(); 

    // 获取结束迭代器，以空的迭代器表示结尾
    iterator end() { return iterator(0, this); }

    //friend int operator==(const hashtable&, const hashtable&);

public:

    // 表格中 buckets 的数目
    size_type bucket_count() const { return _buckets.size(); }

    // 表格中最大容纳 buckets 的数目
    size_type max_bucket_count() const
        { return FASTMAP_PRIME_LIST[(int)FASTMAP_NUM_PRIMES - 1]; } 

    // 获取某个 bucket 中元素的个数函数
    size_type elems_in_bucket(size_type bucket) const; 

    // 插入新元素，不允许重复
    Fast_Pair<iterator, BOOL> insert_unique(const value_type& obj); 

    // 插入新元素，允许重复
    iterator insert_equal(const value_type& obj); 

    // 插入新元素，不允许重复，不重建表格
    Fast_Pair<iterator, BOOL> insert_unique_noresize(const value_type& obj);

    // 插入新元素，允许重复，不重建表格
    iterator insert_equal_noresize(const value_type& obj);
 
    // 插入一批新元素，不允许重复
    void insert_unique(const value_type* f, const value_type* l); 

    // 插入一批新元素，允许重复
    void insert_equal(const value_type* f, const value_type* l); 

    // 查找元素，如果不存在就插入
    reference find_or_insert(const value_type& obj);

    // 搜索元素
    iterator find(const key_type& key); 

    // 统计相同健值的元素个数
    size_type count(const key_type& key) const; 

    // 查找相等元素的起始位置
    Fast_Pair<iterator, iterator> equal_range(const key_type& key);

    // 删除指定键值元素
    size_type erase(const key_type& key);

    // 删除迭代器指向的元素
    void erase(const iterator& it);

    // 删除起始迭代器指向的一批元素
    void erase(iterator first, iterator last);

    // 重建表格，扩容时使用
    void resize(size_type num_elements_hint);

    // 清除表格及所有元素，但保持表格大小
    void clear();

    // Dump the state of an object.
    void dump() const;

private:
    // 获取下一个最接近的质数以扩容并重建表格
    size_type _next_size(size_type n) const
        { return FastMap_Next_Prime(n); }

    // 初始化 buckets vector
    void _initialize_buckets(size_type n); 

    // 以下几个函数用于判断元素的落脚处
    // 最后调用hash functions，取得一个可以执行modulus（取模）运算的数值
    // 有些元素类型无法直接拿来对hashtable的大小进行取模运算，例如字符串const char *
    // 这时候需要做一些转换。实际最后调用Fast_Hash函数对象实现，这个模板类
    // 对几种常见的类型做了特化，以实现获取其hash值。

    // 判断元素的落脚处 版本1 只接受健值
    size_type _bkt_num_key(const key_type& key) const
        { return _bkt_num_key(key, _buckets.size()); }

    // 判断元素的落脚处 版本2 只接受实值 value
    size_type _bkt_num(const value_type& obj) const
        { return _bkt_num_key(_get_key(obj)); }

    // 判断元素的落脚处 版本3 接受健值 和 buckets 个数
    size_type _bkt_num_key(const key_type& key, size_t n) const
        { return _hash(key) % n; }

    // 判断元素的落脚处 版本4 接受实值 value 和 buckets 个数
    size_type _bkt_num(const value_type& obj, size_t n) const
        { return _bkt_num_key(_get_key(obj), n); }

    // 节点配置器 分配一个节点
    hash_node* _new_node(const value_type& obj); 

    // 节点配置器 释放一个节点
    void _delete_node(hash_node* n); 

    // 删除 bucket 中 first 到 last 指向范围之间的元素
    void _erase_bucket(const size_type n, hash_node* first, hash_node* last);

    // 删除 bucket 中起始到 last 指向范围之间的元素
    void _erase_bucket(const size_type n, hash_node* last);

    // 从另一个容器复制所有元素过来
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
    Hashtable _ht;  // 底层机制以 hash table 完成

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
    // 缺省使用大小为100的表格,将由hash table调整为最接近且较大之质数
    Fast_HashMap() : _ht(100, hasher(), key_equal()) {}
    explicit Fast_HashMap(size_type n) : _ht(n, hasher(), key_equal()) {}
    Fast_HashMap(size_type n, const hasher& hf) : _ht(n, hf, key_equal()) {}
    Fast_HashMap(size_type n, const hasher& hf, const key_equal& eql)
        : _ht(n, hf, eql) {}

    // 以下,插入操作全部使用 insert_unique(),不允许键重复
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
    // 所有操作几乎都有hash table对应版本,传递调用就行
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
 * 迭代器前进函数 operator++()
 * 其前进操作是首先尝试从目前所指的节点出发，前进一个位置/节点，由于节点被安置于list
 * 内，所以利用节点的next指针即可轻易完成前进操作。如果目前节点正好是list的尾端，就
 * 跳至下一个bucket身上，那正是指向下一个list的头部节点。
 *
 * @return      指向下一个元素的迭代器
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY>
Fast_Hashtable_Iterator<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>&
Fast_Hashtable_Iterator<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::operator++()
{
    const hash_node* old = _cur; 
    // 如果存在，就是它，否则进入以下 if 流程
    _cur = _cur->_next;
    if( !_cur ) 
    {
        // 根据元素值，定位出下一个 bucket，其起头处就是目的地
        size_type bucket = _ht->_bkt_num(old->_val);
        while( !_cur && ++bucket < _ht->_buckets.size() ) 
            _cur = _ht->_buckets[bucket];
    }
    return *this;
}

/**
 * 迭代器前进函数 operator++(int)
 * 实际是调用上面的前进操作函数operator++()
 *
 * @return      指向下一个元素的迭代器
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
 * 迭代器起始函数 first()
 * 将迭代器指向容器中第一个元素
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
 * 迭代器取值函数 next()
 * 先判断是否已指向末尾，如果是则直接返回 FALSE 表示迭代器已遍历了全部元素。
 * 否则获取当前迭代器指向的元素，并将迭代器指向下一个元素
 *
 * @param next_item     获取当前元素值，只是传引用
 * @return              是否已走到末尾
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
 * 迭代器前进函数 advance()
 * 相当于调用上面的前进操作函数operator++()，不同的是做了很多判断处理
 * 如是否已走到末尾，迭代器是否空等，更安全
 *
 * @return      是否已走到末尾
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
 * 迭代器指向末位判断函数 done()
 * 判断当前迭代器是否已走到末尾，是返回 TRUE，否则返回 FALSE
 *
 * @return      是否已走到末尾
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
 * 构造函数 Fast_Hashtable()
 *
 * @param n     初始化 buckets 容器的大小
 * @param hf    HASH算法函数类
 * @param eql   比较KEY函数类
 * @param ext   获取KEY函数类
 * @param a     内存分配器 allocator
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> inline
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>
    ::Fast_Hashtable(size_type    n,
              const HASHFUNC&     hf,
              const EQUALKEY&     eql,
              const EXTRACTKEY&   ext,
              allocator_type*     a)
: _allocator(a),                    // 缺省使用自己的 allocator
  _release_allocator(FALSE),        // 初始化其他内部成员
  _hash(hf),
  _equals(eql),
  _get_key(ext),
  _buckets(),
  _num_elements(0)
{
    if( _allocator == 0 ) 
    {
        // 缺省自己创建内存分配器 allocator
        _allocator = (allocator_type *) new default_allocator(); 
        _release_allocator = TRUE; 
    }
    _initialize_buckets(n);         // 初始化 n 个 buckets
}

/**
 * 构造函数 Fast_Hashtable()
 *
 * @param n     初始化 buckets 容器的大小
 * @param hf    HASH算法函数类
 * @param eql   比较KEY函数类
 * @param a     内存分配器 allocator
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> inline
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>
    ::Fast_Hashtable(size_type    n,
              const HASHFUNC&     hf,
              const EQUALKEY&     eql,
              allocator_type*     a)
: _allocator(a),                    // 缺省使用自己的 allocator
  _release_allocator(FALSE),        // 初始化其他内部成员
  _hash(hf),
  _equals(eql),
  _get_key(EXTRACTKEY()),
  _buckets(),
  _num_elements(0)
{
    if( _allocator == 0 ) 
    {
        // 缺省自己创建内存分配器 allocator
        _allocator = (allocator_type *) new default_allocator(); 
        _release_allocator = TRUE; 
    }
    _initialize_buckets(n);         // 初始化 n 个 buckets
}

/**
 * ASSIGN构造函数 Fast_Hashtable()
 *
 * @param ht    另一个HASH容器
 * @param a     内存分配器 allocator
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> inline
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>
    ::Fast_Hashtable(const Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>& ht, 
                     allocator_type* a)
: _allocator(a),                    // 缺省使用自己的 allocator
  _release_allocator(FALSE),        // 初始化其他内部成员
  _hash(ht._hash),
  _equals(ht._equals),
  _get_key(ht._get_key),
  _buckets(),
  _num_elements(0)
{
    if( _allocator == 0 ) 
    {
        // 缺省自己创建内存分配器 allocator
        _allocator = (allocator_type *) new default_allocator(); 
        _release_allocator = TRUE; 
    }
    _copy_from(ht);               // 将 ht 的所有元素复制过来
}

/**
 * ASSIGN运算符函数 operator=()
 *
 * @param ht    另一个HASH容器
 * @return      容器自己
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> inline
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>& 
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>
    ::operator= (const Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>& ht)
{
    if( &ht != this )               // 不允许自己付值给自己
    {
        clear();                    // 先清除所有元素节点，但保持 _buckets 的大小
        _hash = ht._hash;           // HASH 算法函数类
        _equals = ht._equals;       // 比较KEY函数类
        _get_key = ht._get_key;     // 获取KEY函数类
        _copy_from(ht);             // 将 ht 的所有元素复制过来
    }
    return *this;
}

/**
 * 析构函数 ~Fast_Hashtable()
 * 删除所有元素节点，释放所有内存。注意 allocator 的释放
 *
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> inline
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::~Fast_Hashtable() 
{ 
    this->clear(); 
    if( _release_allocator == TRUE ) 
        FAST_DELETE(_allocator, allocator_type); 
    // 如果需要释放内存分配器，要释放 allocator
}

/**
 * 获取起始迭代器函数 begin()
 * 需要先在 buckets 中找到第一个元素的 bucket
 *
 * @return      指向第一个元素的迭代器
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> inline
typename Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::iterator 
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::begin()
{ 
    // 从表格中第一个 bucket 找出第一个含有节点的 bucket
    for( size_type n = 0; n < _buckets.size(); ++n )
    {
        // 如果 bucket 不为 0 则不为空，即为第一个节点
        if( _buckets[n] ) 
            return iterator(_buckets[n], this);
    }
    // 找不到返回指向末尾的空迭代器
    return end();
}

/**
 * 获取某个 bucket 中元素的个数函数 begin()
 * 需要遍历 bucket 指向的链表以获得 bucket 中元素的个数
 *
 * @param bucket    第几个 bucket
 * @return          元素的个数
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> inline
typename Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::size_type 
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::elems_in_bucket(size_type bucket) const
{
    size_type result = 0;
    for( hash_node* cur = _buckets[bucket]; cur; cur = cur->_next)
        result += 1;    // 走过 bucket 的链表所有元素，计数累加
    return result;
}

/**
 * 插入新元素，不允许重复 insert_unique() 
 *
 * @param obj   插入数据对象
 * @return      指向新元素的迭代器
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> inline
Fast_Pair<typename Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::iterator, BOOL> 
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::insert_unique(const value_type& obj)
{
    // 判断是否需要重建表格，如需要就扩充
    resize(_num_elements + 1);
    return insert_unique_noresize(obj);
}

/**
 * 插入新元素，允许重复 insert_equal()
 *
 * @param obj   插入数据对象
 * @return      指向新元素的迭代器
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> inline
typename Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::iterator 
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::insert_equal(const value_type& obj)
{
    // 判断是否需要重建表格，如需要就扩充
    resize(_num_elements + 1);
    return insert_equal_noresize(obj);
}

/**
 * 插入不重复的一批新元素函数 insert_unique()
 * 插入一批新元素，健值不允许重复
 *
 * @param f     指向起始新元素的指针
 * @param l     指向结尾新元素的指针
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> inline
void Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>
    ::insert_unique(const value_type* f, const value_type* l)
{
    // 计算新元素数目
    size_type n = l - f;
    // 判断是否需要重建表格，如需要就扩充
    resize(_num_elements + n);
    // 依次插入
    for( ; n > 0; --n, ++f )
        insert_unique_noresize(*f);
}

/**
 * 插入可重复的一批新元素函数 insert_equal()
 * 插入一批新元素，健值允许重复
 *
 * @param f     指向起始新元素的指针
 * @param l     指向结尾新元素的指针
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> inline
void Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>
    ::insert_equal(const value_type* f, const value_type* l)
{
    // 计算新元素数目
    size_type n = l - f;
    // 判断是否需要重建表格，如需要就扩充
    resize(_num_elements + n);
    // 依次插入
    for( ; n > 0; --n, ++f )
        insert_equal_noresize(*f);
}

/**
 * 搜索元素函数 find()
 *
 * @param key   元素的键值
 * @return      指向元素的迭代器
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> 
typename Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::iterator 
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::find(const key_type& key) 
{
    // 首先寻找落在哪一个 bucket 内
    size_type n = _bkt_num_key(key);
    hash_node* first = NULL;
    // 以下，从 bucket list 的头开始，一一比较每一个元素的键值
    // 相等就跳出
    for( first = _buckets[n];
            first && !_equals(_get_key(first->_val), key);
            first = first->_next)
    {}
    return iterator(first, this);
} 

/**
 * 统计相同健值的元素个数函数 count()
 *
 * @param key   元素的键值
 * @return      指向元素的迭代器
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> 
typename Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::size_type 
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::count(const key_type& key) const
{
    // 首先寻找落在哪一个 bucket 内
    const size_type n = _bkt_num_key(key);
    size_type result = 0;
    // 以下，从 bucket list 的头开始，一一比较每一个元素的键值
    // 相等就累加 1
    for( const hash_node* cur = _buckets[n]; cur; cur = cur->_next )
    {
        if( _equals(_get_key(cur->_val), key) )
            ++ result;
    }
    return result;
}

/**
 * 初始化 buckets vector
 *
 * @param n     表格中 buckets 的数目
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> inline
void Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::_initialize_buckets(size_type n)
{
    // 获取大于指定数目n的最小质数
    const size_type n_buckets = _next_size(n);
    // 调整 buckets vector的容量大小
    _buckets.resize(n_buckets);
    // 下面调用第一参数是插入的位置，即从 buckets 最后一个位置插入
    // 第二个参数是插入元素的个数，第三个参数是空值，即下面是初始化 buckets
    _buckets.insert(_buckets.size(), (hash_node*) 0, n_buckets);
    _num_elements = 0;
}

/**
 * 节点配置器 分配一个节点
 *
 * @param obj   初始化节点的值
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> inline
typename Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::hash_node* 
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::_new_node(const value_type& obj)
{
    hash_node* n = _get_node();                         // 分配内存
    n->_next = 0;
    _FAST::fast_construct<value_type>(&n->_val, obj);   // 调用VALUE构造子
    return n;
}

/**
 * 节点配置器 释放一个节点
 *
 * @param n     指向节点指针
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY> inline
void Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::_delete_node(hash_node* n)
{
    _FAST::fast_destroy<value_type>(&n->_val);          // 调用VALUE析构子
    _put_node(n);                                       // 释放指针
}

/**
 * 插入唯一的新元素函数 insert_unique_noresize()
 * 在不需要重建表格的情况下插入新节点，健值不允许重复
 *
 * @param obj   新元素对象
 * @return      指向新元素的迭代器
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY>
Fast_Pair<typename Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::iterator, BOOL> 
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::insert_unique_noresize(const value_type& obj)
{
    // 定位 obj 应位于 #n bucket
    const size_type n = _bkt_num(obj);
    // 令 first 指向 bucket 对应之串行头部
    hash_node* first = _buckets[n];

    // 如果 buckets[n] 已被占用，此时first将不为0，于是进入以下循环
    // 走过bucket所对应的整个链表
    for( hash_node* cur = first; cur; cur = cur->_next ) 
    {
        if( _equals(_get_key(cur->_val), _get_key(obj)) ) 
            // 如果发现与链表中的某键值相同，就不插入，立即返回
            return Fast_Pair<iterator, BOOL>(iterator(cur, this), FALSE);
    }

    // 离开以上循环或根本未进入循环时，first指向bucket所指向链表的头部节点
    hash_node* tmp = _new_node(obj);    // 产生新节点
    tmp->_next = first;
    _buckets[n] = tmp;                  // 令新节点成为链表的第一个节点
    ++_num_elements;                    // 节点个数累加 1

    return Fast_Pair<iterator, BOOL>(iterator(tmp, this), TRUE);
}

/**
 * 插入允许重复的新元素函数 insert_equal_noresize()
 * 在不需要重建表格的情况下插入新节点，健值允许重复
 *
 * @param obj   新元素对象
 * @return      指向新元素的迭代器
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY>
typename Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::iterator 
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::insert_equal_noresize(const value_type& obj)
{
    // 定位 obj 应位于 #n bucket
    const size_type n = _bkt_num(obj);
    // 令 first 指向 bucket 对应之串行头部
    hash_node* first = _buckets[n];

    // 如果 buckets[n] 已被占用，此时first将不为0，于是进入以下循环
    // 走过bucket所对应的整个链表
    for( hash_node* cur = first; cur; cur = cur->_next ) 
    {
        if( _equals(_get_key(cur->_val), _get_key(obj)) ) 
        {
            // 如果发现与链表中的某键值相同，就马上插入，然后返回
            hash_node* tmp = _new_node(obj);    // 产生新节点
            tmp->_next = cur->_next;            // 将新节点插入目前位置
            cur->_next = tmp;
            ++ _num_elements;                   // 节点个数累加 1
            return iterator(tmp, this);         // 返回一个迭代器，指向新增节点
        }
    }

    // 运行到此，表示没有发现重复的键值
    hash_node* tmp = _new_node(obj);            // 产生新节点
    tmp->_next = first;                         // 将新节点插入链表头部
    _buckets[n] = tmp;
    ++ _num_elements;                           // 节点个数累加 1
    return iterator(tmp, this);                 // 返回一个迭代器，指向新增节点
}

/**
 * 查找或插入新元素函数 find_or_insert()
 * 先查找指定健值的元素，如果存在则返回对应的VALUE值，如果不存在则插入此新值
 *
 * @param obj   查找元素对象
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY>
typename Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::reference 
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::find_or_insert(const value_type& obj)
{
    // 调整 buckets vector的容量大小为原容量加1以插入新值
    resize(_num_elements + 1);

    // 定位 obj 应位于 #n bucket
    size_type n = _bkt_num(obj);
    // 令 first 指向bucket 对应之串行头部
    hash_node* first = _buckets[n];

    // 如果 buckets[n] 已被占用，此时first将不为0，于是进入以下循环
    // 走过bucket所对应的整个链表
    for( hash_node* cur = first; cur; cur = cur->_next )
    {
        // 如果发现与链表中的某键值相同，就马上返回其对应值
        if( _equals(_get_key(cur->_val), _get_key(obj)) )
            return cur->_val;
    }

    // 运行到此，表示没有发现相等的键值
    hash_node* tmp = _new_node(obj);        // 产生新节点
    tmp->_next = first;                     // 将新节点插入链表头部
    _buckets[n] = tmp;
    ++ _num_elements;                       // 节点个数累加 1
    return tmp->_val;                       // 返回新增节点对应的值
}

/**
 * 查找相等元素的起始位置函数 equal_range()
 * 先查找指定健值的元素，再查找与之相等的最后一个元素的位置
 * 用于查找键值可重复的元素范围(begin, end]，包含起始位置begin
 * 但不包含结束位置end
 *
 * @param key   查找元素的键值
 * @return      返回指向相等的连续元素范围的起始位置的迭代器
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY>
Fast_Pair<typename Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::iterator,
          typename Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::iterator> 
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::equal_range(const key_type& key)
{
    // 定义迭代器组
    typedef Fast_Pair<iterator, iterator> Pii;
    // 定位 key 应位于 #n bucket
    const size_type n = _bkt_num_key(key);

    // 走过bucket所对应的整个链表，查找键值等于 key 的元素
    for( hash_node* first = _buckets[n]; first; first = first->_next )
    {
        if( _equals(_get_key(first->_val), key) ) 
        {
            // 如果相等，则此当前元素 cur 在起始位置，继续链表的下一个
            // 找到键值不等于 key 的元素，即为结束位置
            for( hash_node* cur = first->_next; cur; cur = cur->_next ) 
                if( !_equals(_get_key(cur->_val), key) )
                    return Pii(iterator(first, this), iterator(cur, this));

            // 如果上面的链表没找到，即起始位置后全是相等的元素或者只有一个
            // 则继续查找下一个 bucket 的第一个元素，即为结束位置
            for( size_type m = n + 1; m < _buckets.size(); ++ m )
                if( _buckets[m] )
                    return Pii(iterator(first, this), iterator(_buckets[m], this));

            // 没找到下一个 bucket，即 end() 为结束位置
            return Pii(iterator(first, this), end());
        }
    }

    // 什么也没找到
    return Pii(end(), end());
}

/**
 * 删除指定键值的所有元素函数 erase()
 * 先查找指定健值的元素，再查找与之相等的下一个元素，依次删除
 *
 * @param key   删除元素的键值
 * @return      返回删除的元素数目
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY>
typename Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::size_type 
Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::erase(const key_type& key)
{
    // 定位 key 应位于 #n bucket
    const size_type n = _bkt_num_key(key);
    hash_node* first = _buckets[n];         // 指向 bucket 里链表第一个元素
    size_type erased = 0;

    if( first ) 
    {
        // 遍历链表里所有元素
        hash_node* cur = first;
        hash_node* next = cur->_next;
        while( next ) 
        {
            // 找出键值等于 key 的元素，删除之
            if( _equals(_get_key(next->_val), key) ) 
            {
                cur->_next = next->_next;   // 断开指向此元素
                _delete_node(next);         // 释放节点占用的内存
                next = cur->_next;          // 指向下一个节点，继续比较
                ++ erased;                  // 删除计数累加 1
                -- _num_elements;           // 元素计数减 1
            }
            else 
            {
                cur = next;
                next = cur->_next;          // 指向下一个节点，继续比较
            }
        }
        // 由于此 bucket 中第一个节点还未比较，所以还需检查一下 first
        if( _equals(_get_key(first->_val), key) ) 
        {
            _buckets[n] = first->_next;     // bucket 指向下一个以断开 first
            _delete_node(first);            // 释放节点占用的内存
            ++ erased;                      // 删除计数累加 1
            -- _num_elements;               // 元素计数减 1
        }
    }

    return erased;
}

/**
 * 删除迭代器指向的元素函数 erase()
 * 先找到迭代器指向的元素所在的 bucket，再查找与迭代器对应的元素并删除
 *
 * @param it    删除元素的迭代器
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY>
void Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>
    ::erase(const iterator& it)
{
    // 获取指向节点的指针
    hash_node* p = it._cur;
    if( p ) 
    {
        // 定位 p 应位于 #n bucket
        const size_type n = _bkt_num(p->_val);
        hash_node* cur = _buckets[n];

        // 如果此 bucket 的链表中的第一个节点就是，删除之
        if( cur == p ) 
        {
            _buckets[n] = cur->_next;           // bucket 指向下一个以断开它
            _delete_node(cur);                  // 释放节点占用的内存
            -- _num_elements;                   // 元素计数减 1
        }
        else 
        {
            // 如果不是，依次遍历余下的节点
            hash_node* next = cur->_next;
            while( next ) 
            {
                if( next == p )                 // 判断是否目标节点：是，删除之
                {
                    cur->_next = next->_next;   // 指向下一个以断开它
                    _delete_node(next);         // 释放节点占用的内存
                    -- _num_elements;           // 元素计数减 1
                    break;
                }
                else                            // 不是，继续..
                {
                    cur = next;
                    next = cur->_next;
                }
            }
        }
    }
}

/**
 * 删除两迭代器指向范围之间的元素函数 erase()
 * 先找到 first 迭代器指向的元素节点，依次删除至 last 迭代器指向的节点
 *
 * @param first   起始元素的迭代器
 * @param last    结束元素的迭代器
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY>
void Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>
    ::erase(iterator first, iterator last)
{
    // 获取 first 和 last 指向的 bucket #n，如果值为空则指向表格最后的空 bucket
    size_type f_bucket = first._cur ? _bkt_num(first._cur->_val) : _buckets.size();
    size_type l_bucket = last._cur  ? _bkt_num(last._cur->_val)  : _buckets.size();

    // 如果两迭代器相等或者两个不存在，直接返回不做处理
    if( first._cur == last._cur )
        return;
    // 如果两迭代器指向元素节点在同一 bucket 里
    else if( f_bucket == l_bucket ) 
        // 删除 bucket 的链表里 first 到 last 之间的节点
        _erase_bucket(f_bucket, first._cur, last._cur);
    else 
    {
        // 如果两迭代器指向元素节点在不同 bucket 里
        // 删除 first bucket 的链表里 first 到 末尾 之间的节点
        _erase_bucket(f_bucket, first._cur, 0);
        // 依次访问 fist bucket 到 last bucket 之间的 buckets（但不包括last）
        for( size_type n = f_bucket + 1; n < l_bucket; ++ n ) 
            // 删除 bucket 中所有节点
            _erase_bucket(n, 0);
        // 如果 last bucket 存在
        if( l_bucket != _buckets.size() )
            // 删除 last bucket 的链表里 起始 到 last 之间的节点
            _erase_bucket(l_bucket, last._cur);
    }
}

/**
 * 重建表格函数 resize()
 * 以下函数判断是否需要重建表格。如果不需要，立即返回。如果需要，就动手……
 *
 * @param num_elements_hint 计入新增数后的表格存放元素的个数
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY>
void Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>
    ::resize(size_type num_elements_hint)
{
    // 以下，“表格重建与否”的判断原则是拿元素个数（把新增元素计入后）
    // 和 bucket vector 的大小比，如果前者大于后者，就重建表格
    // 每个 bucket（list） 的最大容量和buckets vector的大小相同
    const size_type old_n = _buckets.size();

    // 确定真的需要重新配置
    if( num_elements_hint > old_n ) 
    {
        // 找出下一个质数 
        const size_type n = _next_size(num_elements_hint);
        if( n > old_n ) 
        {
            // 建立新的buckets
            FastVector<hash_node*> tmp(n, (hash_node*)(0));

            // 以下处理每一个旧的bucket
            for( size_type bucket = 0; bucket < old_n; ++ bucket ) 
            {
                // 指向节点所对应之串行的起始节点
                hash_node* first = _buckets[bucket];
                // 以下处理每一个旧bucket所含（串行）的每一个节点，都是指针操作
                while( first )  // 串行还没有结束时
                {
                    // 以下找出节点落在哪一个新bucket内
                    size_type new_bucket = _bkt_num(first->_val, n); 
                    // 令旧bucket指向其所对应之串行的下一个节点，以便迭代处理
                    _buckets[bucket] = first->_next;
                    // 将当前节点插入到新bucket内，成为其对应串行的第一个节点
                    first->_next = tmp[new_bucket];
                    tmp[new_bucket] = first;
                    // 回到旧bucket所指的待处理串行，准备处理下一节点
                    first = _buckets[bucket];          
                }
            }
            // FastVector::swap 新旧两个buckets对调
            _buckets.swap(tmp);
            // 注：对调两方如果大小不同，大的会变小，小的会变大
            // 都是交换vector内部元素的值，不重新复制构造对象，相当于指针切换，效率很高
            // 离开时释放local tmp的内存
        }
    }
}

/**
 * 删除bucket中指向范围之间的元素函数 _erase_bucket()
 * 先找到 first 指向的元素节点，依次删除至 last 指向的节点 （不包括 last）
 *
 * @param n       第几个 bucket
 * @param first   起始元素节点的指针
 * @param last    结束元素节点的指针
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY>
void Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>
    ::_erase_bucket(const size_type n, hash_node* first, hash_node* last)
{
    hash_node* cur = _buckets[n];           // 获取 bucket 链表中的起始指针
    if( cur == first )
        _erase_bucket(n, last);             // 如果指针相等，删除从起始位置到last的节点
    else
    {
        hash_node* next = 0;
        for( next = cur->_next; next != first; cur = next, next = cur->_next )
            ;                               // 依次遍历直至 first 节点
        while( next != last )               // 依次删除节点至 last 节点 （不删除 last）
        {
            cur->_next = next->_next;       // 指向下一个以断开它
            _delete_node(next);             // 释放节点占用的内存
            next = cur->_next;              // 指向下一个，继续
            -- _num_elements;               // 元素计数减 1
        }
    }
}

/**
 * 删除bucket中指向范围之间的元素函数 _erase_bucket()
 * 依次删除 bucket 中起始节点至 last 指向的节点 （不包括 last）
 *
 * @param n       第几个 bucket
 * @param last    结束元素节点的指针
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY>
void Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>
    ::_erase_bucket(const size_type n, hash_node* last)
{
    hash_node* cur = _buckets[n];
    while( cur != last )                    // 依次删除节点至 last 节点 （不删除 last）
    {
        hash_node* next = cur->_next;       // 指向下一个以断开它
        _delete_node(cur);                  // 释放节点占用的内存
        cur = next;                         // 指向下一个，继续
        _buckets[n] = cur;                  // 令 bucket 指向此新的起始节点
        -- _num_elements;                   // 元素计数减 1
    }
}

/**
 * 整体删除表格中所有节点函数 clear()
 * 由于整个hashtable 由 vector 和 linked-list 组合而成，因此，复制和整体删除
 * 都需要特别注意内存的释放问题
 *
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY>
void Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>::clear()
{
    // 针对每一个 bucket
    for( size_type i = 0; i < _buckets.size(); ++ i ) 
    {
        hash_node* cur = _buckets[i];
        // 将 bucket list 中的每一个节点删除掉
        while( cur != 0 ) 
        {
            hash_node* next = cur->_next;   // 指向下一个以断开它
            _delete_node(cur);              // 释放节点占用的内存
            cur = next;                     // 指向下一个，继续
        }
        _buckets[i] = 0;                    // 令 bucket 内容为 null 指针
    }
    _num_elements = 0;                      // 令总节点个数为 0
    // 注，buckets vector 并未释放掉空间，仍保有原来大小
}

/**
 * 复制函数 copy_from()
 * 由于整个hashtable 由 vector 和 linked-list 组合而成，因此，复制和整体删除
 * 都需要特别注意内存的释放问题
 *
 * @param ht    被复制的hashtable对象
 */
template<class VALUE, class KEY, class HASHFUNC,
         class EXTRACTKEY, class EQUALKEY>
void Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>
    ::_copy_from(const Fast_Hashtable<VALUE,KEY,HASHFUNC,EXTRACTKEY,EQUALKEY>& ht)
{
    // 先清除己方的 buckets vector，这操作是调用vector::clear，造成所有元素为 0
    _buckets.clear();

    // 为己方的 buckets vector 保留空间，使与对方相同
    // 如果己方空间大于对方，就不动，如果己方空间小于对方，就会增大
    // 这些都是 vector 自身的处理方式
    _buckets.reserve(ht._buckets.size());

    // 下面调用第一参数是插入的位置，由于 buckets已经清空了，所以是 0，即从
    // 第一个位置插入，第二个参数是插入元素的个数，实际即复制的对象的 buckets
    // 中元数的个数，第三个参数是空值，即下面是初始化 buckets
    _buckets.insert(_buckets.size(), (hash_node*)(0), ht._buckets.size());

    // 针对 buckets vector
    for( size_type i = 0; i < ht._buckets.size(); ++ i ) 
    {
        // 复制 vector 的每一个元素,是个指针，指向 hashtable 节点
        const hash_node* cur = ht._buckets[i];
        if( cur ) 
        {
            hash_node* copy = _new_node(cur->_val);
            _buckets[i] = copy;

            // 针对同一个 bucket list，复制每一个节点
            for( hash_node* next = cur->_next; next; 
                 cur = next, next = cur->_next ) 
            {
                copy->_next = _new_node(next->_val);
                copy = copy->_next;
            }
        }
    }
    // 重新更新节点个数（hashtable的大小）
    _num_elements = ht._num_elements;
}

/**
 * 交换hashmap表格函数 swap()
 * 这是一个全局swap函数重载，可实现交换两个不同模板参数类型的容器
 * 实际是调用hashmap.swap()实现，交换后两个表格所有数据都对调，实际
 * 是类似指针对象，效率很高，不用创建巨大的临时容器对象
 *
 * @param ht   第二个hashmap表格对象
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
 * 交换hashmap表格函数 swap()
 * 这是一个全局swap函数重载，可实现交换两个不同模板参数类型的容器
 * 实际是调用hashmap.swap()实现，交换后两个表格所有数据都对调，实际
 * 是类似指针对象，效率很高，不用创建巨大的临时容器对象
 *
 * @param hm1   第一个hashmap表格对象
 * @param hm2   第二个hashmap表格对象
 */
template<class KEY, class TP, class HASHFUNC, class EQUALKEY>
inline void swap(Fast_HashMap<KEY,TP,HASHFUNC,EQUALKEY>& hm1,
                 Fast_HashMap<KEY,TP,HASHFUNC,EQUALKEY>& hm2)
{
    hm1.swap(hm2);
}

/**
 * 跟踪对象数据函数 dump()
 * 将hash table的 buckets及其指向节点链表打印出来，用于调试跟踪
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
 * 获取指定键值的元素值函数 get()
 * 先查找指定键值的元素，如果找到取其内容，返回 0；否则返回 -1
 *
 * @param key   键值
 * @param value 获得的元素值
 * @return      成功与否，成功返回 0，否则返回 -1
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
 * 检查元素值是否存在函数 containsValue()
 * 从容器的第一个元素 begin() 依次遍历到最后一个元素 end()，并逐个判断是否相等
 * 如果找到值相等元素，立即返回 TRUE，最后全部遍历完即没找到返回 FALSE。
 *
 * @param value 元素值
 * @return      找到与否，成功返回 TRUE，否则返回 FALSE
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
