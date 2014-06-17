/*
Sinus
 _____
 
 Ziel ist es, eine sinusförmige Geschwindigkeit zu erzeugen. 
 Die Ungenauigkeit ist auf die Rechenungenauigkeit des Arduinos zurückzuführen.
 */

// Debugging-Infos werden nur über die Serielle Schnittstelle gesendet, wenn DEBUGING definiert ist. Auskommentieren, wenn nicht benötigt
#define DEBUGGING

#include <Servo.h>     //Implementieren der Servo-Bibliothek
#include "MsTimer2.h"  // Einbinden der lokalen MsTimer2 Bibliothek

Servo myservo;     //Erstelllt eine Servoinstanz

#define servoPort 7
#define dipwmPort 11
#define pi 3.1415926

byte Timer_period = 10; // in ms; Periodendauer, bis interruptfunct() wieder aufgerufen wird
const int Durchlaufe_sin_generation = 100;  // Schritte der Sinus generation
int Durchlaufe_counter = 0;                 // Laufvariable im Interrupt
int PeriodCounter = 0;


void interruptfunct(void) // Wird durch MsTimer2 regelmäßig aufgerufen
{
  if (Durchlaufe_counter < Durchlaufe_sin_generation)
  {
    int geschwindigkeit = mappen(sinusrechnung(Durchlaufe_counter));
    myservo.write(geschwindigkeit);
    anzeigen(geschwindigkeit);
    Durchlaufe_counter++;
  }
  else 
  {
    Durchlaufe_counter = 0;
    // TODO: Hier möglicherweise noch die Geschwindigkeit für den nächsten Durchlauf manipulieren
    // geringe zufällige Geschwindigkeitsänderung nach 3 Perioden vornehmen:
    if(PeriodCounter >= 25) {
      int rand = random(10)-5; // rand ist zwischen -5 .. +5
      if((Timer_period+rand)<=5 || (Timer_period+rand)>=50) {
        Timer_period -= rand;
      } else {
        Timer_period += rand;
      }
      PeriodCounter = 0;
    } else
      PeriodCounter++;
    
    
  }
}

void setup() 
{
  #ifdef DEBUGGING
  Serial.begin (115200);
  Serial.println ("Start");
  #endif
  myservo.attach(servoPort);

  MsTimer2::set(10, interruptfunct); // 10ms period <-> 100Hz freqency
  MsTimer2::start();

  myservo.write(0); //maximaler und minimaler Wert zur Konfiguration
  delay(500);
  myservo.write(180);
  delay(500);
  
  randomSeed(analogRead(0)); // Zufallsgenerator mit ADC-Rauschen initialisieren
}


void loop() 
{
  // Nichts mehr, alles läuft im Interrupt ab
}

float sinusrechnung(int durchlaeufe) 
{
  float Bogenmass = (durchlaeufe / Durchlaufe_sin_generation)* 2 * pi; // liegt immer zwischen 0...2pi, es wird immer nur die erste Periodendauer berechnet
  //muss ausgelagert werden, Rechnungen inehalb sin() nicht möglich
  return(sin(Bogenmass));
}

int mappen(int Sinuswert)
{                                
  return( map(round(Sinuswert*100), -100, 100, 105, 135) );    //mappen, also übertragen auf den Bereich 105 - 135 (grad), ist regler- und akkuspezifisch
}


void anzeigen(int Geschwindigkeit) //Zur Kontrolle 
{   
  for (int k=50; k<((int)(Geschwindigkeit/2)); k++)  //Terminal "Oszilloskop"
  {
    #ifdef DEBUGGING
    Serial.print ("X");
    #endif
  }
  #ifdef DEBUGGING
  Serial.print((Geschwindigkeit));
  Serial.println(" Grad");
  #endif
}

