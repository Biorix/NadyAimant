/*
  Fichier:      variationPuissance
  Description:  Contrôle des électroaimants
  Auteur:       Loup Letac et Nady Larchet (issue de l'exemple de Jérôme, Passion-Électronique (https://passionelectronique.fr/))
  Création :    2023-03-07
*/
#include <EduIntro.h>
//*******************************************************************************//
// Association des entrées du L298N, aux sorties utilisées sur notre Arduino Mega //
//*******************************************************************************//
#define borneENA        10      // On associe la borne "ENA" du L298N à la pin D10 de l'Arduino (PWM possible sur cettear broche)
#define borneIN1        9       // On associe la borne "IN1" du L298N à la pin D9 de l'Arduino
#define borneIN2        8       // On associe la borne "IN2" du L298N à la pin D8 de l'Arduino
#define borneIN3        7       // On associe la borne "IN3" du L298N à la pin D7 de l'Arduino  
#define borneIN4        6       // On associe la borne "IN4" du L298N à la pin D6 de l'Arduino
#define borneENB        5       // On associe la borne "ENB" du L298N à la pin D5 de l'Arduino (PWM possible sur cette broche)
#define borneAV         2       // On associe la borne 2 de l'Arduino pour le sens avant (tête de vis)
#define borneAR         3       // On associe la borne 3 de l'Arduino pour le sens avant (cul de vis)
//*************************//
// Constantes du programme //
//*************************//
#define delaiCrenauxRampe          5       // Défini un petit délai, exprimé en millisecondes, avant tout autre changement de vitesse
#define vitesseMinimale            0       // Rapport cylique minimal du signal PWM, pour faire tourner le moteur au minimum de sa vitesse (en pratique, on évitera de trop approcher la valeur  0)
#define vitesseMaximale            255      // Rapport cylique maximal du signal PWM, pour faire tourner le moteur au maximum de sa vitesse
Potentiometer pot(A0);          // On associe le potentiomètre avec la puissance de l'aimant
const char MARCHE_AVANT   = 'V';            // Défini une constante pour la "marche avant" (peu importe la valeur)
const char MARCHE_ARRIERE = 'R';            // Défini une constante pour la "marche arrière" (peu importe la valeur)
const char STOP = 'S';            // Défini une constante pour la "marche arrière" (peu importe la valeur)
const float courantMaxMesure = 2.25;        // courant mesuré maximum

//************************//
// Variables du programme //
//************************//
int pwm;                    // Spécifie la vitesse de rotation du moteur, entre son minimum (0) et son maximum (255) <= signal PWM
bool sensAV;
bool sensAR;

//*******//
// SETUP //
//*******//
void setup() {
  Serial.begin(9600);
  // Configuration de toutes les pins de l'Arduino en "sortie" (car elles attaquent les entrées du module L298N)
  pinMode(borneENA, OUTPUT);
  pinMode(borneIN1, OUTPUT);
  pinMode(borneIN2, OUTPUT);
  pinMode(borneIN3, OUTPUT);
  pinMode(borneIN4, OUTPUT);
  pinMode(borneENB, OUTPUT);
  pinMode(borneENB, OUTPUT); 
  pinMode(borneAV, INPUT);
  pinMode(borneAR, INPUT); 
}

//**************************//
// Boucle principale : LOOP //
//**************************//
void loop() {
  // on inverse pour que le potentiomètre soit de la gauche vers la droite
  pwm = abs(map(pot.read(), 0, 1023, 255, 0));
  sensAV = digitalRead(borneAV);
  sensAR = digitalRead(borneAR);

  // On configure le sens des aimants en "avant"
  if (sensAV == 1 & sensAR == 0){
        configurerSensAimant(MARCHE_AVANT);
  } else if (sensAV == 0 & sensAR == 1){
        configurerSensAimant(MARCHE_ARRIERE);
  } else {
        configurerSensAimant(STOP);
  }

  // On lance le moteur raccordé au pont A, en faisant varier sa vitesse de 0 à sa vitesse maximale (atteinte lorsque le signal PWM a un rapport cyclique égal à 255)
  changeIntensiteAimant(pwm);            // Met à jour la vitesse du moteur, en modifiant le rapport cyclique du signal PWM env

  Serial.print("Courant :");
  Serial.println(pwm*courantMaxMesure/255);

}
//************************************************************************************//
// Fonction : configurerSensAimant()                                         //
// But :      Permet de définir le sens des aimants    //
//************************************************************************************//
void configurerSensAimant(char sensAimant) {

  if(sensAimant == MARCHE_AVANT) {
    // Configuration du L298N en "marche avant", pour le moteur connecté au pont A. Selon sa table de vérité, il faut que :
    digitalWrite(borneIN1, HIGH);                 // L'entrée IN1 doit être au niveau haut
    digitalWrite(borneIN2, LOW);                  // L'entrée IN2 doit être au niveau bas
    digitalWrite(borneIN4, HIGH);                 // L'entrée IN4 est égale à IN1
    digitalWrite(borneIN3, LOW);                  // L'entrée IN3 est égale à IN2
  }

  if(sensAimant == MARCHE_ARRIERE) {
    // Configuration du L298N en "marche arrière", pour le moteur câblé sur le pont A. Selon sa table de vérité, il faut que :
    digitalWrite(borneIN1, LOW);                  // L'entrée IN1 doit être au niveau bas
    digitalWrite(borneIN2, HIGH);                 // L'entrée IN2 doit être au niveau haut
    digitalWrite(borneIN4, LOW);                 // L'entrée IN4 est égale à IN1
    digitalWrite(borneIN3, HIGH);                  // L'entrée IN3 est égale à IN2
  }

  if(sensAimant == STOP) {
    // Configuration du L298N en "marche arrière", pour le moteur câblé sur le pont A. Selon sa table de vérité, il faut que :
    digitalWrite(borneIN1, LOW);                  // L'entrée IN1 doit être au niveau bas
    digitalWrite(borneIN2, LOW);                 // L'entrée IN2 doit être au niveau haut
    digitalWrite(borneIN4, LOW);                 // L'entrée IN4 est égale à IN1
    digitalWrite(borneIN3, LOW);                  // L'entrée IN3 est égale à IN2
  }
}

//************************************************************************************//
// Fonction : changeIntensiteAimant()                                              //
// But :      Change la vitesse de rotation du moteur branché sur le pont A           //
//            (cette valeur doit être comprise entre 0 et 255)                        //
//************************************************************************************//
void changeIntensiteAimant(int nouvelleVitesse) {

  // Génère un signal PWM permanent, de rapport cyclique égal à "nouvelleVitesse" (valeur comprise entre 0 et 255)
  analogWrite(borneENA, nouvelleVitesse);
  analogWrite(borneENB, nouvelleVitesse);
}