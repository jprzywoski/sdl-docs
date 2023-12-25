#ifndef OPTI_CVRT_H
#define OPTI_CVRT_H


/* equivalent de memset, mais pour des shorts (2 octets)
 * (bien pratique quand on veut remplir d'une couleur un buffer
 * en mode rgb565 16 bits...)
 * Tiens, mais qu'est ce que cette fonction fait ici au fait ??
 */
void opti_memwset(unsigned short *dst, unsigned short value, int nb);
	

/* quelques routines de conversion... 
 * ne pas vous affoler en voyant toutes ces declaration:
 * Un meme codage couleur peut posseder 4 noms differents, selon
 * le point de vue ou on le decrit :
 * par exemple pour un mode RGB (depth 24bits, mais alignement 32 bits):
 *             lARGB pour un int [ARGB] sur une machine Less significant byte first
 *             mBGRA pour un int [BGRA]                 Most
 *             bBGRA pour 4 octets B-G-R-A
 *              ARGB pour un int [ARGB] sur la machine actuellement utilisee
 * => les 3 premieres denominations sont identiques, ainsi que la 4eme sur un PC
 * 
 * Donc en pratique, on utilisera plutot le nom sans prefixe "par defaut"
 * sauf pour specifier sans ambiguité un format(enfin c'est vous qui voyez,
 * et puis de toute facon tout sera bien explique dans l'optilib finale... ;) )
 * 
 * Evidemment ca se casse la gueule pour le 16bits, vu qu'une composante est
 * a cheval sur 2octets... donc ici RGB_565 = un short [RGB] vu par un PC
 */

/* 24/32 bits vers 16 bits */
void SOpti_cvrt_ARGB_8888_RGB_565(unsigned char *src, unsigned char *dst, int nbpixels);
void SOpti_cvrt_bBGRA_8888_RGB_565(unsigned char *src, unsigned char *dst, int nbpixels);
void SOpti_cvrt_mBGRA_8888_RGB_565(unsigned char *src, unsigned char *dst, int nbpixels);
void SOpti_cvrt_lARGB_8888_RGB_565(unsigned char *src, unsigned char *dst, int nbpixels);
	
/* 16 bits vers 24/32 */
void SOpti_cvrt_RGB_565_ARGB_8888(unsigned char *src, unsigned char *dst, int nbpixels);
void SOpti_cvrt_RGB_565_bBGRA_8888(unsigned char *src, unsigned char *dst, int nbpixels);
void SOpti_cvrt_RGB_565_mBGRA_8888(unsigned char *src, unsigned char *dst, int nbpixels);
void SOpti_cvrt_RGB_565_lARGB_8888(unsigned char *src, unsigned char *dst, int nbpixels);

/* 24/32 bits exotique ? */
void SOpti_cvrt_ABGR_8888_RGB_565(unsigned char *src, unsigned char *dst, int nbpixels);
void SOpti_cvrt_bRGBA_8888_RGB_565(unsigned char *src, unsigned char *dst, int nbpixels);
void SOpti_cvrt_mRGBA_8888_RGB_565(unsigned char *src, unsigned char *dst, int nbpixels);
void SOpti_cvrt_lABGR_8888_RGB_565(unsigned char *src, unsigned char *dst, int nbpixels);

#endif
