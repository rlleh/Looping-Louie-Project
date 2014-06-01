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

byte Timer_period_laeng = 255; // Timer Counter value auf 256 setzten (größtmöglichster Wert für kleinste Frequenz); Hiermit kann die Frequenz geregelt werden
// Byte reicht, da Timer2 nur 8 Bit hat
const int Durchlaufe_sin_generation = 100;  // Schritte der Sinus generation
int Durchlaufe_counter = 0;                 // Laufvariable im Interrupt
int CounterValue  = 150;                   // Timer Counter value auf 150 setzten (mittlerer Wert für mittlere Frequenz); Hiermit kann die Frequenz geregelt werden; Bereich[1;255]
byte PeriodCounter = 0;

void setup() 
{
  #ifdef DEBUGGING
  Serial.begin (9600);
  Serial.println ("Start");
  #endif
  myservo.attach(servoPort);

  // Timer 2 zur Interruptgenerierung für konstante Frequenzen initialisieren
  noInterrupts();   // alle Interrupts deaktivieren
  TCNT2  = CounterValue;
  TCCR2A = 0;       // Konfiguration von Timer2 löschen
  TCCR2B = 0;
  TCCR2B = (1 << CS12 | 0 << CS11 | 1 << CS10 );   // Prescaler: 1024
  // --> Timer Frequenz: 16 000 000 Hz / 1024 = 15 625 Hz
  //  --> Interrupt-Frequenz bei Timer Counter value von 256 (Maixumum): 15 625 Hz / 256 = ~61Hz
  TIMSK2 |= (1 << TOIE2); // Interrupt Timer2_OVF_vect beim Überlauf aktivieren
  interrupts();     // Alle Interrupts wieder aktivieren

  myservo.write(0); //maximaler und minimaler Wert zur Konfiguration
  delay(1000);
  myservo.write(180);
  delay(1000);
  
  randomSeed(analogRead(0)); // Zufallsgenerator mit ADC-Rauschen initialisieren
}  


ISR(Timer2_OVF_vect) // interrupt service routine that wraps a user defined function supplied by attachInterrupt
{
  TCNT2 = CounterValue; // preload timer
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
    if(PeriodCounter >= 3) {
      int rand = random(10,31)-15;
      if((CounterValue+rand)<=50 || (CounterValue+rand)>=254) {
        CounterValue -= rand;
      } else {
        CounterValue += rand;
      }
      PeriodCounter = 0;
    } else
      PeriodCounter++;
    
    
  }
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

