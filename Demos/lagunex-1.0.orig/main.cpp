//Librerias SDL:
#define MAIN

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include "main.h"
#include <time.h>
#ifdef WIN32
    #include <dir.h>
#endif

// Definidos en objetos.h
class Sprite {
private:
    int tipo;
    bool imagencargada;
    SDL_Surface *imagen;

    void cargarimagen(char *nombre);
public:
    int anchoframe,altoframe,totalframes;

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

struct Raton {
    int x;
    int y;
    bool pulsado;
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
    int x, y, movx,tipoNPC,punto_xabs_bala,idxnpc,idynpc;
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
class Clase_Lista {
private:
    int tamano;
    void *lista;
public:
    Clase_Lista();
    int Buscar_Cadena(char *cadena);
    void *valor_at(int posicion);
    void borrar(int posicion);
    void insertar(void *objeto,int posicion);
    int tamano_actual(void);
    void vaciar(void);
    bool vacia(void);
};
extern Tipo_personaje *player;
extern   struct Raton mouse;
extern Barra_Control *b_sonido[2];
extern Mus_Efec *sonido[TOTAL_SONIDOS];

// Fin de definidos en objetos.h ********************************************************

// Definidos en motor.h
void Inicializar(void);
void CargarImagenes(void);
void DibujarMenu(int actual);
void CargarMapa(char *nivel);
void ResetTimeBase(void);
void DibujarEscena(void);
void MoverBalas(void);
void Mover_elementos(void);
void Controles(Uint8 keys[]);
int CurrentTime(void);
void iniciarnivel(void);
void Texto(char *cadena,int r, int g, int b);
void Finalizar(void);
void GenerarLista(void);
// Fin de definidos en motor.h ********************************************************
extern clase_cola *balas_esperando;
extern Clase_Lista *opciones;

using namespace std;

#ifdef WIN32
void mueve_a_dir_trabajo(char pathapp[]) {
    int i, longitud=strlen(pathapp);
    char path[255];

    // Buscamos hasta qué punto llega el path
    for (i=longitud-1; ((i>=0) && (pathapp[i]!='\\') && (pathapp[i]!='/')); i--) ;
    if (i>0) {
        strncpy(path,pathapp,i);
        chdir(path);
        printf("%s",path);
    }
}
#endif

int main(int argc, char *argv[]) {
    srand((unsigned int) time(NULL));
    SDL_Event event;//Para capturar los distintos eventos
    Uint8 *keys;
        int done=0;//Si en algun momento cambia de 0, se saldrá del programa.
    //estado_juego 0 indica menu, estado 1, jugando y estado 2 es en pausa

    #ifdef WIN32
    mueve_a_dir_trabajo(argv[0]); // Path completo de nuestra aplicación, incluida el ejecutable
    #endif
    if (SDL_Init(SDL_INIT_EVERYTHING)<0) { //Si retorna -1 la inicalizacion, cerramos.
        printf("Error al iniciar: %s",SDL_GetError());
        return 1;
    }

    SDL_putenv("SDL_VIDEO_CENTERED=center");
    screen = SDL_SetVideoMode(ANCHO_PANTALLA,ALTO_PANTALLA,16,SDL_HWSURFACE | SDL_DOUBLEBUF);
    //Creamos la pantalla (800x600, a 16 bits),
    //usamos buffer doble sobre la tarjeta gráfica.

    if (screen==NULL) { //Si no se ha podido crear la imagen devolvemos error:
        printf("Error al inicalizar los graficos: \n %s",SDL_GetError());
        return 1;
    }
    SDL_WM_SetCaption( "Lagunex", NULL);
    atexit(SDL_Quit);//En el caso de que se produzca un error, liberamos la memoria.

    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY,MIX_DEFAULT_FORMAT,0,512)<0) {
        printf("No se ha podido inicializar el audio: %s",Mix_GetError());
        //return 1;
    }

    atexit(Mix_CloseAudio);
    if (TTF_Init() <0) {
        printf("No se pudo iniciar la libreria de textos: %s",SDL_GetError());
        //return 1;
    }
    atexit(TTF_Quit);
    // Activa el Joystick en caso de que exista
    if (SDL_NumJoysticks() >= 1) {
        joystick = SDL_JoystickOpen(0);
        SDL_JoystickEventState(SDL_ENABLE);
    };

    Inicializar();
    CargarImagenes();
    //Bucle02 principal
    int i;
    bool salir=false;
    while (done==0) {

        switch (estado_juego) {
        case 0: //En el caso de que estemos en el menu, lo dibujamos
            //Esta funcion se encargará de mostrar la imagen menu en pantalla
            sonido[4]->play();
            while (estado_juego==0 && done==0) {
                keys=SDL_GetKeyState(NULL);
                SDL_Delay(50);
                while (SDL_PollEvent(&event)) { //Mientras se haya producido un evento
                    if (event.type == SDL_QUIT)
                        done=1;
                    if (event.type == SDL_MOUSEMOTION) {
                        mouse.x = event.motion.x;
                        mouse.y = event.motion.y;
                        DibujarMenu(seleccion_actual);
                    }

                    if (event.type== SDL_MOUSEBUTTONDOWN)
                        mouse.pulsado=true;

                    if (event.type== SDL_MOUSEBUTTONUP) {
                        mouse.pulsado=false;
                        for (i=0;i<2;i++)
                            b_sonido[i]->capturado=false;
                    }

                    else if (event.type == SDL_KEYDOWN) { //Si se el evento es la pulsacion de una tecla...
                        if (!seleccion_nivel) {
                            if (event.key.keysym.sym == SDLK_ESCAPE) {
                                done=1;
                            } else if (keys[SDLK_DOWN] && seleccion_actual<TOTAL_OPCIONES-1) {
                                seleccion_actual++;
                            } else if (keys[SDLK_UP] && seleccion_actual>0) {
                                seleccion_actual--;

                            } else if (keys[SDLK_RETURN]) {
                                SDL_ShowCursor(SDL_DISABLE);
                                if (seleccion_actual==0) { //Si es iniciar juego...
                                    estado_juego=1;
                                    nivel=1;
                                    CargarMapa("");
                                } else if (seleccion_actual==1) { //mapas
                                    GenerarLista();
                                    seleccion_nivel=true;
                                    seleccion_actual=4;
                                    if (opciones->tamano_actual()<seleccion_actual)
                                        seleccion_actual=opciones->tamano_actual()/2;

                                } else if (seleccion_actual==2){//controles
                                    salir=false;
                                    SDL_Rect desti2;desti2.x=0;desti2.y=0;
                                    SDL_BlitSurface(controles,NULL,screen,&desti2);
                                    SDL_Flip(screen);
                                    while(!salir)
                                        while (SDL_PollEvent(&event)) {
                                            SDL_Delay(20);
                                            if (event.key.keysym.sym == SDLK_ESCAPE) {
                                                salir=true;
                                            }
                                        }

                                } else if (seleccion_actual==3){//creditos
                                    salir=false;
                                    SDL_Rect desti2;desti2.x=0;desti2.y=0;
                                    SDL_BlitSurface(creditos,NULL,screen,&desti2);
                                    SDL_Flip(screen);
                                    while(!salir)
                                        while (SDL_PollEvent(&event)) {
                                            SDL_Delay(20);
                                            if (event.key.keysym.sym == SDLK_ESCAPE) {
                                                salir=true;
                                            }
                                        }
                                }
                                else
                                    done=1;

                            }
                        }
                        else {
                            if (event.key.keysym.sym == SDLK_ESCAPE) {
                                seleccion_nivel=false;
                                seleccion_actual=1;
                            } else if (keys[SDLK_DOWN] && seleccion_actual<opciones->tamano_actual()-1) {
                                seleccion_actual++;
                            } else if (keys[SDLK_UP] && seleccion_actual>0) {
                                seleccion_actual--;
                            }
                            else if (keys[SDLK_RETURN]) {
                                SDL_ShowCursor(SDL_DISABLE);
                                estado_juego=1;
                                nivel=0;
                                CargarMapa((char*)opciones->valor_at(seleccion_actual));
                                seleccion_nivel=false;
                                seleccion_actual=1;

                            }
                        }
                    }
                }
                DibujarMenu(seleccion_actual);
            }
            break;
        case 1:
            // Dibujado del fondo
            static SDL_Rect dest;
            int tinicio,tseg=0;

            dest.x=0;
            dest.y=0;
            tiempo=0;
            SDL_BlitSurface(imgfondo,NULL,screen,&dest);
            ciclosps=0;
            keys=SDL_GetKeyState(NULL);
            sonido[4]->stop();
            sonido[0]->play();
            jugando=true;
            player->vidas=3;
            player->vidas_anteriores=3;
            player->puntos=0;
            iniciarnivel();
            ResetTimeBase();
            TRonda=SDL_GetTicks();
            while ((estado_juego==1) && (jugando)) {
                tinicio=CurrentTime();
                DibujarEscena();
                while (SDL_PollEvent(&event)) { //Mientras se haya producido un evento
                    if (event.type == SDL_QUIT) {
                        done=1;
                    }
                }
                keys=SDL_GetKeyState(NULL);
                Controles(keys);
                Mover_elementos();

                if (keys[SDLK_ESCAPE]) // Salir de partida
                    jugando=false;

                if (!jugando) {
                    estado_juego=0;
                    SDL_ShowCursor(SDL_ENABLE);
                    sonido[0]->stop();
                }
                //Control frame rate EFICIENTE AL 1000%
                tiempo=CurrentTime();
                ciclosps++;
                tseg+=(tiempo-tinicio);
                if ( (tiempo-tinicio) < 1000/FPS ) //Control de FPS (60 fps)
                    SDL_Delay( 1000/FPS - (tiempo-tinicio) );
                tseg+=(CurrentTime()-tinicio)-(tiempo-tinicio) ;
                if ((tseg>=1000) && (jugando)) {
                    sprintf(fpstxt,"HWSurface: %s PUNTOS: %d BALAS: %d",
                            screen->flags && SDL_HWSURFACE?"SI":"NO",player->puntos,5-player->c_disparos);
                    //Texto(fpstxt,0,200,50);
                    tseg=0;
                }
                if (ciclosps>4000)
                    ciclosps=0;
            }
            break;
        }
    }
    Finalizar();
    return 0;
}


