/*
  Autores:  Agustín Rodríguez
            Ramón Luque Luque
  Fecha: 17/11/2008
  Archivo: objetos.h
  Descripción: Definición de todos los objetos y estructuras necesarios para la
    el desarrollo del juego
*/
// Librerías necesarias

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include "main.h"
// Estructuras para el guardado de fondos antes de la escritura los personajes y NPCS
// para su posterior recuperación
struct tile {
  int x[MAX_TILES],y[MAX_TILES],dibujados,xabs_ant;
  bool recupera;
};

struct tileNPC {
  int x[TOTAL_NPCS],y[TOTAL_NPCS],dibujados;
  int tipo[TOTAL_NPCS];
};

struct tilepersonaje {
  int x,y;
};

struct tilebalaspropias {
  int x[TOTAL_BALAS_PROPIAS],y[TOTAL_BALAS_PROPIAS],dibujados;
};

struct tilebalasenemigas {
  int x[TOTAL_BALAS_ENEMIGAS],y[TOTAL_BALAS_ENEMIGAS],dibujados;
};

struct tileexplosiones {
  int x[TOTAL_EXPLOSIONES],y[TOTAL_EXPLOSIONES],dibujados;
};
struct Raton{
    int x;
    int y;
    bool pulsado;
};
struct tipo_cuadro {
    int x,y,w,h;
};
struct tipo_cuadro_dibujo {
    struct tipo_cuadro cuadro_sup,cuadro_inf,cuadro_cent;
};

// Definición de variables externas
class Sprite {
  private:
    int tipo;
    bool imagencargada;
    SDL_Surface *imagen;

    void cargarimagen(char *nombre);
  public:
    int anchoframe,altoframe,totalframes;
    struct tipo_cuadro_dibujo cuadros;
    Sprite(int xini1,int yini1,int wini1,int hini1,int xini2,int yini2,int wini2,int hini2,int xini3,int yini3,int wini3,int hini3);
    void tiposprite(int elemento,char *nombre);
    void liberarimagen(void);
    void frame_a_screen(int nroframe,int x,int y);
    // tipo=-4: explosión
    // tipo=-3: balas enemigas
    // tipo=-2: balas propias
    // tipo=-1: tiles de dibujado
    // tipo=0 : personaje
    // tipo>0 : NPCs
};

class Tipo_personaje { // Estructura básica del personaje del juego
  public:
    // Propiedades
    int x,y,xant,yant,xabs_ant,xsig,ysig,xabs_sig,xini,yini,vidas,vidas_anteriores,inerciax,inerciay,altura,puntos,apuntes,ciclosframe,frameactual,control_repeticion_disparos,c_disparos,posy_anterior;
    bool estado,llave,orientacion;
    Sprite *spriteasociado;
    // Métodos
    void sig_xy(void);
    void mueve_1(void);
    bool existe_movimiento(void);
    bool colisionNPCs(void);
    bool colisiontiles(void);
    bool colisionbalas(void);
};

class Tipo_NPC { // Estructura genérica para los NPC's
  public:
    // Propiedades
    bool dibujar,estado,atravesable,tipodesplazamiento,balalanzada,ejemovimiento;
    int tipoNPC,idx,idy,x,y,movx,rangoini,rangofin,direcciondisparo,periodobalas,
      puntos_off,tipoobjeto,ctdorvelocidad,vidas,frameactual,punto_xabs,xsig,ysig,xant,yant;
      // tipoobjeto: 0=llave
      //             1=vida
    Sprite *spriteasociado;
    // Métodos
    void updatemovx(void);
    void sig_xy(void);
    void mueve_1(void);
    bool existe_movimiento(void);
    bool colisiontiles(void);
    void colisionbalas(void);
    void colisionPlataformas(void);
};

class Tipo_Balas {
  public:
    // Propiedades
    bool activa,propia0_enemiga1,colisionada;
    int x,y,movx,tipoNPC,frameactual,contadorvelocidad,periodo,punto_xabs; // Punto_xabs define el valor de xabs cuando se crea la bala
    Sprite *spriteasociado;
    // Métodos
    void updatexy(void);
    void colisiontile(void);
};

struct Tipo_Explosion {
    // Propiedades
    Sprite *spriteasociado;
    int frameactual,x,y,vueltas,punto_xabs;
    bool activa;
};

class Mus_Efec {
  private:
    char ruta[50];
    Mix_Music *AddressMusic; // Si es del tipo de música de fondo
    Mix_Chunk *AddressChunk; // Si es del tipo efecto
    int canal;
  public:
    bool iniciado,pausado,Musica0_Efecto1;
    void cargar(bool Mus0_Efec1,char *ruta);
    void liberar(void);
    void play(void);
    void pause(void);
    void stop(void);
    void volumen(int vol); // Valor entre 0 y 128
};

class Barra_Control {
  public:
    // Propiedades
    float x,y,w,h,min,max,value;
    bool capturado;
    // Metodos
    void dibujar(void);
    void mover(void);
    bool comprobar_click(void);
};

struct bala_en_espera {
    int x, y, movx,tipoNPC,punto_xabs_bala,idxnpc,idynpc,tipoobjeto;
} ;

struct tipo_cola {
    void *contenido;
    struct tipo_cola *sig;
};

class clase_cola {
    private:
        struct tipo_cola *cola;
        int v_tamano;
    public:
        clase_cola(void);
        void meter(void *item);
        void sacar(void);
        int tamano(void);
        void *valor(void);
        bool vacia(void);
};

struct tipo_colision {
    bool col_sup,col_inf,col_drcha,col_izqda,colisionado;
};

class clase_colision {
    private:
        struct tipo_cuadro_dibujo *c1,*c2;  // Cuadros de colisión
                                            // 1: El que colisiona
                                            // 2: Con el que colisiona
        int px1,py1,px2,py2;                    // Posiciones x e y de los cuadros en la pantalla (no valores absolutos)
    public:
        struct tipo_colision col1,col2;

        clase_colision(void);
        void asignar_cuadro(struct tipo_cuadro_dibujo *c, int cuadro,int posx,int posy);
        void detectar(void);
};
struct elemento {
    void *datos;
    struct elemento *siguiente;
};

class Clase_Lista {
private:
    int tamano;
    struct elemento *lista;
    void Swap(void **ptr1,void **ptr2);
public:
    Clase_Lista();
    int Buscar_Cadena(char *cadena);
    void *valor_at(int posicion);
    void *nodo_at(int posicion);
    void borrar(int posicion);
    void insertar(void *objeto,int posicion);
    int tamano_actual(void);
    void vaciar(void);
    bool vacia(void);
    void ordenar_caracteres(void);
};
  Tipo_personaje *player;
  Tipo_NPC *npc[TOTAL_NPCS];
  Tipo_Balas *Balas_Enemigas[TOTAL_BALAS_ENEMIGAS];
  Tipo_Balas *Balas_Propias[TOTAL_BALAS_PROPIAS];
  Mus_Efec *sonido[TOTAL_SONIDOS];
  Barra_Control *b_sonido[2];
  struct tile tiles;
  struct tileNPC tilesNPC;
  struct tilepersonaje tilespersonaje;
  struct tilebalaspropias tilesbalaspropias;
  struct tilebalasenemigas tilesbalasenemigas;
  struct tileexplosiones tilesexplosiones;
  struct Tipo_Explosion explosiones[TOTAL_EXPLOSIONES];
  struct Raton mouse;
  Sprite *Sprite_tiles_estaticos;
  Sprite *Sprite_personaje;
  Sprite *Sprite_NPCs[NRO_NPCS_DEFINIDOS];
  Sprite *Sprite_bala_propia;
  Sprite *Sprite_bala_enemiga[NRO_BALAS_ENEMIGAS_DEFINIDAS];
  Sprite *Sprite_explosion;
  clase_cola *balas_esperando;
  clase_colision *colision;
  Clase_Lista *opciones;
