/**********************************************************************************\

    CRandom.h - �������������ģ��

    ֧�����ɾ��ȷֲ�����̬�ֲ�����������������㷨�ɹ�ѡ��

    ʹ�ò��裺

        1������һ�� CRandom �������磬���ϣ��ʹ��һ������ L'Ecuyer with Bays-
           Durhan shuffle �㷨�ķ��� double ��������ķ��������������������

           CRandom<double, RandLEcuyerShuffle> crand;

        2������ѡ���������ӣ�crand.SetSeed( ... ) ��������� 0 �����߲��������ӣ�
           ��Ĭ��ʹ��ϵͳʱ��������

        3������Ϳ����� crand ����������ˣ����磺

           // generate a double type uniform deviate in open interval (0.0, 1.0)
           double ur = crand.Uniform01();

           // generate a 32-bit integer type uniform deviate in [0 .. N-1]
           Int32 ir = crand.UniformN( N );

           // generate a double type gaussian deviate with  specified  ��ֵ mean��
           // �ͱ�׼�� sigma
           double gr = crand.Gaussian(mean, sigma);  // gr is in R^1

    Created on 2011/09/15 by Ziyang Ma(maziyang08@gmail.com)

\**********************************************************************************/

#pragma once

#include "UTCommon.h"

//----------------
// ����������㷨
//----------------
enum ERandGenerator
{
    RandStdCpp           ,  // C++ rand(), many flaws, not recommended
    RandParkMillerShuffle,  // Park and Miller with Bays-Durhan shuffle
    RandLEcuyerShuffle      // L'Ecuyer with Bays-Durhan shuffle
};

/* ע�⣺�������� T ���������T �����Ǹ����ͣ�float��double �� long double��
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
    // ctor��seed Ϊ���ӣ�0 ��ʾ��ϵͳʱ�䣩
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
    // ��������
    //----------
    void SetSeed(Int32 seed = 0)  // 0 ��ʾ��ϵͳʱ��
                 
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
    // ���ɿ����䣨0��1���ϵľ��ȷֲ��������
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
    // ���� 0 .. N-1 ���������ˣ��ľ��ȷֲ����������
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
    // ���ɾ�ֵΪ mean����׼�� sigma ����̬�ֲ��������
    // ����Ĭ��Ϊ��׼��̬�ֲ����� ��ֵΪ 0����׼��Ϊ 1��
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
    // �� Schrage �㷨��������ؼ��� z = z*a%m������ m �������� Int32 �����ֵ
    //----------------------------------------------------------------------------
    template <Int32 a, Int32 m, Int32 q, Int32 r>
    void SchrageMulaModm(Int32 &z) 
    {
        /* Schrage �㷨��

           ������ m = a * q + r�����У�q = [m/a]���������֣���r = m % a��������
           �� r �Ƚ�С������r < q ��ʹ�� [m/q] < [m/r]�����Լ� 0 < z < m-1 ʱ��
           ����֤������������ʵ��

           1��0 <= a*(z%q) <= m-1

              ������Ϊ 0 <= a*(z%q) <= a*(q-1) <= m-a <= m-1��a ��Ȼ���� >= 1��

           2��0 <= r*[z/q] <= m-1

              ������Ϊ 0 <= r*[z/q] <= r*[m/q] < r*[m/r] <= m

              ���ԣ������ z = k * q + x�����У�k = [z/q], x = z % q�����У�

              z * a % m = (k*a*q+x*a) % m = (k*(m-r)+x*a) % m ����Ϊ Y�����У�

              �� Y >= 0 ʱ��Y ��Ȼ���� a*x - r*k         ��x �༴ z - k * q��
              �� Y <  0 ʱ��Y ��Ȼ���� a*x - r*k + m

           ����� Schrage �㷨
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

            // �� rsq ���ӣ�0��1���ϵľ��ȷֲ�

            fac = sqrt(T(-2.0) * log(rsq) / rsq);

            // now make the Box-Muller tansformation to get two normal
            // deviates, return one and save the other for next time

            gset = v1 * fac;

            bset = true;

            return v2 * fac;
        }
    }

};  // class CRandom
