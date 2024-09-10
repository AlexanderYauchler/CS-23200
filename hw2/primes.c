/* File: primes.c */
/* Author: Britton Wolfe */
/* Date: July 16, 2009 */
/* This program outputs all the primes in the range given */
/* by the command line arguments */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int main(int argc, const char** argv){

    int lowerBound, upperBound;

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
  
    {
        // Whether we have found a prime or not
        unsigned flag = 1;

        // Loop through our range
        for (int curr = lowerBound; curr <= upperBound; curr++) {
            // Not composite nubers, no need to consider
            if (curr == 1 || curr == 0) { continue; }

            flag = 1;
            // Check from 2 -> (num / 2)
            for (int check = 2; check <= curr / 2; check++) {
                // Check if it is divisible by our current step (non-prime)
                if (curr % check == 0) {
                    flag = 0;
                    break;
                }
            }

            // If our flag is still set, we have a prime, print it!
            if (flag) {
                printf("%d\n", curr);
            }
        }
    }

    // Toodaloo!
    return 0;
}
