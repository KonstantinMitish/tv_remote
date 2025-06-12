#include <IRremote.hpp>

static constexpr uint8_t IR_PIN = 4;// PIN D2

static const uint8_t pin_to_gpio[] = {
        GPIO_NUM_16,
        GPIO_NUM_5,
        GPIO_NUM_4,
        GPIO_NUM_0,
        GPIO_NUM_2,
        GPIO_NUM_14,
        GPIO_NUM_12,
        GPIO_NUM_13,
};
static const size_t pins_count = sizeof(pin_to_gpio) / sizeof(pin_to_gpio[0]);

static const uint8_t button_to_gpio[] = {
        /* 0x0 */ 0xFF,
        /* 0x1 */ pin_to_gpio[1],// SOURCE
        /* 0x2 */ pin_to_gpio[3],// POWER
        /* 0x3 */ 0xFF,
        /* 0x4 */ 0xFF,
        /* 0x5 */ 0xFF,
        /* 0x6 */ 0xFF,
        /* 0x7 */ pin_to_gpio[4],// VOLUME_UP
        /* 0x8 */ 0xFF,
        /* 0x9 */ 0xFF,
        /* 0xA */ 0xFF,
        /* 0xB */ pin_to_gpio[5],// VOLUME_DOWN
        /* 0xC */ 0xFF,
        /* 0xD */ 0xFF,
        /* 0xE */ 0xFF,
        /* 0xF */ 0xFF};
static const size_t buttons_count = sizeof(button_to_gpio) / sizeof(button_to_gpio[0]);

void switch_buttons(uint8_t button_mask) {
    for (int i = 0; i < buttons_count; i++) {
        if (button_to_gpio[i] == 0xFF) {
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
    while (!Serial)
        ;
    for (int i = 0; i < buttons_count; i++) {
        if (button_to_gpio[i] != 0xFF) {
            pinMode(button_to_gpio[i], OUTPUT);
        }
    }
    IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);
    Serial.println("IR Receiver initialized. Waiting for codes...");
}

static constexpr uint32_t RELEASE_TIMEOUT = 200;

void loop() {
    static bool keyIsDown = false;
    static uint32_t lastRecvTime = 0;
    static uint32_t lastCommand = 0;
    if (IrReceiver.decode() && IrReceiver.decodedIRData.protocol == SAMSUNG) {
        if (!keyIsDown) {
            keyIsDown = true;
            switch_buttons(1 << (IrReceiver.decodedIRData.command & 0xF));
            Serial.print(F("Key DOWN: 0x"));
            Serial.println(lastCommand, HEX);
        }
        lastRecvTime = millis();
    } else if (keyIsDown && (millis() - lastRecvTime > RELEASE_TIMEOUT)) {
        keyIsDown = false;
        Serial.println(F("All keys UP"));
        switch_buttons(0);
    }

    IrReceiver.resume();
}
