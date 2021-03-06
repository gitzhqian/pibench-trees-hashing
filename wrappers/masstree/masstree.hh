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
#ifndef MASSTREE_HH
#define MASSTREE_HH
#include "compiler.hh"
#include "str.hh"
#include "ksearch.hh"

namespace Masstree {
using lcdf::Str;
using lcdf::String;

template <typename T> class value_print;

template <int LW = 15, int IW = LW> struct nodeparams {
    static const  int leaf_width = LW;
    static  const int internode_width = IW;
    static  const bool concurrent = true;
    static  const bool prefetch = false;
    static  const int bound_method = bound_method_binary;
    static  const int debug_level = 0;
    static  const bool printable_keys = true;
    typedef uint64_t ikey_type;
};

template <int LW, int IW>   const int nodeparams<LW, IW>::leaf_width;
template <int LW, int IW>   const int nodeparams<LW, IW>::internode_width;
template <int LW, int IW>  const  int nodeparams<LW, IW>::debug_level;

template <typename P> class node_base;
template <typename P> class leaf;
template <typename P> class internode;
template <typename P> class leafvalue;
template <typename P> class key;
template <typename P> class basic_table;
template <typename P> class unlocked_tcursor;
template <typename P> class tcursor;

//huanchen-static
template <typename P> class massnode;
template <typename P> class massnode_multivalue;
template <typename P> class massnode_dynamicvalue;
template <typename P> class stcursor;
template <typename P> class stcursor_multivalue;
template <typename P> class stcursor_dynamicvalue;
template <typename P> class stcursor_scan;
template <typename P> class stcursor_scan_multivalue;
template <typename P> class stcursor_scan_dynamicvalue;
template <typename P> class stcursor_merge;
template <typename P> class stcursor_merge_multivalue;
template <typename P> class stcursor_merge_dynamicvalue;
template <typename P> class leafvalue_static;
template <typename P> class leafvalue_static_multivalue;

template <typename P>
class basic_table {
  public:
    typedef P param_type;
    typedef node_base<P> node_type;
    typedef leaf<P> leaf_type;
    typedef internode<P> internode_type;
    typedef typename P::value_type value_type;
    typedef typename P::threadinfo_type threadinfo;
    typedef unlocked_tcursor<P> unlocked_cursor_type;
    typedef tcursor<P> cursor_type;

  //huanchen-static
  typedef stcursor<P> static_cursor_type;
  typedef stcursor_multivalue<P> static_multivalue_cursor_type;
  typedef stcursor_dynamicvalue<P> static_dynamicvalue_cursor_type;
  typedef stcursor_scan<P> static_cursor_scan_type;
  typedef stcursor_scan_multivalue<P> static_multivalue_cursor_scan_type;
  typedef stcursor_scan_dynamicvalue<P> static_dynamicvalue_cursor_scan_type;
  typedef stcursor_merge<P> static_cursor_merge_type;
  typedef stcursor_merge_multivalue<P> static_cursor_merge_multivalue_type;
  typedef stcursor_merge_dynamicvalue<P> static_cursor_merge_dynamicvalue_type;

    inline basic_table();

    void initialize(threadinfo& ti);
    void destroy(threadinfo& ti);
    void destroy_novalue(threadinfo& ti); //huanchen

    inline node_type* root() const;
    inline node_type* fix_root();

  //huanchen-static
  inline node_type* static_root() const;
  inline void set_static_root(node_type *staticRoot);

    bool get(Str key, value_type& value, threadinfo& ti) const;

    template <typename F>
    int scan(Str firstkey, bool matchfirst, F& scanner, threadinfo& ti) const;
    template <typename F>
    int rscan(Str firstkey, bool matchfirst, F& scanner, threadinfo& ti) const;

    template <typename F>
    inline int modify(Str key, F& f, threadinfo& ti);
    template <typename F>
    inline int modify_insert(Str key, F& f, threadinfo& ti);

    inline void print(FILE* f = 0, int indent = 0) const;

  private:
    node_type* root_;
    node_type* static_root_; //huanchen-static

    template <typename H, typename F>
    int scan(H helper, Str firstkey, bool matchfirst,
             F& scanner, threadinfo& ti) const;

    friend class unlocked_tcursor<P>;
    friend class tcursor<P>;
};

} // namespace Masstree
#endif
