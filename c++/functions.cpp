#include <immintrin.h>
#include "pgol.h"

float fast_rsqrt(float x){
	// fast reciprocal square root
	return _mm_cvtss_f32( _mm_rsqrt_ss( _mm_set_ss(x)));
}

float fast_sqrt(float x){
	// fast square root
	return x * fast_rsqrt(x);
}

