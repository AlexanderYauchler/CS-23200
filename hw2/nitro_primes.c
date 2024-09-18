#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <xmmintrin.h>

int main(int argc, const char** argv){
    uint_fast64_t lowerBound, upperBound;

    char buffer[65536];
    setvbuf(stdout, buffer, _IOFBF, sizeof buffer);

    if(argc != 3){
        fprintf(stderr, "USAGE: %s lowerBound upperBound\n", argv[0]);
        return -1;
    }

    lowerBound = atoi(argv[1]);
    upperBound = atoi(argv[2]);

    if(lowerBound < 1 || upperBound < 1){
        fprintf(stderr, "ERROR: both the lowerBound (%llu) and the upperBound (%llu)"
              " must be positive.\n", lowerBound, upperBound);
        return -2;
    }

    if(lowerBound > upperBound) {
        fprintf(stderr, "ERROR: lowerBound (%llu) must not be higher than upperBound (%llu)\n"
            , lowerBound, upperBound);
        return -3;
    }

    char *sieveFault = calloc(upperBound+1, sizeof(char));

    sieveFault[0] = 1;
    sieveFault[1] = 1;

    // INTEL FAST SQUIRT
    __m128 value = _mm_set_ss(upperBound);
    __m128 result = _mm_sqrt_ss(value);
    uint_fast32_t fsquirt = _mm_cvtss_si32(result);

    for (uint_fast64_t i = 2; i <= fsquirt+1; i++) {
        if (sieveFault[i] == 0) {
            uint_fast64_t j = i*2;
            while (j <= upperBound) {
                sieveFault[j] = 1;
                j += i;
            }
        }
    }

    for (uint_fast64_t x = lowerBound; x <= upperBound; x++) {
        if (sieveFault[x] == 0) {
            printf("%lld\n", x);
        }
    }

    free(sieveFault);
    fflush(stdout);

    return 0;
}