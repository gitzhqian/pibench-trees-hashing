#ifndef __MASSTREE_WRAPPER_HPP__
#define __MASSTREE_WRAPPER_HPP__

#include "tree_api.hpp"

#include <cstdint>
#include <iostream>
#include <type_traits>
//#include <map>
#include <cstring>
#include <array>
//#include <mutex>
//#include <shared_mutex>
#include "masstree.hh"
#include "query_masstree.hh"
//#include "masstree_tcursor.hh"
//#include "masstree_insert.hh"
//#include "masstree_remove.hh"
//#include "masstree_scan.hh"
#include "mtIndexAPI.hh"

typedef mt_index<Masstree::default_table> MapType;
template<typename Key, typename T>
class masstree_wrapper : public tree_api
{
public:
    masstree_wrapper();
    virtual ~masstree_wrapper();

    virtual bool find(const char* key, size_t key_sz, char* value_out) override;
    virtual bool insert(const char* key, size_t key_sz, const char* value, size_t value_sz) override;
    virtual bool update(const char* key, size_t key_sz, const char* value, size_t value_sz) override;
    virtual bool remove(const char* key, size_t key_sz) override;
    virtual int scan(const char* key, size_t key_sz, int scan_sz, char*& values_out) override;

private:
    MapType *map_;
    threadinfo *ti;
//    std::shared_mutex mutex_;
};

template<typename Key, typename T>
masstree_wrapper<Key,T>::masstree_wrapper()
{
    map_ = new MapType{};

    ti = new threadinfo();
    map_->setup(ti);

    return;
}

template<typename Key, typename T>
masstree_wrapper<Key,T>::~masstree_wrapper()
{
    delete map_;
}

template<typename Key, typename T>
bool masstree_wrapper<Key,T>::find(const char* key, size_t key_sz, char* value_out)
{
//    std::shared_lock lock(mutex_);
//    threadinfo *ti = threadinfo::make(threadinfo::TI_PROCESS, -1);
    Str val;
    bool rt;
    if  (std::is_arithmetic<Key>::value)
    {
//        auto it = map_.g .find(*reinterpret_cast<Key*>(const_cast<char*>(key)));
        rt = map_->get(key, sizeof(key), val, ti);
        if (rt == false)
            return false;

        if  (std::is_arithmetic<T>::value)
            memcpy(value_out, &val.s, sizeof(T));
        else
            memcpy(value_out, val.s, val.length());
    }
    else
    {
        auto it = map_->get(key, sizeof(key), val, ti);
        if (it == false)
            return false;

        if  (std::is_arithmetic<T>::value)
            memcpy(value_out, &val.s, sizeof(T));
        else
            memcpy(value_out, val.s, val.length());
    }
   // std::cout << "find result:"  <<   std::string(val.s, val.len) << std::endl;
    return true;
}


template<typename Key, typename T>
bool masstree_wrapper<Key, T>::insert(const char* key, size_t key_sz, const char* value, size_t value_sz)
{
//    std::unique_lock lock(mutex_);

//    Key k;
//    threadinfo *ti = threadinfo::make(threadinfo::TI_PROCESS, -1);

//    if (std::is_arithmetic<Key>::value)
//        k = *reinterpret_cast<Key*>(const_cast<char*>(key));
//    else
//        k = std::string(key, key_sz);
//
//    T v;
//    if (std::is_arithmetic<T>::value)
//        v = *reinterpret_cast<T*>(const_cast<char*>(value));
//    else
//        v = std::string(value, value_sz);
    bool ret = map_->put_uv(key,key_sz,value, value_sz, ti);
   // std::cout << "insert result:"  <<  ret << std::endl;
    return ret;
}

template<typename Key, typename T>
bool masstree_wrapper<Key, T>::update(const char* key, size_t key_sz, const char* value, size_t value_sz)
{
//    std::unique_lock lock(mutex_);

//    typename std::map<Key,T>::iterator it;
//    if constexpr (std::is_arithmetic<Key>::value)
//        it = map_.find(*reinterpret_cast<Key*>(const_cast<char*>(key)));
//    else
//        it = map_.find(std::string(key, key_sz));
//
//    if (it == map_.end())
//        return false;
//    else
//    {
//        if constexpr (std::is_arithmetic<T>::value)
//            it->second = *reinterpret_cast<T*>(const_cast<char*>(value));
//        else
//            it->second = std::string(value, value_sz);
//
//        return true;
//    }
    return true;
}

template<typename Key, typename T>
bool masstree_wrapper<Key,T>::remove(const char* key, size_t key_sz)
{
//    std::unique_lock lock(mutex_);

//    if (std::is_arithmetic<Key>::value)
//        return map_.erase(*reinterpret_cast<Key*>(const_cast<char*>(key))) == 1;
//    else
//        return map_.erase(std::string(key, key_sz)) == 1;
    return true;
}

template<typename Key, typename T>
int masstree_wrapper<Key,T>::scan(const char* key, size_t key_sz, int scan_sz, char*& values_out)
{
//    std::shared_lock lock(mutex_);

    constexpr size_t ONE_MB = 1ULL << 20;
    static thread_local std::array<char, ONE_MB> results;

    int scanned;
	int return_count;
    char* dst = reinterpret_cast<char*>(results.data());

    Str resultss[scan_sz];
//    threadinfo *ti = threadinfo::make(threadinfo::TI_PROCESS, -1);

    int key_len = sizeof(key);

    return_count = map_->get_next_n(resultss, (char *)&key, &key_len, scan_sz, ti);
    for(scanned = 0;scanned < return_count; scanned++){
        memcpy(dst, resultss[scanned].s, resultss[scanned].len);
        dst += resultss[scanned].len;
    }

    values_out = results.data();
    //std::cout << "scan result:"  <<  values_out << std::endl;
    //std::cout << "scan scan_sz:"  <<  scan_sz << std::endl;
    //std::cout << "scan return_count:"  << return_count << std::endl;
    return scanned;
}

#endif
