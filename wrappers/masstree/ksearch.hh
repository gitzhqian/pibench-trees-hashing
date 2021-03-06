/* Masstree
 * Eddie Kohler, Yandong Mao, Robert Morris
 * Copyright (c) 2012-2014 President and Fellows of Harvard College
 * Copyright (c) 2012-2014 Massachusetts Institute of Technology
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, subject to the conditions
 * listed in the Masstree LICENSE file. These conditions include: you must
 * preserve this copyright notice, and you cannot mention the copyright
 * holders in advertising related to the Software without their permission.
 * The Software is provided WITHOUT ANY WARRANTY, EXPRESS OR IMPLIED. This
 * notice is a summary of the Masstree LICENSE file; the license in that file
 * is legally binding.
 */
#ifndef KSEARCH_HH
#define KSEARCH_HH 1
#include "kpermuter.hh"

template <typename KA, typename T>
struct key_comparator {
    int operator()(const KA &ka, const T &n, int p) {
        return key_compare(ka, n, p);
    }
};


template <typename KA, typename T, typename F>
int key_upper_bound_by(const KA &ka, const T &n, F comparator)
{
    typename key_permuter<T>::type perm = key_permuter<T>::permutation(n);
    int l = 0, r = perm.size();
    while (l < r) {
        int m = (l + r) >> 1;
        int mp = perm[m];
        int cmp = comparator(ka, n, mp);
        if (cmp < 0)
            r = m;
        else if (cmp == 0)
            return m + 1;
        else
            l = m + 1;
    }
    return l;
}

template <typename KA, typename T>
inline int key_upper_bound(const KA &ka, const T &n)
{
    return key_upper_bound_by(ka, n, key_comparator<KA, T>());
}

template <typename KA, typename T, typename F>
int key_lower_bound_by(const KA &ka, const T &n, F comparator)
{
    typename key_permuter<T>::type perm = key_permuter<T>::permutation(n);
    int l = 0, r = perm.size();
    while (l < r) {
        int m = (l + r) >> 1;
        int mp = perm[m];
        int cmp = comparator(ka, n, mp);
        if (cmp < 0)
            r = m;
        else if (cmp == 0)
            return m;
        else
            l = m + 1;
    }
    return l;
}

template <typename KA, typename T>
inline int key_lower_bound(const KA &ka, const T &n)
{
    return key_lower_bound_by(ka, n, key_comparator<KA, T>());
}

template <typename KA, typename T, typename F>
int key_lower_bound_with_position_by(const KA &ka, const T &n, int &position, F comparator)
{
    typename key_permuter<T>::type perm = key_permuter<T>::permutation(n);
    int l = 0, r = perm.size();
    while (l < r) {
        int m = (l + r) >> 1;
        int mp = perm[m];
        int cmp = comparator(ka, n, mp);
        if (cmp < 0)
            r = m;
        else if (cmp == 0) {
            position = mp;
            return m;
        } else
            l = m + 1;
    }
    position = -1;
    return l;
}

template <typename KA, typename T>
inline int key_lower_bound_with_position(const KA &ka, const T &n, int &position)
{
    return key_lower_bound_with_position_by(ka, n, position, key_comparator<KA, T>());
}


template <typename KA, typename T, typename F>
int key_find_upper_bound_by(const KA &ka, const T &n, F comparator)
{
    typename key_permuter<T>::type perm = key_permuter<T>::permutation(n);
    int l = 0, r = perm.size();
    while (l < r) {
        int lp = perm[l];
        int cmp = comparator(ka, n, lp);
        if (cmp < 0)
            break;
        else
            ++l;
    }
    return l;
}

template <typename KA, typename T, typename F>
int key_find_lower_bound_by(const KA &ka, const T &n, F comparator)
{
    typename key_permuter<T>::type perm = key_permuter<T>::permutation(n);
    int l = 0, r = perm.size();
    while (l < r) {
        int lp = perm[l];
        int cmp = comparator(ka, n, lp);
        if (cmp <= 0)
            break;
        else
            ++l;
    }
    return l;
}

template <typename KA, typename T, typename F>
int key_find_lower_bound_with_position_by(const KA &ka, const T &n, int &position, F comparator)
{
    typename key_permuter<T>::type perm = key_permuter<T>::permutation(n);
    int l = 0, r = perm.size();
    while (l < r) {
        int lp = perm[l];
        int cmp = comparator(ka, n, lp);
        if (cmp < 0)
            break;
        else if (cmp == 0) {
            position = lp;
            return l;
        } else
            ++l;
    }
    position = -1;
    return l;
}


struct key_bound_binary {
    static const bool is_binary = true;
    template <typename KA, typename T>
    static inline int upper(const KA &ka, const T &n) {
        return key_upper_bound_by(ka, n, key_comparator<KA, T>());
    }
    template <typename KA, typename T>
    static inline int lower(const KA &ka, const T &n) {
        return key_lower_bound_by(ka, n, key_comparator<KA, T>());
    }
    template <typename KA, typename T, typename F>
    static inline int lower_by(const KA &ka, const T &n, F comparator) {
        return key_lower_bound_by(ka, n, comparator);
    }
    template <typename KA, typename T>
    static inline int lower_with_position(const KA &ka, const T &n, int &position) {
        return key_lower_bound_with_position_by(ka, n, position, key_comparator<KA, T>());
    }
    template <typename KA, typename T, typename F>
    static inline int lower_with_position_by(const KA &ka, const T &n, int &position, F comparator) {
        return key_lower_bound_with_position_by(ka, n, position, comparator);
    }
};

struct key_bound_linear {
    static const bool is_binary = false;
    template <typename KA, typename T>
    static inline int upper(const KA &ka, const T &n) {
        return key_find_upper_bound_by(ka, n, key_comparator<KA, T>());
    }
    template <typename KA, typename T>
    static inline int lower(const KA &ka, const T &n) {
        return key_find_lower_bound_by(ka, n, key_comparator<KA, T>());
    }
    template <typename KA, typename T, typename F>
    static inline int lower_by(const KA &ka, const T &n, F comparator) {
        return key_find_lower_bound_by(ka, n, comparator);
    }
    template <typename KA, typename T>
    static inline int lower_with_position(const KA &ka, const T &n, int &position) {
        return key_find_lower_bound_with_position_by(ka, n, position, key_comparator<KA, T>());
    }
    template <typename KA, typename T, typename F>
    static inline int lower_with_position_by(const KA &ka, const T &n, int &position, F comparator) {
        return key_find_lower_bound_with_position_by(ka, n, position, comparator);
    }
};


enum {
    bound_method_fast = 0,
    bound_method_binary,
    bound_method_linear
};
template <int max_size, int method = bound_method_fast> struct key_bound {};
template <int max_size> struct key_bound<max_size, bound_method_binary> {
    typedef key_bound_binary type;
};
template <int max_size> struct key_bound<max_size, bound_method_linear> {
    typedef key_bound_linear type;
};
template <int max_size> struct key_bound<max_size, bound_method_fast> {
    typedef typename key_bound<max_size, (max_size > 16 ? bound_method_binary : bound_method_linear)>::type type;
};

#endif
