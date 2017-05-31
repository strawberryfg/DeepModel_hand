/**********************************************************************************\

    CRandom.h - 随机数发生器类模板

    支持生成均匀分布和正态分布的随机数，有三种算法可供选择

    使用步骤：

        1、定义一个 CRandom 对象。例如，如果希望使用一个采用 L'Ecuyer with Bays-
           Durhan shuffle 算法的返回 double 型随机数的发生器，则可以这样做：

           CRandom<double, RandLEcuyerShuffle> crand;

        2、（可选）设置种子：crand.SetSeed( ... ) 如果参数是 0 ，或者不设置种子，
           则默认使用系统时钟做种子

        3、下面就可以用 crand 生成随机数了，例如：

           // generate a double type uniform deviate in open interval (0.0, 1.0)
           double ur = crand.Uniform01();

           // generate a 32-bit integer type uniform deviate in [0 .. N-1]
           Int32 ir = crand.UniformN( N );

           // generate a double type gaussian deviate with  specified  均值 mean，
           // 和标准差 sigma
           double gr = crand.Gaussian(mean, sigma);  // gr is in R^1

    Created on 2011/09/15 by Ziyang Ma(maziyang08@gmail.com)

\**********************************************************************************/

#pragma once

#include "UTCommon.h"

//----------------
// 随机数生成算法
//----------------
enum ERandGenerator
{
    RandStdCpp           ,  // C++ rand(), many flaws, not recommended
    RandParkMillerShuffle,  // Park and Miller with Bays-Durhan shuffle
    RandLEcuyerShuffle      // L'Ecuyer with Bays-Durhan shuffle
};

/* 注意：生成类型 T 的随机数，T 必须是浮点型（float、double 或 long double）
*/

template <typename T, ERandGenerator generator = RandParkMillerShuffle>
class CRandom
{
    ASSERT_REAL_TYPE(T);

    COMPILE_TIME_ASSERT( generator==RandStdCpp || generator==RandParkMillerShuffle ||
                         generator==RandLEcuyerShuffle );

    /////////////////////////
    //                     //
    //  public interfaces  //
    //                     //
    /////////////////////////

public:

    //---------------------------------------
    // ctor，seed 为种子（0 表示用系统时间）
    //---------------------------------------
    explicit CRandom(Int32 seed = 0) 
        : idum(0), iy(0), idum2(0), bset(false), gset(T(0))
    {
        SetSeed( seed );
    }

    //------
    // dtor
    //------
    ~CRandom()  {}

    //----------
    // 设置种子
    //----------
    void SetSeed(Int32 seed = 0)  // 0 表示用系统时间
                 
    {
        if(0 == seed)
        {
            LARGE_INTEGER iNow;  iNow.LowPart = 0;  iNow.HighPart = 0;
            QueryPerformanceCounter( &iNow );
            seed = static_cast<Int32>( iNow.LowPart );
        }

        idum = seed>0 ? -seed : seed;  // make idum negative

        iy = 0;

        initialize( TYPE_BY_INT<generator>() );

        bset = false;
    }

    //----------------------------------------
    // 生成开区间（0，1）上的均匀分布的随机数
    //----------------------------------------
    const T Uniform01() 
    {
        T temp = generate_uniform( TYPE_BY_INT<generator>() );

        while(T(0)==temp || T(1.0)==temp)
        {
            // almost never occurred if not using RandStdCpp generator
            temp = generate_uniform( TYPE_BY_INT<generator>() );
        }

        return temp;
    }

    //------------------------------------------------
    // 生成 0 .. N-1 （包括两端）的均匀分布的随机整数
    //------------------------------------------------
    Int32 UniformN(Int32 N) 
    {
        // 0,1 .. N, N+1
        Int32 j = static_cast<Int32>(generate_uniform(TYPE_BY_INT<generator>()) *
                                                    static_cast<T>(N + 1)
                                                   );

        while(0==j || (N+1)==j)
        {
            j = static_cast<Int32>(generate_uniform(TYPE_BY_INT<generator>()) *
                                               static_cast<T>(N + 1)
                                              );
        }

        return j-1;  // make it to 0 .. N-1
    }

    //--------------------------------------------------
    // 生成均值为 mean，标准差 sigma 的正态分布的随机数
    // 参数默认为标准正态分布（即 均值为 0，标准差为 1）
    //--------------------------------------------------
    const T Gaussian(T mean = T(0), T sigma = T(1.0)) 
    {
        return generate_normal() * sigma + mean;
    }

    ///////////////////////////////////////////
    //                                       //
    //  internal data members and functions  //
    //                                       //
    ///////////////////////////////////////////

private:  // CONSTANTS

    // shuffle table size
    static const Int32 NTAB = 32;

    // for Park and Miller's generator
    static const Int32 IA = 16807     ;
    static const Int32 IM = 2147483647;
    static const Int32 IQ = 127773    ;  // Int32(IM/IA)
    static const Int32 IR = 2836      ;  // IM % IA
    static const Int32 NDIV = 1 + (IM-1) / NTAB;

    // for L'Ecuyer's generator
    static const Int32 IM1  = 2147483563;
    static const Int32 IM2  = 2147483399;
    static const Int32 IMM1 = IM1 - 1   ;
    static const Int32 IA1 = 40014;
    static const Int32 IA2 = 40692;
    static const Int32 IQ1 = 53668;  // Int32(IM1/IA1)
    static const Int32 IQ2 = 52774;  // Int32(IM2/IA2)
    static const Int32 IR1 = 12211;  // IM1 % IA1
    static const Int32 IR2 = 3791 ;  // IM2 % IA2
    static const Int32 NDIV1 = 1 + IMM1 / NTAB;

private:

    // shuffle table
    Int32 iv[NTAB];

    // for uniform generators
    Int32 idum;
    Int32 iy  ;

    // for L'Ecuyer's generator
    Int32 idum2;

    // for Normal Gaussian generator
    bool bset;
    T    gset;

private:

    //----------------------------------------------------------------------------
    // 用 Schrage 算法不带溢出地计算 z = z*a%m，其中 m 几乎等于 Int32 的最大值
    //----------------------------------------------------------------------------
    template <Int32 a, Int32 m, Int32 q, Int32 r>
    void SchrageMulaModm(Int32 &z) 
    {
        /* Schrage 算法：

           不妨设 m = a * q + r，其中，q = [m/a]（整数部分），r = m % a（余数）
           当 r 比较小（即，r < q 且使得 [m/q] < [m/r]），以及 0 < z < m-1 时，
           可以证明如下两个事实：

           1、0 <= a*(z%q) <= m-1

              这是因为 0 <= a*(z%q) <= a*(q-1) <= m-a <= m-1（a 显然必须 >= 1）

           2、0 <= r*[z/q] <= m-1

              这是因为 0 <= r*[z/q] <= r*[m/q] < r*[m/r] <= m

              所以，如果记 z = k * q + x，其中，k = [z/q], x = z % q，则有：

              z * a % m = (k*a*q+x*a) % m = (k*(m-r)+x*a) % m 令其为 Y，则有：

              当 Y >= 0 时，Y 必然等于 a*x - r*k         （x 亦即 z - k * q）
              当 Y <  0 时，Y 必然等于 a*x - r*k + m

           这就是 Schrage 算法
        */

        Int32 k = z / q;

        z = a * (z-k*q) - r*k;

        if(z < 0) z += m;
    }

    //------------------------------------
    // initialize - initialize generators
    //------------------------------------

    void initialize(TYPE_BY_INT<RandStdCpp>) 
    {
        // for C++ rand() generator, initialize by calling srand( -idum )

        srand( -idum );
    }

    void initialize(TYPE_BY_INT<RandParkMillerShuffle>) 
    {
        // for Park and Miller's shuffle generator, initialize iy, idum, iv[] according to idum

        if((-idum) < 1) idum = 1;  // be sure to prevent idum == 0
        else idum = -idum;

        // 8 warm-ups
        for(Int32 j=0; j<8; ++j) SchrageMulaModm<IA, IM, IQ, IR>( idum );

        // load the shuffle table (after 8 warm-ups)
        for(Int32 j=NTAB-1; j>=0; --j)
        {
            SchrageMulaModm<IA, IM, IQ, IR>( idum );

            iv[j] = idum;
        }

        iy = iv[0];
    }

    void initialize(TYPE_BY_INT<RandLEcuyerShuffle>) 
    {
        // for L'Ecuyer's shuffle generator, initialize iy, idum, idum2, iv[] according to idum

        if((-idum) < 1) idum = 1;  // be sure to prevent idum == 0
        else idum = -idum;

        idum2 = idum;

        // 8 warm-ups
        for(Int32 j=0; j<8; ++j) SchrageMulaModm<IA1, IM1, IQ1, IR1>( idum );

        // load the shuffle table (after 8 warm-ups)
        for(Int32 j=NTAB-1; j>=0; --j)
        {
            SchrageMulaModm<IA1, IM1, IQ1, IR1>( idum );

            iv[j] = idum;
        }

        iy = iv[0];
    }

    //---------------------------------------------------------------------------------------------
    // generate_uniform - generate uniform deviates between 0 and 1 using (possibly) shuffle table
    //---------------------------------------------------------------------------------------------

    T generate_uniform(TYPE_BY_INT<RandStdCpp>) 
    {
        // C++ rand() generator, returned value is in [0, 1]

        return static_cast<T>(rand()) / static_cast<T>(RAND_MAX);
    }

    T generate_uniform(TYPE_BY_INT<RandParkMillerShuffle>) 
    {
        // Park and Miller's shuffle generator, returned value is almost always in (0, 1)

        Int32 j;

        if(idum<=0 || !iy) initialize( TYPE_BY_INT<generator>() );

        SchrageMulaModm<IA, IM, IQ, IR>( idum );

        j = iy / NDIV;  // j will be in the range 0 .. NTAB-1

        // output the previously stored value and refill the shuffle table
        iy = iv[j];
        iv[j] = idum;

        T temp = static_cast<T>(iy) / static_cast<T>(IM);

        return GIVMin(temp, T(1.0));
    }

    T generate_uniform(TYPE_BY_INT<RandLEcuyerShuffle>) 
    {
        Int32 j;

        if(idum <= 0) initialize( TYPE_BY_INT<generator>() );

        SchrageMulaModm<IA1, IM1, IQ1, IR1>( idum );

        SchrageMulaModm<IA2, IM2, IQ2, IR2>( idum2 );

        j = iy / NDIV1;  // j will be in the range 0 .. NTAB-1

        // shuffle idum, combine idum and idum2 to generate the output

        iy = iv[j] - idum2;

        iv[j] = idum;

        if(iy < 1) iy += IMM1;

        T temp = static_cast<T>(iy) / static_cast<T>(IM1);

        return GIVMin(temp, T(1.0));
    }

    //---------------------------------------------------------------------------------------------
    // generate_normal - generates a normally distributed deviate with zero mean and unit variance
    //---------------------------------------------------------------------------------------------

    T generate_normal() 
    {
        T fac, rsq, v1, v2;

        if( bset )
        {
            // we have an extra deviate handy, so unset the flag and return it

            bset = false;

            return gset;
        }
        else
        {
            // we don't have an extra deviate handy, so pick two uniform numbers
            // in the square extending from -1 to +1 in each direction,  see if
            // they are in the unit circle and not both zero, if not, try again
            do
            {
                v1 = T(2.0) * Uniform01() - T(1.0);
                v2 = T(2.0) * Uniform01() - T(1.0);

                rsq = v1*v1 + v2*v2;
            }
            while(rsq>=T(1.0) || rsq==T(0));

            // 则 rsq 服从（0，1）上的均匀分布

            fac = sqrt(T(-2.0) * log(rsq) / rsq);

            // now make the Box-Muller tansformation to get two normal
            // deviates, return one and save the other for next time

            gset = v1 * fac;

            bset = true;

            return v2 * fac;
        }
    }

};  // class CRandom
