#include <SPI.h> // SPI library
#include <MFRC522.h> // RFID library (https://github.com/miguelbalboa/rfid)
#include <ESP8266WiFi.h>
constexpr uint8_t RST_PIN = 5; //
constexpr uint8_t SS_PIN = 4; //

// Assign LED output variables to GPIO pins
const int LEDmerah = 0;
const int LEDkuning = 2;

String cardid = "";
String state = "";

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance
WiFiServer server(80); // Set web server port number to 80

void setup() {
  WiFi.mode(WIFI_AP); //Our ESP8266-12E is an AccessPoint
  WiFi.softAP("Hello_IoT", "12345678"); // Provide the (SSID, password); .
  server.begin(); // Start the HTTP Server

  //Looking under the hood
  Serial.begin(115200); //Start communication between the ESP8266-12E and the monitor window
  IPAddress HTTPS_ServerIP= WiFi.softAPIP(); // Obtain the IP of the Server
  Serial.print("Server IP is: "); // Print the IP to the monitor window
  Serial.println(HTTPS_ServerIP);
  
  SPI.begin();      // Initiate SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  Serial.println("------------------------------------\n");

  pinMode(LEDmerah, OUTPUT);
  pinMode(LEDkuning, OUTPUT);
  // Set outputs to LOW
  digitalWrite(LEDmerah, LOW);
  digitalWrite(LEDkuning, LOW);
}

void loop() {
  WiFiClient client = server.available();   // Listen for incoming clients
  // Display the HTML web page
  client.println("<!DOCTYPE html><html>");
  client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  client.println("<meta http-equiv='refresh' content='1'>");
  client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
  
  // Web Page Heading
  client.println("<body><h1>ESP8266 Web Server</h1>");
  client.println("");
  client.println("<p>Card ID : </p>");
  
  if (mfrc522.PICC_IsNewCardPresent()) { // Look for new cards
    if (mfrc522.PICC_ReadCardSerial()) { // Select one of the cards
      Serial.println("ID Card Get");
      Serial.print("Card ID : ");
      String content = "";

      for (byte i = 0; i < mfrc522.uid.size; i++) {
        // Dump a byte array as hex values to Serial
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);

        // Dump a byte array as hex values to String
        content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
        content.concat(String(mfrc522.uid.uidByte[i], HEX)); //
      }

      content.toUpperCase(); // Make string become UPPERCASE
      mfrc522.PICC_HaltA();
      if (content.substring(1) == "AA 88 78 89") { // DE 95 45 83 
        digitalWrite(LEDkuning,HIGH);
        delay(3000);
        digitalWrite(LEDkuning,LOW);
        state = "--- Akses Diberikan ---";
        state += "Selamat Datang Doni";
      }
      else {
        digitalWrite(LEDmerah,HIGH);
        delay(3000);
        digitalWrite(LEDmerah,LOW);
        state = "--- Akses ditolak ---";
      }
      cardid = content.substring(1);
      Serial.println("");
    }
  }
  client.println("<p>"+ cardid + "</p>");
  client.println("<p>"+ state + "</p>");
}
