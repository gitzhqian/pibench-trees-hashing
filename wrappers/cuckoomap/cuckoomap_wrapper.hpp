#ifndef __CUCKOOMAP_WRAPPER_HPP__
#define __CUCKOOMAP_WRAPPER_HPP__

#include "tree_api.hpp"

#include <cstdint>
#include <iostream>
#include <type_traits>
#include <list>
#include <cstring>
#include <array>
#include <mutex>
#include <shared_mutex>
#include "cuckoohash_map.hh"

using namespace libcuckoo;

template<typename Key, typename T>
class cuckoomap_wrapper : public tree_api
{
public:
    cuckoomap_wrapper();
    virtual ~cuckoomap_wrapper();
    
    virtual bool find(const char* key, size_t key_sz, char* value_out) override;
    virtual bool insert(const char* key, size_t key_sz, const char* value, size_t value_sz) override;
    virtual bool update(const char* key, size_t key_sz, const char* value, size_t value_sz) override;
    virtual bool remove(const char* key, size_t key_sz) override;
    virtual int scan(const char* key, size_t key_sz, int scan_sz, char*& values_out) override;

private:
    libcuckoo::cuckoohash_map<Key,T> map_;
//    std::shared_mutex mutex_;
};

template<typename Key, typename T>
cuckoomap_wrapper<Key,T>::cuckoomap_wrapper()
{
}

template<typename Key, typename T>
cuckoomap_wrapper<Key,T>::~cuckoomap_wrapper()
{
}

template<typename Key, typename T>
bool cuckoomap_wrapper<Key,T>::find(const char* key, size_t key_sz, char* value_out)
{
//    std::shared_lock lock(mutex_);
    T vl;
    if constexpr (std::is_arithmetic<Key>::value)
    {
        auto it = map_.find((*reinterpret_cast<Key*>(const_cast<char*>(key))),vl);
        if (it == false)
            return false;

        if constexpr (std::is_arithmetic<T>::value)
            memcpy(value_out, &vl, sizeof(T));
        else
            memcpy(value_out, vl.c_str(), vl.size());
    }
    else
    {
        auto it = map_.find((std::string(key, key_sz)),vl);
        if (it == false)
            return false;

        if constexpr (std::is_arithmetic<T>::value)
            memcpy(value_out, &vl, sizeof(T));
        else
            memcpy(value_out, vl.c_str(), vl.size());
    }
    //std::cout << "find result:"  <<  value_out << std::endl;
    return true;
}


template<typename Key, typename T>
bool cuckoomap_wrapper<Key, T>::insert(const char* key, size_t key_sz, const char* value, size_t value_sz)
{
//    std::unique_lock lock(mutex_);

    Key k;
    if constexpr (std::is_arithmetic<Key>::value)
        k = *reinterpret_cast<Key*>(const_cast<char*>(key));
    else
        k = std::string(key, key_sz);

    T v;
    if constexpr (std::is_arithmetic<T>::value)
        v = *reinterpret_cast<T*>(const_cast<char*>(value));
    else
        v = std::string(value, value_sz);

    bool ret = map_.insert(k,v);
    //std::cout << "insert result:"  <<  ret << std::endl;
    return ret;
}

template<typename Key, typename T>
bool cuckoomap_wrapper<Key, T>::update(const char* key, size_t key_sz, const char* value, size_t value_sz)
{
//    std::unique_lock lock(mutex_);

//    typename libcuckoo::cuckoohash_map<Key,T>::iterator ;
    bool it;
    T vl;
    if constexpr (std::is_arithmetic<Key>::value)
        it = map_.find((*reinterpret_cast<Key*>(const_cast<char*>(key))),vl);
    else
        it = map_.find((std::string(key, key_sz)),vl);

    if (it == false)
        return false;
    else
    {
//        if constexpr (std::is_arithmetic<T>::value)
//            it->second = *reinterpret_cast<T*>(const_cast<char*>(value));
//        else
//            it->second = std::string(value, value_sz);

        return true;
    }
}

template<typename Key, typename T>
bool cuckoomap_wrapper<Key,T>::remove(const char* key, size_t key_sz)
{
//    std::unique_lock lock(mutex_);

    if constexpr (std::is_arithmetic<Key>::value)
        return map_.erase(*reinterpret_cast<Key*>(const_cast<char*>(key))) == 1;
    else
        return map_.erase(std::string(key, key_sz)) == 1;
}

template<typename Key, typename T>
int cuckoomap_wrapper<Key,T>::scan(const char* key, size_t key_sz, int scan_sz, char*& values_out)
{
//    std::shared_lock lock(mutex_);

//    typename libcuckoo::cuckoohash_map<Key,T>::iterator it;
    T vl;
    constexpr size_t ONE_MB = 1ULL << 20;
    static thread_local std::array<char, ONE_MB> results;

    int scanned;
    char* dst = reinterpret_cast<char*>(results.data());

    for(scanned=0; scanned < scan_sz ; ++scanned)
    {
        T vl;
        if constexpr (std::is_arithmetic<Key>::value)
        {
            auto it = map_.find((*reinterpret_cast<Key*>(const_cast<char*>(key))),vl);

            if (it == false)
                return false;

            if constexpr (std::is_arithmetic<T>::value){
                memcpy(dst, &vl, sizeof(T));
                dst += sizeof(T);
            } else{
                memcpy(dst, vl.c_str(), vl.size());
                dst += vl.size();
            }
        }
        else
        {
            auto it = map_.find((std::string(key, key_sz)),vl);
            if (it == false)
                return false;

            if constexpr (std::is_arithmetic<T>::value){
                memcpy(dst, &vl, sizeof(T));
                dst += sizeof(T);
            } else{
                memcpy(dst, vl.c_str(), vl.size());
                dst += vl.size();
            }
        }
    }

    values_out = results.data();
    //std::cout << "scan result:"  <<  values_out << std::endl;
    return scanned;
}

#endif
