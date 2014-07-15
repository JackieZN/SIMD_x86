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
		struct Func
		{
			typedef void (*FuncPtr)(const uint8_t * y, size_t yStride, const uint8_t * u, size_t uStride, const uint8_t * v, size_t vStride, 
				size_t width, size_t height, uint8_t * hue, size_t hueStride);

			FuncPtr func;
			std::string description;

			Func(const FuncPtr & f, const std::string & d) : func(f), description(d) {}

			void Call(const View & y, const View & u, const View & v, View & hue) const
			{
				TEST_PERFORMANCE_TEST(description);
				func(y.data, y.stride, u.data, u.stride, v.data, v.stride, y.width, y.height, hue.data, hue.stride);
			}
		};	
	}

#define FUNC(function) Func(function, #function)

	bool YuvToHueAutoTest(int width, int height, const Func & f1, const Func & f2, bool is420)
	{
		bool result = true;

		std::cout << "Test " << f1.description << " & " << f2.description << " [" << width << ", " << height << "]." << std::endl;

		const int uvWidth = is420 ? width/2 : width;
		const int uvHeight = is420 ? height/2 : height;

		View y(width, height, View::Gray8, NULL, TEST_ALIGN(width));
		FillRandom(y);
		View u(uvWidth, uvHeight, View::Gray8, NULL, TEST_ALIGN(uvWidth));
		FillRandom(u);
		View v(uvWidth, uvHeight, View::Gray8, NULL, TEST_ALIGN(uvWidth));
		FillRandom(v);

		View hue1(width, height, View::Gray8, NULL, TEST_ALIGN(width));
		View hue2(width, height, View::Gray8, NULL, TEST_ALIGN(width));

		TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(y, u, v, hue1));

		TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(y, u, v, hue2));

		result = result && Compare(hue1, hue2, 0, true, 64, 256);

		return result;
	}

    bool YuvToHueAutoTest(const Func & f1, const Func & f2, bool is420)
    {
        bool result = true;

        const int D = is420 ? E : O;

        result = result && YuvToHueAutoTest(W, H, f1, f2, is420);
        result = result && YuvToHueAutoTest(W + D, H - D, f1, f2, is420);
        result = result && YuvToHueAutoTest(W - D, H + D, f1, f2, is420);

        return result;
    }

	bool Yuv444pToHueAutoTest()
	{
		bool result = true;

		result = result && YuvToHueAutoTest(FUNC(Simd::Base::Yuv444pToHue), FUNC(SimdYuv444pToHue), false);

#ifdef SIMD_SSE2_ENABLE
        if(Simd::Sse2::Enable)
            result = result && YuvToHueAutoTest(FUNC(Simd::Sse2::Yuv444pToHue), FUNC(SimdYuv444pToHue), false);
#endif 

#ifdef SIMD_AVX2_ENABLE
        if(Simd::Avx2::Enable)
            result = result && YuvToHueAutoTest(FUNC(Simd::Avx2::Yuv444pToHue), FUNC(SimdYuv444pToHue), false);
#endif 

#ifdef SIMD_VSX_ENABLE
        if(Simd::Vsx::Enable)
            result = result && YuvToHueAutoTest(FUNC(Simd::Vsx::Yuv444pToHue), FUNC(SimdYuv444pToHue), false);
#endif 

        return result;
	}

	bool Yuv420pToHueAutoTest()
	{
		bool result = true;

        result = result && YuvToHueAutoTest(FUNC(Simd::Base::Yuv420pToHue), FUNC(SimdYuv420pToHue), true);

#ifdef SIMD_SSE2_ENABLE
        if(Simd::Sse2::Enable)
            result = result && YuvToHueAutoTest(FUNC(Simd::Sse2::Yuv420pToHue), FUNC(SimdYuv420pToHue), true);
#endif 

#ifdef SIMD_AVX2_ENABLE
        if(Simd::Avx2::Enable)
            result = result && YuvToHueAutoTest(FUNC(Simd::Avx2::Yuv420pToHue), FUNC(SimdYuv420pToHue), true);
#endif 

#ifdef SIMD_VSX_ENABLE
        if(Simd::Vsx::Enable)
            result = result && YuvToHueAutoTest(FUNC(Simd::Vsx::Yuv420pToHue), FUNC(SimdYuv420pToHue), true);
#endif

		return result;
	}

    //-----------------------------------------------------------------------

    bool YuvToHueDataTest(bool create, int width, int height, const Func & f, bool is420)
    {
        bool result = true;

        Data data(f.description);

        std::cout << (create ? "Create" : "Verify") << " test " << f.description << " [" << width << ", " << height << "]." << std::endl;

        const int uvWidth = is420 ? width/2 : width;
        const int uvHeight = is420 ? height/2 : height;

        View y(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        View u(uvWidth, uvHeight, View::Gray8, NULL, TEST_ALIGN(uvWidth));
        View v(uvWidth, uvHeight, View::Gray8, NULL, TEST_ALIGN(uvWidth));

        View hue1(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        View hue2(width, height, View::Gray8, NULL, TEST_ALIGN(width));

        if(create)
        {
            FillRandom(y);
            FillRandom(u);
            FillRandom(v);

            TEST_SAVE(y);
            TEST_SAVE(u);
            TEST_SAVE(v);

            f.Call(y, u, v, hue1);

            TEST_SAVE(hue1);
        }
        else
        {
            TEST_LOAD(y);
            TEST_LOAD(u);
            TEST_LOAD(v);

            TEST_LOAD(hue1);

            f.Call(y, u, v, hue2);

            TEST_SAVE(hue2);

            result = result && Compare(hue1, hue2, 0, true, 64);
        }

        return result;
    }

    bool Yuv420pToHueDataTest(bool create)
    {
        bool result = true;

        result = result && YuvToHueDataTest(create, DW, DH, FUNC(SimdYuv420pToHue), true);

        return result;
    }

    bool Yuv444pToHueDataTest(bool create)
    {
        bool result = true;

        result = result && YuvToHueDataTest(create, DW, DH, FUNC(SimdYuv444pToHue), false);

        return result;
    }
}