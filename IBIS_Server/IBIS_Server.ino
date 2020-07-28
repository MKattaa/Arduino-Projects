#include <Ethernet.h> // Used for Ethernet

// **** Buffer SETTING ****
//  CONSTANTS
#define SIZE_OF_BUFFER 64 // Maximum size of buffer
#define SUCCESS 0
#define FAILURE -1
//
String circularBuffer[SIZE_OF_BUFFER] = {}; // Empty circular buffer
int readIndex = 0;  // Index of the read pointer
int writeIndex = 0;  // Index of the write pointer
int bufferLength = 0;  // Number of values in circular buffer

// **** ETHERNET SETTING ****
byte mac[] = { 0xA8, 0x61, 0x0A, 0xAE, 0x71, 0x31 };                                       
IPAddress ip(192, 168, 1, 200);                        
EthernetServer server(8080);

// **** Serielle Daten ****
String incomingData = "000000"; // Für eingehende serielle Daten

// **** Fahrt Information ****
String busLinie = "000";
boolean wagenHaelt = false;        // Muss aus einer externer Quelle erhalten werden (Das Ticketsgerät hat diese Information nicht!)
String naechsteHalteStelle = "Keine Angabe";
String letzteHalteStelle = "Leere Fahrt";
String nachrichtAnFahrer = "-1";    // Wäre eine Mögliche, um die Funktionen vom Arduino zu erweitern
String orginalIncomingData = "-1";  // -1 entsprecht garnichts

int writeInBuffer() {
  // Check if buffer is full
  if (bufferLength == SIZE_OF_BUFFER) {
    // Buffer is full!
    // Serial.print("Buffer is full!");
    return FAILURE;
  } else {
    circularBuffer[writeIndex] = incomingData;
    bufferLength++;  // Increase buffer size after writing
    writeIndex++;  // Increase writeIndex position to prepare for next write

    // If at last index in buffer, set writeIndex back to 0
    if (writeIndex == SIZE_OF_BUFFER) {
      writeIndex = 0;
    }
    return SUCCESS;
  }
}

String readFromBuffer() {  
  String data;    
  // Check if buffer is empty
  if (bufferLength == 0) {
    // Buffer is empty!
    // Serial.print("Buffer is empty!");
    return String(FAILURE);
  }
  
  data = circularBuffer[readIndex];
  bufferLength--;  // Decrease buffer size after reading
  readIndex++;   // Increase readIndex position to prepare for next read

  // If at last index in buffer, set readIndex back to 0
  if (readIndex == SIZE_OF_BUFFER) {
    readIndex = 0;
  }
  return data;
}

void analyseInstruction() {
  String instruction = readFromBuffer();
  orginalIncomingData = incomingData;
  switch (instruction.charAt(0))
  {
  case 'l':         //l ist eine Abkürzung für Bus Linie
    busLinie = instruction.substring(1, 4);
    break;
  case 'v':         //v ist eine Abkürzung für Voranzeige z.B. anzeigen nächste Haltestelle
    naechsteHalteStelle = instruction.substring(1);
    break;    
  case 'a':         //z ist eine Abkürzung für Anzeigen
    letzteHalteStelle = instruction.substring(1);
    break;  
  default:
    //Instruktion ist unbekannt -> garnicht reagieren
    break;
  }
}

void setup() {
  // Öffnet die serielle Schnittstelle und stellt die Datenrate auf 9600 Bit/s ein
  Serial.begin(9600);

  // Ethernet und Server Connection starten:
  Ethernet.begin(mac, ip);
  server.begin();

  // Serial.print("Server IP Address: ");
  // Serial.println(Ethernet.localIP());
}

void loop() {

  // -------------------------------------------------------------
  // UART Schnittstelle (Serial USB)
  // -------------------------------------------------------------
  // Antworten, wenn Daten erhalten werden:
  if (Serial.available() > 0) {
    // Lies das eingehende Byte:
    incomingData = Serial.readString();
    writeInBuffer();
    
    analyseInstruction();

    // Ausgabe:
    // Serial.print("I received: ");
    // Serial.println(incomingData);
  }
  // -------------------------------------------------------------


  // -------------------------------------------------------------
  // Ethernet Schnittstelle
  // -------------------------------------------------------------
  // listen for incoming clients
  EthernetClient client = server.available();

  if (client) // New Connection with andriod TV screen
  {  
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;

    while (client.connected()) 
    {
      if (client.available()) 
      {
        char c = client.read();
        //unsigned int count = incomingData.length()-1;
        //incomingData.remove(count, 1); //String removing chars from index i plus count c -> remove(i, c)

        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) 
        {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json");
          client.println("Connection: close");  // the connection will be closed after completion of the response         
          client.println();
          client.println("{\"busLinie\": " + busLinie + ", \"wagenHaelt\": " + wagenHaelt + ", \"naechsteHalteStelle\": " + naechsteHalteStelle + ", \"letzteHalteStelle\": " + letzteHalteStelle + ", \"nachrichtAnFahrer\": " + nachrichtAnFahrer + ", \"orginalIncomingData\": " + orginalIncomingData + "}");
          client.println();
          break;
        }

        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') 
        {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      } 
    }

    // give the web browser time to receive the data
    delay(10);

    // close the connection:
    client.stop();    // Client Disconnected
  }
  // -------------------------------------------------------------
}
