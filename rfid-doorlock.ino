#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include<WiFiClientSecure.h>
#define RST_PIN   0    // Reset pin for MFRC522 - Module 1
#define SS_PIN    5    // Slave Select pin for MFRC522 - Module 1

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

void connect2Wifi();
void readUID();
bool isEntry();
int isConnected();
#define WRITE_BTN 25
#define WRITE_ADMIN_BTN 26
#define DELETE_BTN 27
#define WIFI_SSID "RECHARGE AIPOYANDHA"
#define WIFI_PASS "smuggler"
#define WIFI_IND 2
#define DOOR 14
byte  entryDt [][4] = {
//  {0x3, 0xfd, 0xd3, 0xfa},
 {0xa3, 0x9c, 0x97, 0x1d}, {0xbd, 0xf4, 0xdf, 0x6b}, {0xb0 ,0x82, 0x0d, 0x3c}, {0xa4, 0x2e, 0xec, 0xa9},
 {0xA3 ,0x57,0x34,0xC5}
};















WiFiClientSecure client; // Create a WiFiClientSecure object
void setup() {
  Serial.begin(9600); // Initialize serial communication
  Serial.println("init...");
  pinMode(DOOR,OUTPUT);
  SPI.begin(); // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522
  Serial.println("RFID Ready!");
  WiFi.mode(WIFI_STA)  ;
  connect2Wifi();
  client.setInsecure();
}







MFRC522::Uid lastReadUID ;
void openDoor();
void loop() {
  // Look for new cards
  digitalWrite(WIFI_IND, isConnected());
  if ( mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    Serial.println("card ");
    // Print UID of the card
    readUID();

    if (isEntry()) {
      openDoor();
      log2Google();
    }






  }


  delay(100);
}
void openDoor() {
  Serial.println("opening door");
  digitalWrite(DOOR, HIGH);
  delay(2000);
  digitalWrite(DOOR, LOW);
}
bool isEntry() {
  for (int card = 0; card < sizeof(entryDt)/4; card++) {
    int flag = true;
    for (byte i = 0; i < 3; i++) {

      if (lastReadUID.uidByte[i] != entryDt[card][i]) {
        flag = false; break;
      }


    }
    if (flag == true)return true;
  }
  return false;
}
int isConnected() {
  return  (WiFi.status() == WL_CONNECTED) ? HIGH : LOW;
}

void connect2Wifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASS);
}



void readUID() {
  Serial.print("UID:");
  lastReadUID = mfrc522.uid;
  printUID(lastReadUID);
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
void printUID(MFRC522::Uid uid) {
  String content = "";
  byte letter;
  for (byte i = 0; i < uid.size; i++) {
    Serial.print(uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(uid.uidByte[i], HEX);
  }
  Serial.println();
  Serial.println();
}


//----------------------------------------Host & httpsPort
const char* host = "script.google.com";
const int httpsPort = 443;
//----------------------------------------
// Subroutine for sending data to Google Sheets




// Google spreadsheet script ID
String GAS_ID = "AKfycbzLUt0KaPNqIcEX53tXAx_Ti3WBCizY7AjPHps9AapngVLmtWxx9RXBxWAsAjq_TrxB";



void log2Google() {
  Serial.println("==========");
  Serial.print("connecting to ");
  Serial.println(host);

  //----------------------------------------Connect to Google host
  if (!(client.connected() || client.connect(host, httpsPort))) {
    Serial.println("connection failed");
    return;
  }
  //----------------------------------------

  //----------------------------------------Processing data and sending data

  String url = "/macros/s/" + GAS_ID + "/exec?card=" + lastReadUID.uidByte[0] + "-" + lastReadUID.uidByte[1] + "-" + lastReadUID.uidByte[2] + "-" + lastReadUID.uidByte[3] + "-" + lastReadUID.uidByte[4] ;
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("request sent");
  //----------------------------------------

  //----------------------------------------Checking whether the data was sent successfully or not
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
      
    }
  }
  //----------------------------------------
}
