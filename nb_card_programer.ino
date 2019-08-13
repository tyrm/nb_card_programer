#include "secrets.h"
#include <M5Stack.h>
#include <WiFi.h>
#include <Wire.h>
#include "MFRC522_I2C.h"

// State
int prog_state = 0;
#define STATE_READY       0
#define STATE_ASK_FORMAT  1

#define STATE_ERROR_UNSUPPORTED_CARD  2

// Registers
byte regWorkingUID[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

byte regWorkingUIDLen = 0;

// Wifi Vars
const char* ssid     = WIFI_SSID;
const char* password = WIFI_PASSWORD;

// 0x28 is i2c address on SDA. Check your address with i2cscanner if not match.
MFRC522 mfrc522(0x28);   // Create MFRC522 instance.
MFRC522::MIFARE_Key nfcKeyPublic;
MFRC522::MIFARE_Key nfcKeyAdmin;
MFRC522::MIFARE_Key nfcKeyGate;

void setup() {
  M5.begin(true, false, true);
  
  Serial.begin(115200);           // Initialize serial communications with the PC
  Wire.begin();                   // Initialize I2C

  // Init Wifi
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //**********//
  // Init NFC //
  //**********//
  mfrc522.PCD_Init();             // Init MFRC522
  ShowReaderDetails();            // Show details of PCD - MFRC522 Card Reader details

  // Init Keys
  byte nfcKeyPublicBytes[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}; // Publicly Readable Key
  byte nfcKeyAdminBytes[6]  = KEY_ADMIN; // Admin Machine Key
  byte nfcKeyGateBytes[6]   = KEY_GATE; // Gate
  
  for (byte i = 0; i < 6; i++) {
    nfcKeyPublic.keyByte[i] = nfcKeyPublicBytes[i];
    nfcKeyAdmin.keyByte[i]  = nfcKeyAdminBytes[i];
    nfcKeyGate.keyByte[i]   = nfcKeyGateBytes[i];
  }
}

void loop() {
  switch (prog_state) {
    case STATE_READY:
      StateReady();
      break;
    case STATE_ASK_FORMAT:
      StateErrorUnsupportedCard();
      break;
    case STATE_ERROR_UNSUPPORTED_CARD:
      StateErrorUnsupportedCard();
      break;
    default:
      prog_state = STATE_READY;
      break;
  }

}

void ShowReaderDetails() {
  // Get the MFRC522 software version
  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.print(F("MFRC522 Software Version: 0x"));
  Serial.print(v, HEX);
  if (v == 0x91)
    Serial.print(F(" = v1.0"));
  else if (v == 0x92)
    Serial.print(F(" = v2.0"));
  else
    Serial.print(F(" (unknown)"));
  Serial.println("");
  // When 0x00 or 0xFF is returned, communication probably failed
  if ((v == 0x00) || (v == 0xFF)) {
    Serial.println(F("WARNING: Communication failure, is the MFRC522 properly connected?"));
  }
}

/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *bBuffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize ; i++) {
        Serial.print(bBuffer[i] < 0x10 ? " 0" : " ");
        if (i % 16 == 15) {
          Serial.println(bBuffer[i], HEX);
          
        } else {
          
          Serial.print(bBuffer[i], HEX);
        }
    }
}
