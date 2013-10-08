/*
* Simd Library.
*
* Copyright (c) 2011-2013 Yermalayeu Ihar.
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
#include "Simd/SimdEnable.h"
#include "Simd/SimdLoad.h"
#include "Simd/SimdStore.h"
#include "Simd/SimdConst.h"
#include "Simd/SimdMemory.h"
#include "Simd/SimdGrayToBgra.h"

namespace Simd
{
    namespace Base
    {
        SIMD_INLINE uint GrayToBgra(uint gray, uint alpha)
        {
            return gray | (gray << 8) | (gray << 16)  | (alpha << 24);
        }

        void GrayToBgra(const uchar *gray, size_t width, size_t height, size_t grayStride, uchar *bgra, size_t bgraStride, uchar alpha)
        {
            for(size_t row = 0; row < height; ++row)
            {
				for(size_t col = 0; col < width; ++col)
                    ((uint*)bgra)[col] = GrayToBgra(gray[col], alpha);
                gray += grayStride;
                bgra += bgraStride;
            }
        }
    }

#ifdef SIMD_SSE2_ENABLE    
    namespace Sse2
    {
        template <bool align> SIMD_INLINE void GrayToBgra(uchar * bgra, __m128i gray, __m128i alpha)
        {
            __m128i bgLo = _mm_unpacklo_epi8(gray, gray);
            __m128i bgHi = _mm_unpackhi_epi8(gray, gray);
            __m128i raLo = _mm_unpacklo_epi8(gray, alpha);
            __m128i raHi = _mm_unpackhi_epi8(gray, alpha);

            Store<align>((__m128i*)bgra + 0, _mm_unpacklo_epi16(bgLo, raLo));
            Store<align>((__m128i*)bgra + 1, _mm_unpackhi_epi16(bgLo, raLo));
            Store<align>((__m128i*)bgra + 2, _mm_unpacklo_epi16(bgHi, raHi));
            Store<align>((__m128i*)bgra + 3, _mm_unpackhi_epi16(bgHi, raHi));
        }

        template <bool align> void GrayToBgra(const uchar *gray, size_t width, size_t height, size_t grayStride, uchar *bgra, size_t bgraStride, uchar alpha)
        {
            assert(width >= A);
			if(align)
				assert(Aligned(bgra) && Aligned(bgraStride) && Aligned(gray) && Aligned(grayStride));

            __m128i _alpha = _mm_set1_epi8(alpha);
            size_t alignedWidth = AlignLo(width, A);
            for(size_t row = 0; row < height; ++row)
            {
                for(size_t col = 0; col < alignedWidth; col += A)
                {
                    __m128i _gray = Load<align>((__m128i*)(gray + col));
                    GrayToBgra<align>(bgra + 4*col, _gray, _alpha);
                }
                if(alignedWidth != width)
                {
                    __m128i _gray = Load<false>((__m128i*)(gray + width - A));
                    GrayToBgra<false>(bgra + 4*(width - A), _gray, _alpha);
                }
                gray += grayStride;
                bgra += bgraStride;
            }
        }

		void GrayToBgra(const uchar *gray, size_t width, size_t height, size_t grayStride, uchar *bgra, size_t bgraStride, uchar alpha)
		{
			if(Aligned(bgra) && Aligned(gray) && Aligned(bgraStride) && Aligned(grayStride))
				GrayToBgra<true>(gray, width, height, grayStride, bgra, bgraStride, alpha);
			else
				GrayToBgra<false>(gray, width, height, grayStride, bgra, bgraStride, alpha);
		}
    }
#endif// SIMD_SSE2_ENABLE

    void GrayToBgra(const uchar *gray, size_t width, size_t height, size_t grayStride, uchar *bgra, size_t bgraStride, uchar alpha)
    {
#ifdef SIMD_AVX2_ENABLE
        if(Avx2::Enable && width >= Avx2::A)
            Avx2::GrayToBgra(gray, width, height, grayStride, bgra, bgraStride, alpha);
        else
#endif//SIMD_AVX2_ENABLE 
#ifdef SIMD_SSE2_ENABLE
        if(Sse2::Enable && width >= Sse2::A)
            Sse2::GrayToBgra(gray, width, height, grayStride, bgra, bgraStride, alpha);
        else
#endif//SIMD_SSE2_ENABLE       
            Base::GrayToBgra(gray, width, height, grayStride, bgra, bgraStride, alpha);
    }
}