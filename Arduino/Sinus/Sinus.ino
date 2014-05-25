/*
Sinus
_____

Ziel ist es, eine sinusförmige Geschwindigkeit zu erzeugen. 
Die Ungenauigkeit ist auf die Rechenungenauigkeit des Arduinos zurückzuführen.
*/


#define servoPort 7          
#define pi 3.1415926

float AnzahlDurchlaeufe;   //Anzahl der erfolgten Schleifendurchläufe, zur Berechnung des Sinuswerts benutzt (so ist auch die Genauigkeit bzw. Geschwindigkeit der Änderung einstellbar.)
float Sinuswert;           //Ergebnis der Sinusrechnung
float Prozent;             //prozentuale Zeit der max. Zeit, die das Signal auf max. Spannung ist (500micos = 0%, 1500micos = 100%)
float pause;               //Zeit in ms, die Spannung auf max. Spannung ist
float Bogenmass;

byte Genauigkeit = 100; //Standart ist 100, je höher die Zahl, desto höher ist die Genauigkeit 
byte Verzoegerung = 3; //Standart ist 3, es beschreibt, wie oft hinter einander ein bestimmter Wert ausgegeben wird, so ist die Änderung der Geschwindigkeitsänderung einstallbar

void setup() 
{
  Serial.begin (9600);
  Serial.println ("Start");
  pinMode (servoPort, OUTPUT);
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
  Bogenmass = (AnzahlDurchlaeufe / Genauigkeit )* pi;   //muss ausgelagert werden, Rechnungen inehalb sin() nicht möglich
  Sinuswert = sin(Bogenmass);
}

void mappen()
{
  Prozent = Sinuswert*100;                     //da der maximale Sinuswert 1 beträgt
  pause = map (Prozent, 0, 100, 500, 1500);    //mappen, also übertragen auf den Bereich 500 - 1500
}

void bewegung() //Wie in "tech. Umsetzung" beschrieben wird das PWM-Signal erzeugt
{
  for (int i = 1; i<Verzoegerung; i++) 
  {
    digitalWrite(servoPort, HIGH);    //Maximalspannung am ServoPort
    delayMicroseconds (pause);        //Pause mit der bestimmten Länge (1ms = 1000 microsekunden)
    digitalWrite(servoPort, LOW);     //Minimalspannung am ServoPort
    delayMicroseconds(20000 - pause); //um eine 50Hz-Frequenz zu erzeugen
  }
}

void anzeigen() //Zur Kontrolle der Variablen 
{
   Serial.println ("Anzahl Durchlaeufe:"); 
   Serial.println (AnzahlDurchlaeufe);
   Serial.println ("Bogenmass:");
   Serial.println (Bogenmass);
   Serial.println ("Sinuswert:");
   Serial.println (Sinuswert);
   Serial.println ("Prozent:");
   Serial.println (Prozent);
   Serial.println ("Pause:"); 
}


  
