/*
Sinus
 _____
 
 Ziel ist es, eine sinusförmige Geschwindigkeit zu erzeugen. 
 Die Ungenauigkeit ist auf die Rechenungenauigkeit des Arduinos zurückzuführen.

 Die Geschwindigkeitsänderung kann durch Durchlaufe_sin_generation eingestellt werden:
 Ein größerer Wert verlangsamt die Geschwindigkeitsänderung, kleinere Werte führen zum schnelleren Durchlauf der Sinus-Welle
 Die Werte sollen zwischen 25 und 500 liegen.
 */

// Debugging-Infos werden nur über die Serielle Schnittstelle gesendet, wenn DEBUGING definiert ist. Auskommentieren, wenn nicht benötigt
#define DEBUGGING

#include <Servo.h>     //Implementieren der Servo-Bibliothek
#include "MsTimer2.h"  // Einbinden der lokalen MsTimer2 Bibliothek

Servo myservo;     //Erstelllt eine Servoinstanz

#define servoPort 7
#define dipwmPort 6 // benutzt timer0 für pwm

#define pi 3.1415926

int Timer_period = 50; // in ms; Periodendauer, bis interruptfunct() wieder aufgerufen wird
const int Durchlaufe_sin_generation = 50;  // Schritte der Sinus generation
int Durchlaufe_counter = 0;                 // Laufvariable im Interrupt
int PeriodCounter = 0;


void interruptfunct(void) // Wird durch MsTimer2 regelmäßig aufgerufen
{
  if (Durchlaufe_counter < Durchlaufe_sin_generation)
  {
    int geschwindigkeit_servo = mappen(sinusrechnung(Durchlaufe_counter));
    myservo.write(geschwindigkeit_servo);
    
    int geschwindigkeit_dipwm = mappen_dipwm(sinusrechnung(Durchlaufe_counter));
    analogWrite(dipwmPort, geschwindigkeit_dipwm);
    
    anzeigen(geschwindigkeit_servo);
    Durchlaufe_counter++;
  }
  else 
  {
    Durchlaufe_counter = 0;
    // Hier möglicherweise noch die Geschwindigkeit für den nächsten Durchlauf manipulieren
    // geringe zufällige Geschwindigkeitsänderung nach 3 Perioden vornehmen:
    if(PeriodCounter >= 1) { // wird alle 2 vollstänigen sinus-durchläufe aufgerufen
      int rand = random(50)-25; // rand ist zwischen -5 .. +5
      if((Timer_period+rand)>=80 || (Timer_period+rand)<=30) {
        Timer_period -= rand;
      } else {
        Timer_period += rand;
      }
      PeriodCounter = 1;
      MsTimer2::stop();
      MsTimer2::set(Timer_period, interruptfunct);
      MsTimer2::start();
      #ifdef DEBUGGING
      Serial.print("Timer_period=");
      Serial.print(Timer_period);
      Serial.print("; rand=");
      Serial.println(rand);
      #endif
    } else {
      PeriodCounter++;
    }
  }
}

void setup() 
{
  #ifdef DEBUGGING
  Serial.begin (115200);
  Serial.println ("Start");
  #endif
  myservo.attach(servoPort);
  pinMode(dipwmPort, OUTPUT);
  
  MsTimer2::set(Timer_period, interruptfunct); // 10ms period <-> 100Hz freqency
  MsTimer2::start();

  myservo.write(0); //maximaler und minimaler Wert zur Konfiguration
  delay(1000);
  myservo.write(180);
  delay(1000);
  
  randomSeed(analogRead(0)); // Zufallsgenerator mit ADC-Rauschen initialisieren
}


void loop() 
{
  // Nichts mehr, alles läuft im Interrupt ab
}

float sinusrechnung(int durchlaeufe) 
{
  float Bogenmass = ((float)durchlaeufe / (float)Durchlaufe_sin_generation)* (float)2 * pi; // liegt immer zwischen 0...2pi, es wird immer nur die erste Periodendauer berechnet
  //muss ausgelagert werden, Rechnungen innerhalb sin() nicht möglich
  return(sin(Bogenmass)*100);
}

int mappen(int Sinuswert)
{                                
  return( map(round(Sinuswert), -100, 100, 105, 135) );    //mappen, also übertragen auf den Bereich 105 - 135 (grad), ist regler- und akkuspezifisch
}

int mappen_dipwm(int Sinuswert)
{                                
  return( map(round(Sinuswert), -100, 100, 0, 254) );    //mappen, also übertragen auf den Bereich 0..254 für einen Duty cycle zwischen 0% und 100% (analogWrite/pwm)
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
  Serial.println("Grad");
  #endif
}

