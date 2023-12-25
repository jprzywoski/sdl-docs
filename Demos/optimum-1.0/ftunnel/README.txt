

Free-directionnal Tunnel
========================

CorTeX / Optimum

Ce programme se balade dans un tunnel avec position  et direction de
vis�e libre.

Alors Comment Ca Marche ?

La generation est divisee en 2 parties :
 
 * Un calcul exact avec des arctangentes et des racines carrees,
   effectu� sur une grille de largeur 8 pixels
 * Une interpolation entre ces pixels calcul�, pour les 64 pixels 
   de chaque case de la grille
   
-----

 Le calcul exact:

  Vous voulez la formule ?...
 on considere le tunnel orient� le long de l'axe Z	
 pour faciliter le calcul, la position de l'observateur est en (X,0,Z);
 l'absence de Y est compens� par un Dalpha qui sert � donner l'angle
 en coordonn�es cylindrique de l'observateur (enfin a peu pres)
 
 Pour definir l'orientation de visee, on utilise des coordonn�es
 spherique (alpha, theta) puis on transmet a la routine de calcul les
 vecteurs C, EX, EY (vecteur de visee, vecteur X-Ecran et Y-Ecran)
 A partir de ces vecteurs on calcul l'intersection, pour chaque pixel,
 du rayon de visee avec le cylindre (en X et Y) a partir d'une
 petite equation de 2e degre, d'ou on tire l'angle du point
 (stock� dans le tableau alpha[][]) et qui correspond a "x" dans la
 texture; on calcule ensuite Z (zede[][]) qui correspond a "y" dans la
 texture.
 
 Bon ben maintenant, si tout se passe comme je le prevois, vous n'avez
 rien compris a mes explications, je vous engage donc a regard� le
 source qui n'est guere  mieux comment�.

--- 
 Interpolation

 Pour l'image finale, on interpole par zones de 8x8 pixels a partir
 des coordonn�es calcul�es precedemment.
 Pour cela on transforme les Floats en nombres a virgules fixes 16-16,
 et on progresse dans la texture par une methode incrementale des plus
 classiques.
 
 On notera qu'il y a un petit probleme a la jonction entre les deux
 extremit�s de la textures, qui se rejoignent en 0 = 2*PI; 
 L'interpolation bete produit une interpolation entre 0 et 128 (par
 ex) alors qu'en fait c'est 0 - (-1).
  Pour l'instant j'ai juste mis un petit Fix qui n'est pas terrible
 (on egalise les deux bornes, ce qui produit des marches d'escalier)

---
 Quelques notes :
 ce genre de methode est applicable a plein de domaines ou la formule
 exacte est complexe (mais elle doit etre continue si on ne veut pas
 se compliquer les choses)
 
 J'ai mis en commentaire dans la routine de calcul quelques ptits
 changement qui produisent des effets rigolos, vous testerez ca
 
 
***********
 cortex rating :
  * optimalisation : 4  ( interpolation entre les pixels exacts;
			  cependant le code d'interpolation meriterait
			  d'etre fait en assembleur a la main - gcc
			  n'est pas optimal de ce cot� la (mettez un
			  -S dans les CCFLAG pour obtenir la sortie 
			  assembleur))
  * Portabilit� / Compatibilit�: 5
			  les pixels sont simplement transfer�s de la
			  texture vers le buffer, donc pas de pb pour
			  changer de bpp (mais vivement que je finisse
			  optiLib, quand meme...)
			  Pour d'autres systemes que Intel, faut voir
			  pour la taille des int etc... pour le fixed-point

 * finition : 3           Ben je me suis pas foul� pour la texture...
			  et la trajectoire est assez hasardeuse

----
 greetings :

 L'id�e de ce programme est originellement inspir�e par une doc de BlackAxe / KoLOr;
 merci a lui !


------
 CTX / Ultimatum  , 25 Mars 98
		
