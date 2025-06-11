#include <IRremote.hpp>

const uint8_t IR_PIN = 2;

void setup() {
  Serial.begin(9600);

  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);
  Serial.println("IR Receiver initialized. Waiting for codes...");
}

void loop() {
  if (IrReceiver.decode()) {

    Serial.print(F("Protocol: "));
    Serial.println(IrReceiver.decodedIRData.protocol, DEC);

    Serial.print(F("Address: 0x"));
    Serial.println(IrReceiver.decodedIRData.address, HEX);

    Serial.print(F("Command: 0x"));
    Serial.println(IrReceiver.decodedIRData.command, HEX);

    Serial.print(F("Raw data: 0x"));
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);

    Serial.print(F("Bits: "));
    Serial.println(IrReceiver.decodedIRData.numberOfBits);

    Serial.println();

    IrReceiver.resume();
  }
  delay(100);
}
