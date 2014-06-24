/*
* Simd Library Tests.
*
* Copyright (c) 2011-2014 Yermalayeu Ihar.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#include "Test/TestUtils.h"
#include "Test/TestPerformance.h"
#include "Test/TestData.h"
#include "Test/Test.h"

namespace Test
{
    namespace
    {
        struct Func1
        {
            typedef void (*FuncPtr)(const uint8_t * src, size_t srcStride, size_t width, size_t height, 
                uint8_t saturation, uint8_t boost, uint8_t * dx, size_t dxStride, uint8_t * dy, size_t dyStride);

            FuncPtr func;
            std::string description;

            Func1(const FuncPtr & f, const std::string & d) : func(f), description(d) {}

            void Call(const View & src, uint8_t saturation, uint8_t boost, View &  dx, View & dy) const
            {
                TEST_PERFORMANCE_TEST(description);
                func(src.data, src.stride, src.width, src.height, saturation, boost, dx.data, dx.stride, dy.data, dy.stride);
            }
        };
    }
#define FUNC1(function) Func1(function, #function)

    bool TextureBoostedSaturatedGradientAutoTest(int width, int height, int saturation, int boost, const Func1 & f1, const Func1 & f2)
    {
        bool result = true;

        std::cout << "Test " << f1.description << " & " << f2.description 
            << " [" << width << ", " << height << "] <" << saturation << ", " << boost << ">." << std::endl;

        View src(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        FillRandom(src);

        View dx1(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        View dy1(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        View dx2(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        View dy2(width, height, View::Gray8, NULL, TEST_ALIGN(width));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(src, saturation, boost, dx1, dy1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(src, saturation, boost, dx2, dy2));

        result = result && Compare(dx1, dx2, 0, true, 32, 0, "dx");
        result = result && Compare(dy1, dy2, 0, true, 32, 0, "dy");

        return result;
    }

    bool TextureBoostedSaturatedGradientAutoTest(int width, int height, const Func1 & f1, const Func1 & f2)
    {
        bool result = true;

        result = result && TextureBoostedSaturatedGradientAutoTest(width, height, 32, 3, f1, f2);
        result = result && TextureBoostedSaturatedGradientAutoTest(width, height, 16, 4, f1, f2);
        result = result && TextureBoostedSaturatedGradientAutoTest(width, height, 16, 5, f1, f2);

        return result;
    }

    bool TextureBoostedSaturatedGradientAutoTest(const Func1 & f1, const Func1 & f2)
    {
        bool result = true;

        result = result && TextureBoostedSaturatedGradientAutoTest(W, H, f1, f2);
        result = result && TextureBoostedSaturatedGradientAutoTest(W + 1, H - 1, f1, f2);
        result = result && TextureBoostedSaturatedGradientAutoTest(W - 1, H + 1, f1, f2);

        return result;
    }

    bool TextureBoostedSaturatedGradientAutoTest()
    {
        bool result = true;

        result = result && TextureBoostedSaturatedGradientAutoTest(FUNC1(Simd::Base::TextureBoostedSaturatedGradient), FUNC1(SimdTextureBoostedSaturatedGradient));

#ifdef SIMD_SSE2_ENABLE
        if(Simd::Sse2::Enable)
            result = result && TextureBoostedSaturatedGradientAutoTest(FUNC1(Simd::Sse2::TextureBoostedSaturatedGradient), FUNC1(SimdTextureBoostedSaturatedGradient));
#endif 

#ifdef SIMD_AVX2_ENABLE
        if(Simd::Avx2::Enable)
            result = result && TextureBoostedSaturatedGradientAutoTest(FUNC1(Simd::Avx2::TextureBoostedSaturatedGradient), FUNC1(SimdTextureBoostedSaturatedGradient));
#endif 

#ifdef SIMD_VSX_ENABLE
        if(Simd::Vsx::Enable)
            result = result && TextureBoostedSaturatedGradientAutoTest(FUNC1(Simd::Vsx::TextureBoostedSaturatedGradient), FUNC1(SimdTextureBoostedSaturatedGradient));
#endif 

        return result;
    }

    namespace
    {
        struct Func2
        {
            typedef void (*FuncPtr)(const uint8_t * src, size_t srcStride, size_t width, size_t height, 
                uint8_t boost, uint8_t * dst, size_t dstStride);

            FuncPtr func;
            std::string description;

            Func2(const FuncPtr & f, const std::string & d) : func(f), description(d) {}

            void Call(const View & src, uint8_t boost, View &  dst) const
            {
                TEST_PERFORMANCE_TEST(description);
                func(src.data, src.stride, src.width, src.height, boost, dst.data, dst.stride);
            }
        };
    }
#define FUNC2(function) Func2(function, #function)

    bool TextureBoostedUvAutoTest(int width, int height, int boost, const Func2 & f1, const Func2 & f2)
    {
        bool result = true;

        std::cout << "Test " << f1.description << " & " << f2.description 
            << " [" << width << ", " << height << "] <" << boost << ">." << std::endl;

        View src(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        FillRandom(src);

        View dst1(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        View dst2(width, height, View::Gray8, NULL, TEST_ALIGN(width));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(src, boost, dst1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(src, boost, dst2));

        result = result && Compare(dst1, dst2, 0, true, 32, 0);

        return result;
    }

    bool TextureBoostedUvAutoTest(int width, int height, const Func2 & f1, const Func2 & f2)
    {
        bool result = true;

        result = result && TextureBoostedUvAutoTest(width, height, 3, f1, f2);
        result = result && TextureBoostedUvAutoTest(width, height, 4, f1, f2);
        result = result && TextureBoostedUvAutoTest(width, height, 5, f1, f2);

        return result;
    }

    bool TextureBoostedUvAutoTest()
    {
        bool result = true;

        result = result && TextureBoostedUvAutoTest(W, H, FUNC2(Simd::Base::TextureBoostedUv), FUNC2(SimdTextureBoostedUv));
        result = result && TextureBoostedUvAutoTest(W + 1, H - 1, FUNC2(Simd::Base::TextureBoostedUv), FUNC2(SimdTextureBoostedUv));
        result = result && TextureBoostedUvAutoTest(W - 1, H + 1, FUNC2(Simd::Base::TextureBoostedUv), FUNC2(SimdTextureBoostedUv));

#if defined(SIMD_SSE2_ENABLE) && defined(SIMD_AVX2_ENABLE)
        if(Simd::Sse2::Enable && Simd::Avx2::Enable)
        {
            result = result && TextureBoostedUvAutoTest(W, H, FUNC2(Simd::Sse2::TextureBoostedUv), FUNC2(Simd::Avx2::TextureBoostedUv));
            result = result && TextureBoostedUvAutoTest(W + 1, H - 1, FUNC2(Simd::Sse2::TextureBoostedUv), FUNC2(Simd::Avx2::TextureBoostedUv));
            result = result && TextureBoostedUvAutoTest(W - 1, H + 1, FUNC2(Simd::Sse2::TextureBoostedUv), FUNC2(Simd::Avx2::TextureBoostedUv));
        }
#endif 

        return result;
    }

    namespace
    {
        struct Func3
        {
            typedef void (*FuncPtr)(const uint8_t * src, size_t srcStride, size_t width, size_t height, 
                const uint8_t * lo, size_t loStride, const uint8_t * hi, size_t hiStride, int64_t * sum);

            FuncPtr func;
            std::string description;

            Func3(const FuncPtr & f, const std::string & d) : func(f), description(d) {}

            void Call(const View & src, const View & lo, const View & hi, int64_t * sum) const
            {
                TEST_PERFORMANCE_TEST(description);
                func(src.data, src.stride, src.width, src.height, lo.data, lo.stride, hi.data, hi.stride, sum);
            }
        };
    }
#define FUNC3(function) Func3(function, #function)

    bool TextureGetDifferenceSumAutoTest(int width, int height, const Func3 & f1, const Func3 & f2)
    {
        bool result = true;

        std::cout << "Test " << f1.description << " & " << f2.description << " [" << width << ", " << height << "]." << std::endl;

        View src(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        FillRandom(src);
        View lo(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        FillRandom(lo);
        View hi(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        FillRandom(hi);

        int64_t s1, s2;

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(src, lo, hi, &s1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(src, lo, hi, &s2));

        TEST_CHECK_VALUE(s);

        return result;
    }

    bool TextureGetDifferenceSumAutoTest(const Func3 & f1, const Func3 & f2)
    {
        bool result = true;

        result = result && TextureGetDifferenceSumAutoTest(W, H, f1, f2);
        result = result && TextureGetDifferenceSumAutoTest(W + 3, H - 3, f1, f2);
        result = result && TextureGetDifferenceSumAutoTest(W - 3, H + 3, f1, f2);

        return result;
    }

    bool TextureGetDifferenceSumAutoTest()
    {
        bool result = true;

        result = result && TextureGetDifferenceSumAutoTest(FUNC3(Simd::Base::TextureGetDifferenceSum), FUNC3(SimdTextureGetDifferenceSum));

#ifdef SIMD_SSE2_ENABLE
        if(Simd::Sse2::Enable)
            result = result && TextureGetDifferenceSumAutoTest(FUNC3(Simd::Sse2::TextureGetDifferenceSum), FUNC3(SimdTextureGetDifferenceSum));
#endif 

#ifdef SIMD_AVX2_ENABLE
        if(Simd::Avx2::Enable)
            result = result && TextureGetDifferenceSumAutoTest(FUNC3(Simd::Avx2::TextureGetDifferenceSum), FUNC3(SimdTextureGetDifferenceSum));
#endif 

#ifdef SIMD_VSX_ENABLE
        if(Simd::Vsx::Enable)
            result = result && TextureGetDifferenceSumAutoTest(FUNC3(Simd::Vsx::TextureGetDifferenceSum), FUNC3(SimdTextureGetDifferenceSum));
#endif 

        return result;
    }

    namespace
    {
        struct Func4
        {
            typedef void (*FuncPtr)(const uint8_t * src, size_t srcStride, size_t width, size_t height, 
                int shift, uint8_t * dst, size_t dstStride);

            FuncPtr func;
            std::string description;

            Func4(const FuncPtr & f, const std::string & d) : func(f), description(d) {}

            void Call(const View & src, int shift, View &  dst) const
            {
                TEST_PERFORMANCE_TEST(description);
                func(src.data, src.stride, src.width, src.height, shift, dst.data, dst.stride);
            }
        };
    }
#define FUNC4(function) Func4(function, #function)

    bool TexturePerformCompensationAutoTest(int width, int height, int shift, const Func4 & f1, const Func4 & f2)
    {
        bool result = true;

        std::cout << "Test " << f1.description << " & " << f2.description 
            << " [" << width << ", " << height << "] <" << shift << ">." << std::endl;

        View src(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        FillRandom(src);

        View dst1(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        View dst2(width, height, View::Gray8, NULL, TEST_ALIGN(width));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(src, shift, dst1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(src, shift, dst2));

        result = result && Compare(dst1, dst2, 0, true, 32, 0);

        return result;
    }

    bool TexturePerformCompensationAutoTest(int width, int height, const Func4 & f1, const Func4 & f2)
    {
        bool result = true;

        result = result && TexturePerformCompensationAutoTest(width, height, 17, f1, f2);
        result = result && TexturePerformCompensationAutoTest(width, height, 3, f1, f2);
        result = result && TexturePerformCompensationAutoTest(width, height, 0, f1, f2);
        result = result && TexturePerformCompensationAutoTest(width, height, -4, f1, f2);
        result = result && TexturePerformCompensationAutoTest(width, height, -33, f1, f2);

        return result;
    }

    bool TexturePerformCompensationAutoTest()
    {
        bool result = true;

        result = result && TexturePerformCompensationAutoTest(W, H, FUNC4(Simd::Base::TexturePerformCompensation), FUNC4(SimdTexturePerformCompensation));
        result = result && TexturePerformCompensationAutoTest(W + 1, H - 1, FUNC4(Simd::Base::TexturePerformCompensation), FUNC4(SimdTexturePerformCompensation));
        result = result && TexturePerformCompensationAutoTest(W - 1, H + 1, FUNC4(Simd::Base::TexturePerformCompensation), FUNC4(SimdTexturePerformCompensation));

#if defined(SIMD_SSE2_ENABLE) && defined(SIMD_AVX2_ENABLE)
        if(Simd::Sse2::Enable && Simd::Avx2::Enable)
        {
            result = result && TexturePerformCompensationAutoTest(W, H, FUNC4(Simd::Sse2::TexturePerformCompensation), FUNC4(Simd::Avx2::TexturePerformCompensation));
            result = result && TexturePerformCompensationAutoTest(W + 1, H - 1, FUNC4(Simd::Sse2::TexturePerformCompensation), FUNC4(Simd::Avx2::TexturePerformCompensation));
            result = result && TexturePerformCompensationAutoTest(W - 1, H + 1, FUNC4(Simd::Sse2::TexturePerformCompensation), FUNC4(Simd::Avx2::TexturePerformCompensation));
        }
#endif 

        return result;
    }

    //-----------------------------------------------------------------------

    bool TextureBoostedSaturatedGradientDataTest(bool create, int width, int height, const Func1 & f)
    {
        bool result = true;

        Data data(f.description);

        std::cout << (create ? "Create" : "Verify") << " test " << f.description << " [" << width << ", " << height << "]." << std::endl;

        View src(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        View dx1(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        View dy1(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        View dx2(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        View dy2(width, height, View::Gray8, NULL, TEST_ALIGN(width));

        const int saturation = 16, boost = 4; 

        if(create)
        {
            FillRandom(src);

            TEST_SAVE(src);

            f.Call(src, saturation, boost, dx1, dy1);

            TEST_SAVE(dx1);
            TEST_SAVE(dy1);
        }
        else
        {
            TEST_LOAD(src);

            TEST_LOAD(dx1);
            TEST_LOAD(dy1);

            f.Call(src, saturation, boost, dx2, dy2);

            TEST_SAVE(dx2);
            TEST_SAVE(dy2);

            result = result && Compare(dx1, dx2, 0, true, 32, 0, "dx");
            result = result && Compare(dy1, dy2, 0, true, 32, 0, "dy");
        }

        return result;
    }

    bool TextureBoostedSaturatedGradientDataTest(bool create)
    {
        bool result = true;

        result = result && TextureBoostedSaturatedGradientDataTest(create, DW, DH, FUNC1(SimdTextureBoostedSaturatedGradient));

        return result;
    }

    bool TextureGetDifferenceSumDataTest(bool create, int width, int height, const Func3 & f)
    {
        bool result = true;

        Data data(f.description);

        std::cout << (create ? "Create" : "Verify") << " test " << f.description << " [" << width << ", " << height << "]." << std::endl;

        View src(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        View lo(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        View hi(width, height, View::Gray8, NULL, TEST_ALIGN(width));

        int64_t s1, s2;

        if(create)
        {
            FillRandom(src);
            FillRandom(lo);
            FillRandom(hi);

            TEST_SAVE(src);
            TEST_SAVE(lo);
            TEST_SAVE(hi);

            f.Call(src, lo, hi, &s1);

            TEST_SAVE(s1);
        }
        else
        {
            TEST_LOAD(src);
            TEST_LOAD(lo);
            TEST_LOAD(hi);

            TEST_LOAD(s1);

            f.Call(src, lo, hi, &s2);

            TEST_SAVE(s2);

            TEST_CHECK_VALUE(s);
        }

        return result;
    }

    bool TextureGetDifferenceSumDataTest(bool create)
    {
        bool result = true;

        result = result && TextureGetDifferenceSumDataTest(create, DW, DH, FUNC3(SimdTextureGetDifferenceSum));

        return result;
    }
}
