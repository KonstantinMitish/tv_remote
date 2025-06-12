#include <Arduino.h>
#include <IRremote.hpp>

static constexpr uint8_t IR_PIN = 4;// PIN D2

static const uint8_t UNUSED = 0xFF;

static const uint8_t button_to_gpio[] = {
        /* 0x0 */ UNUSED,
        /* 0x1 */ 5,// SOURCE -> D1
        /* 0x2 */ 0,// POWER -> D3
        /* 0x3 */ UNUSED,
        /* 0x4 */ UNUSED,
        /* 0x5 */ UNUSED,
        /* 0x6 */ UNUSED,
        /* 0x7 */ 2,// VOLUME_UP -> D4
        /* 0x8 */ UNUSED,
        /* 0x9 */ UNUSED,
        /* 0xA */ UNUSED,
        /* 0xB */ 14,// VOLUME_DOWN -> D5
        /* 0xC */ UNUSED,
        /* 0xD */ UNUSED,
        /* 0xE */ UNUSED,
        /* 0xF */ UNUSED};
static const size_t buttons_count = sizeof(button_to_gpio) / sizeof(button_to_gpio[0]);

void switch_buttons(uint8_t button_mask) {
    for (int i = 0; i < buttons_count; i++) {
        if (button_to_gpio[i] == UNUSED) {
            continue;
        }
        if (button_mask & (1 << i)) {
            digitalWrite(button_to_gpio[i], HIGH);
        } else {
            digitalWrite(button_to_gpio[i], LOW);
        }
    }
}

void setup() {
    Serial.begin(74880);
    for (int i = 0; i < buttons_count; i++) {
        if (button_to_gpio[i] != UNUSED) {
            pinMode(button_to_gpio[i], OUTPUT);
            digitalWrite(button_to_gpio[i], LOW);
        }
    }
    IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);
    Serial.println(F("IR Receiver initialized. Waiting for codes..."));
}

static constexpr uint32_t RELEASE_TIMEOUT = 200;

void loop() {
    static bool keyIsDown = false;
    static uint32_t lastRecvTime = 0;
    if (IrReceiver.decode()) {
        if (IrReceiver.decodedIRData.protocol != SAMSUNG || IrReceiver.decodedIRData.command >= buttons_count) {
            IrReceiver.resume();
            return;
        }
        if (!keyIsDown) {
            keyIsDown = true;
            switch_buttons(1 << IrReceiver.decodedIRData.command);
            Serial.print(F("Key DOWN: 0x"));
            Serial.println(IrReceiver.decodedIRData.command, HEX);
        }
        lastRecvTime = millis();
        IrReceiver.resume();
    } else if (keyIsDown && (millis() - lastRecvTime > RELEASE_TIMEOUT)) {
        keyIsDown = false;
        Serial.println(F("All keys UP"));
        switch_buttons(0);
    }
}
