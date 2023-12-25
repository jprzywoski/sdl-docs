 ________________/  the CTX files   \
|                                    |
| difference                         |
| 16 & 32 bpp                        |
|                                    |
| corTeX / Optimum  14 Dec. 1998     |
|                                    |
|____________________________________|

Sample data files can be found at:
http://optimum.citeweb.net/FX/difference/difference_gfx.tar.gz

<English>

 [the FX]

As the main effect routine has only one line of code, lets show it :

for (i=0; i<pixels; i++) 
     *(dst+i) =  abs( *(src+i) - *(dst+i));

You can see immediatly this is quite a simple one..
What it does is a "difference" (hence the name) between two
gfx buffers and put the result in one of the two.
"difference" is not "substraction" ! Diff. performs a
backward wrapping to the result (the abs() ) wich prevents from
abrupt 0->255 transitions (or sticking to 0 when underflow).

This effect is merely taken from the layer operations in Gimp
(The Linux drawing tool similar to photoshop)
(http://www.gimp.org/); I coded it only
to see how it behaves into animation after seeing the quite
psychedelic results on stills, and the interference patterns
that result of "differencing" an image from itself slightly 
distorted.

Yes I know it is really not a super-new effect (Interference exists
for many years...); 
Perhaps it can gain new result depending of what gfx/textures you use (see the
screenshot at the webpage or the sample pattern to see the variety of result)
By the way I coded it only in C, not assembly
(appart from the classical convertion for 16 bpp).The fps is
very perfectible and optimization is left to the reader, as usual when
I was too lazy to do it myself at the end of the white night..

 [ gfx Data format ]
You need to start the program with a file name in parameter.
This is where the gfx texture is read from. As I dont want to write a
gif/jpg loader, I only read raw datas in 32bpp; and as Gimp can
output nearly raw data quite simply (the .gbr gimp brush) this is what
I read. So the file must be a 32bits/pixel raw dump, with a dummy
header of 39 bytes (skipped), and image dimension 256x256. Nothing is
tested ! If you want to create a .gbr readable by the prog, do this :
(with Gimp of course..)
-have your image set to 256x256 true color
-have only one layer
-add an alpha channel to the layer (right click on the layer
window... then menu...)
-add layer mask(same thing) 
(The layer mask is here to ensure you have a 32bpp, and not 24bpp...
this is only for padding purpose)
-save as: type the name with a .gbr extension
-accept the default save parameter (to not change the header size...)
-OK!

 I know this is a dirty thing... write a general gfx loader for me
 if you want :)


<Francais>

 [ L' FX proprement dit ]

Comme la routine de base prend seulement une ligne, la voila :

for (i=0; i<pixels; i++) 
     *(dst+i) =  abs( *(src+i) - *(dst+i));

On voit tout de suite qu'il est pas tres complique...
Ce qu'il fait est une "difference" (d'ou le nom) entre deux buffers
graphiques et met le resultat dans l'un d'eux.
"difference" n'est pas "soustraction" ! Diff. effectue un rebouclage
(backward wrapping dans la version toefelisee...) (l' abs() ) qui 
annule les transitions brutales 0->255 (ou les saturation a 0 en cas
d'underflow).

Cet effect est pris de base dans les operations entre layers de Gimp
(le prog Linux similaire a photoshop) (http://www.gimp.org/)
Je l'ai juste code pour voir ce que ca donnait dans des animations,
apres avoir vu les resultats plutot psychedeliques sur image fixe, ou
les motifs d'interference qd on l'applique sur une meme image un peu
decalee.

Oui, je sais, c'est pas le super-nouvo fx hyper-exclusif
(Interference, ca existe depuis quelques annees hein...); mais
peut-etre plutot que ca peut donner de nouveaux resultats selon les
gfx/textures utilisees (voir les screenshot du site / patterns
d'exemple pour voir la variete de resultat).
D'ailleur c'est tout en C, pas d'assembleur (a part les
incontournables routines de conversion 16bpp...). le taux de FPS est
perfectible, jvous laisse faire ca, moi j'avais la flemme a la fin
de ma nuit blanche..

 [ format des donnees graphiques ]
Il faut lancer le programme avec un nom de fichier en parametre.
C'est ici que la texture sera lue. Comme j'ai pas envie d'ecrire un
loader de gif/jpg, le prog ne sait lire que du raw 32bpp; et comme
Gimp sait ecrire du presque Raw simplement (le .gbr Gimp brush) c'est
ce que je lis. Les fichiers doivent donc etre en 32bpp raw, 256x256,
et un header de 39 octets (.gbr ..pas examine de toute facon). Rien
n'est teste ! Si vous voulez creer un .gbr lisible par le prog, faire
ceci : (sous Gimp bien sur...)
-Avoir votre image en 256x256, true-color
-n'avoir qu'un layer
-ajouter un alpha-channel au layer (click droit dans la layer-window,
puis menu..)
- add layer mask (pareil)
(L'alpha channel n'est la que pour assurer le 32bpp au lieu du
24bpp... c'est juste du padding)
-Save As : un nom avec extension .gbr
- accepter les parametres par defaut (pour ne pas changer la taille du
header..)
-OK!

 Ouai c'est crade... t'a qu'a m'ecrire un loader general de fichier
 graphique si tu veux !..


-- 

ctx / optimum
Tue Dec 15 14:56:35 MET 1998
