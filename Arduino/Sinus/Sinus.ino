/*
Sinus
_____

Ziel ist es, eine sinusförmige Geschwindigkeit zu erzeugen. 
Die Ungenauigkeit ist auf die Rechenungenauigkeit des Arduinos zurückzuführen.
*/


const byte servoPort = 7;          
const int pi = 3.1415926;

float AnzahlDurchlaeufe;   //Anzahl der erfolgten Schleifendurchläufe, zur Berechnung des Sinuswerts benutzt (so ist auch die Genauigkeit bzw. Geschwindigkeit der Änderung einstellbar.)
float Sinuswert;           //Ergebnis der Sinusrechnung
float Prozent;             //prozentuale Zeit der max. Zeit, die das Siganl auf max. Spannung ist (500micos = 0%, 1500micos = 100%)
float pause;               //Zeit in ms, die Spannung auf max. Spannung ist
float Bogenmass;

 byte kontrolle;

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
  if (AnzahlDurchlaeufe <100)
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
  Bogenmass = (AnzahlDurchlaeufe / 100 )* pi;   //muss ausgelagert werden, Rechnungen inehalb sin() nicht möglich
  Sinuswert = sin(Bogenmass);
}

void mappen()
{
  Prozent = Sinuswert*100;                     //da der maximale Sinuswert 1 beträgt
  pause = map (Prozent, 0, 100, 500, 1500);    //mappen, also übertragen auf den Bereich 500 - 1500
}

void bewegung() //Wie in "tech. Umsetzung" beschrieben wird das PWM-Signal erzeugt
{
  digitalWrite(servoPort, HIGH);  //Maximalspannung am ServoPort
  delayMicroseconds (pause);  //Pause mit der bestimmten Länge (1 ms= 1000 microsekunden)
  digitalWrite(servoPort, LOW);   //mMnimalspannung am ServoPort
  delay(20);
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
