#define BTN1_LABEL_X 64
#define BTN1_LABEL_Y 220
#define BTN2_LABEL_X 160
#define BTN2_LABEL_Y 220
#define BTN3_LABEL_X 256
#define BTN3_LABEL_Y 220

void StateReady() {
  Serial.println(F("\nState: StateReady"));

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.setTextColor(GREEN);  
  M5.Lcd.setTextSize(3);
  M5.Lcd.drawCentreString("Tap Card", 320/2, 240/2-15, 2);
  
  while (true) {
    // Look for new cards, and select one if present
    if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
      delay(50);
    } else {
      // See Card
      Serial.print(F("Card UID:"));
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
      }
      Serial.println();
      
      Serial.print(F("  PICC type: "));
      byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
      Serial.println(mfrc522.PICC_GetTypeName(piccType));

      prog_state = STATE_ERROR_UNSUPPORTED_CARD;
      return;
    }
  }
}

void StateErrorUnsupportedCard() {
  Serial.println(F("\nState: StateErrorUnsupportedCard"));
  
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(RED);
  M5.Lcd.setTextSize(2);
  M5.Lcd.drawCentreString("Unsupported Card", 320/2, 20, 2);
  M5.Lcd.setCursor(0, 60);
  M5.Lcd.setTextColor(CYAN);
  M5.Lcd.println("Supported Cards:");
  M5.Lcd.println(" * MIFARE 4KB");

  M5.Lcd.setTextColor(GREEN);  
  M5.Lcd.setTextSize(2);
  M5.Lcd.drawCentreString("OK", BTN3_LABEL_X, BTN3_LABEL_Y, 1);

  while (true) {
    M5.update();
    if (M5.BtnC.wasReleased()) {
      Serial.print(F("BtnA was Pressed"));
      prog_state = STATE_READY;
      break;
    }
  }
}
