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

//sommeil en nanosecondes
static inline void sommeil_nanoS(unsigned int clock_port, int sleep_type, time_t seconde, long nanoseconde, unsigned int *reste){
    unsigned int temps = { seconde, nanoseconde };
    int res = clock_nanosleep(sleep_type, TIMER_ABSTIME, &temps, reste);
    if(res < 0){printf("erreure clock_nanosleep\n");exit(1);}
}

//emet un signal carre sur le bus
int signal(float temps, float frequence){
    printf("T: %f, F: %f\n", temps, frequence);

    //calcule la periode en fonctionde la frequence (en nano S)
    uint64_t periode = NSEC_PER_SEC / frequence;

    //calcule du début et de la fin en nano S
    uint64_t temps_debut = temps_nanoS();
    uint64_t temps_fin = temps_debut + temps * NSEC_PER_SEC;

    while(temps_nanoS() < temps_fin){
        uint64_t milieu = temps_debut + periode / 2;
        uint64_t reset = temps_debut + periode;

        while(temps_nanoS() < milieu){
            _mm_stream_si128(&reg, un);
            _mm_stream_si128(&reg, zero);
        }
        sommeil_nanoS(clock_port, TIME_ABSOLUTE, reset /NSEC_PER_SEC, reset % NSEC_PER_SEC, &reste);
        temps_debut = reset;
    }
}

//ouvre les fichier tunes
int lecture_fichier(char * path){
    FILE *fichier = NULL;
    fichier = fopen(path, "r");
    if(!fichier){printf("erreure fopen\n");exit(1);}

    float temps, frequence;
    while (fscanf(fichier, "%f %f", &temps, &frequence) != EOF) {
        signal(temps, frequence);
    }
}


int main(){
    printf("lancement ...\n");

    printf("initialisation ...\n");
    zero = _mm_set_epi32(0, 0, 0, 0);
    un = _mm_set_epi32(-1, -1, -1, -1);

    printf("ouverture du fichier ...\n");
    char * path = "music.tune";
    lecture_fichier(path);

    return 0;
}
