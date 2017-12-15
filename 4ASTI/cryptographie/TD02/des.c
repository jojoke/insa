/****************************************************************
*                                                               *
*  -------------------------- des.c --------------------------  *
*                                                               *
****************************************************************/

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "bit.h"
#include "encrypt.h"

/****************************************************************
*                                                               *
*  Correspondances pour la transformation de la cl�.            *
*                                                               *
****************************************************************/

static const int Des_Transform[56] = {

   57, 49, 41, 33, 25, 17,  9,  1, 58, 50, 42, 34, 26, 18,
   10,  2, 59, 51, 43, 35, 27, 19, 11,  3, 60, 52, 44, 36,
   63, 55, 47, 39, 31, 23, 15,  7, 62, 54, 46, 38, 30, 22,
   14,  6, 61, 53, 45, 37, 29, 21, 13,  5, 28, 20, 12,  4

};

/****************************************************************
*                                                               *
*  Nombre de rotations pour le calcul des sous-cl�s.            *
*                                                               *
****************************************************************/

static const int Des_Rotations[16] = {

   1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1

};

/****************************************************************
*                                                               *
*  Correspondances pour le choix permut� des sous-cl�s.         *
*                                                               *
****************************************************************/

static const int Des_Permute[48] = {

   14, 17, 11, 24,  1,  5,  3, 28, 15,  6, 21, 10,
   23, 19, 12,  4, 26,  8, 16,  7, 27, 20, 13,  2,
   41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
   44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32

};

/****************************************************************
*                                                               *
*  Correspondances pour la permutation initiale des blocs.      *
*                                                               *
****************************************************************/

static const int Des_Initial[64] = {

   58, 50, 42, 34, 26, 18, 10,  2, 60, 52, 44, 36, 28, 20, 12, 4,
   62, 54, 46, 38, 30, 22, 14,  6, 64, 56, 48, 40, 32, 24, 16, 8,
   57, 49, 41, 33, 25, 17,  9,  1, 59, 51, 43, 35, 27, 19, 11, 3,
   61, 53, 45, 37, 29, 21, 13,  5, 63, 55, 47, 39, 31, 23, 15, 7

};

/****************************************************************
*                                                               *
*  Correspondances pour la fonction d'expansion des blocs.      *
*                                                               *
****************************************************************/

static const int Des_Expansion[48] = {

   32,  1,  2,  3,  4,  5,  4,  5,  6,  7,  8,  9,
    8,  9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17,
   16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25,
   24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32,  1

};

/****************************************************************
*                                                               *
*  Tableaux pour les substitutions par bo�tes-S sur les blocs.  *
*                                                               *
****************************************************************/

static const int Des_Sbox[8][4][16] = {

 {
 {14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7},
 { 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8},
 { 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0},
 {15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13},
 },

 {
 {15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10},
 { 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5},
 { 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15},
 {13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9},
 },

 {
 {10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8},
 {13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1},
 {13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7},
 { 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12},
 },

 {
 { 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15},
 {13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9},
 {10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4},
 { 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14},
 },

 {
 { 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9},
 {14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6},
 { 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14},
 {11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3},
 },

 {
 {12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11},
 {10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8},
 { 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6},
 { 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13},
 },

 {
 { 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1},
 {13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6},
 { 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2},
 { 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12},
 },

 {
 {13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7},
 { 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2},
 { 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8},
 { 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11},
 },

};

/****************************************************************
*                                                               *
*  Correspondances pour la permutation-P.                       *
*                                                               *
****************************************************************/

static const int Des_Pbox[32] = {

  16,  7, 20, 21, 29, 12, 28, 17,  1, 15, 23, 26,  5, 18, 31, 10,
   2,  8, 24, 14, 32, 27,  3,  9, 19, 13, 30,  6, 22, 11,  4, 25

};

/****************************************************************
*                                                               *
*  Correspondances pour la permutation finale des blocs.        *
*                                                               *
****************************************************************/

static const int Des_Final[64] = {

  40,  8, 48, 16, 56, 24, 64, 32, 39,  7, 47, 15, 55, 23, 63, 31,
  38,  6, 46, 14, 54, 22, 62, 30, 37,  5, 45, 13, 53, 21, 61, 29,
  36,  4, 44, 12, 52, 20, 60, 28, 35,  3, 43, 11, 51, 19, 59, 27,
  34,  2, 42, 10, 50, 18, 58, 26, 33,  1, 41,  9, 49, 17, 57, 25

};

/****************************************************************
*                                                               *
*  Type pour indiquer si l'on chiffre ou d�chiffre.             *
*                                                               *
****************************************************************/

typedef enum DesEorD_ {chiffrement, dechiffrement} DesEorD;

/****************************************************************
*                                                               *
*  ------------------------- permute -------------------------  *
*                                                               *
****************************************************************/

static void permute(unsigned char *bits, const int *corresp, 
                    int n) {

unsigned char      temp[8];

int                i;

/****************************************************************
*                                                               *
* Permute le tampon � l'aide d'une correspondance de n entr�es. *
*                                                               *
****************************************************************/

memset(temp, 0, (int)ceil(n / 8));

for (i = 0; i < n; i++)
   bit_set(temp, i, bit_get(bits, corresp[i] - 1));

memcpy(bits, temp, (int)ceil(n / 8));

return;

}

/****************************************************************
*                                                               *
*  ------------------------ des_main -------------------------  *
*                                                               *
****************************************************************/

static int des_main(const unsigned char *source, 
                    unsigned char *cible, 
                    const unsigned char *cle, DesEorD sens) {

static unsigned char sous_cles[16][7];

unsigned char        temp[8],
                     cle_g[4],
                     cle_d[4],
                     bloc_g[6],
                     bloc_d[6],
                     bloc_f[6],
                     bloc_x[6],
                     bloc_s;

int                  lig,
                     col,
                     i,
                     j,
                     k,
                     p;

/****************************************************************
*                                                               *
*  Si cle vaut NULL, on utilise les sous-cl�s d'un appel        *
*  pr�c�dent.                                                   *
*                                                               *
****************************************************************/

if (cle != NULL) {

   /*************************************************************
   *                                                            *
   *  Copie locale de la cl�.                                   *
   *                                                            *
   *************************************************************/

   memcpy(temp, cle, 8);

   /*************************************************************
   *                                                            *
   *  Permutation et compression de la cl� en 56 bits.          *
   *                                                            *
   *************************************************************/

   permute(temp, Des_Transform, 56);

   /*************************************************************
   *                                                            *
   *  On d�coupe la cl� en deux blocs de 28 bits.               *
   *                                                            *
   *************************************************************/

   memset(cle_g, 0, 4);
   memset(cle_d, 0, 4);

   /* Construction des cles gauche et droite */
   for (j = 0; j < 28; j++)
      bit_set(cle_g, j, bit_get(temp, j));

   /*************************************************************
   *                                                            *
   *  Calcul des sous-cl�s pour chaque round.                   *
   *                                                            *
   *************************************************************/

   for (i = 0; i < 16; i++) {

      /**********************************************************
      *                                                         *
      *  Rotation de chaque bloc en fonction du round.          *
      *                                                         *
      **********************************************************/

      /* Etape de rotation */
      bit_rot_left(cle_g, 28, Des_Rotations[i]);
      //TODO
      bit_rot_left(cle_d, 28, Des_Rotations[i]);

      /**********************************************************
      *                                                         *
      *  Concat�nation des blocs dans une seule sous-cl�.       *
      *                                                         *
      **********************************************************/

      for (j = 0; j < 28; j++)
      {
         bit_set(sous_cles[i], j, bit_get(cle_g, j));
         bit_set(sous_cles[i], j + 28, bit_get(cle_d, j));
      }

      /**********************************************************
      *                                                         *
      *  Ex�cution du choix permut�.                            *
      *                                                         *
      **********************************************************/

      /* Phase de permutation */
      permute(sous_cles[i], Des_Permute, 48);

   }

}

/****************************************************************
*                                                               *
*  Copie locale du texte source.                                *
*                                                               *
****************************************************************/

memcpy(temp, source, 8);

/****************************************************************
*                                                               *
*  Permutation initiale.                                        *
*                                                               *
****************************************************************/

permute(temp, Des_Initial, 64);

/****************************************************************
*                                                               *
*  Division du texte source en un bloc gauche et un bloc droit  *
*  de 32 bits.                                                  *
*                                                               *
****************************************************************/

memcpy(bloc_g, &temp[0], 4);
memcpy(bloc_d, &temp[4], 4);

/****************************************************************
*                                                               *
*  Chiffrement ou d�chiffrement du texte source.                *
*                                                               *
****************************************************************/

for (i = 0; i < 16; i++) {

   /*************************************************************
   *                                                            *
   *  D�but du calcul de f.                                     *
   *                                                            *
   *************************************************************/

   memcpy(bloc_f, bloc_d, 4);

   /*************************************************************
   *                                                            *
   *  Permute et �tend la copie du bloc droit en 48 bits.       *
   *                                                            *
   *************************************************************/

   permute(bloc_f, Des_Expansion, 48);

   /*************************************************************
   *                                                            *
   *  Applique la sous-cl� appropri�e au round.                 *
   *                                                            *
   *************************************************************/

   if (sens == chiffrement) {

     /***********************************************************
      *                                                         *
      *  Pour chiffrer, les sous-cl�s sont appliqu�es           *
      *  en ordre croissant.                                    *
      *                                                         *
      **********************************************************/

      bit_xor(bloc_f, sous_cles[i], bloc_x, 48);
      memcpy(bloc_f, bloc_x, 6);

      }

   else {

      /**********************************************************
      *                                                         *
      *  Pour d�chiffrer, les sous-cl�s sont appliqu�es         *
      *  en ordre d�croissant.                                  *
      *                                                         *
      **********************************************************/

 	  //TODO 
     bit_xor(bloc_f, sous_cles[15-i], bloc_x, 48);
	  memcpy(bloc_f, bloc_x, 6);

   }

   /*************************************************************
   *                                                            *
   *  Substitutions par bo�tes-S.                               *
   *                                                            *
   *************************************************************/

   p = 0;

   for (j = 0; j < 8; j++) {

      /**********************************************************
      *                                                         *
      *  Calcule une ligne et une colonne dans les tableaux     *
      *  des bo�tes-S.                                          *
      *                                                         *
      **********************************************************/

      lig = (bit_get(bloc_f, (j * 6)+0) * 2) + 
            (bit_get(bloc_f, (j * 6)+5) * 1);
      col = (bit_get(bloc_f, (j * 6)+1) * 8) + 
            (bit_get(bloc_f, (j * 6)+2) * 4) +
            (bit_get(bloc_f, (j * 6)+3) * 2) + 
            (bit_get(bloc_f, (j * 6)+4) * 1);

      /**********************************************************
      *                                                         *
      *  Substitution par bo�te-S du bloc courant de 6 bits.    *
      *                                                         *
      **********************************************************/

      bloc_s = (unsigned char)Des_Sbox[j][lig][col];

      for (k = 4; k < 8; k++) {

         bit_set(bloc_f, p, bit_get(&bloc_s, k));
         p++;

      }

   }

   /*************************************************************
   *                                                            *
   *  Permutation-P pour terminer f.                            *
   *                                                            *
   *************************************************************/

   permute(bloc_f, Des_Pbox, 32);

   /*************************************************************
   *                                                            *
   *  Calcul du OU Exclusif du bloc gauche et de f.             *
   *                                                            *
   *************************************************************/

   //TODO completer :
   bit_xor(bloc_g, bloc_f, bloc_x, 32);

   /*************************************************************
   *                                                            *
   *  Configure le bloc gauche pour le round.                   *
   *                                                            *
   *************************************************************/

   memcpy(bloc_g, bloc_d, 4);

   /*************************************************************
   *                                                            *
   *  Configure le bloc droit pour le round.                    *
   *                                                            *
   *************************************************************/

   memcpy(bloc_d, bloc_x, 4);

}

/****************************************************************
*                                                               *
*  Configure le texte cible avec la r�union des blocs droit     *
*  et gauche.                                                   *
*                                                               *
****************************************************************/

memcpy(&cible[0], bloc_d, 4);
memcpy(&cible[4], bloc_g, 4);

/****************************************************************
*                                                               *
*  Permutation finale.                                          *
*                                                               *
****************************************************************/

permute(cible, Des_Final, 64);

return 0;

}

/****************************************************************
*                                                               *
*  ---------------------- des_encipher -----------------------  *
*                                                               *
****************************************************************/

void des_encipher(const unsigned char *clair, 
                  unsigned char *chiffre,
                  const unsigned char *cle) {

des_main(clair, chiffre, cle, chiffrement);

return;

}

/****************************************************************
*                                                               *
*  ---------------------- des_decipher -----------------------  *
*                                                               *
****************************************************************/

void des_decipher(const unsigned char *chiffre, 
                  unsigned char *clair,
                  const unsigned char *cle) {

des_main(chiffre, clair, cle, dechiffrement);

return;

}
