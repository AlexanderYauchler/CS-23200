#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <xmmintrin.h>
#include <pthread.h>

int main(int argc, const char** argv){
    uint_fast32_t lowerBound, upperBound;

    if(argc != 3){
        fprintf(stderr, "USAGE: %s lowerBound upperBound\n", argv[0]);
        return -1;
    }
  
    lowerBound = atoi(argv[1]);
    upperBound = atoi(argv[2]);
  
    // Edge case of upper < lower is included here now
    if (lowerBound < 1 || upperBound < 1 || upperBound <= lowerBound){
        fprintf(stderr, "ERROR: both the lowerBound (%d) and the upperBound (%d)"
	          " must be positive.\n", lowerBound, upperBound);
        return -2;
    }

    // Buffer to print in chunks (silly fast)
    char buffer[upperBound];
    setvbuf(stdout, buffer, _IOFBF, sizeof(buffer));
  
    {   // Primes main loop
        // Whether we have found a prime or not
        unsigned flag = 1;

        // Loop through our range
        for (uint_fast32_t curr = lowerBound; curr <= upperBound; curr++) {
            // Not composite nubers, no need to consider
            if (curr == 1 || curr == 0) { continue; }

            flag = 1;

            // INTEL fast sqrt()
            __m128 value = _mm_set_ss(curr);
            __m128 result = _mm_sqrt_ss(value);
            int fsqrt = _mm_cvtss_si32(result);

            // Check from 2 -> (num / 2)
            for (uint_fast32_t check = 2; check <= fsqrt; check++) {
                // Check if it is divisible by our current step (non-prime)
                if (curr % check == 0) {
                    flag = 0;
                    break;
                }
            }

            // If our flag is still set, we have a prime, print it!
             if (flag) { printf("%u\n", curr); }
        }

        fflush(stdout);
    }

    // Toodaloo!
    return 0;
}