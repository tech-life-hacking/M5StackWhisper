#ifndef TRANSCRIPTION_MANAGER_H
#define TRANSCRIPTION_MANAGER_H

#include <WiFiClientSecure.h>
#include "WifiManager.h"

class TranscriptionManager {
   private:
    const char* API_HOST    = "api.openai.com";
    const uint16_t API_PORT = 443;
    const char* API_PATH    = "/v1/audio/transcriptions";
    const char* API_KEY;
    WiFiClientSecure client;

   public:
    TranscriptionManager(const char* api_key);
    String transcriptions(const String& filename);
};

#endif  // TRANSCRIPTION_MANAGER_H
