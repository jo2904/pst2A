#include <stdio.h>
#include <stdlib.h>

#include <emmintrin.h>
#include <unistd.h>

//definition de la variation des donnés du bus => création de bruit
__m128i un;
__m128i zero;


int signal(){
    printf("signal");
}

int main(){
    printf("lancement ...\n");

    printf("initialisation ...\n");
    zero = _mm_set_epi32(0, 0, 0, 0);
    un = _mm_set_epi32(-1, -1, -1, -1);

    
    signal();
}
