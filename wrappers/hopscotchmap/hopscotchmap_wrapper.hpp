#ifndef __HOPSCOTCHMAP_WRAPPER_HPP__
#define __HOPSCOTCHMAP_WRAPPER_HPP__

#include "tree_api.hpp"

#include <cstdint>
#include <iostream>
#include <type_traits>
//#include <map>
#include <cstring>
#include <array>
//#include <mutex>
//#include <shared_mutex>
#include "hopscotch_map.h"

template<typename Key, typename T>
class hopscotchmap_wrapper : public tree_api
{
public:
    hopscotchmap_wrapper();
    virtual ~hopscotchmap_wrapper();
    
    virtual bool find(const char* key, size_t key_sz, char* value_out) override;
    virtual bool insert(const char* key, size_t key_sz, const char* value, size_t value_sz) override;
    virtual bool update(const char* key, size_t key_sz, const char* value, size_t value_sz) override;
    virtual bool remove(const char* key, size_t key_sz) override;
    virtual int scan(const char* key, size_t key_sz, int scan_sz, char*& values_out) override;

private:
    tsl::hopscotch_map<Key,T> map_;
    //std::shared_mutex mutex_;
};

template<typename Key, typename T>
hopscotchmap_wrapper<Key,T>::hopscotchmap_wrapper()
{
}

template<typename Key, typename T>
hopscotchmap_wrapper<Key,T>::~hopscotchmap_wrapper()
{
}

template<typename Key, typename T>
bool hopscotchmap_wrapper<Key,T>::find(const char* key, size_t key_sz, char* value_out)
{
    //std::shared_lock lock(mutex_);

    if constexpr (std::is_arithmetic<Key>::value)
    {
        auto it = map_.find(*reinterpret_cast<Key*>(const_cast<char*>(key)));
        if (it == map_.end())
            return false;

        if constexpr (std::is_arithmetic<T>::value)
            memcpy(value_out, &it->second, sizeof(T));
        else
            memcpy(value_out, it->second.c_str(), it->second.size());
       // std::cout << "find result:"  <<  value_out << std::endl;
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
bool hopscotchmap_wrapper<Key, T>::insert(const char* key, size_t key_sz, const char* value, size_t value_sz)
{
    //std::unique_lock lock(mutex_);

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

    bool ret = map_.insert(std::make_pair(k,v)).second;
    //std::cout << "insert result:"  <<  ret << std::endl;
    return ret;
}

template<typename Key, typename T>
bool hopscotchmap_wrapper<Key, T>::update(const char* key, size_t key_sz, const char* value, size_t value_sz)
{
    //std::unique_lock lock(mutex_);

//    typename tsl::hopscotch_map<Key,T>::iterator it;
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
bool hopscotchmap_wrapper<Key,T>::remove(const char* key, size_t key_sz)
{
   // std::unique_lock lock(mutex_);

    if constexpr (std::is_arithmetic<Key>::value)
        return map_.erase(*reinterpret_cast<Key*>(const_cast<char*>(key))) == 1;
    else
        return map_.erase(std::string(key, key_sz)) == 1;
}

template<typename Key, typename T>
int hopscotchmap_wrapper<Key,T>::scan(const char* key, size_t key_sz, int scan_sz, char*& values_out)
{
   // std::shared_lock lock(mutex_);

    constexpr size_t ONE_MB = 1ULL << 20;
    static thread_local std::array<char, ONE_MB> results;

    int scanned;
    char* dst = reinterpret_cast<char*>(results.data());
    if constexpr (std::is_arithmetic<Key>::value)
    {
         for(auto it = map_.cbegin(); it != map_.cend(); ++it){

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
    }else{
        for(auto it = map_.cbegin(); it != map_.cend(); ++it){
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
    return scanned;
}

#endif
