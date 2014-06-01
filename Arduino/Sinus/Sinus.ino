/*
Sinus
_____

Ziel ist es, eine sinusförmige Geschwindigkeit zu erzeugen. 
Die Ungenauigkeit ist auf die Rechenungenauigkeit des Arduinos zurückzuführen.
*/

// Debugging-Infos werden nur über die Serielle Schnittstelle gesendet, wenn DEBUGING definiert ist. Auskommentieren, wenn nicht benötigt
#define DEBUGGING

#include <Servo.h> //Implementieren der Servo-Bibliothek
 
Servo myservo;     //Erstelllt eine Servoinstanz
                
#define servoPort 7          
#define pi 3.1415926

float AnzahlDurchlaeufe=0; //Anzahl der erfolgten Schleifendurchläufe, zur Berechnung des Sinuswerts benutzt (so ist auch die Genauigkeit der Änderung einstellbar.)
float Sinuswert;           //Ergebnis der Sinusrechnung
float Geschwindigkeit;     //Ausgabesignal, das die Geschwindigkeit bestimmt 
float Bogenmass;

byte Genauigkeit = 100; //Standart ist 100, je höher die Zahl, desto höher ist die Genauigkeit 
byte Verzoegerung = 1;  //Standart ist 1, es beschreibt, wie oft hinter einander ein bestimmter Wert ausgegeben wird, so ist die Änderung der Geschwindigkeitsänderung einstallbar

void setup() 
{
  #ifdef DEBUGGING
  Serial.begin (9600);
  Serial.println ("Start");
  #endif
  myservo.attach(servoPort);
  
  myservo.write(0);     //maximaler und minimaler Wert zur Konfiguration
  delay(1000);
  myservo.write(180);
  delay(1000);
}  
  

void loop() 
{
  sinusrechnung();
  mappen();
  bewegung();
  anzeigen();
  if (AnzahlDurchlaeufe < Genauigkeit)
  {
    AnzahlDurchlaeufe++;
  }
  else 
  {
    AnzahlDurchlaeufe = 0;
  }
}

void sinusrechnung() 
{
  Bogenmass = (AnzahlDurchlaeufe / Genauigkeit )* 2 * pi;   //muss ausgelagert werden, Rechnungen inehalb sin() nicht möglich
  Sinuswert = sin(Bogenmass);
}

void mappen()
{                                
  Geschwindigkeit = (Sinuswert*15)+120 ;
}

void bewegung()
{
  for (int i = 0; i<Verzoegerung; i++) 
  {
  myservo.write(Geschwindigkeit);
  }
}

void anzeigen() //Zur Kontrolle 
{   
   for (int k=50; k<((int)(Geschwindigkeit/2)); k++)  //Terminal "Oszilloskop"
   {
     #ifdef DEBUGGING
     Serial.print ("X");
     #endif
   }
   #ifdef DEBUGGING
  Serial.println("");
  #endif
}



  

