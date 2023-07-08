#include <SPIFFS.h>
#include <M5Unified.h>
#include "WifiManager.h"
#include "wifi_credentials.h"
#include "OpenAIAPIToken.h"
#include "TranscriptionManager.h"

WifiManager wifiManager(WIFI_SSID, WIFI_PASS);

TranscriptionManager transcriptionManager(OPENAI_TOKEN);

String text;

void setup() {
    M5.begin();
    SPIFFS.begin();

    wifiManager.connect();
}

void loop() {
    M5.update();
    if (!(M5.Touch.getCount() && M5.Touch.getDetail(0).wasClicked())) {
        return;
    }

    text     = transcriptionManager.transcriptions("/sample.wav");
    M5.Lcd.print(text);
}