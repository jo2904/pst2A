#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>
#include <emmintrin.h>
#include <unistd.h>

#ifdef __MACH__
#include <mach/mach_traps.h>
#include <mach/mach_time.h>
#endif

#define TIME_ABSOLUTE CLOCK_REALTIME

//definition d'un pointeur 
// permet d'utiliser toujpurs la meme adresse et donc le meme bus
__m128i reg;

//definition de la variation des donnés du bus => création de bruit
__m128i un;
__m128i zero;

//definition de la nano seconde 
#ifndef NSEC_PER_SEC
#define NSEC_PER_SEC 1000000000ull
#endif

//definition de la clock
unsigned int clock_port;

//definition du reste (buffer inutile)
struct timespec reste;

//recupere le temps en nanosecondes
static inline uint64_t temps_nanoS(void){
    struct timespec tp;
    int res = clock_gettime(CLOCK_REALTIME, &tp);
    if(res < 0){printf("erreure clock_gettime\n");exit(1);}
    uint64_t resultat = tp.tv_sec * NSEC_PER_SEC + tp.tv_nsec;
    return resultat;
}

static inline void clock_sleep_trap( unsigned int clock_port, int sleep_type, time_t sec, long nsec, struct timespec *remain) {
    struct timespec req = { sec, nsec };
    int res = clock_nanosleep(sleep_type, TIMER_ABSTIME, &req, remain);
    if (res < 0) {
        perror("clock_nanosleep");
        exit(1);
    }
}

static inline void sommeil_nanoS(unsigned int clock_port, int sleep_type, time_t seconde, long nanoseconde, unsigned int *reste){
    unsigned int temps = { seconde, nanoseconde };
    int res = clock_nanosleep(sleep_type, TIMER_ABSTIME, &temps, reste);
    if(res < 0){printf("erreure clock_nanosleep\n");exit(1);}
}

int main(void){

    uint64_t temps11 = temps_nanoS();
    sommeil_nanoS(clock_port, TIME_ABSOLUTE, 1651740662044520111 /NSEC_PER_SEC, 1651740662044520111 % NSEC_PER_SEC, &reste);
    //clock_sleep_trap(clock_port, TIME_ABSOLUTE, 1651740662044520111 / NSEC_PER_SEC, 1651740662044520111 % NSEC_PER_SEC, &reste);
    uint64_t temps12 = temps_nanoS();
    printf("%ld\n", temps12 - temps11);


    uint64_t temps1 = temps_nanoS();
    clock_sleep_trap(clock_port, TIME_ABSOLUTE, 1651740662044520111 / NSEC_PER_SEC, 1651740662044520111 % NSEC_PER_SEC, &reste);
    uint64_t temps2 = temps_nanoS();
    printf("%ld\n", temps2 - temps1);

    

    return 0;
}