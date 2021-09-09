#ifndef __GOOGLETREE_WRAPPER_HPP__
#define __GOOGLETREE_WRAPPER_HPP__

#include "tree_api.hpp"

#include <cstdint>
#include <iostream>
#include <type_traits>
//#include <map>
#include <cstring>
#include <array>
//#include <mutex>
//#include <shared_mutex>
#include "btree_map.h"


//using namespace btree;

template<typename Key, typename T>
class googletree_wrapper : public tree_api
{
public:
    googletree_wrapper();
    virtual ~googletree_wrapper();
    
    virtual bool find(const char* key, size_t key_sz, char* value_out) override;
    virtual bool insert(const char* key, size_t key_sz, const char* value, size_t value_sz) override;
    virtual bool update(const char* key, size_t key_sz, const char* value, size_t value_sz) override;
    virtual bool remove(const char* key, size_t key_sz) override;
    virtual int scan(const char* key, size_t key_sz, int scan_sz, char*& values_out) override;

private:
    btree::btree_map<Key,T> map_;
//    std::shared_mutex mutex_;
};

template<typename Key, typename T>
googletree_wrapper<Key,T>::googletree_wrapper()
{
}

template<typename Key, typename T>
googletree_wrapper<Key,T>::~googletree_wrapper()
{
}

template<typename Key, typename T>
bool googletree_wrapper<Key,T>::find(const char* key, size_t key_sz, char* value_out)
{
//    std::shared_lock lock(mutex_);

    if constexpr (std::is_arithmetic<Key>::value)
    {
        auto it = map_.find(*reinterpret_cast<Key*>(const_cast<char*>(key)));
        if (it == map_.end())
            return false;

        if constexpr (std::is_arithmetic<T>::value)
            memcpy(value_out, &it->second, sizeof(T));
        else
            memcpy(value_out, it->second.c_str(), it->second.size());
    //std::cout << "find result:"  <<   value_out << std::endl;
    }
    else
    {
        auto it = map_.find(std::string(key, key_sz));
        if (it == map_.end())
            return false;

        if constexpr (std::is_arithmetic<T>::value)
            memcpy(value_out, &it->second, sizeof(T));
        else
            memcpy(value_out, it->second.c_str(), it->second.size());
    //std::cout << "find result:"  <<  value_out << std::endl;
    }

    return true;
}


template<typename Key, typename T>
bool googletree_wrapper<Key, T>::insert(const char* key, size_t key_sz, const char* value, size_t value_sz)
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

    bool ret =  map_.insert(std::make_pair(k,v)).second;
   // std::cout << "insert result:"  <<  ret << std::endl;
    return ret;
}

template<typename Key, typename T>
bool googletree_wrapper<Key, T>::update(const char* key, size_t key_sz, const char* value, size_t value_sz)
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
bool googletree_wrapper<Key,T>::remove(const char* key, size_t key_sz)
{
//    std::unique_lock lock(mutex_);

    if constexpr (std::is_arithmetic<Key>::value)
        return map_.erase(*reinterpret_cast<Key*>(const_cast<char*>(key))) == 1;
    else
        return map_.erase(std::string(key, key_sz)) == 1;
}

template<typename Key, typename T>
int googletree_wrapper<Key,T>::scan(const char* key, size_t key_sz, int scan_sz, char*& values_out)
{
//    std::shared_lock lock(mutex_);

    constexpr size_t ONE_MB = 1ULL << 20;
    static thread_local std::array<char, ONE_MB> results;

    int scanned;
    char* dst = reinterpret_cast<char*>(results.data());
    if constexpr (std::is_arithmetic<Key>::value)
    {
        auto it = map_.lower_bound(*reinterpret_cast<Key*>(const_cast<char*>(key)));
        if (it == map_.end())
            return 0;

        for(scanned=0; (scanned < scan_sz) && (it != map_.end()); ++scanned,++it)
        {
            memcpy(dst, &it->first, sizeof(Key));
            dst += sizeof(T);

            if constexpr (std::is_arithmetic<T>::value)
            {
                memcpy(dst, &it->second, sizeof(T));
                dst += sizeof(T);
            }
            else
            {
                memcpy(dst, it->second.c_str(), it->second.size());
                dst += it->second.size();
            }
        }
    }
    else
    {
        auto it = map_.lower_bound(std::string(key, key_sz));
        if (it == map_.end())
            return 0;

        for(scanned=0; (scanned < scan_sz) && (it != map_.end()); ++scanned,++it)
        {
            memcpy(dst, it->first.c_str(), it->first.size());
            dst += sizeof(T);

            if constexpr (std::is_arithmetic<T>::value)
            {
                memcpy(dst, &it->second, sizeof(T));
                dst += sizeof(T);
            }
            else
            {
                memcpy(dst, it->second.c_str(), it->second.size());
                dst += it->second.size();
            }
        }
    }
    values_out = results.data();
    //std::cout << "scan result:"  <<  values_out << std::endl;
	//std::cout << "scan count:"  <<  scanned << std::endl;
    return scanned;
}

#endif
