/*
   Sehr gerne darf dieser Quellcode kopiert und verändert werden.
   In diesem Beispiel habe ich auf eine Kommentierung verzichtet,
   da im Text alles ausführlich erklärt ist.
*/
 
int ledPin = 4;
 
char befehl;
 
void setup() {
  pinMode(ledPin, OUTPUT);
 
  Serial.begin(9600); // Aufbau einer seriellen Verbindung
 
  Serial.println("LED Steuerung");
  Serial.print("Die LED ist an Pin ");
  Serial.print(ledPin);
  Serial.println(" angeschlossen");
}
 
void loop() {
 
  Serial.println("Es stehen folgenden Funktionen zu Verfügung:");
  Serial.println("Einschalten: 'e'");
  Serial.println("Ausschalten: 'a'");
 
  while (befehl != 'e' && befehl != 'a')
  {
    befehl = Serial.read();
  }
 
  if (befehl == 'e') {
    digitalWrite(ledPin, HIGH);
  }
 
  if (befehl == 'a') {
    digitalWrite(ledPin, LOW);
  }
 
  Serial.print("Die LED wurde ");
 
  if (befehl == 'e') {
    Serial.println("eingeschaltet.");
  }
 
  if (befehl == 'a') {
    Serial.println("ausgeschaltet.");
  }
 
  befehl = 'r';
 
}
