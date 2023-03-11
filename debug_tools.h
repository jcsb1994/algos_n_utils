/*! Created on Sat Mar 11 2023
    \author jcsb1994
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    \brief Static/dynamic asserts, ...
*/

#define compile_assert(c, msg) typedef char static_assertion_##msg[(c)?1:-1]

#define run_assert(c) ((c) ? 0 : -1)
