#define OBJETOS
#include <string.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_image.h>
#include "main.h"
#include "objetos.h"

// Definidos en motor.h ************************************************************
extern int mapa[TOTAL_ANCHO_TILES][ALTO_PANTALLA_TILES];
void DibujarVidas(int posx,int posy);
// Fin de definidos en motor.h *****************************************************

// Clase Sprite
Sprite::Sprite(int xini1,int yini1,int wini1,int hini1,int xini2,int yini2,int wini2,int hini2,int xini3,int yini3,int wini3,int hini3)
{
    cuadros.cuadro_sup.x=xini1;
    cuadros.cuadro_sup.y=yini1;
    cuadros.cuadro_sup.w=wini1;
    cuadros.cuadro_sup.h=hini1;
    cuadros.cuadro_cent.x=xini2;
    cuadros.cuadro_cent.y=yini2;
    cuadros.cuadro_cent.w=wini2;
    cuadros.cuadro_cent.h=hini2;
    cuadros.cuadro_inf.x=xini3;
    cuadros.cuadro_inf.y=yini3;
    cuadros.cuadro_inf.w=wini3;
    cuadros.cuadro_inf.h=hini3;
}

void Sprite::tiposprite(int elemento,char *nombre)
{
    // elemento=-4: tiles de explosión
    // elemento=-3: tiles de balas enemigas
    // elemento=-2: tiles de balas propias
    // elemento=-1: tiles de dibujado
    // elemnento=0 : personaje
    // elemento>0 : NPCs
    tipo=elemento;
    switch (elemento)
    {
    case -4 :
        anchoframe=ANCHO_EXPLOSION;
        altoframe=ALTO_EXPLOSION;
        break;
    case -3 :
    case -2 :
        anchoframe=ANCHO_BALA;
        altoframe=ALTO_BALA;
    case -1 :
        anchoframe=ANCHO_TILE;
        altoframe=ALTO_TILE;
        break;
    case 0 :
        anchoframe=ANCHO_PERSONAJE+1;
        altoframe=ALTO_PERSONAJE+1;
        break;
    default :
        anchoframe=ANCHO_NPCS[elemento-1];
        altoframe=ALTO_NPCS[elemento-1];
    }
    cargarimagen(nombre);
    totalframes=imagen->w / anchoframe;
}

void Sprite::cargarimagen(char *nombre)
{
    char ruta[50];
    SDL_Surface *temporal;
    sprintf(ruta,"%s/%s",DIR_CONTENIDO,nombre);
    temporal=IMG_Load(ruta);
    SDL_SetColorKey(temporal,SDL_SRCCOLORKEY,SDL_MapRGB(temporal->format,255,0,0));
    imagen=SDL_DisplayFormat(temporal);
}

void Sprite::liberarimagen(void)
{
    if (imagen!=NULL)
        SDL_FreeSurface(imagen);
}

void Sprite::frame_a_screen(int nroframe,int x,int y)
{
    // tipo=-4: explosión
    // tipo=-3: balas enemigas
    // tipo=-2: balas propias
    // tipo=-1 : tile
    // tipo=0 : personaje
    // tipo>0 : NPCs
    SDL_Rect cuad,cuad2;

    if (nroframe<=totalframes)   // No puede cargar un frame por encima de los que tiene
    {
        switch (tipo)
        {
        case -4:
            cuad.x=nroframe*ANCHO_EXPLOSION; // Posición del frame en el sprite
            cuad.y=0;
            cuad.w=ANCHO_EXPLOSION;
            cuad.h=ALTO_EXPLOSION;
            cuad2.x=x;  // Posición del frame en la pantalla
            cuad2.y=y;
            cuad2.w=ANCHO_EXPLOSION;
            cuad2.h=ALTO_EXPLOSION;
            // Guardado de tiles en memoria
            tilesexplosiones.x[tilesexplosiones.dibujados]=cuad2.x;
            tilesexplosiones.y[tilesexplosiones.dibujados]=cuad2.y;
            tilesexplosiones.dibujados++;
            break;
        case -3:
            cuad.x=nroframe*ANCHO_BALA; // Posición del frame en el sprite
            cuad.y=0;
            cuad.w=ANCHO_BALA;
            cuad.h=ALTO_BALA;
            cuad2.x=x;  // Posición del frame en la pantalla
            cuad2.y=y;
            cuad2.w=ANCHO_BALA;
            cuad2.h=ALTO_BALA;
            // Guardado de tiles en memoria
            tilesbalasenemigas.x[tilesbalasenemigas.dibujados]=cuad2.x;
            tilesbalasenemigas.y[tilesbalasenemigas.dibujados]=cuad2.y;
            tilesbalasenemigas.dibujados++;
            break;
        case -2:
            cuad.x=nroframe*ANCHO_BALA; // Posición del frame en el sprite
            cuad.y=0;
            cuad.w=ANCHO_BALA;
            cuad.h=ALTO_BALA;
            cuad2.x=x;  // Posición del frame en la pantalla
            cuad2.y=y;
            cuad2.w=ANCHO_BALA;
            cuad2.h=ALTO_BALA;
            // Guardado de tiles en memoria
            tilesbalaspropias.x[tilesbalaspropias.dibujados]=cuad2.x;
            tilesbalaspropias.y[tilesbalaspropias.dibujados]=cuad2.y;
            tilesbalaspropias.dibujados++;
            break;
        case -1:
            cuad.x=nroframe*ANCHO_TILE; // Posición del frame en el sprite
            cuad.y=0;
            cuad.w=ANCHO_TILE;
            cuad.h=ALTO_TILE;
            cuad2.x=x;  // Posición del frame en la pantalla
            cuad2.y=y;
            cuad2.w=ANCHO_TILE;
            cuad2.h=ALTO_TILE;
            // Guardado de tiles en memoria
            tiles.x[tiles.dibujados]=cuad2.x;
            tiles.y[tiles.dibujados]=cuad2.y;
            tiles.dibujados++;
            break;
        case 0:
            cuad.x=nroframe*(ANCHO_PERSONAJE+1); // Posición del frame en el sprite
            cuad.y=0;
            cuad.w=ANCHO_PERSONAJE+1;
            cuad.h=ALTO_PERSONAJE+1;
            cuad2.x=x;  // Posición del frame en la pantalla
            cuad2.y=y;
            cuad2.w=ANCHO_PERSONAJE+1;
            cuad2.h=ALTO_PERSONAJE+1;
            // Guardado del fondo del personaje en memoria
            tilespersonaje.x=cuad2.x;
            tilespersonaje.y=cuad2.y;
            // Anchura del frame a guardar
            break;
        default:
            cuad.x=nroframe*ANCHO_NPCS[tipo-1]; // Posición del frame en el sprite
            cuad.y=0;
            cuad.w=ANCHO_NPCS[tipo-1];
            cuad.h=ALTO_NPCS[tipo-1];
            cuad2.x=x;  // Posición del frame en la pantalla
            cuad2.y=y;
            cuad2.w=ANCHO_NPCS[tipo-1];
            cuad2.h=ALTO_NPCS[tipo-1];
            // Guardado de tiles en memoria
            tilesNPC.x[tilesNPC.dibujados]=cuad2.x;
            tilesNPC.y[tilesNPC.dibujados]=cuad2.y;
            tilesNPC.tipo[tilesNPC.dibujados]=tipo-1;
            tilesNPC.dibujados++;
            // Anchura del frame a guardar
        }
        // Exposición de la imagen en pantalla
        SDL_BlitSurface(imagen,&cuad,screen,&cuad2);
    }
}

// Clase Tipo_personaje

void Tipo_personaje::sig_xy(void)
{
    static int x_abs_max=0;
    static int ctdorcaida=0;

    xant=x; // Guardamos el valor anterior para las colisiones con tiles
    yant=y;
    xabs_ant=x_abs;
    if (!x_abs_max) // Para aceleración del cálculo (sólo se calcula la primera vez)
        x_abs_max=TOTAL_ANCHO_TILES*ANCHO_TILE-ANCHO_PANTALLA;
    // Primero calculamos el punto al que debería llegar
    if (inerciax>0)
        if ((x>ANCHO_MOV_PANTALLA_FIN)&&(x_abs<x_abs_max))
            xabs_sig+=(int)((float)inerciax/1.2);
        else if (x<ANCHO_PANTALLA-ANCHO_PERSONAJE)
            xsig+=(int)((float)inerciax/1.2);
        else ;
    else if (inerciax<0)
        if ((x<ANCHO_MOV_PANTALLA_INI)&&(x_abs>0))
            xabs_sig+=(int)((float)inerciax/1.2);
        else if (x>0)
            xsig+=(int)((float)inerciax/1.2);
        else ;
    else ;

    if (altura>20)   // Si está subiendo
    {
        altura-=(altura/10);
        ysig-=(altura/10);
    }
    else   // Si baja
    {
        altura=0;
        if (!inerciay)   // Si colisionó con el suelo
        {
            ctdorcaida=0;
            inerciay=1;
        }
        if ((ctdorcaida==3) || (ctdorcaida==6) || (ctdorcaida==9) || (ctdorcaida==11)) // Va acelerando gradualmente
            inerciay++;
        if (inerciay>MAX_INERCIA)
            inerciay=MAX_INERCIA;
        if (ctdorcaida<11)
            ctdorcaida++;
    }
    ysig+=player->inerciay;
    // Ahora comprobamos paso a paso si existe alguna colisión intermedia
}

bool Tipo_personaje::existe_movimiento(void)
{
    return (x!=xsig) || (y!=ysig) || (x_abs!=xabs_sig);
}

void Tipo_personaje::mueve_1(void)
{
    int movx=0,movy=0,movx_abs=0; // Existencia de movimiento en los distintos ejes
    static bool eje=false;

    if (xsig>x)
        movx=1;
    else if (xsig<x)
        movx=-1;
    if (xabs_sig>x_abs)
        movx_abs=1;
    else if (xabs_sig<x_abs)
        movx_abs=-1;
    if (ysig>y)
        movy=1;
    else if (ysig<y)
        movy=-1;
    eje=!eje; // se alterna el movimento en ejes para que 1 baje o suba y 1 vaya a izqda o drcha
    if ((eje && !movx && !movx_abs) || (!eje && !movy)) // por si no hay movimiento en el eje indicado se cambia al otro
        eje=!eje;
    if (eje && (movx || movx_abs))
    {
        xabs_ant=x_abs;
        x_abs+=movx_abs;
        xant=x;
        x+=movx;
    }
    if (!eje && movy)
    {
        yant=y;
        y+=movy;
    }
    //printf("x: %d y: %d eje: %s\n",x,y,eje?"X":"Y");
    if (y>ALTO_PANTALLA){
        if (player->estado)
        vidas--;
        player->estado=false;
    }
    if ((y==ysig) || (x==xsig) || (x_abs==xabs_sig)) // Hacemos que siempre comience el movimiento el el eje x
        eje=true;
}

bool Tipo_personaje::colisionNPCs(void)
{
    int contador=0;
    static int x_abs_max=0;
    bool colision_con_NPCs=false;

    if (!x_abs_max) // Para aceleración del cálculo (sólo se calcula la primera vez)
        x_abs_max=TOTAL_ANCHO_TILES*ANCHO_TILE-ANCHO_PANTALLA;
    colision->asignar_cuadro(&player->spriteasociado->cuadros,1,player->x,player->y);
    // Sólo comprobamos los activos sin pasarnos del total de NPCS
    while (contador<TOTAL_NPCS)
    {
        if (npc[contador]->estado)
        {
            colision->asignar_cuadro(&npc[contador]->spriteasociado->cuadros,2,npc[contador]->x,npc[contador]->y);
            colision->detectar();
            if (colision->col1.colisionado)
                colision_con_NPCs=true;
            if (colision->col1.colisionado) // Dependiendo del tipo de NPC tendremos una reacción del NPC o el personaje
                switch (npc[contador]->tipoNPC)
                {
                case 1 :
                case 2 :// NPC de desplazamiento lateral, Persecución, lanzador
                    if (colision->col1.col_inf)    // El personaje lo destruye
                    {
                        npc[contador]->estado=false;
                        mapa[npc[contador]->idx][npc[contador]->idy]=0;
                        player->puntos+=npc[contador]->puntos_off;
                        player->altura=SALTO;
                    }
                    else   // NPC destruye al personaje
                    {
                        if (player->estado)
                            player->vidas--;
                        player->estado=false;
                    }
                    DibujarVidas(730,578);
                    break;
                case 3 :
                    if (colision->col1.col_inf) // Se apoya encima
                    {
                        player->inerciay=1;
                        ysig=yant;
                    }
                    else
                        xsig=xant;
                    break;
                case 4 : // NPC boss
                case 6 : // Zona de riesgo
                if (player->estado)
                    player->vidas--;
                    player->estado=false;
                    DibujarVidas(730,578);
                    break;
                case 5 : // NPC Plataforma móvil
                    if (npc[contador]->ejemovimiento) // Movimiento horizontal
                        if (colision->col1.col_inf) // Colisión inferior del personaje
                        {
                            player->inerciay=1;
                            if (npc[contador]->movx>0) // Desplazamiento a derecha
                                if ((x>ANCHO_MOV_PANTALLA_FIN)&&(x_abs<x_abs_max))
                                    xabs_sig+=npc[contador]->movx;
                                else if (x<=ANCHO_PANTALLA-ANCHO_PERSONAJE)
                                    xsig+=npc[contador]->movx;
                                else ;
                            else if (npc[contador]->movx<0) // Desplazamiento a izquierda
                                if ((x<ANCHO_MOV_PANTALLA_INI)&&(x_abs>0))
                                    xabs_sig+=npc[contador]->movx;
                                else if (x>0)
                                    xsig+=npc[contador]->movx;
                                else ;
                            altura=0;
                            ysig=yant; // Se vuelve a la posición antes de la colisión
                        }
                        else
                        {
                            if (colision->col1.col_sup) // Colisión superior del personaje
                            {
                                xsig=xant;
                                ysig=yant;
                                altura=0;
                            }
                            else // Colisión lateral
                                if (colision->col1.col_drcha)
                                    xsig--;
                                else if (colision->col1.col_izqda)
                                    xsig++;
                        }
                    else // Movimiento vertical
                        if (colision->col1.col_inf) // Colisión inferior del personaje
                        {
                            if (npc[contador]->movx>0) // Movimiento descendente de la plataforma
                                ysig=yant;
                            else

                                ysig=y-1; // Se vuelve a la posición antes de la colisión

                            inerciay=1;

                        }
                        else // Colisión superior del personaje
                        {
                            if (colision->col1.col_sup) {
                                if (npc[contador]->movx>0) // Movimento descendente de la plataforma
                                    ysig=y+1;
                                else // Movimiento ascendente de la plataforma
                                    ysig=y+1;
                                xsig=xant;
                                altura=0;
                            }
                            else // Colisión lateral
                                if (colision->col1.col_drcha)
                                    xsig--;
                                else if (colision->col1.col_izqda)
                                    xsig++;
                        }
                    break;
                case 7 : // NPC Objetos recolectables
                    switch (npc[contador]->tipoobjeto)
                    {
                    case 2 :
                        player->c_disparos-=5;
                        break;
                    case 1 :
                        vidas++;

                        break;
                    }
                    mapa[npc[contador]->idx][npc[contador]->idy]=0;
                    npc[contador]->estado=false;
                    npc[contador]->dibujar=false;
                    DibujarVidas(730,578);
                    break;
                }
        }
        contador++;
    }
    return colision_con_NPCs;
}

bool Tipo_personaje::colisiontiles(void)
{
    int i,j,x1,y1,x2,y2,xtile1,ytile1,xtile2,ytile2;
    bool existe_colision=false;

    // Hacer comprobación con todos los tiles que continen al personaje
    x1=x_abs+x;
    y1=y;
    x2=x1+ANCHO_PERSONAJE-1;
    y2=y1+ALTO_PERSONAJE-1;
    xtile1=x1/ANCHO_TILE;
    ytile1=y1/ALTO_TILE; // esquina superior izquierda
    xtile2=x2/ANCHO_TILE;
    ytile2=(y2+1)/ALTO_TILE; // esquina inferior derecha
    if (ytile2<ALTO_PANTALLA_TILES)   // Si no se ha superado el rango inferior de la pantalla
    {
        colision->asignar_cuadro(&player->spriteasociado->cuadros,1,x1,y1);

        for (i=xtile1; !existe_colision && (i<=xtile2); i++)   // columnas
        {
            for (j=ytile1; !existe_colision&& (j<=ytile2); j++)   // filas
            {
                if ((mapa[i][j]>0) && (mapa[i][j]<65))
                {
                    colision->asignar_cuadro(&Sprite_tiles_estaticos->cuadros,2,i*ANCHO_TILE,j*ALTO_TILE);
                    colision->detectar();
                    if (colision->col1.colisionado)
                    {

                        if (colision->col1.col_inf)
                        {
                            inerciay=0;
                            y=ysig=yant;

                        }
                        else if (colision->col1.col_sup)
                        {
                            y=ysig=yant;
                            altura=0;
                        }
                        else if ((colision->col1.col_drcha) || (colision->col1.col_izqda))
                        {
                            x_abs=xabs_sig=xabs_ant;
                            x=xsig=xant;
                            inerciax=0;
                        }
                        existe_colision|=colision->col1.colisionado;
                    }
                }
            }
        }
    }
    return existe_colision;
}

bool Tipo_personaje::colisionbalas(void)
{
    int contador;
    bool continuar=true,colision_con_bala=false;

    colision->asignar_cuadro(&player->spriteasociado->cuadros,1,x,y);
    for (contador=0; continuar&&(contador<TOTAL_BALAS_ENEMIGAS); contador++)
        if (Balas_Enemigas[contador]->activa &&  !Balas_Enemigas[contador]->colisionada)   // Si está activa se realiza comprobación
        {
            colision->asignar_cuadro(&Balas_Enemigas[contador]->spriteasociado->cuadros,2,Balas_Enemigas[contador]->x,Balas_Enemigas[contador]->y);
            colision->detectar();
            if (colision->col1.colisionado)  // Colisión con bala por definición
                if (colision->col1.col_sup || colision->col1.col_inf) // Si colisionamos con la bala por arriba o abajo, destruimos la bala
                {
                    Balas_Enemigas[contador]->colisionada=true;
                    altura=SALTO;
                    puntos+=3;
                }
                else   // Muere el personaje
                {
                    if (player->estado) {
                        vidas--;
                        player->estado = false;
                        continuar=false;
                        colision_con_bala=true;
                        DibujarVidas(730,578);
                    }
                }
        }
    return colision_con_bala;
}

// Clase Tipo_NPC

void Tipo_NPC::updatemovx(void)   // actualización del movimientos NPC de persecución, desplazamiento lateral y plataformas móviles
{
    int inercia_negativa,inercia_positiva;

    inercia_negativa=movx<0?movx:-movx;
    inercia_positiva=movx>0?movx:-movx;
    switch (tipoNPC)
    {
    case 1 : // Desplazamiento lateral
        if ((punto_xabs+x<=rangoini) || (punto_xabs+x>=rangofin))
            movx=-movx;
        break;
    case 5 : // plataforma móvil
        if (ejemovimiento) // para plataforma horizontal
            if ((punto_xabs+x<rangoini) || (punto_xabs+x>rangofin))
                movx=-movx;
            else ;
        else // para plataforma vertical
            if ((y<rangoini) || (y>rangofin))
                movx=-movx;
            else ;
        break;
    case 2 : // Persecución
        movx=(punto_xabs+x<x_abs+player->x)?inercia_positiva:inercia_negativa;
        break;
    case 3 :

    case 4:
        if (player->x<x)
            direcciondisparo=false;
        else
            direcciondisparo=true;
    break;
    }
}

void Tipo_NPC::sig_xy(void)
{
    if (estado) {
        xant=punto_xabs-x_abs+x;
        yant=y;
        punto_xabs=x_abs;
        if ((tipoNPC==1) || (tipoNPC==2) || (tipoNPC==4)) // Desplazamiento lateral, persecución y boss
        {
            xsig=xant+movx;
            ysig=yant+CAIDA_NPC;
        }
        else if (tipoNPC==5) // Plataforma móvil
        {
            if (ejemovimiento) // para plataforma horizontal
                xsig=xant+movx;
            else   // para plataforma vertical
            {
                xsig=xant;
                ysig=yant+movx;
            }
        }
        else if ((tipoNPC==6) || (tipoNPC==3) || (tipoNPC==7)) // Zona de riesgo, lanzador y, vidas y llaves, boss
        {

            xsig=xant;
        }
    }
}

bool Tipo_NPC::existe_movimiento(void)
{
    return (x!=xsig) || (y!=ysig);
}

void Tipo_NPC::mueve_1(void)
{
    int muevex=0,muevey=0; // Existencia de movimiento en los distintos ejes
    static bool eje=false;


    if (xsig>x)
        muevex=1;
    else if (xsig<x)
        muevex=-1;
    if (ysig>y)
        muevey=1;
    else if (ysig<y)
        muevey=-1;
    eje=!eje; // se alterna el movimento en ejes para que 1 baje o suba y 1 vaya a izqda o drcha
    if ((eje && !muevex) || (!eje && !muevey)) // por si no hay movimiento en el eje indicado se cambia al otro
        eje=!eje;
    if (eje && muevex)
    {
        xant=x;
        x+=muevex;
    }
    if (!eje && muevey)
    {
        yant=y;
        y+=muevey;
    }
    if ((y==ysig) || (x==xsig)) // Hacemos que siempre comience el movimiento el el eje x
        eje=true;
}

bool Tipo_NPC::colisiontiles(void)
{
    int i,j,x1,y1,x2,y2,xtile1,ytile1,xtile2,ytile2;
    bool existe_colision=false;

    // Hacer comprobación con todos los tiles que continen al personaje
    if (tipoNPC==1 || tipoNPC==2 || tipoNPC==4)
    {
        x1=x_abs+x+1;
        y1=y;
        x2=x1+ANCHO_NPCS[tipoNPC-1]-1;
        y2=y1+ALTO_NPCS[tipoNPC-1]-1;
        xtile1=x1/ANCHO_TILE;
        ytile1=y1/ALTO_TILE; // esquina superior izquierda
        xtile2=x2/ANCHO_TILE;
        ytile2=(y2+1)/ALTO_TILE; // esquina inferior derecha
        if (ytile2<ALTO_PANTALLA_TILES)   // Si no se ha superado el rango inferior de la pantalla
        {
            colision->asignar_cuadro(&spriteasociado->cuadros,1,x1,y1);

            for (i=xtile1; !existe_colision && (i<=xtile2); i++)   // columnas
            {
                for (j=ytile1; !existe_colision&& (j<=ytile2); j++)   // filas
                {
                    if ((mapa[i][j]>0) && (mapa[i][j]<65))
                    {
                        colision->asignar_cuadro(&Sprite_tiles_estaticos->cuadros,2,i*ANCHO_TILE,j*ALTO_TILE);
                        colision->detectar();
                        if (colision->col1.colisionado)
                        {
                            if (colision->col1.col_inf) {
                                ysig=y=yant-1;
                            }
                            else if (colision->col1.col_sup)
                            {
                                // No hacer nada si existe colisión superior ya que los NPCs no saltan
                            }
                            else if ((colision->col1.col_drcha) || (colision->col1.col_izqda))
                            {
                                if (colision->col1.col_drcha)
                                    xsig=x=xant-1;
                                else {
                                    xsig=x=xant+1;
                                }
                            }
                            existe_colision|=colision->col1.colisionado;
                        }
                    }
                }
            }
        }
    }
    return existe_colision;
}
void Tipo_NPC::colisionbalas(void)
{
    int contador;
    bool continuar=true;

    colision->asignar_cuadro(&spriteasociado->cuadros,1,x,y);
    for (contador=0; continuar && (contador<TOTAL_BALAS_PROPIAS); contador++)
        if (Balas_Propias[contador]->activa)   // Si está activa se realiza comprobación
        {
            colision->asignar_cuadro(&Balas_Propias[contador]->spriteasociado->cuadros,2,Balas_Propias[contador]->x,Balas_Propias[contador]->y);
            colision->detectar();
            if (colision->col1.colisionado)

            {
                if (tipoNPC==4) {
                    vidas--;
                    if (vidas==0)
                        pasar_a_siguiente_nivel=true;
                }
                else
                if ((tipoNPC!=5) && (tipoNPC!=6) && (tipoNPC!=7))
                {
                    mapa[idx][idy]=0;
                    player->puntos+=puntos_off;
                    estado=false;
                    continuar=false;
                }
                DibujarVidas(730,578);
                if (tipoNPC!=7)
                    Balas_Propias[contador]->activa=false;
            }
        }
}

void Tipo_NPC::colisionPlataformas(void)
{
    int i;

    if ((tipoNPC==4) || (tipoNPC==2)) {
        colision->asignar_cuadro(&spriteasociado->cuadros,1,punto_xabs+x,y);
        // Sólo colisionan los NPCs diferentes del que buscamos que estén activos y que sean bosses o de persecución
        for (i=0; i<TOTAL_NPCS; i++)
            // Si no es él mismo y es de tipo plataforma
            if (npc[i]->estado && (npc[i]->tipoNPC==5))
            {
                colision->asignar_cuadro(&npc[i]->spriteasociado->cuadros,2,npc[i]->punto_xabs+npc[i]->x,npc[i]->y);
                colision->detectar();
                if (colision->col1.col_inf) // Colisión inferior del personaje
                {
                    if (npc[i]->ejemovimiento) // Movimiento horizontal
                    {

                        x=xsig+=npc[i]->movx;
                        y=ysig=yant; // Se vuelve a la posición antes de la colisión
                    }
                    else // Movimiento vertical
                    {
                        if (npc[i]->movx>0) // Movimiento descendente de la plataforma
                            ysig=y+npc[i]->movx;
                        else {

                            ysig=y+npc[i]->movx-1; // Se vuelve a la posición antes de la colisión

                        }
                    }
                }
            }
    }
}

// Clase Tipo_Balas

void Tipo_Balas::updatexy(void)
{
    if (activa)
    {

            x+=punto_xabs-x_abs+movx;
            punto_xabs=x_abs;

        if (x<0 || x>ANCHO_PANTALLA+ANCHO_BALA)
            activa=false;
        if (activa)
            colisiontile();
    }
}

void Tipo_Balas::colisiontile(void)
{
    int x1,y1,x2,y2,xtile1,ytile1,xtile2,ytile2,xtile3,ytile3,xtile4,ytile4;
    bool col_drcha,col_izqda;

    x1=x_abs+x;
    y1=y;
    x2=x1+ANCHO_BALA;
    y2=y1+ALTO_BALA;

    // Calculamos los tiles
    xtile1=x1/ANCHO_TILE;
    ytile1=y1/ALTO_TILE; // esquina superior izquierda
    xtile2=x2/ANCHO_TILE;
    ytile2=ytile1; // esquina superior derecha
    xtile3=xtile1;
    ytile3=y2/ALTO_TILE; // esquina inferior izquierda
    xtile4=xtile2;
    ytile4=ytile3; // esquina inferior derecha
    // colisiones
    col_drcha=(mapa[xtile2][ytile2]>0 && mapa[xtile2][ytile2]<65) || (mapa[xtile4][ytile4]>0 && mapa[xtile4][ytile4]<65); // ¿colisión derecha?
    col_izqda=(mapa[xtile1][ytile1]>0 && mapa[xtile1][ytile1]<65) || (mapa[xtile3][ytile3]>0 && mapa[xtile3][ytile3]<65); // ¿colisión izquierda?
    // Comprobación de colisiones
    if (col_drcha || col_izqda)    // Colisión
    {
        //activa=false;
        colisionada=true;
    }
}

// Clase Mus_Efec
void Mus_Efec::cargar(bool Mus0_Efec1,char *ruta)
{
    char fichero[50];

    Musica0_Efecto1=Mus0_Efec1;
    iniciado=pausado=false;
    sprintf(fichero,"%s/%s",DIR_CONTENIDO,ruta);
    switch (Mus0_Efec1)
    {
    case 0 :
        AddressMusic=Mix_LoadMUS(fichero);
        break;
    case 1 :
        AddressChunk=Mix_LoadWAV(fichero);
        break;
    }
}

void Mus_Efec::liberar(void)
{
    if (Musica0_Efecto1)
        Mix_FreeChunk(AddressChunk);
    else
        Mix_FreeMusic(AddressMusic);
}

void Mus_Efec::play(void)
{
    if (sonar)
        if (pausado)
        {
            pausado=false;
            if (Musica0_Efecto1)
                Mix_Resume(canal);
            else
                Mix_ResumeMusic();
        }
        else
        {
            iniciado=true;
            if (Musica0_Efecto1)
            {
                canal=Mix_PlayChannel(-1,AddressChunk,0); // Se reproduce el efecto 1 sóla vez
            }
            else
                canal=Mix_FadeInMusic(AddressMusic,-1,1000); // Se reproduce de forma indefinida
        }
}

void Mus_Efec::pause(void)
{
    if (iniciado)
    {
        pausado=true;
        if (Musica0_Efecto1)
            Mix_Pause(canal);
        else
            Mix_PauseMusic();
    }
}

void Mus_Efec::stop(void)
{
    if (iniciado)
    {
        iniciado=false;
        if (Musica0_Efecto1)
            Mix_HaltChannel(canal);
        else
            Mix_FadeOutMusic(1000);
    }
}

void Mus_Efec::volumen(int vol)   // Valor entre 0 y 128
{
    int i;
    // Hacemos una búsqueda de todos los efectos o música para colocarlos al mismo volumen
    if (Musica0_Efecto1)
        for (i=0; i<TOTAL_SONIDOS; i++)
            if (sonido[i]->Musica0_Efecto1)
                Mix_VolumeChunk(sonido[i]->AddressChunk,vol);
            else ;
    else
        Mix_VolumeMusic(vol); // Hay un canal especial para las músicas y no puede haber 2 simultaneamente
}

// Clase Barra_Control
void Barra_Control::dibujar(void)
{
    SDL_Rect destino;
    // Dibujado de la barra mayor vertical
    destino.x=(int)x;
    destino.y=(int)y;
    destino.h=(int)h;
    destino.w=(int)w;
    SDL_FillRect(screen, &destino,SDL_MapRGB(screen->format,0,100,200));//dibujamos la barra
    //dibujamos el controlador
    destino.w=10;
    destino.h=25;
    destino.y-=5;
    destino.x=(int)((w/max)*value+x);
    SDL_FillRect(screen, &destino,SDL_MapRGB(screen->format,100,125,0));
}

// Clase Barra_Control
void Barra_Control::mover(void)
{
    if (capturado)

        value=((mouse.x-x)/(w/max));
    if (value<0)
        value=0;
    if (value>max)
        value=max;

}

bool Barra_Control::comprobar_click(void)
{
    if (mouse.pulsado)
    {
        if ((mouse.x>=(w/max)*value+x && mouse.x<=(w/max)*value+x+20)&&(mouse.y>=y && mouse.y<=y+h )) // Si se encuentra en el rango de la barra
            capturado=true;
        return true;
    }
    else
        return false;
}

clase_cola::clase_cola(void)
{
    cola=NULL;
    v_tamano=0;
}

void clase_cola::meter(void *item)
{
    struct tipo_cola *aux1,*aux2;

    aux1=new struct tipo_cola; // Elemento nuevo
    aux1->contenido=item;
    aux1->sig=NULL;
    aux2=cola; // Posición actual de la cola
    if (aux2==NULL) // Se añade el primer elemento
        cola=aux1;
    else   // Se añade un elemento por detrás
    {
        while (aux2->sig!=NULL) // buscamos el último elemento
            aux2=aux2->sig;
        aux2->sig=aux1; // Lo insertamos detrás el último
    }
    v_tamano++;
}

void clase_cola::sacar(void)
// Hay que asegurarse de que el elemento ha sido liberado previamente
{
    struct tipo_cola *aux;

    if (cola!=NULL)   // Si existe elemento en la cola
    {
        aux=cola;
        if (cola->sig!=NULL) // Si existen al menos 2 elementos
            cola=cola->sig;
        else // Si sólo existe 1 elemento
            cola=NULL;
        delete aux;
        v_tamano--;
    }
}

int clase_cola::tamano(void)
{
    return v_tamano;
}

void *clase_cola::valor(void)
{
    return cola->contenido;
}

bool clase_cola::vacia(void)
{
    return (cola==NULL);
}



// Clase clase_colision

clase_colision::clase_colision(void)
{
    c1=NULL;
    c2=NULL;
}

void clase_colision::asignar_cuadro(struct tipo_cuadro_dibujo *c, int cuadro,int posx,int posy)
{
    if (cuadro==1)
    {
        c1=c;
        px1=posx;
        py1=posy;
        //printf("y_pers: %d\n",posy+c1->cuadro_inf.y+c1->cuadro_inf.h);
    }
    else
    {
        c2=c;
        px2=posx;
        py2=posy;
        //printf("y_npc: %d\n",posy+c2->cuadro_sup.y);
    }
}

void clase_colision::detectar(void)
{
    // Comprobación de colisiones según la siguiente fórmula
    //(x1+w1)>x2)&&((y1+h1)>y2)&&((x2+w2)>x1)&&((y2+h2)>y1)
    int x1,y1,w1,h1,x2,y2,w2,h2,i,j,centro1,centro2;

    // Inicializar las colisiones
    col1.col_sup=false;
    col1.col_inf=false;
    col1.col_drcha=false;
    col1.col_izqda=false;
    col1.colisionado=false;
    col2.col_sup=false;
    col2.col_inf=false;
    col2.col_drcha=false;
    col2.col_izqda=false;
    col2.colisionado=false;
    // Buscar las colisiones
    for (i=0; !col1.colisionado && (i<3); i++)   // Los tres cuadros de c1
    {
        switch (i)
        {
        case 0 : // cuadro superior
            x1=px1+c1->cuadro_sup.x;
            y1=py1+c1->cuadro_sup.y;
            w1=c1->cuadro_sup.w;
            h1=c1->cuadro_sup.h;
            break;
        case 1 : // cuadro central
            x1=px1+c1->cuadro_cent.x;
            y1=py1+c1->cuadro_cent.y;
            w1=c1->cuadro_cent.w;
            h1=c1->cuadro_cent.h;
            break;
        case 2 : // cuadro inferior
            x1=px1+c1->cuadro_inf.x;
            y1=py1+c1->cuadro_inf.y;
            w1=c1->cuadro_inf.w;
            h1=c1->cuadro_inf.h;
            break;
        }
        centro1=x1+w1/2;
        for (j=0; !col1.colisionado && (j<3); j++)   // Los tres cuadros de c2
        {
            switch (j)
            {
            case 0 : // cuadro superior
                x2=px2+c2->cuadro_sup.x;
                y2=py2+c2->cuadro_sup.y;
                w2=c2->cuadro_sup.w;
                h2=c2->cuadro_sup.h;
                //if (i==2)
                    //printf("y_1: %d y_2: %d\n",y1+h1,y2);
                break;
            case 1 : // cuadro central
                x2=px2+c2->cuadro_cent.x;
                y2=py2+c2->cuadro_cent.y;
                w2=c2->cuadro_cent.w;
                h2=c2->cuadro_cent.h;
                break;
            case 2 : // cuadro inferior
                x2=px2+c2->cuadro_inf.x;
                y2=py2+c2->cuadro_inf.y;
                w2=c2->cuadro_inf.w;
                h2=c2->cuadro_inf.h;
                break;
            }
            centro2=x2+(w2/2);

            if (((x1+w1-1)>=x2)&&((y1+h1-1)>=y2)&&((x2+w2-1)>=x1)&&((y2+h2-1)>=y1))   // Colisión
            {

                col1.colisionado=true; // Mostramos en ambos elementos que ha habido colisión
                col2.colisionado=true;
                // Detectar que tipo de colisión es
                if (!i && (j==2)) // c1 tiene colision superior
                    if ((y1==y2+h2-1) || (y1==y2+h2))
                    {
                        col1.col_sup=true;
                        col2.col_inf=true;
                    }
                if ((i==2) && !j) // c1 tiene colision inferior
                    if ((y1+h1-1==y2) || (y1+h1==y2))
                    {
                        col1.col_inf=true;
                        col2.col_sup=true;
                    }
                // c1 y c2 tienen colision lateral (puede tenerla por cualquiera de los tres cuadros)
                // Buscamos si la colisión es derecha o izquierda
                if (!col1.col_sup && !col1.col_inf)
                    if (centro2<centro1)   // Colisión de c1 por la izquierda y de c2 por la derecha
                    {
                        col1.col_izqda=true;
                        col2.col_drcha=true;
                    }
                    else   // Colisión de c1 por la derecha y de c2 por la izquierda
                    {
                        col1.col_drcha=true;
                        col2.col_izqda=true;
                    }
            }
        }
    }
}

Clase_Lista::Clase_Lista()
{
    lista=NULL;
    tamano=0;
};
int Clase_Lista::tamano_actual(void)
{
    return tamano;
};

void Clase_Lista::insertar(void *objeto,int posicion)
{
    int contador=0;
    struct elemento *auxiliar,*nuevo;
    nuevo=new struct elemento;
    nuevo->datos=objeto;
    nuevo->siguiente=NULL;
    auxiliar=(struct elemento*)lista;
    if (lista==NULL)
        lista=nuevo;
    else
    {
        while (auxiliar->siguiente!=NULL && contador<posicion)  //posicion del anterior
        {
            auxiliar=auxiliar->siguiente;
            contador++;
        }
        nuevo->siguiente=auxiliar->siguiente;
        auxiliar->siguiente=nuevo;

    }
    tamano++;
};

void Clase_Lista::borrar(int posicion)
{
    int contador=0;
    struct elemento *aux,*tmp;
    aux=(struct elemento*)lista;


    if (lista!=NULL)
    {
        if (!posicion) // Borrar el primer elemento
        {
            lista=aux->siguiente;
            delete (aux);
        }
        else {
            while ((aux->siguiente!=NULL) && (contador<posicion-1))
            {
                aux=aux->siguiente;
                contador++;
            }
            if (contador==posicion-1) // Hemos encontrado el que queremos borrar
            {
                tmp=aux->siguiente;
                aux->siguiente=tmp->siguiente;
                delete tmp;
            }
            else ; // No hacemos nada si queremos borrar un elemento que es posterior a los que existen
        }
        tamano--;
    }

};

void *Clase_Lista::valor_at(int posicion)
{
    int contador=0;
    struct elemento *aux;
    aux=(struct elemento*)lista;
    if (lista!=NULL)
    {
        while (aux->siguiente!=NULL && contador<posicion)
        {
            contador++;
            aux=aux->siguiente;
        }
        return aux->datos;
    } else
        return NULL;
};
void *Clase_Lista::nodo_at(int posicion)
{
    int contador=0;
    struct elemento *aux;
    aux=(struct elemento*)lista;
    if (lista!=NULL)
    {
        while (aux->siguiente!=NULL && contador<posicion)
        {
            contador++;
            aux=aux->siguiente;
        }
        return aux;
    } else
        return NULL;
};

bool Clase_Lista::vacia()
{
    return (lista==NULL);
};
void Clase_Lista::vaciar()
{
    while (!vacia())
        borrar(0);
};

void Clase_Lista::Swap(void **ptr1,void **ptr2)
{
    void *ptraux;
    ptraux=*ptr1;
    *ptr1=*ptr2;
    *ptr2=ptraux;
};
void Clase_Lista::ordenar_caracteres(void)
{
    int b;
    struct elemento *ptr1,*ptr2;
    for (int a=0; a<tamano_actual(); a++)
        for (b=tamano_actual()-2; b>=a; b--)
            if (strcmp((char*)valor_at(b),(char*)valor_at(b+1))>0)
            {
                ptr1=(struct elemento *)nodo_at(b);
                ptr2=(struct elemento *)nodo_at(b+1);

                Swap(&(ptr1->datos),&(ptr2->datos));
            }
}
