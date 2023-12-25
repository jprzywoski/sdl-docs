#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <string>
#include <dirent.h>
#include <vector>
#include <iostream>
#include "main.h"
using namespace std;
//Variables necesarias
int NPCs_activos=0,explosiones_activas=0;
int mapa[TOTAL_ANCHO_TILES][ALTO_PANTALLA_TILES],fondo=0;
TTF_Font *fuente;
TTF_Font *fuentemenu;
SDL_Surface *transicion;
SDL_Surface *mascara,*miniatura,*Spuntos,*Smuni;
SDL_Surface *fondo_menu,*fondo_menu_seleccion,*fin;
SDL_Surface *flechas;

enum tipo_tile {TILE_NINGUNO,TILE_TRANSPARENTE,TILE_SOLIDO,TILE_SOLIDO_DESLIZANTE,TILE_PLATAFORMA};

// Funciones del motor.cpp
void CargarImagenes(void);//Esta funcion se encargará de introducir las imagenes en los sprites.
void DibujarMenu(int actual);//Hecho
void DibujarEscena(void);
void MoverBalas(void);//Hecho
void Inicializar(void);//Hecho. Esta funcion debe de encargarse de establecer todos los valores de las estructuras.
void iniciarnivel(void); // Hecho.
void Finalizar(void); // Hecho. Esta función se encarga de destruir todas las variables creadas inicialmente de forma dinámica.
void CargarMapa(char *nivel);//Hecho
void Mover_elementos(void);//Hecho
void CrearDisparoPersonaje(void);//Hecho
void Texto(char *cadena,int r, int g, int b);//Hecho
void ResetTimeBase(void);//Hecho
int CurrentTime(void);//Hecho
void Controles(Uint8 keys[]);//Hecho
void DibujarExplosiones(void);//Hecho
void DibujarNPCs(void);//Hecho
void DibujarBalas(void);//Hecho
void Desactivar_Ocultos(void);
void CambiarModoPantalla(void);
void Activar_NPCs_Visibles(void);
void CrearDisparoEnemigo(void);
void Desactivar_NPCs(void);
tipo_tile TileInferior(void);
// Inicializar, guardar y recuperar los tiles guardados
void inicializarfondos (void);//Hecho
// El guadado de tiles está implementada en el método frame_a_screen por tema de eficiencia
void recuperartiles(void);//Hecho
void recuperartilesNPC(void);//Hecho
void recuperartilespersonaje(void);//Hecho
void recuperartilesbalaspropias(void);//Hecho
void recuperartilesbalasenemigas(void);//Hecho
void recuperartilesexplosiones(void);//Hecho
void recuperartodostiles(void);//Hecho
void DibujarVidas(int posx,int posy);
void GenerarLista(void);
int getdir (string dir, vector<string> &files);
void generamunicion();
