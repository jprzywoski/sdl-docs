#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include "main.h"
#include "motor.h"

// Definidas en objetos.h ***************************************************
struct tile
{
    int x[MAX_TILES],y[MAX_TILES],dibujados,xabs_ant;
    bool recupera;
};

struct tileNPC
{
    int x[TOTAL_NPCS],y[TOTAL_NPCS],dibujados;
    int tipo[TOTAL_NPCS];
};

struct tilepersonaje
{
    int x,y;
};

struct tilebalaspropias
{
    int x[TOTAL_BALAS_PROPIAS],y[TOTAL_BALAS_PROPIAS],dibujados;
};

struct tilebalasenemigas
{
    int x[TOTAL_BALAS_ENEMIGAS],y[TOTAL_BALAS_ENEMIGAS],dibujados;
};

struct tileexplosiones
{
    int x[TOTAL_EXPLOSIONES],y[TOTAL_EXPLOSIONES],dibujados;
};
struct Raton
{
    int x;
    int y;
    bool pulsado;
};

struct tipo_cuadro
{
    int x,y,w,h;
};
struct tipo_cuadro_dibujo
{
    struct tipo_cuadro cuadro_sup,cuadro_inf,cuadro_cent;
};

class Sprite
{
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

class Tipo_personaje   // Estructura básica del personaje del juego
{
public:
    // Propiedades
    int x,y,xant,yant,xabs_ant,xsig,ysig,xabs_sig,xini,yini,vidas,vidas_anteriores,inerciax,inerciay,altura,puntos,apuntes,ciclosframe,frameactual,control_repeticion_disparos,c_disparos,posy_anterior;
    bool estado,llave,orientacion;
    Sprite *spriteasociado;
    // Métodos
    void sig_xy(void);
    bool existe_movimiento(void);
    void mueve_1(void);
    bool colisionNPCs(void);
    bool colisiontiles(void);
    bool colisionbalas(void);
};

class Tipo_NPC   // Estructura genérica para los NPC's
{
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
    bool existe_movimiento(void);
    void mueve_1(void);
    bool colisiontiles(void);
    void colisionbalas(void);
    void colisionPlataformas(void);
};

class Clase_Lista
{
private:
    int tamano;
    void *lista;
    void Swap(void **ptr1,void **ptr2);
public:
    Clase_Lista();
    int Buscar_Cadena(char *cadena);
    void *valor_at(int posicion);
    void borrar(int posicion);
    void insertar(void *objeto,int posicion);
    int tamano_actual(void);
    void vaciar(void);
    bool vacia(void);
    void ordenar_caracteres(void);
};

class Tipo_Balas
{
public:
    // Propiedades
    bool activa,propia0_enemiga1,colisionada;
    int x,y,movx,tipoNPC,frameactual,contadorvelocidad,periodo,punto_xabs; // Punto_xabs define el valor de xabs cuando se crea la bala
    Sprite *spriteasociado;
    // Métodos
    void updatexy(void);
    void colisiontile(void);
};

struct Tipo_Explosion
{
    // Propiedades
    Sprite *spriteasociado;
    int frameactual,x,y,vueltas,punto_xabs;
    bool activa;
};

class Mus_Efec
{
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

class Barra_Control
{
public:
    // Propiedades
    float x,y,w,h,min,max,value;
    bool capturado;
    // Metodos
    void dibujar(void);
    void mover(void);
    bool comprobar_click(void);
};

struct bala_en_espera
{
    int x, y, movx,tipoNPC,punto_xabs_bala,idxnpc,idynpc,tipoobjeto;
} ;

struct tipo_cola
{
    void *contenido;
    struct tipo_cola *sig;
};

class clase_cola
{
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

struct tipo_colision
{
    bool col_sup,col_inf,col_drcha,col_izqda,colisionado;
};

class clase_colision
{
private:
    struct tipo_cuadro_dibujo *c1,*c2;  // Cuadros de colisión
    // 1: El que colisiona
    // 2: Con el que colisiona
    int px1,py1,px2,py2;                // Posiciones x e y de los cuadros en la pantalla (no valores absolutos)
public:
    struct tipo_colision col1,col2;

    clase_colision(void);
    void asignar_cuadro(struct tipo_cuadro_dibujo *c, int cuadro,int posx,int posy);
    void detectar(void);
};
// Definicion de variables externas
extern Tipo_personaje *player;
extern Tipo_NPC *npc[TOTAL_NPCS];
extern Tipo_Balas *Balas_Enemigas[TOTAL_BALAS_ENEMIGAS];
extern Tipo_Balas *Balas_Propias[TOTAL_BALAS_PROPIAS];
extern Mus_Efec *sonido[TOTAL_SONIDOS];
extern Barra_Control *b_sonido[2];
extern struct tile tiles;
extern struct tileNPC tilesNPC;
extern struct tilepersonaje tilespersonaje;
extern struct tilebalaspropias tilesbalaspropias;
extern struct tilebalasenemigas tilesbalasenemigas;
extern struct tileexplosiones tilesexplosiones;
extern struct Tipo_Explosion explosiones[TOTAL_EXPLOSIONES];
extern struct Raton mouse;
extern Sprite *Sprite_tiles_estaticos;
extern Sprite *Sprite_personaje;
extern Sprite *Sprite_NPCs[NRO_NPCS_DEFINIDOS];
extern Sprite *Sprite_bala_propia;
extern Sprite *Sprite_bala_enemiga[NRO_BALAS_ENEMIGAS_DEFINIDAS];
extern Sprite *Sprite_explosion;
extern clase_cola *balas_esperando;
extern clase_colision *colision;
extern Clase_Lista *opciones;
// Fin de definidas en objetos.h *************************************************

void CargarImagenes(void)
{
    int i;
    char auxcad[50];

    SDL_Surface *temporal,*temporal2,*temporal3,*temporal4,*tmp5,*tmp6,*tmp7;
    // Sprites estáticos
    strcpy(auxcad,DIR_CONTENIDO);
    strcat(auxcad,"/menu.jpg");
    fondo_menu=IMG_Load(auxcad);
    strcpy(auxcad,DIR_CONTENIDO);
    strcat(auxcad,"/controles.jpg");
    controles=IMG_Load(auxcad);
    strcpy(auxcad,DIR_CONTENIDO);
    strcat(auxcad,"/menu_nivel.jpg");
    fondo_menu_seleccion=IMG_Load(auxcad);
    Sprite_tiles_estaticos->tiposprite(-1,"imagenes_nivel.bmp");
    // Sprite del personaje
    Sprite_personaje->tiposprite(0,"personaje.bmp");
    player->spriteasociado=Sprite_personaje; // Asignación del personaje

    //Sprite de una explosion
    Sprite_explosion->tiposprite(-4,"explopsion.bmp");
    for (i=0; i<TOTAL_EXPLOSIONES; i++) // Asignamos aquí el sprite de la explosión ya que es único en todas las explosiones
        explosiones[i].spriteasociado=Sprite_explosion;
    //Sprite de una bala propia
    Sprite_bala_propia->tiposprite(-2,"proyectil_propio.bmp");
    for (i=0;i<TOTAL_BALAS_PROPIAS;i++) // Asignamo aquí el sprite de la bala ya que es única en el personaje
        Balas_Propias[i]->spriteasociado=Sprite_bala_propia;

    for (i=0; i<NRO_BALAS_ENEMIGAS_DEFINIDAS; i++)
    {

        switch(i) {
            case 0 : // Bala general
                Sprite_bala_enemiga[i]->tiposprite(-3,"bala.bmp");
                break;
            case 1 : // Bala de Arturo
                Sprite_bala_enemiga[i]->tiposprite(-3,"proyectil_dfd.bmp");
                break;
            case 2 : // Bala de Lázaro
                Sprite_bala_enemiga[i]->tiposprite(-3,"proyectil_lazaro.bmp");
                break;
            case 3 : // Bala de Isabel
                Sprite_bala_enemiga[i]->tiposprite(-3,"proyectil_isabel.bmp");
                break;
            case 4 : // Bala de Máximo
                Sprite_bala_enemiga[i]->tiposprite(-3,"ubuntu_logo.bmp");
                break;
        }
    }

    //Sprite de los NPCs
    for (i=1; i<=NRO_NPCS_DEFINIDOS; i++)
    {
        sprintf(auxcad,"npc%d.bmp",i); // Comienza por el 1
        Sprite_NPCs[i-1]->tiposprite(i,auxcad);
    }

    // Carga de imagen de fondo;
    int nfondo=(rand()%4)+1;
    sprintf(auxcad,"%s/fondo%d.jpg",DIR_CONTENIDO,nfondo);
    tmp5=IMG_Load(auxcad);
    imgfondo=SDL_DisplayFormat(tmp5);
    // Carga de imagen de miniatura
    strcpy(auxcad,DIR_CONTENIDO);
    strcat(auxcad,"/miniatura_vidas.bmp");
    temporal2=SDL_LoadBMP(auxcad);
    SDL_SetColorKey(temporal2,SDL_SRCCOLORKEY,SDL_MapRGB(temporal2->format,255,0,0));
    miniatura=SDL_DisplayFormat(temporal2);
    strcpy(auxcad,DIR_CONTENIDO);
    strcat(auxcad,"/flechas.bmp");
    temporal=SDL_LoadBMP(auxcad);
    SDL_SetColorKey(temporal,SDL_SRCCOLORKEY,SDL_MapRGB(temporal->format,255,0,0));
    flechas=SDL_DisplayFormat(temporal);

    strcpy(auxcad,DIR_CONTENIDO);
    strcat(auxcad,"/creditos.jpg");
    temporal3=IMG_Load(auxcad);
    creditos=SDL_DisplayFormat(temporal3);

    strcpy(auxcad,DIR_CONTENIDO);
    strcat(auxcad,"/felicitacion.jpg");
    temporal4=IMG_Load(auxcad);

    strcpy(auxcad,DIR_CONTENIDO);
    strcat(auxcad,"/puntos.bmp");
    tmp6=SDL_LoadBMP(auxcad);
    SDL_SetColorKey(tmp6,SDL_SRCCOLORKEY,SDL_MapRGB(tmp6->format,255,0,0));
    Spuntos=SDL_DisplayFormat(tmp6);

    strcpy(auxcad,DIR_CONTENIDO);
    strcat(auxcad,"/ammo_mini.bmp");
    tmp7=SDL_LoadBMP(auxcad);
    SDL_SetColorKey(tmp7,SDL_SRCCOLORKEY,SDL_MapRGB(tmp7->format,255,0,0));
    Smuni=SDL_DisplayFormat(tmp7);

    fin=SDL_DisplayFormat(temporal4);
    SDL_FreeSurface(temporal);
    SDL_FreeSurface(temporal2);
    SDL_FreeSurface(temporal3);
    SDL_FreeSurface(temporal4);
    SDL_FreeSurface(tmp5);
    SDL_FreeSurface(tmp6);
    SDL_FreeSurface(tmp7);
}

void DibujarMenu(int actual)
{
    int i,j;
    int rangoini,rangofin;
    bool verificacion=false;
    char opcionesmenu[TOTAL_OPCIONES][30];
    SDL_Surface *texto;
    SDL_Color bgcolor,fgcolor;
    SDL_Rect dest;

    strcpy(opcionesmenu[0]," Iniciar juego ");
    strcpy(opcionesmenu[1]," Niveles");
    strcpy(opcionesmenu[2]," Controles ");
    strcpy(opcionesmenu[3]," Creditos ");
    strcpy(opcionesmenu[4]," Salir ");
    dest.x=0;
    dest.y=0;
    bgcolor.r=0;
    bgcolor.g=150;
    bgcolor.b=201;


    if (!seleccion_nivel)
    {
        SDL_BlitSurface(fondo_menu,NULL,screen,0);
        for (i=0;i<2;i++)
        {
            for (j=0;j<2;j++)
            {
                if ((j!=i) && b_sonido[j]->capturado)
                    verificacion=true;
            }
            if (!verificacion && b_sonido[i]->comprobar_click())
                b_sonido[i]->mover();
            verificacion=false;
            sonido[i]->volumen(int(b_sonido[i]->value));//sonido 0 musica, sonido >0 efectos
        }

        for (i=0;i<2;i++)
            b_sonido[i]->dibujar();
        for (i=0;i<TOTAL_OPCIONES;i++)
        {
            if (i==actual)
            {
                fgcolor.r=0;
                fgcolor.g=200;
                fgcolor.b=100;
            }
            else
            {

                fgcolor.r=0;
                fgcolor.g=150;
                fgcolor.b=200;
            }

            texto=TTF_RenderText_Blended(fuentemenu,opcionesmenu[i],fgcolor);

            if (i<1)
                dest.y=i*40+80;
            else
                dest.y=i*40+220;
            dest.x=250;
            SDL_BlitSurface(texto,NULL,screen,&dest);
            SDL_FreeSurface(texto);
        }
    }
    else
    {
        //
            SDL_BlitSurface(fondo_menu_seleccion,NULL,screen,0);
        bool flecha_superior=false,flecha_inferior=false;;
        if (seleccion_actual-4>=0)
        {
            rangoini=seleccion_actual-4;
            flecha_superior=true;
        }
        else
            rangoini=0;
        rangofin=seleccion_actual;
        //
        for (i=rangoini;i<rangofin;i++)
        {
            fgcolor.r=0;
            fgcolor.g=150;
            fgcolor.b=200;
            texto=TTF_RenderText_Shaded(fuentemenu,(char *)opciones->valor_at(i),fgcolor,bgcolor);
            dest.y=(5-(seleccion_actual-i))*40+80;
            dest.x=250;
            SDL_SetColorKey(texto,SDL_SRCCOLORKEY|SDL_RLEACCEL, SDL_MapRGB(texto->format,0,150,201));
            SDL_SetAlpha(texto,SDL_SRCALPHA|SDL_RLEACCEL,60*(i-seleccion_actual));//ponemos el alpha...
            SDL_BlitSurface(texto,NULL,screen,&dest);
            SDL_FreeSurface(texto);
        }
        //dibujamos la seleccion actual
        fgcolor.r=0;
        fgcolor.g=200;
        fgcolor.b=100;
        texto=TTF_RenderText_Blended(fuentemenu,(char *)opciones->valor_at(seleccion_actual),fgcolor);
        dest.y=(5)*40+80;
        dest.x=250;


        SDL_BlitSurface(texto,NULL,screen,&dest);
        SDL_FreeSurface(texto);
        //dibujamos las 4 delanteras
        if (seleccion_actual+4<opciones->tamano_actual())
        {
            rangofin=seleccion_actual+4;
            flecha_inferior=true;
        }
        else
            rangofin=opciones->tamano_actual()-1;
        rangoini=seleccion_actual+1;
        //
        for (i=rangoini;i<=rangofin;i++)
        {
            fgcolor.r=0;
            fgcolor.g=150;
            fgcolor.b=200;

            texto=TTF_RenderText_Shaded(fuentemenu,(char *)opciones->valor_at(i),fgcolor,bgcolor);

            dest.y=(5+(i-seleccion_actual))*40+80;
            dest.x=250;
            SDL_SetColorKey(texto,SDL_SRCCOLORKEY|SDL_RLEACCEL, SDL_MapRGB(texto->format,0,150,201));
            SDL_SetAlpha(texto,SDL_SRCALPHA|SDL_RLEACCEL,60*(seleccion_actual-i));//ponemos el alpha...
            SDL_BlitSurface(texto,NULL,screen,&dest);
            SDL_FreeSurface(texto);
        }
        //dibujamos las flechas
        SDL_Rect rflechas,forigen;
        if (flecha_superior)
        {
            rflechas.x=450;
            rflechas.y=200;
            forigen.x=0;
            forigen.y=0;
            forigen.w=32;
            forigen.h=35;
            SDL_BlitSurface(flechas,&forigen,screen,&rflechas);


        }

        if (flecha_inferior)
        {
            rflechas.x=450;
            rflechas.y=350;
            forigen.x=0;
            forigen.y=85;
            forigen.w=32;
            forigen.h=35;
            SDL_BlitSurface(flechas,&forigen,screen,&rflechas);
        }
    }

    SDL_Flip(screen);
}

void DibujarExplosiones()
{
    int i,b;

    for (i=0; i<TOTAL_NPCS; i++)   //Recorremos todos los npc's para ver si hay algun npc
    {
        if (npc[i]->dibujar && !npc[i]->estado)   //Si esta dibujado pero esta muerto
        {
            npc[i]->dibujar=false;
            for (b=0;b<TOTAL_EXPLOSIONES;b++)//buscamos explosion libre
                if (!explosiones[b].activa)   //si hay una libre
                {
                    explosiones[b].activa=true;//la activamos y le damos la posicion del npc
                    explosiones[b].x=npc[i]->x;
                    explosiones[b].y=npc[i]->y;
                    explosiones[b].punto_xabs=x_abs;
                    explosiones_activas++;
                    b=TOTAL_EXPLOSIONES;
                    sonido[2]->stop();
                    sonido[3]->stop();
                    sonido[3]->play();
                }
        }
        if (i<TOTAL_BALAS_ENEMIGAS)
            if (Balas_Enemigas[i]->activa && Balas_Enemigas[i]->colisionada)  //Si hay una bala activa y colisionada
            {
                Balas_Enemigas[i]->activa=false;
                Balas_Enemigas[i]->colisionada=false;
                for (b=0;b<TOTAL_EXPLOSIONES;b++)//buscamos explosion libre
                    if (!explosiones[b].activa)   //si hay una libre
                    {
                        explosiones[b].activa=true;//la activamos y le damos la posicion del npc
                        explosiones[b].x=Balas_Enemigas[i]->x;
                        explosiones[b].y=Balas_Enemigas[i]->y-10;
                        explosiones[b].punto_xabs=x_abs;
                        explosiones_activas++;
                        b=TOTAL_EXPLOSIONES;
                        sonido[2]->stop();
                        sonido[3]->stop();
                        sonido[3]->play();
                    }
            }
        if (i<TOTAL_BALAS_PROPIAS)
            if (Balas_Propias[i]->activa && Balas_Propias[i]->colisionada)  //Si hay una bala activa y colisionada
            {
                Balas_Propias[i]->activa=false;
                Balas_Propias[i]->colisionada=false;
                for (b=0;b<TOTAL_EXPLOSIONES;b++)//buscamos explosion libre
                    if (!explosiones[b].activa)   //si hay una libre
                    {
                        explosiones[b].activa=true;//la activamos y le damos la posicion del npc
                        explosiones[b].x=Balas_Propias[i]->x;
                        explosiones[b].y=Balas_Propias[i]->y-10;
                        explosiones[b].punto_xabs=x_abs;
                        explosiones_activas++;
                        b=TOTAL_EXPLOSIONES;
                        sonido[2]->stop();
                        sonido[3]->stop();
                        sonido[3]->play();
                    }
            }
    }

    for (i=0; i<TOTAL_EXPLOSIONES; i++)//Recorremos las explosiones y si hay alguna activa la dibujamos y movemos frames
        if (explosiones[i].activa)
        {
            Sprite_explosion->frame_a_screen(explosiones[i].frameactual,explosiones[i].x+(explosiones[i].punto_xabs-x_abs),explosiones[i].y);
            explosiones[i].frameactual++;
            if (explosiones[i].frameactual>=Sprite_explosion->totalframes)
            {
                explosiones[i].activa=false;
                explosiones[i].frameactual=0;
                explosiones_activas--;
            }
        }
}

void DibujarNPCs()
{
    int i;
    for (i=0;i<TOTAL_NPCS;i++)
        if (npc[i]->dibujar && npc[i]->estado)
            npc[i]->spriteasociado->frame_a_screen(npc[i]->frameactual,npc[i]->x,npc[i]->y);
}
void DibujarBalas()
{
    int i;
    //mias
    for (i=0;i<TOTAL_BALAS_PROPIAS;i++)
        if (Balas_Propias[i]->activa)
        {
            Balas_Propias[i]->spriteasociado->frame_a_screen(Balas_Propias[i]->frameactual,Balas_Propias[i]->x,Balas_Propias[i]->y);
            if (!(ciclosps%5))
                Balas_Propias[i]->frameactual++;
            if (Balas_Propias[i]->frameactual>2)
                Balas_Propias[i]->frameactual=0;
        }
    //enemigas
    for (i=0;i<TOTAL_BALAS_ENEMIGAS;i++)
        if (Balas_Enemigas[i]->activa)
            if (Balas_Enemigas[i]->tipoNPC==3)
                if(Balas_Enemigas[i]->movx<0)
                    Balas_Enemigas[i]->spriteasociado->frame_a_screen(0,Balas_Enemigas[i]->x,Balas_Enemigas[i]->y);
                else
                    Balas_Enemigas[i]->spriteasociado->frame_a_screen(1,Balas_Enemigas[i]->x,Balas_Enemigas[i]->y);
            else
                Balas_Enemigas[i]->spriteasociado->frame_a_screen(0,Balas_Enemigas[i]->x,Balas_Enemigas[i]->y);
}

void DibujarEscena(void)
{
    static bool fadeout;
    SDL_Rect rectpantalla;
    rectpantalla.x=0;
    rectpantalla.y=0;
    rectpantalla.h=screen->h-ALTO_TILE;
    rectpantalla.w=screen->w;
    //Dibujamos los tiles:
    int horizontal,vertical,tile_inicial=0,tile_final=0,contador;


    Activar_NPCs_Visibles();
    Desactivar_Ocultos();
    generamunicion();
    //Posiciones de la camara
    tile_inicial=x_abs/ANCHO_TILE;
    if (x_abs%ANCHO_TILE)
        tile_final=tile_inicial+ANCHO_PANTALLA_TILES+1;
    else
        tile_final=tile_inicial+ANCHO_PANTALLA_TILES;
    //Dibujamos los tiles:
    for (horizontal=tile_inicial; horizontal<=tile_final; horizontal++)
        for (vertical=0;vertical<ALTO_PANTALLA_TILES;vertical++)
            if ((mapa[horizontal][vertical]!=0) && ( ((mapa[horizontal][vertical]>0) && (mapa[horizontal][vertical]<70)) ))
                Sprite_tiles_estaticos->frame_a_screen(mapa[horizontal][vertical],(horizontal*ANCHO_TILE)-x_abs,vertical*ALTO_TILE);
            else if (mapa[horizontal][vertical]==90)
            {
                player->xini=horizontal*ANCHO_TILE;
                player->yini=vertical*ALTO_TILE;
            }
    player->spriteasociado->frame_a_screen(player->frameactual,player->x,player->y);//dibujamos al jugador
    DibujarBalas();
    DibujarNPCs();
    DibujarExplosiones();




    // Dibujar miniatura
    if (fadeout)   //Y se hizo la luz
    {
        transicion=SDL_DisplayFormat(screen);
        contador=255;
        mascara=SDL_CreateRGBSurface(SDL_SWSURFACE,ANCHO_PANTALLA,ALTO_PANTALLA,16,0,0,0,0);
        while (contador>16)
        {
            contador-=16;
            SDL_BlitSurface(transicion,&rectpantalla,screen,&rectpantalla);
            SDL_SetAlpha(mascara,SDL_SRCALPHA|SDL_RLEACCEL,contador);//ponemos el alpha...
            SDL_BlitSurface(mascara,&rectpantalla,screen,&rectpantalla);//bliteamos
            SDL_Flip(screen);
            SDL_Delay(20);
        }
        SDL_BlitSurface(transicion,&rectpantalla,screen,&rectpantalla);
        SDL_FreeSurface(mascara);
        SDL_FreeSurface(transicion);
    }
    else
    {
        SDL_Flip(screen);
    }

    fadeout=false;
    if (player->vidas>=0)
    {
        if (pasar_a_siguiente_nivel) {
            if (nivel==0) {
                jugando=false;
                pasar_a_siguiente_nivel=false;
            }
            nivel++;
            if (nivel<5) {
                int nfondo=(rand()%4)+1;
                char auxcad[50];
                SDL_FreeSurface(imgfondo);
                SDL_Surface *tmp5;
                SDL_Color fgcolor;fgcolor.r=100;fgcolor.g=200;fgcolor.b=100;
                SDL_Rect des2;des2.x=200;des2.y=250;
                TRonda=(SDL_GetTicks()-TRonda)/1000;
                sprintf(auxcad,"Nivel completado en %d segundos",TRonda);

                tmp5=TTF_RenderText_Blended(fuentemenu,auxcad,fgcolor);
                SDL_FillRect(screen,0,0);
                SDL_BlitSurface(tmp5,NULL,screen,&des2);
                SDL_FreeSurface(tmp5);
                SDL_Flip(screen);
                sprintf(auxcad,"%s/fondo%d.jpg",DIR_CONTENIDO,nfondo);
                tmp5=IMG_Load(auxcad);
                imgfondo=SDL_DisplayFormat(tmp5);
                SDL_FreeSurface(tmp5);
                SDL_Delay(2000);
                SDL_BlitSurface(imgfondo,NULL,screen,0);
                Desactivar_NPCs();
                CargarMapa("");
                TRonda=SDL_GetTicks();
                if (player->vidas<3)
                    player->vidas=3;
            }
            else {
                jugando=false;
                pasar_a_siguiente_nivel=false;
                SDL_BlitSurface(fin,NULL,screen,NULL);
                SDL_Flip(screen);
                SDL_Delay(10000);
            }
        }
        if ((player->vidas<player->vidas_anteriores || pasar_a_siguiente_nivel) && jugando)
        {
            pasar_a_siguiente_nivel=false;
            iniciarnivel();
            player->vidas_anteriores=player->vidas;
            transicion=SDL_DisplayFormat(screen);//guardamamos una copia de la pantalla
            contador=0;
            mascara=SDL_CreateRGBSurface(SDL_SWSURFACE,ANCHO_PANTALLA,ALTO_PANTALLA,16,0,0,0,0);
            while (contador<255)   //Hasta que no este negra la cosa no nos vamos.
            {
                SDL_SetAlpha(mascara,SDL_SRCALPHA|SDL_RLEACCEL,contador);//ponemos el alpha...
                SDL_BlitSurface(mascara,&rectpantalla,screen,&rectpantalla);//bliteamos
                SDL_Flip(screen);//mostramos la imagen
                contador+=16;//aumentamos el alpha

                SDL_Delay(20);
            }
            SDL_BlitSurface(transicion,&rectpantalla,screen,&rectpantalla);//recuperamos la imagen anterior para recuperar despues los tiles...
            SDL_FreeSurface(transicion);
            SDL_FreeSurface(mascara);
            fadeout=true;
        }
        else if (player->vidas>player->vidas_anteriores) {
            DibujarVidas(730,578);
            player->vidas_anteriores = player->vidas;
        }
    }
    else
    {
        jugando=false;
        player->vidas_anteriores=3;
    }
    recuperartodostiles();
}

tipo_tile TileInferior(void)
{
    int x1,y1,x2,y2,xtile1,xtile2,ytile,i;
    bool existe_colision=false;
    tipo_tile retorno=TILE_NINGUNO;

    // Buscamos el tipo de tile que tenemos debajo
    x1=x_abs+player->x;
    y1=player->y+1; // El +1 es porque partimos de una posición inicial en la que no hay colisión y buscamos si
    // en el punto inferior la hay
    x2=x1+ANCHO_PERSONAJE-1;
    y2=y1+player->spriteasociado->cuadros.cuadro_inf.y+player->spriteasociado->cuadros.cuadro_inf.h-1;
    xtile1=x1/ANCHO_TILE; // tile inferior izquierdo
    xtile2=x2/ANCHO_TILE; // tile inferior derecho
    ytile=y2/ALTO_TILE;
    colision->asignar_cuadro(&player->spriteasociado->cuadros,1,x1,y1);
    if (mapa[xtile1][ytile]>0 && mapa[xtile1][ytile]<70)
    {
        colision->asignar_cuadro(&Sprite_tiles_estaticos->cuadros,2,xtile1*ANCHO_TILE,(ytile*ALTO_TILE));
        colision->detectar();
        existe_colision|=colision->col1.col_inf;
    }
    if (mapa[xtile2][ytile]>0 && mapa[xtile2][ytile]<70)
    {
        colision->asignar_cuadro(&Sprite_tiles_estaticos->cuadros,2,xtile2*ANCHO_TILE,(ytile*ALTO_TILE));
        colision->detectar();
        existe_colision|=colision->col1.col_inf;
    }
    // Buscamos el tipo de tile que tenemos debajo
    if (existe_colision && ((mapa[xtile1][ytile]>0 && mapa[xtile1][ytile]<65) || (mapa[xtile2][ytile]>0 && mapa[xtile2][ytile]<65)))
    {
        retorno=TILE_SOLIDO;
    }
    else if (existe_colision && ((mapa[xtile1][ytile]>64 && mapa[xtile1][ytile]<70) || (mapa[xtile2][ytile]>64 && mapa[xtile2][ytile]<70)))
        retorno=TILE_TRANSPARENTE;
    if ((retorno==TILE_SOLIDO) && (mapa[xtile1][ytile]==TILE_DESLIZANTE || mapa[xtile2][ytile]==TILE_DESLIZANTE))
        retorno=TILE_SOLIDO_DESLIZANTE;
    // Consideramos los NPCs de plataforma como tiles sólidos
    if ((retorno==TILE_NINGUNO) || (retorno==TILE_TRANSPARENTE))
        for (i=0; i<TOTAL_NPCS; i++)
            if ((npc[i]->estado) && ((npc[i]->tipoNPC==5) || (npc[i]->tipoNPC==3)))
            {
                colision->asignar_cuadro(&npc[i]->spriteasociado->cuadros,2,x_abs+npc[i]->x,npc[i]->y);

                colision->detectar();
                if (colision->col1.colisionado)
                    retorno=TILE_PLATAFORMA;
          }
    return retorno;
}
void MoverBalas(void)
{
    int i;

    //Balas enemigas
    for (i=0;i<TOTAL_BALAS_ENEMIGAS;i++)
        Balas_Enemigas[i]->updatexy();
    // Balas propias
    for (i=0;i<TOTAL_BALAS_PROPIAS;i++)
        Balas_Propias[i]->updatexy();

}

void Inicializar(void)
{
    int i;
    char auxcad[50];

    strcpy(auxcad,DIR_CONTENIDO);
    strcat(auxcad,"/cour.ttf");
    fuente = TTF_OpenFont(auxcad,18);//fuente, tamaÃ±o
    strcpy(auxcad,DIR_CONTENIDO);
    strcat(auxcad,"/verdanab.ttf");
    fuentemenu = TTF_OpenFont(auxcad,25);


    // Inicializamos todos los Sprites
    Sprite_tiles_estaticos=new Sprite(0,0,ANCHO_TILE,2,0,2,ANCHO_TILE,ALTO_TILE-4,0,ALTO_TILE-2,ANCHO_TILE,2);
    Sprite_personaje=new Sprite(8,8,16,16,8,24,16,16,8,40,16,8);
    for (i=0; i<NRO_NPCS_DEFINIDOS; i++)
        if (i==2) // Lanzador
            Sprite_NPCs[i]=new Sprite(2,2,25,4,2,6,25,33,2,39,25,3);
        else if (i==1) // Perseguidor
            Sprite_NPCs[i]=new Sprite(16,0,52,8,12,8,60,8,24,16,36,12);
        else if (i==4) // Plataforma
            Sprite_NPCs[i]=new Sprite(7,0,114,7,4,7,120,8,15,15,98,8);
        else if (i==3) // Boss
            Sprite_NPCs[i]=new Sprite(15,0,34,33,13,33,44,14,13,47,48,17);
        else if (i==6) // Vidas y munición
            Sprite_NPCs[i]=new Sprite(0,0,32,2,0,2,32,28,0,30,32,2);
        else // Resto de NPCs
            Sprite_NPCs[i]=new Sprite(8,0,16,16,8,16,16,16,8,32,16,12);
    Sprite_bala_propia=new Sprite(0,0,ANCHO_BALA,1,0,1,ANCHO_BALA,ALTO_BALA-2,0,ALTO_BALA-1,ANCHO_BALA,1);
    for (i=0; i<NRO_BALAS_ENEMIGAS_DEFINIDAS; i++) // Hacemos la definición de los cuadros de todas las balas iguales
            Sprite_bala_enemiga[i]=new Sprite(0,0,ANCHO_BALA,2,0,2,ANCHO_BALA,ALTO_BALA-4,0,ALTO_BALA-2,ANCHO_BALA,2);
    Sprite_explosion=new Sprite(0,0,ANCHO_EXPLOSION,2,0,2,ANCHO_EXPLOSION,ALTO_EXPLOSION-4,0,ALTO_EXPLOSION-2,ANCHO_EXPLOSION,2);

    // Los sprites asociados se asignan en la asignación del elemento nuevo, excepto en el caso del personaje que es sólo uno
    player=new Tipo_personaje;//Inicializacion del personaje.
    player->spriteasociado=Sprite_personaje;
    player->vidas=3;
    player->llave=false;

    //Inicializar NPC
    for (i=0;i<TOTAL_NPCS;i++)
        npc[i]=new Tipo_NPC;
    // El tipo se asigna cuando se detecta un nuevo NPC en pantalla: npc[i]->spriteasociado->tipo= ...

    //Inicializar balas enemigas
    for (i=0;i<TOTAL_BALAS_ENEMIGAS;i++)
        Balas_Enemigas[i]=new Tipo_Balas;

    //Inicalizar balas propias
    for (i=0;i<TOTAL_BALAS_PROPIAS;i++)
        Balas_Propias[i]=new Tipo_Balas;

    //Inicializacion de audio
    for (i=0;i<TOTAL_SONIDOS;i++)
        sonido[i]=new Mus_Efec;
    sonido[0]->cargar(false,"musica1.mp3"); // Música ambiente
    sonido[1]->cargar(true,"efecto1.wav"); // efecto salto
    sonido[2]->cargar(true,"efecto2.wav"); // efecto disparo
    sonido[3]->cargar(true,"efecto3.wav"); // efecto explosión
    sonido[4]->cargar(false,"Lagunex_menu.mp3");//Música menu
    sonido[5]->cargar(false,"nonstop.mp3");//Musica 2 del juego;

    // Inicialización del guardado de todos los tiles que irán en pantalla
    inicializarfondos();
    //Inicializacion de las barras
    for (i=0;i<2;i++)
    {
        b_sonido[i]= new Barra_Control;
        b_sonido[i]->x=262;/*posicionx*/
        b_sonido[i]->y=125+(i+1)*30+(i*15);/*posiciony*/
        b_sonido[i]->w=250;/*ancho*/
        b_sonido[i]->h=15;/*alto*/
        b_sonido[i]->value=20;/*Valor inicial*/
        b_sonido[i]->min=0;/*Valor minimo*/
        b_sonido[i]->max=128;/*valor maximo*/
        b_sonido[i]->capturado=false;
    }
    b_sonido[1]->y+=25;
    mouse.pulsado=false;
    balas_esperando=new clase_cola;

    opciones=new Clase_Lista();
}

void iniciarnivel(void)
{
    int i;
    if((rand()%4)<2)
        sonido[0]->play();
    else
        sonido[5]->play();
    // Los sprites asociados se asignan en la asignación del elemento nuevo, excepto en el caso del personaje que es sólo uno
    player->x=player->xini;
    player->y=player->yini;
    if (player->x>ANCHO_PANTALLA/2)
    {
        x_abs=player->x-ANCHO_PANTALLA/2;
        player->x=ANCHO_PANTALLA/2;
    }
    else
        x_abs=0;
    player->inerciax=0;
    player->altura=0;
    player->apuntes=0;
    player->estado=true;
    player->orientacion=true;
    player->ciclosframe=16;
    player->frameactual=0;
    player->control_repeticion_disparos=0;
    player->c_disparos=0;
    player->xabs_ant=player->xabs_sig=x_abs;
    player->xsig=player->x;
    player->ysig=player->y;
    //Inicializar NPC
    for (i=0;i<TOTAL_NPCS;i++)
    {
        npc[i]->dibujar=false;
        npc[i]->estado=false;
        // El tipo se asigna cuando se detecta un nuevo NPC en pantalla: npc[i]->spriteasociado->tipo= ...
    }

    //Inicializar balas enemigas
    for (i=0;i<TOTAL_BALAS_ENEMIGAS;i++)
    {
        Balas_Enemigas[i]->activa=false;
        Balas_Enemigas[i]->frameactual=0;
        Balas_Enemigas[i]->x=0;
        Balas_Enemigas[i]->y=0;
        Balas_Enemigas[i]->movx=0;
        Balas_Enemigas[i]->tipoNPC=0;
        Balas_Enemigas[i]->contadorvelocidad=0;
        Balas_Enemigas[i]->periodo=2;
        Balas_Enemigas[i]->propia0_enemiga1=true;
        Balas_Enemigas[i]->colisionada=false;
    }

    //Inicalizar balas propias
    for (i=0;i<TOTAL_BALAS_PROPIAS;i++)
    {
        Balas_Propias[i]->activa=false;
        Balas_Propias[i]->x=0;
        Balas_Propias[i]->frameactual=0;
        Balas_Propias[i]->y=0;
        Balas_Propias[i]->movx=0;
        Balas_Propias[i]->tipoNPC=0;
        Balas_Propias[i]->contadorvelocidad=0;
        Balas_Propias[i]->periodo=2;
        Balas_Propias[i]->propia0_enemiga1=false;
        Balas_Propias[i]->colisionada=false;
    }

    //Inicalizar explosiones (no es necesario crearlas ya que son estructuras, en vez de clases)
    for (i=0; i<TOTAL_EXPLOSIONES; i++)
    {
        explosiones[i].activa=false;
        explosiones[i].x=0;
        explosiones[i].y=0;
        explosiones[i].vueltas=0;
        explosiones[i].frameactual=0;
    }
    Desactivar_NPCs();

    // elementos activos
    NPCs_activos=0;
    explosiones_activas=0; //x_abs=0;
    //Vaciamos la cola
    struct bala_en_espera *aux_bala_en_espera;
    while (!balas_esperando->vacia())
    {
        aux_bala_en_espera=(struct bala_en_espera*)balas_esperando->valor();
        delete aux_bala_en_espera;
        balas_esperando->sacar();
    }
    // Inicializamos el tipo colisiones
    colision=new clase_colision;
    DibujarVidas(730,578);

}



void Finalizar(void)   // Destrucción de las estructuras creadas
{
    int i;

    // Cerrar la fuente
    TTF_CloseFont(fuente);

    // Finalizamos todos los Sprites
    Sprite_tiles_estaticos->liberarimagen(); // Sprite de Tiles estáticos
    delete Sprite_tiles_estaticos;
    Sprite_personaje->liberarimagen(); // Sprite del personaje
    delete Sprite_personaje;
    for (i=0; i<NRO_NPCS_DEFINIDOS; i++)   // Sprites de los distintos NPCs
    {
        Sprite_NPCs[i]->liberarimagen();
        delete Sprite_NPCs[i];
    }
    Sprite_bala_propia->liberarimagen(); // Sprite de la bala propia
    delete Sprite_bala_propia;
    for (i=0; i<NRO_BALAS_ENEMIGAS_DEFINIDAS; i++)   // Sprites de las distintas balas enemigas
    {
        Sprite_bala_enemiga[i]->liberarimagen();
        delete Sprite_bala_enemiga[i];
    }
    Sprite_explosion->liberarimagen(); // Sprite de la explosión
    delete Sprite_explosion;

    // Finalizamos las distintas variables de clases inicializadas
    delete player; // personaje
    for (i=0; i<TOTAL_NPCS; i++) // NPCs
        delete npc[i];
    for (i=0; i<TOTAL_BALAS_ENEMIGAS; i++) // Balas enemigas
        delete Balas_Enemigas[i];
    for (i=0; i<TOTAL_BALAS_PROPIAS; i++) // Balas propias
        delete Balas_Propias[i];
    // Las explosiones no es necesario liberarlas porque son estructuras, no clases
    for (i=0; i<TOTAL_SONIDOS; i++) // Sonidos
        sonido[i]->stop();
    SDL_Delay(1000); // Esperamos un segundo antes de destruirlo para dar tiempo a que acabe el FADEOFF
    for (i=0; i<TOTAL_SONIDOS; i++)
    {
        sonido[i]->liberar();
        delete sonido[i];
    }
    //Vaciamos la cola
    struct bala_en_espera *aux_bala_en_espera;
    while (!balas_esperando->vacia())
    {
        aux_bala_en_espera=(struct bala_en_espera*)balas_esperando->valor();
        delete aux_bala_en_espera;
        balas_esperando->sacar();
    }
    // Liberamos la clase colision
    delete colision;
    // Liberamos la lista de opciones
    opciones->vaciar();
    delete opciones;
    // Liberamos las imágenes iniciales creadas como menú y fondo
    SDL_FreeSurface(imgfondo);
    SDL_FreeSurface(screen);
    SDL_FreeSurface(fondo_menu);
    SDL_FreeSurface(flechas);
    SDL_FreeSurface(mascara);
}

void CargarMapa(char *txtnivel)
{
    int h=0,v=0;
    char buffer[11521];
    bool encontrado=false;
    char ruta[50];


    FILE *fichero;
    if (strcmp(txtnivel,""))
        sprintf(ruta,"%s/%s",DIR_CONTENIDO,txtnivel);
    else
        sprintf(ruta,"%s/mapa%d.MAP",DIR_CONTENIDO,nivel);
    printf("Cargando mapa... %s\n",ruta);
    fichero = fopen(ruta, "rb+");
    fread( buffer,1, 11520, fichero );
    for (h=0;h<TOTAL_ANCHO_TILES;h++)
        for (v=0;v<ALTO_PANTALLA_TILES;v++)
        {
            mapa[h][v]=0;
            mapa[h][v]=(int)buffer[v*TOTAL_ANCHO_TILES+h];
            if (mapa[h][v]==90 && !encontrado)
            {
                mapa[h][v]=0;
                player->xini=h*ANCHO_TILE;
                player->yini=v*ALTO_TILE;
                encontrado=true;
            }
        }
    fclose(fichero);
}

void Mover_elementos(void)
{
    bool existe_colision=false;
    MoverBalas();
    // MOVER NPCS
    int i;
    static bool direccion;
    CrearDisparoEnemigo();
    for (i=0;i<TOTAL_NPCS;i++)
        if (npc[i]->estado)   //
        {
            // printf("tipo NPC: %d\n",npc[i]->tipoNPC);
            npc[i]->updatemovx();
            npc[i]->sig_xy();

            while (player->estado && npc[i]->existe_movimiento())   // Movemos el NPC punto a punto
            {
                //printf("Bucle NPCs atrapado con: %d\n",npc[i]->tipoNPC);
                npc[i]->mueve_1();
                //printf("xsig1: %d x: %d",npc[i]->xsig,npc[i]->x);
                npc[i]->colisiontiles();
                //printf("xsig2: %d x : %d\n",npc[i]->xsig,npc[i]->x);
                player->colisionNPCs();
                npc[i]->colisionPlataformas();
                while (player->estado && player->existe_movimiento()) {
                    //printf("NPC: %d\n",npc[i]->tipoNPC);
                    player->mueve_1();
                }
            }
// TODO (Ramón#1#): Arreglar problema de NPC2 desplazándose continuamente a derecha e izquierda
            npc[i]->colisionbalas();
            if (npc[i]->tipoNPC==1)
            {
                if (!(ciclosps%15))
                {
                    npc[i]->frameactual++;
                    if (npc[i]->movx>0)   //======Si va hacia la izquierda=====
                    {
                        if (npc[i]->frameactual>4)
                            npc[i]->frameactual=2;
                        if (direccion)
                            npc[i]->frameactual=2;
                        direccion=false;
                    }
                    else   //=========Si va a la derecha
                    {
                        if (npc[i]->frameactual>7)
                            npc[i]->frameactual=5;
                        if (!direccion)
                            npc[i]->frameactual=5;
                        direccion=true;
                    }

                }
                //==========================Fin===================================
            }
            else if(npc[i]->tipoNPC==2)
            {
                if (!(ciclosps%5))
                {
                    npc[i]->frameactual++;
                    if (npc[i]->movx<0)   //======Si va hacia la izquierda=====
                    {
                        if (npc[i]->frameactual>11)
                            npc[i]->frameactual=6;
                        if (direccion)
                            npc[i]->frameactual=6;
                        direccion=false;
                    }
                    else   //=========Si va a la derecha
                    {
                        if (npc[i]->frameactual>5)
                            npc[i]->frameactual=0;
                        if (!direccion)
                            npc[i]->frameactual=0;
                        direccion=true;
                    }

                }
                //==========================Fin===================================
            }
        }

    //MOVER PERSONAJE
    //FALTA CORREGIR BUG EN EL PUNTO ALGIDO DE LA SUBIDA, FRAME ERRONEO
    player->sig_xy();
    existe_colision=false;
    while (player->estado && player->existe_movimiento())
    {
        player->mueve_1();
        existe_colision=player->colisiontiles();
        existe_colision|=player->colisionNPCs();
        existe_colision|=player->colisionbalas();
        /*for (i=0;i<TOTAL_NPCS;i++)
            if (npc[i]->estado)
            {
                npc[i]->colisiontiles();
                //npc[i]->colisionPlataformas();
                while(npc[i]->estado && npc[i]->existe_movimiento()) {
                    npc[i]->mueve_1();
                }
            }*/
    }
    if (player->control_repeticion_disparos>10)
        if (player->orientacion)
            player->frameactual=22;
        else
            player->frameactual=21;
    else
        if ((player->inerciax==0) || (player->altura>0)) // Si no hay inercia o si está subiendo
            if (player->altura>0) // Está subiendo
                if (player->orientacion)
                    player->frameactual=20; // Posicion de salto hacia la derecha
                else
                    player->frameactual=19; // Posición de salto hacia la izquierda
            else // No hay inercia
                if (player->orientacion)
                    player->frameactual=24; // Posición estática hacia la derecha
                else
                    player->frameactual=23; // Posición estática hacia la izquierda
        else   // Está en movimiento
        {
            player->ciclosframe+=(player->ciclosframe<10)?1:-player->ciclosframe; // Cuenta indefinidamente de 0 a 59
            if (ciclosps%5==0)
            {
                player->frameactual++;
                if (!player->orientacion)
                {
                    if (player->frameactual>8)   // Posición corriendo a derecha 1
                    {
                        player->frameactual=0;
                    }
                }
                else
                    if (player->frameactual>16)
                    {
                        player->frameactual=9;
                    }

            }
        }
    Uint8 *keys;
    keys=SDL_GetKeyState(NULL);
    tipo_tile valor_tile=TileInferior();
    if ((valor_tile==TILE_SOLIDO_DESLIZANTE) && (!(keys[SDLK_RIGHT] || keys[SDLK_d]) && !(keys[SDLK_LEFT] || keys[SDLK_a]) ))
    {
        if (player->orientacion)
            player->frameactual=24;
        else
            player->frameactual=23;
    }

    if ((player->posy_anterior!=player->y)&&(TileInferior()!=TILE_PLATAFORMA))
    {
        if (player->orientacion)
            player->frameactual=20;
        else
            player->frameactual=19;
        player->posy_anterior=player->y;
    }
}
void CrearDisparoPersonaje(void)
{
    int i;
    //al destruir, colisionar o cualquier cosa de una bala, se de debe de restar 1 a totalbalaspropias_dibujadas.
    if (!player->control_repeticion_disparos && player->c_disparos<TOTAL_BALAS_PROPIAS) //Con esto evitamos lanzar más de una bala cada CICLOS_REPETICION_DISPARO
        for (i=0; i<TOTAL_BALAS_PROPIAS; i++)
        {
            if (Balas_Propias[i]->activa==false)   //Buscamos bala fuera de uso
            {
                Balas_Propias[i]->y=player->y+12;   //La altura será la actual del personaje
                Balas_Propias[i]->punto_xabs=x_abs;
                if (player->orientacion==true)   //Si miramos a la derecha
                {
                    Balas_Propias[i]->x=player->x+ANCHO_PERSONAJE;
                    Balas_Propias[i]->movx=3;

                }
                else   //Si miramos a la izquierda
                {
                    Balas_Propias[i]->x=player->x-ANCHO_PERSONAJE;
                    Balas_Propias[i]->movx=-3;
                }
                sonido[2]->play();
                Balas_Propias[i]->activa=true;
                i=TOTAL_BALAS_PROPIAS;
                player->control_repeticion_disparos=CICLOS_REPETICION_DISPARO;
                player->c_disparos++;
                DibujarVidas(730,578);
            }
        }
}

void CrearDisparoEnemigo(void)
{
    int i,a,sig_bala=-1;
    struct bala_en_espera *nueva_bala;
    bool encontrado;

    //Buscamos las balas que se tienen que crear
    for (i=0; i<TOTAL_NPCS; i++)
        if (balas_esperando->tamano()<MAXIMA_BALAS_COLA)
        {
            if (npc[i]->dibujar && npc[i]->estado && ((npc[i]->tipoNPC==3) || (npc[i]->tipoNPC==4))) //Si es del tipo lanzador o boss y esta activo
                if (!(ciclosps%npc[i]->periodobalas))   //cada 50 vueltas lanzamos...
                {
                    nueva_bala=new struct bala_en_espera;
                    nueva_bala->punto_xabs_bala=npc[i]->punto_xabs;
                    nueva_bala->x=npc[i]->x+(ANCHO_NPCS[npc[i]->tipoNPC-1]/2);
                    nueva_bala->y=npc[i]->y+5;
                    if (npc[i]->direcciondisparo)
                        nueva_bala->movx=3;
                    else
                        nueva_bala->movx=-3;
                    nueva_bala->tipoNPC=npc[i]->tipoNPC;
                    nueva_bala->tipoobjeto=npc[i]->tipoobjeto;
                    nueva_bala->idxnpc=npc[i]->idx;
                    nueva_bala->idynpc=npc[i]->idy;
                    balas_esperando->meter(nueva_bala);
                }
        }
    for (a=0;a<TOTAL_BALAS_ENEMIGAS;a++)
    {
        sig_bala=-1;
        if (!Balas_Enemigas[a]->activa)
        {
            sig_bala=a;
            a=TOTAL_BALAS_ENEMIGAS;
        }
        if ((sig_bala!=-1) && (!balas_esperando->vacia()))
        {
           nueva_bala=(struct bala_en_espera*)balas_esperando->valor();
            encontrado=false;
            for (i=0;i<TOTAL_NPCS;i++)
            {
                if (npc[i]->estado && npc[i]->idx==nueva_bala->idxnpc && npc[i]->idy==nueva_bala->idynpc)
                    encontrado=true;
            }
            if (encontrado)
            {
                Balas_Enemigas[sig_bala]->activa=true;
                Balas_Enemigas[sig_bala]->x=nueva_bala->x+nueva_bala->punto_xabs_bala-x_abs;
                Balas_Enemigas[sig_bala]->punto_xabs=x_abs;
                Balas_Enemigas[sig_bala]->y=nueva_bala->y;
                Balas_Enemigas[sig_bala]->movx=nueva_bala->movx;
                Balas_Enemigas[sig_bala]->tipoNPC=nueva_bala->tipoNPC;
                if (nueva_bala->tipoNPC!=4) // Si no es un boss
                    Balas_Enemigas[sig_bala]->spriteasociado=Sprite_bala_enemiga[0];
                else
                    Balas_Enemigas[sig_bala]->spriteasociado=Sprite_bala_enemiga[nueva_bala->tipoobjeto];
                delete nueva_bala;
                balas_esperando->sacar(); // Borra la primera bala de la lista
            }
            else
            {
                delete nueva_bala;
                balas_esperando->sacar(); // Borra la primera bala de la lista
            }
        }
        // El tipo se asigna cuando se detecta un nuevo NPC en pantalla: npc[i]->spriteasociado->tipo= ...
    }
}

void Texto(char *cadena,int r, int g, int b)
{
    SDL_Surface *texto;
    SDL_Color bgcolor,fgcolor;
    SDL_Rect dest;

    fgcolor.r=r;
    fgcolor.g=g;
    fgcolor.b=b;
    bgcolor.r=0;
    bgcolor.g=0;
    bgcolor.b=0;
    texto=TTF_RenderText_Shaded(fuente,cadena,fgcolor,bgcolor);
    dest.y=576;
    dest.x=0;


    SDL_BlitSurface(texto,NULL,screen,&dest);
    SDL_FreeSurface(texto);
    // destruimos la fuente de letra
}

void Controles(Uint8 keys[])
{
    static Sint16 joyx=0,joyy=0,boton=0,yanterior;
    static bool saltando;
    tipo_tile valor_tile = TileInferior();

    if (joystick!=NULL)   // Leemos los controles del joystick
    {
        joyx = SDL_JoystickGetAxis(joystick, 0);
        joyy = SDL_JoystickGetAxis(joystick, 1);
        boton = SDL_JoystickGetButton(joystick,0);
    }

    // Pulsar hacia la derecha
    if (((joyx>10) || (keys[SDLK_RIGHT] || keys[SDLK_d])) && player->inerciax<MAX_INERCIA)
    {
        if (valor_tile!=TILE_SOLIDO_DESLIZANTE)   //si el tile inferior no es de deslizamiento...
        {
            player->inerciax+=1;
            player->orientacion=true;
        }
        else
            if (player->inerciax<0)
            {
                if (ciclosps%5==0)
                {
                    player->inerciax+=1;
                    player->orientacion=true;
                }
            }
            else
            {
                player->inerciax+=1;
                player->orientacion=true;
            }

    }
    // Pulsar hacia la izquierda
    if (((joyx<-10) || (keys[SDLK_LEFT] || keys[SDLK_a])) && player->inerciax>MIN_INERCIA)
    {
        if (valor_tile!=TILE_SOLIDO_DESLIZANTE)   //si el tile inferior no es de deslizamiento...
        {
            player->inerciax-=1;
            player->orientacion=false;
        }
        else
            if (player->inerciax>0)
            {
                if (ciclosps%5==0)
                {
                    player->inerciax-=1;
                    player->orientacion=false;
                }
            }
            else
            {
                player->inerciax-=1;
                player->orientacion=false;
            }

    }
    // No se pulsa nada, desaceleración
    if ((((joyx>=-10) && (joyx<=10)) && !(keys[SDLK_RIGHT] || keys[SDLK_d]) && !(keys[SDLK_LEFT] || keys[SDLK_a])))
    {
        if (valor_tile==TILE_PLATAFORMA)
            if (player->inerciax>0)
                player->inerciax-=1;
            else if (player->inerciax<0)
                player->inerciax+=1;
            else ;
        else
        {
            if (player->y==yanterior )
            {
                if (valor_tile!=TILE_SOLIDO_DESLIZANTE)   //si el tile inferior no es de deslizamiento...
                {
                    if (player->inerciax>0)
                        player->inerciax-=1;
                    else if (player->inerciax<0)
                        player->inerciax+=1;
                }
                else
                {
                    if ((ciclosps%40==0))
                    {
                        if (player->inerciax>0)
                            player->inerciax-=1;
                        else if (player->inerciax<0)
                            player->inerciax+=1;
                    }
                }

            }
        }
    }
    else   //Si estamos cayendo, frenamos pero más lento de lo normal -- ¿realmente es cuando estamos cayendo?
    {
        if (ciclosps%2==0)
        {
            if (player->inerciax>0)
                player->inerciax-=1;
            else if (player->inerciax<0)
                player->inerciax+=1;
        }
    }
    if (keys[SDLK_F12])
        CambiarModoPantalla();
    if ((((joyy<-10) || (keys[SDLK_UP] || keys[SDLK_w] || keys[SDLK_SPACE])) && (player->altura==0)) && !saltando)
    {
        // Para saltar también debe haber colisión inferior con tiles o con plataforma móvil
        if (valor_tile==TILE_SOLIDO || valor_tile==TILE_SOLIDO_DESLIZANTE || valor_tile==TILE_PLATAFORMA)   // ¿colisión inferior?
        {
            player->altura=SALTO;
            sonido[1]->play();
            saltando=true;
            player->inerciay=0;
        }
    }
    else if (!(keys[SDLK_UP] || keys[SDLK_w]|| keys[SDLK_SPACE]))
        saltando=false;
    if (player->control_repeticion_disparos) player->control_repeticion_disparos--;
    if (boton || keys[SDLK_RCTRL] || keys[SDLK_LCTRL] ||keys[SDLK_RSHIFT] || keys[SDLK_LSHIFT])
        CrearDisparoPersonaje();
    yanterior=player->y;
}

void ResetTimeBase(void)
{
    ini_milisegundos=SDL_GetTicks();
}
int CurrentTime(void)
{
    fin_milisegundos=SDL_GetTicks();
    return fin_milisegundos-ini_milisegundos;
}

// Inicializar, guardar y recuperar los tiles guardados

void inicializarfondos (void)
{
    tiles.dibujados=0;
    tilesNPC.dibujados=0;
    tilesbalaspropias.dibujados=0;
    tilesbalasenemigas.dibujados=0;
    tilesexplosiones.dibujados=0;
}

// El guadado de tiles está implementada en el método frame_a_screen por tema de eficiencia

void recuperartodostiles(void)
{
    int i;
    SDL_Rect cuadscreen,cuadtile;
    /* El orden de recuperado de los tiles marca el orden de dibujado de ellos,
       que tiene que ser inverso a este */

    // Recuperado de fondo de las explosiones
    for (i=0; i<tilesexplosiones.dibujados; i++)
    {
        cuadscreen.x=cuadtile.x=tilesexplosiones.x[i];
        cuadscreen.y=cuadtile.y=tilesexplosiones.y[i];
        cuadscreen.w=cuadtile.w=ANCHO_EXPLOSION;
        cuadscreen.h=cuadtile.h=ALTO_EXPLOSION;
        SDL_BlitSurface(imgfondo,&cuadtile,screen,&cuadscreen);
    }
    tilesexplosiones.dibujados=0;

    // Recuperado de fondo de las balas enemigas
    for (i=0; i<tilesbalasenemigas.dibujados; i++)
    {
        cuadscreen.x=cuadtile.x=tilesbalasenemigas.x[i];
        cuadscreen.y=cuadtile.y=tilesbalasenemigas.y[i];
        cuadscreen.w=cuadtile.w=ANCHO_BALA;
        cuadscreen.h=cuadtile.h=ALTO_BALA;
        SDL_BlitSurface(imgfondo,&cuadtile,screen,&cuadscreen);
    }
    tilesbalasenemigas.dibujados=0;

    // Recuperado de fondo de las balas propias
    for (i=0; i<tilesbalaspropias.dibujados; i++)
    {
        cuadscreen.x=cuadtile.x=tilesbalaspropias.x[i];
        cuadscreen.y=cuadtile.y=tilesbalaspropias.y[i];
        cuadscreen.w=cuadtile.w=ANCHO_BALA;
        cuadscreen.h=cuadtile.h=ALTO_BALA;
        SDL_BlitSurface(imgfondo,&cuadtile,screen,&cuadscreen);
    }
    tilesbalaspropias.dibujados=0;

    // Recuperado de fondo de los NPCs
    for (i=0; i<tilesNPC.dibujados; i++)
    {
        cuadscreen.x=cuadtile.x=tilesNPC.x[i];
        cuadscreen.y=cuadtile.y=tilesNPC.y[i];
        cuadscreen.w=cuadtile.w=ANCHO_NPCS[tilesNPC.tipo[i]];
        cuadscreen.h=cuadtile.h=ALTO_NPCS[tilesNPC.tipo[i]];
        SDL_BlitSurface(imgfondo,&cuadtile,screen,&cuadscreen);
    }
    tilesNPC.dibujados=0;

    // Recuperado de fondo del personaje
    cuadscreen.x=cuadtile.x=tilespersonaje.x;
    cuadscreen.y=cuadtile.y=tilespersonaje.y;
    cuadscreen.w=cuadtile.w=ANCHO_PERSONAJE+1;
    cuadscreen.h=cuadtile.h=ALTO_PERSONAJE+1;
    SDL_BlitSurface(imgfondo,&cuadtile,screen,&cuadscreen);

    // Recuperado de fondo de los tiles
    for (i=0; i<tiles.dibujados; i++)
    {
        cuadscreen.x=cuadtile.x=tiles.x[i];
        cuadscreen.y=cuadtile.y=tiles.y[i];
        cuadscreen.w=cuadtile.w=ANCHO_TILE;
        cuadscreen.h=cuadtile.h=ALTO_TILE;
        SDL_BlitSurface(imgfondo,&cuadtile,screen,&cuadscreen);
    }
    tiles.dibujados=0;
}

void CambiarModoPantalla(void)
{
    SDL_Rect dest;
    static bool fullscreen=false;

    if (fullscreen)   //si esta a fullscreen
    {
        screen = SDL_SetVideoMode(ANCHO_PANTALLA,ALTO_PANTALLA,16,SDL_HWSURFACE | SDL_DOUBLEBUF );
        fullscreen=false;

    }
    else
    {
        screen = SDL_SetVideoMode(ANCHO_PANTALLA,ALTO_PANTALLA,16,SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
        fullscreen=true;

    }
    dest.x=0;
    dest.y=0;
    tiempo=0;
    SDL_BlitSurface(imgfondo,NULL,screen,&dest);
    SDL_Delay(100);
}

void Activar_NPCs_Visibles(void)
{
    int mapaaux[ANCHO_PANTALLA_TILES+1][ALTO_PANTALLA_TILES]; // mapa auxiliar (utilizado debido a plataformas móviles)
    int i,j,k,sig_npc,xtile,nro_en_mapa,cont=0,tileinicial=0;

    // Hacemos una copia de la parte visible del mapa en mapaaux
    xtile=x_abs/ANCHO_TILE;
    for (i=0; i<ANCHO_PANTALLA_TILES+1; i++) // Columnas
        for (j=0; j<ALTO_PANTALLA_TILES; j++) // Filas
            mapaaux[i][j]=mapa[i+xtile][j];
    // Comprobamos los NPCs que existen por activar
    for (i=0; i<ANCHO_PANTALLA_TILES+1; i++) // Columas
        for (j=0; j<ALTO_PANTALLA_TILES; j++)   // Filas
        {
            nro_en_mapa=mapa[xtile+i][j];

            if ((nro_en_mapa>69) && (nro_en_mapa<1000) && (nro_en_mapa!=90))   // NPCs por activar
            {
                // Buscamos el siguiente NPC libre

                sig_npc=-1;
                for (k=0; k<TOTAL_NPCS; k++)
                    if (!npc[k]->estado && !npc[k]->dibujar)
                    {
                        sig_npc=k;
                        k=TOTAL_NPCS;
                    }
                if (sig_npc>=0)   // Se ha encontrado npc libre
                {
                    // Activamos la parte común a todos los NPCs
                    npc[sig_npc]->estado=true;
                    npc[sig_npc]->dibujar=true;
                    npc[sig_npc]->idx=xtile+i;
                    npc[sig_npc]->idy=j;
                    npc[sig_npc]->vidas=0;
                    npc[sig_npc]->x=npc[sig_npc]->xant=npc[sig_npc]->xsig=(xtile+i)*ANCHO_TILE-x_abs;
                    npc[sig_npc]->y=npc[sig_npc]->yant=npc[sig_npc]->ysig=(j-1)*ALTO_TILE;
                    npc[sig_npc]->punto_xabs=x_abs;
                    npc[sig_npc]->frameactual=0;
                    npc[sig_npc]->rangoini=(xtile+i)*ANCHO_TILE;
                    npc[sig_npc]->rangofin=(xtile+i)*ANCHO_TILE;
                    npc[sig_npc]->puntos_off=5;

                    mapa[xtile+i][j]+=1000;
                    // Comprobamos qué tipo de NPC es para activarlo

                    switch (nro_en_mapa)
                    {
                    case 85: // Plataforma móvil horizontal vel. 1
                    case 86: // Plataforma móvil horizontal vel. 2
                    case 87: // Plataforma móvil vertical vel. 1
                    case 88: // Plataforma móvil vertical vel. 2
                        // Activamos la parte común a los NPCs de plataforma móvil
                        npc[sig_npc]->tipoNPC=5;
                        npc[sig_npc]->atravesable=false;
                        npc[sig_npc]->ctdorvelocidad=0;
                        npc[sig_npc]->frameactual=0;
                        npc[sig_npc]->spriteasociado=Sprite_NPCs[4];
                        npc[sig_npc]->x=npc[sig_npc]->xant=npc[sig_npc]->xsig=(xtile+i)*ANCHO_TILE-x_abs;
                        npc[sig_npc]->y=npc[sig_npc]->yant=npc[sig_npc]->ysig=j*ALTO_TILE;
                        if ((nro_en_mapa==85) || (nro_en_mapa==86))   // Plataformas de movimiento horizontal a distintas velocidades
                        {
                            // Activamos la parte específica a los NPCs de plataforma móvil de movimiento horizontal
                            npc[sig_npc]->ejemovimiento=true; // Determina movimiento horizontal
                            if (nro_en_mapa==85) // Para velocidad del movimiento
                                npc[sig_npc]->movx=3;
                            else
                                npc[sig_npc]->movx=5;
                            // Borramos el resto de la plataforma horizontal y calculamos el rango
                            for (k=i; k<ANCHO_PANTALLA_TILES+1; k++)
                                if (mapaaux[k][j]==nro_en_mapa)
                                    mapaaux[k][j]=0;
                                else
                                    k=ANCHO_PANTALLA_TILES+1;
                            // Calculamos el primer tile donde comienza el rango en el mapa
                            for (k=xtile+i-1; k>=0; k--)
                                if (mapa[k][j]==nro_en_mapa)
                                    mapa[k][j]+=1000; // Nos aseguramos de no volver a activar este tile como un NPC distinto
                                else
                                {
                                    npc[sig_npc]->rangoini=(k+1)*ANCHO_TILE;
                                    npc[sig_npc]->idx=k+1;
                                    npc[sig_npc]->x= npc[sig_npc]->rangoini-x_abs;
                                    k=-1;
                                }
                            // Calculamos el último tile donde termina el rango en el mapa
                            for (k=xtile+i+1; k<TOTAL_ANCHO_TILES; k++)
                                if (mapa[k][j]==nro_en_mapa)
                                    mapa[k][j]+=1000; // Nos aseguramos de no volver a activar este tile como un NPC distinto
                                else
                                {
                                    npc[sig_npc]->rangofin=k*ANCHO_TILE-1-ANCHO_NPCS[npc[sig_npc]->tipoNPC-1];
                                    k=TOTAL_ANCHO_TILES;
                                }
                        }
                        else   // Plataformas de movimiento vertical a distintas velocidades
                        {
                            npc[sig_npc]->ejemovimiento=false; // Determina movimiento vertical
                            if (nro_en_mapa==87) // Para velocidad del movimiento
                                npc[sig_npc]->movx=3;
                            else
                                npc[sig_npc]->movx=5;
                            // Borramos el resto de la plataforma vertical y calculamos el rango
                            npc[sig_npc]->rangoini=j*ALTO_TILE;
                            for (k=j+1; k<ALTO_PANTALLA_TILES; k++)
                                if (mapaaux[i][k]==nro_en_mapa)
                                {
                                    mapaaux[i][k]=0;
                                    mapa[xtile+i][k]+=1000;
                                }
                                else
                                {
                                    npc[sig_npc]->rangofin=k*ALTO_TILE-1-ALTO_NPCS[npc[sig_npc]->tipoNPC-1];
                                    k=ALTO_PANTALLA_TILES;
                                }
                            //printf("id: %d Rango_ini: %d Rango_fin: %d\n",npc[sig_npc]->idx,npc[sig_npc]->rangoini,npc[sig_npc]->rangofin);
                        }
                        //printf("P.Móvil %s Pos_NPC: %d x: %d y: %d r_ini:%d r_fin: %d\n",npc[sig_npc]->ejemovimiento?"Horizontal":"Vertical",sig_npc,npc[sig_npc]->x,npc[sig_npc]->y,npc[sig_npc]->rangoini,npc[sig_npc]->rangofin);
                        break;
                    case 71: // Persecución Vel. 1
                    case 72: // Persecución Vel. 2
                        //printf("Persecución Pos_NPC: %d x: %d y: %d\n",sig_npc,npc[sig_npc]->x,npc[sig_npc]->y);
                        if (player->x<npc[sig_npc]->x)
                            npc[sig_npc]->movx=-2;//Si el jugador esta a la izquierda movimiento izquierda
                        else
                            npc[sig_npc]->movx=2;
                        npc[sig_npc]->tipoNPC=2;
                        npc[sig_npc]->atravesable=false;
                        npc[sig_npc]->ctdorvelocidad=0;
                        npc[sig_npc]->frameactual=2;
                        npc[sig_npc]->spriteasociado=Sprite_NPCs[1];
                        //test
                        /*tileinicial=j;//arreglar para que sea el valor inicial del npc
                        while (!(mapa[xtile+i][tileinicial+1]>0 && mapa[xtile+i][tileinicial+1]<64)&& tileinicial<ALTO_PANTALLA_TILES)
                        {
                            tileinicial++;
                        }*/
                        npc[sig_npc]->y=(j*ALTO_TILE)-ALTO_TILE;
                        //fintest
                        break;
                    case 73: // Boss 1
                    case 74: // Boss 2
                    case 75: // Boss 3
                    case 76: // boss 4
                        //printf("Boss Pos_NPC: %d x: %d y: %d\n",sig_npc,npc[sig_npc]->x,npc[sig_npc]->y);
                        npc[sig_npc]->puntos_off=50;
                        npc[sig_npc]->movx=0;
                        npc[sig_npc]->tipoNPC=4;
                        npc[sig_npc]->vidas=2+2*npc[sig_npc]->tipoobjeto;
                        npc[sig_npc]->atravesable=false;
                        npc[sig_npc]->ctdorvelocidad=0;
                        npc[sig_npc]->direcciondisparo=true;
                        npc[sig_npc]->balalanzada=false;
                        npc[sig_npc]->tipoobjeto=nro_en_mapa-72;
                        npc[sig_npc]->vidas=2+2*npc[sig_npc]->tipoobjeto;
                        npc[sig_npc]->periodobalas=150-(npc[sig_npc]->tipoobjeto*15);
                        npc[sig_npc]->spriteasociado=Sprite_NPCs[3];
                        npc[sig_npc]->frameactual=nro_en_mapa-73;
                        npc[sig_npc]->y=(j*ALTO_TILE)-ALTO_TILE;
                        //fintest
                        break;
                    case 77:
                    case 78:
                    case 79:
                    case 80://conserje
                        //printf("Conserje Pos_NPC: %d x: %d y: %d\n",sig_npc,npc[sig_npc]->x,npc[sig_npc]->y);
                        npc[sig_npc]->spriteasociado=Sprite_NPCs[0];
                        npc[sig_npc]->tipoNPC=1;
                        npc[sig_npc]->movx=2;
                        npc[sig_npc]->ctdorvelocidad=0;
                        npc[sig_npc]->frameactual=2;
                        npc[sig_npc]->tipodesplazamiento=true;
                        cont=0;
                        //test
                        tileinicial=j;//arreglar para que sea el valor inicial del npc

                        while (!(mapa[xtile+i][tileinicial+1]>0 && mapa[xtile+i][tileinicial+1]<64)&&
                            tileinicial<ALTO_PANTALLA_TILES)
                            tileinicial++;
                        npc[sig_npc]->y=(tileinicial-1)*ALTO_TILE;
                        //fintest
                        do
                        {
                            npc[sig_npc]->rangoini=(xtile+i-cont)*ANCHO_TILE; //Calculamos las distancias limite (pared/precipicio)
                            cont++;
                        }
                        while (!(mapa[xtile+i-cont][tileinicial]>0 && mapa[xtile+i-cont][tileinicial]<64) &&
                            (npc[sig_npc]->rangoini>0) && (mapa[xtile+i-cont][tileinicial+1]>0 && mapa[xtile+i-cont][tileinicial+1]<64));

                        cont=npc[sig_npc]->rangoini/ANCHO_TILE;
                        do
                        {
                            npc[sig_npc]->rangofin=cont*ANCHO_TILE;
                            cont++;
                        }
                        while (!(mapa[cont][tileinicial]>0 && mapa[cont][tileinicial]<64 ) &&
                            (mapa[cont][tileinicial+1]>0 && mapa[cont][tileinicial+1]<64));

                        npc[sig_npc]->x= npc[sig_npc]->rangofin-3-npc[sig_npc]->punto_xabs;
                        npc[sig_npc]->atravesable=false;
                        break;
                    case 81:
                    case 82:
                    case 83:
                    case 84://lanzador
                        npc[sig_npc]->spriteasociado=Sprite_NPCs[2];
                        npc[sig_npc]->tipoNPC=3;
                        npc[sig_npc]->balalanzada=false;
                        if (nro_en_mapa==81)
                        {
                            npc[sig_npc]->direcciondisparo=false;
                            npc[sig_npc]->periodobalas=100;
                        }
                        else if (nro_en_mapa==82)
                        {
                            npc[sig_npc]->direcciondisparo=true;
                            npc[sig_npc]->periodobalas=100;
                        }
                        else if (nro_en_mapa==83)
                        {
                            npc[sig_npc]->direcciondisparo=false;
                            npc[sig_npc]->periodobalas=150;
                        }
                        else
                        {
                            npc[sig_npc]->periodobalas=150;
                            npc[sig_npc]->direcciondisparo=true;
                        }
                        tileinicial=j;
                        while (!(mapa[xtile+i][tileinicial+1]>0 && mapa[xtile+i][tileinicial+1]<64) && tileinicial<ALTO_PANTALLA_TILES)
                        {
                            tileinicial++;
                        }

                        npc[sig_npc]->y=npc[sig_npc]->ysig=(tileinicial*ALTO_TILE)-ALTO_TILE+6;
                        //printf("Lanzador Pos_NPC: %d x: %d y: %d\n",sig_npc,npc[sig_npc]->x,npc[sig_npc]->y);
                        break;
                    case 91: // Riesgo 1
                    case 92: // Riesgo 2
                    case 93: // Riesgo 3
                    case 94: // Riesgo 4
                        npc[sig_npc]->spriteasociado=Sprite_NPCs[5];
                        npc[sig_npc]->tipoNPC=6;
                        //printf("Zona de riesgo Pos_NPC: %d x: %d y: %d\n",sig_npc,npc[sig_npc]->x,npc[sig_npc]->y);
                        break;
                    case 70 : // Llave
                    case 89 : // Vida
                        npc[sig_npc]->spriteasociado=Sprite_NPCs[6];
                        if (nro_en_mapa==70)
                        {
                            npc[sig_npc]->tipoobjeto=2;
                            npc[sig_npc]->frameactual=0;
                        }
                        else if (nro_en_mapa==89)
                        {
                            npc[sig_npc]->tipoobjeto=1;
                            npc[sig_npc]->frameactual=1;
                        }
                        npc[sig_npc]->tipoNPC=7;
                        //printf("Vida Pos_NPC: %d x: %d y: %d\n",sig_npc,npc[sig_npc]->x,npc[sig_npc]->y);
                        break;
                    }
                }
            }
        }
}

void Desactivar_NPCs(void)   // Devuelve el mapa a su posición sin activación de NPCs
{
    int i,j;

    for (i=0; i<TOTAL_ANCHO_TILES; i++)
        for (j=0; j<ALTO_PANTALLA_TILES; j++)
            if (mapa[i][j]>=1000)
            {
                mapa[i][j]-=1000;

            }
}

void Desactivar_Ocultos(void)
{
    int i,b;
    for (i=0;i<TOTAL_NPCS;i++)
    {
        if (npc[i]->estado)
        {
            // Las plataformas verticales tienen un rango que no se puede tener en cuenta por lo que
            // se toma en cuenta aparte.
            // Se comprueba si es una plataforma vertical, si ha salido de pantalla por la derecha o la izquierda y si su
            // activación también ha salido
            if ((npc[i]->tipoNPC==5) && (!npc[i]->ejemovimiento))
                if (((npc[i]->punto_xabs+npc[i]->x > x_abs+ANCHO_PANTALLA) &&
                    (npc[i]->idx*ANCHO_TILE > x_abs+ANCHO_PANTALLA)) ||
                    ((npc[i]->punto_xabs+npc[i]->x+ANCHO_NPCS[4] < x_abs) &&
                    (npc[i]->idx*ANCHO_TILE < x_abs)))
                {
                    npc[i]->estado=false;
                    npc[i]->dibujar=false;
                    for (b=npc[i]->rangoini/ALTO_TILE; b<=(npc[i]->rangofin+ALTO_NPCS[npc[i]->tipoNPC-1])/ALTO_TILE;b++)
                        mapa[npc[i]->idx][b]-=1000;
                }
                else ;
            // Si visualmente el npc ha salido de pantalla por la izquierda o por la derecha y su activación
            // también ha salido y su rangofin también lo ha hecho
            else if (((npc[i]->punto_xabs+npc[i]->x<x_abs-ANCHO_NPCS[npc[i]->tipoNPC-1]) &&
                ((npc[i]->idx*ANCHO_TILE)<x_abs+ANCHO_NPCS[npc[i]->tipoNPC-1]) && (npc[i]->rangofin+ANCHO_NPCS[npc[i]->tipoNPC-1]<x_abs)) ||
                (npc[i]->punto_xabs+npc[i]->x>x_abs+ANCHO_PANTALLA &&
                ((npc[i]->idx*ANCHO_TILE)>(x_abs+ANCHO_PANTALLA))&& (npc[i]->rangoini>x_abs+ANCHO_PANTALLA)))
            {
                if (npc[i]->tipoNPC!=5) // Las plataformas desactivan varias partes del mapa
                    mapa[npc[i]->idx][npc[i]->idy]-=1000;
                npc[i]->estado=false;
                npc[i]->dibujar=false;
                if (npc[i]->tipoNPC==5 && npc[i]->ejemovimiento) // Horizontal
                    for (b=(npc[i]->rangoini/ANCHO_TILE); b<=((npc[i]->rangofin+ANCHO_NPCS[4])/ANCHO_TILE); b++)
                        mapa[b][npc[i]->idy]-=1000;
            }
            // Si visualmente el npc ha salido de pantalla por la derecha y su activación
            // también ha salido y su rangoini también lo ha hecho            /*else if
            {
                if (npc[i]->tipoNPC!=5)
                    mapa[npc[i]->idx][npc[i]->idy]-=1000;
                npc[i]->estado=false;
                npc[i]->dibujar=false;
                if (npc[i]->tipoNPC==5 && npc[i]->ejemovimiento) {// Horizontal
                    for (b=(npc[i]->rangoini/ANCHO_TILE); b<=((npc[i]->rangofin+ANCHO_NPCS[4])/ANCHO_TILE); b++)
                    {
                        mapa[b][npc[i]->idy]-=1000;
                    }
                    printf("desactivado por la derecha\n");
                }
            }*/
        }
    }
}
void DibujarVidas(int posx,int posy)
{
    SDL_Rect destino;
    char auxcad[30];

    SDL_Surface *texto;
    SDL_Color bgcolor,fgcolor;
    fgcolor.r=255;
    fgcolor.g=255;
    fgcolor.b=255;
    bgcolor.r=0;
    bgcolor.g=0;
    bgcolor.b=0;
    destino.y=posy;
    destino.x=posx-200;
    sprintf(auxcad,":%4d      :%4d     x %d",player->puntos,TOTAL_BALAS_PROPIAS-player->c_disparos,player->vidas);
    texto=TTF_RenderText_Shaded(fuente,auxcad,fgcolor,bgcolor);
    SDL_BlitSurface(texto,NULL,screen,&destino);
    destino.x=posx-225;
    SDL_BlitSurface(Spuntos,NULL,screen,&destino);
    destino.x=posx-110;
    SDL_BlitSurface(Smuni,NULL,screen,&destino);
    destino.x=posx;
    destino.y=posy;
    SDL_BlitSurface(miniatura,NULL,screen,&destino);
    SDL_FreeSurface(texto);
}

void GenerarLista()
{
    char auxcad[50];

    strcpy(auxcad,DIR_CONTENIDO);
    strcat(auxcad,"/");
    string dir = string(auxcad);
    vector<string> files = vector<string>();
    char *cadena;

    opciones->vaciar();
    getdir(dir,files);
    unsigned int i;
    for (i = 0;i < files.size();i++)
    {
        if (((files.at(i).c_str()[strlen(files.at(i).c_str())-1])=='p')||((files.at(i).c_str()[strlen(files.at(i).c_str())-1])=='P'))
            if (((files.at(i).c_str()[strlen(files.at(i).c_str())-2])=='A')||((files.at(i).c_str()[strlen(files.at(i).c_str())-2])=='a'))
                if (((files.at(i).c_str()[strlen(files.at(i).c_str())-3])=='m')||((files.at(i).c_str()[strlen(files.at(i).c_str())-3])=='M'))
                {
                    cadena=new char[strlen(files.at(i).c_str())+2];
                    strcpy(cadena,files.at(i).c_str());
                    opciones->insertar(cadena,i);
                }
    }
    opciones->ordenar_caracteres();
    dir.erase();
}

int getdir (string dir, vector<string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if ((dp  = opendir(dir.c_str())) == NULL)
    {

        return -1;
    }

    while ((dirp = readdir(dp)) != NULL)
    {
        files.push_back(string(dirp->d_name));
    }
    closedir(dp);
    return 0;
}

void generamunicion()
{
    int i,j;
    bool encontrado=false;

    for (i=0; i<TOTAL_NPCS; i++)
        // Si existe un boss activo
        if (npc[i]->tipoNPC==4) {

            i=TOTAL_NPCS;
            for (j=0; j<TOTAL_NPCS; j++)
                // Si existe ya una munición activa
                if ((npc[j]->tipoNPC==7) && (npc[j]->tipoobjeto==2) && (npc[j]->estado)) {

                    // Movemos la munción

                    npc[j]->yant=npc[j]->ysig=npc[j]->y+=CAIDA_NPC;

                    if (npc[j]->y>=ALTO_PANTALLA) {
                        DibujarVidas(730,578);
                        npc[j]->xant=npc[j]->xsig=npc[j]->x=rand()%ANCHO_PANTALLA;
                        npc[j]->punto_xabs=x_abs;
                        npc[j]->y=-ALTO_NPCS[6];
                    }
                    j=TOTAL_NPCS;
                    encontrado = true;
                }
            if (!encontrado)
            {

                // Creamos un npc con los datos de la munición de forma aleatoria x
                for (j=0; j<TOTAL_NPCS; j++)
                    if (!npc[j]->estado) {
                        npc[j]->estado=true;
                        npc[j]->dibujar=true;
                        npc[j]->idx=0;
                        npc[j]->idy=0;
                        npc[j]->vidas=0;
                        npc[j]->xant=npc[j]->xsig=npc[j]->x=rand()%ANCHO_PANTALLA;
                        npc[j]->punto_xabs=x_abs;
                        npc[j]->yant=npc[j]->ysig=npc[j]->y=-ALTO_NPCS[6];
                        npc[j]->punto_xabs=x_abs;
                        npc[j]->frameactual=0;
                        npc[j]->rangoini=0;
                        npc[j]->rangofin=0;
                        npc[j]->puntos_off=0;
                        npc[j]->spriteasociado=Sprite_NPCs[6];
                        npc[j]->tipoobjeto=2;
                        npc[j]->frameactual=0;
                        npc[j]->tipoNPC=7;
                        j=TOTAL_NPCS;
                    }
            }
        }
}
