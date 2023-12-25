#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>

#define TOTAL_NPCS 20
#define TOTAL_BALAS_ENEMIGAS 15
#define TOTAL_BALAS_PROPIAS 5
#define TOTAL_SONIDOS 6
#define ALTO_PANTALLA_TILES 24
#define ANCHO_PANTALLA_TILES 24
#define TOTAL_ANCHO_TILES 480
#define ANCHO_PANTALLA 800
#define ALTO_PANTALLA 600
#define ANCHO_MOV_PANTALLA_INI 200
#define ANCHO_MOV_PANTALLA_FIN 450
#define MAX_INERCIA 5
#define MIN_INERCIA -5
#define SALTO 150
#define ANCHO_PERSONAJE 31
#define ALTO_PERSONAJE 47
#define ALTO_TILE 24
#define ANCHO_TILE 32
#define ANCHO_BALA 32
#define ALTO_BALA 24
#define TOTAL_EXPLOSIONES 5
#define ANCHO_EXPLOSION 32
#define ALTO_EXPLOSION 48
#define NRO_NPCS_DEFINIDOS 7
#define NRO_BALAS_ENEMIGAS_DEFINIDAS 5 // 1 de los personajes y 4 de los distintos bosses
#define CICLOS_REPETICION_DISPARO 20
#define TOTAL_OPCIONES 5
#define TILE_DESLIZANTE 31
#define MAX_TILES 400
#define FPS 60
#define CAIDA_NPC 3

#define ANCHO_MAX_NPC 128
#define ALTO_MAX_NPC 48
#define MAXIMA_BALAS_COLA 50
#ifdef WIN32
    #define DIR_CONTENIDO "./contenido"
#else
    #define DIR_CONTENIDO "./contenido"
#endif


//Variables de main:

#ifdef MAIN
// Constantes problemáticas
int ALTO_NPCS[7]={48,28,42,64,24,48,24};
int ANCHO_NPCS[7]={32,83,28,64,128,32,32};
int VELOCIDADES_NPCS[7]={3,3,3,3,3,3,3};
int seleccion_actual=0,TRonda;

// Variables
SDL_Surface *screen,*imgfondo,*creditos,*controles;//globaliiisimas
int nivel,x_abs=0,tiempo,ciclosps;
Uint32 ini_milisegundos, fin_milisegundos, frametime;//Variables para el control de velocidad
int estado_juego=0;//usada en main.cpp only
char fpstxt[256];//main.cpp only
SDL_Joystick *joystick=NULL;
bool jugando,sonar=true,seleccion_nivel=false,pasar_a_siguiente_nivel=false;//sonar es de ramon solo, agonia!!
#else
// Constantes problemáticas
extern int seleccion_actual;
extern int COLOR_CLAVE;
extern int ALTO_NPCS[7];
extern int ANCHO_NPCS[7];
extern int VELOCIDADES_NPCS[7];
// Variables
extern SDL_Surface *screen,*imgfondo,*creditos,*controles;
extern int nivel,x_abs,tiempo,ciclosps,balaspropias_activas,balasenemigas_activas;
extern Uint32 ini_milisegundos, fin_milisegundos, frametime;//Variables para el control de velocidad
extern int estado_juego;
extern char fpstxt[];
extern SDL_Joystick *joystick;
extern bool jugando,sonar,seleccion_nivel,pasar_a_siguiente_nivel;
extern int TRonda;
#endif

