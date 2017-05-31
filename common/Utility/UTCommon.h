/*
    UTCommon.h - Common including file for Utilities

    Created on 2011/09/02 by Ziyang Ma(maziyang08@gmail.com)
*/

#pragma once

#define UT_DEBUG    1

#include<iostream>
#include<limits>

#include <float.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>

#ifndef __CUDACC__
#include <windows.h>
#endif

#include "UTTypedef.h"

#if defined(_DEBUG) && !defined(UT_DEBUG)
#define UT_DEBUG    1
#endif

#define UT_ERROR(str)    { std::cerr<<(str)<<std::endl; assert(false); }

#define MACRO_CAT(X, Y)    X##Y

//----------------------------------------------------------
// Expand and concatenate two arguments into a lexical unit
//----------------------------------------------------------
#define MACRO_XCAT(X, Y)    MACRO_CAT(X, Y)  // force expansion


#define MACRO_STRINGIFY(X)    #X

//----------------------------------
// Make the expanded macro a string
//----------------------------------
#define MACRO_XSTRINGIFY(X)    MACRO_STRINGIFY(X)

//-----------------------------------------------------------------------
// Compile time assertion
//
// e.g., an assertion failure results in error C2118: negative subscript
//-----------------------------------------------------------------------
#define COMPILE_TIME_ASSERT(expr)    \
    typedef char MACRO_XCAT(COMPILE_TIME_ASSERT_ERROR_, __LINE__)[(expr)?1:-1]

//------------------------------------------------------------------
// Compile time type comparision
//
// e.g., IS_SAME_TYPE(U, V) is true if U and V are equivalent types
//------------------------------------------------------------------
namespace NIsSameType_
{
    template <typename U, typename V>
    struct ctest_ { enum { result_ = false }; };

    template <typename U>
    struct ctest_<U, U> { enum { result_ = true }; };
}
#define IS_SAME_TYPE(TYPE1, TYPE2)    (NIsSameType_::ctest_<TYPE1, TYPE2 >::result_)

//------------------------------------------------------------------------------
// Compile time type selection
//
// e.g., TYPE_SELECT(b, TYPE1, TYPE2) is TYPE1 if b is true, or TYPE2 otherwise
//------------------------------------------------------------------------------
namespace NTypeSelect_
{
    template <bool b, typename T, typename U>
    struct select_ { typedef T    result_; };

    template <typename T, typename U>
    struct select_<false, T, U> { typedef U    result_; };
}
#define TYPE_SELECT(b, TYPE1, TYPE2)    NTypeSelect_::select_<b, TYPE1, TYPE2 >::result_

//-----------------------------------------------------------------------
// Compile time real type assertion
//
// e.g., an assertion failure results in error C2118: negative subscript
//-----------------------------------------------------------------------
#define ASSERT_REAL_TYPE(t)    \
    COMPILE_TIME_ASSERT( IS_SAME_TYPE(t, float)  || \
                         IS_SAME_TYPE(t, double) || \
                         IS_SAME_TYPE(t, long double) )

#define ASSERT_INTEGER_TYPE(t)    \
    COMPILE_TIME_ASSERT( IS_SAME_TYPE(t, bool)           || \
                         IS_SAME_TYPE(t, char)           || \
                         IS_SAME_TYPE(t, unsigned char)  || \
                         IS_SAME_TYPE(t, signed char)    || \
                         IS_SAME_TYPE(t, short)          || \
                         IS_SAME_TYPE(t, unsigned short) || \
                         IS_SAME_TYPE(t, int)            || \
                         IS_SAME_TYPE(t, unsigned int)   || \
                         IS_SAME_TYPE(t, long)           || \
                         IS_SAME_TYPE(t, unsigned long)  || \
                         IS_SAME_TYPE(t, UInt8)       || \
                         IS_SAME_TYPE(t, UInt16)      || \
                         IS_SAME_TYPE(t, UInt32)      || \
                         IS_SAME_TYPE(t, UInt64)      || \
                         IS_SAME_TYPE(t, Int8)        || \
                         IS_SAME_TYPE(t, Int16)       || \
                         IS_SAME_TYPE(t, Int32)       || \
                         IS_SAME_TYPE(t, Int64)       || \
                         IS_SAME_TYPE(t, givMInt) )

template <typename T>
inline bool IsMemoryOverlap(const T *p1, const size_t len1,  // len1 个 T 元素
                            const T *p2, const size_t len2)  // len2 个 T 元素
{
    if(!p1 || !p2) return false;  // 这个必须的

    if(p1 <= p2)
    {
        if(p1+len1 > p2) return true;
    }
    else
    {
        if(p2+len2 > p1) return true;
    }

    return false;
}

//-------------------------------------------------------------------------------
// Type by int
//
// e.g., TYPE_BY_INT<5> generates a one to one correspondence bet. integer 5 and
//       struct TYPE_BY_INT<5>
//-------------------------------------------------------------------------------
template <int i_> struct TYPE_BY_INT { enum { value_ = i_ }; };

template <typename T>
inline const T& GIVMin(const T &a, const T &b)  // might throw if operator < () is overloaded
{
    return (a<b) ? a : b;
}

template <typename T>
inline const T& GIVMax(const T &a, const T &b)  // might throw if operator < () is overloaded
{
    return (b<a) ? a : b;
}

// workaround: in msvc++, you cannot directly use std::numeric_limits<...>::max
#pragma push_macro("max")
#pragma push_macro("min")
#undef max
#undef min

//------------------------------------
// Numerical limits for integer types
//------------------------------------

template <typename T>  // T 可以是 ASSERT_INTEGER_TYPE(T) 所接受的整型
class IntLimits
{
    ASSERT_INTEGER_TYPE(T);

public:

    // minimum finite value
    static const T min_value() { return std::numeric_limits<T>::min(); }

    // maximum finite value
    static const T max_value() { return std::numeric_limits<T>::max(); }

};  // class IntLimits

//-------------------------------------------
// Numerical limits for floating point types
//-------------------------------------------

template <typename T>
class FPLimits
{
    COMPILE_TIME_ASSERT(IS_SAME_TYPE(T, float) || IS_SAME_TYPE(T, double) ||
                        IS_SAME_TYPE(T, long double));

public:

    // number of digits (in decimal base) that can be represented without change
    static int digits10() { return std::numeric_limits<T>::digits10; }

    // if x <= denorm_min(), DO NOT USE 1/x
    static const T denorm_min() { return std::numeric_limits<T>::denorm_min(); }

    /* Machine epsilon: the distance between 1 and the smallest value greater
    than 1 that is representable, equivalent to MATLAB's eps
    */
    static const T epsilon() { return std::numeric_limits<T>::epsilon(); }

    // minimum positive normalized value
    static const T min_positive_value() { return std::numeric_limits<T>::min(); }

    // maximum finite value
    static const T max_value() { return std::numeric_limits<T>::max(); }

    // positive infinity
    static const T infinity() { return std::numeric_limits<T>::infinity(); }

    //----------------------------------------------------------------------------------
    // for any floating point number, one and ONLY ONE of the following three functions
    // returns true

    // is x a finite number?
    static bool is_finite(volatile T x) { return !is_nan(x) && !is_inf(x); }

    // is x an NaN? (see IEEE 754 standard)
    static bool is_nan(volatile T x) { return !is_le_1(&x) && !is_ge_1(&x); }

    // is x an infinite number (positive infinity or negative infinity)?
    static bool is_inf(volatile T x) { return (x==infinity() || x==-infinity()); }

    //----------------------------------------------------------------------------------

private:

    // tricks for preventing compiler optimization
    static bool is_le_1(volatile T *px) { return *px <= T(1); }
    static bool is_ge_1(volatile T *px) { return *px >= T(1); }

};  // class FPLimits

#pragma pop_macro("min")
#pragma pop_macro("max")

#pragma warning(disable:4481)  // warning C4481: nonstandard extension used ...

/// Uncopyable

class Uncopyable
{
protected:

    Uncopyable() {}
    virtual ~Uncopyable() {}

private:

    Uncopyable(const Uncopyable &);
    Uncopyable& operator = (const Uncopyable &);
};
