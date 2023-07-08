#include "TranscriptionManager.h"
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <M5Unified.h>
#include "rootCA.h"

TranscriptionManager::TranscriptionManager(const char* api_key) : API_KEY(api_key) {
    client.setCACert(root_ca);
}

String TranscriptionManager::transcriptions(const String& filename) {
    char boundary[64] = "------------------------";
    for (auto i = 0; i < 2; ++i) {
        ltoa(random(0x7fffffff), boundary + strlen(boundary), 16);
    }

    if (!client.connect(API_HOST, API_PORT)) {
        return "connection failed";
    }

    File file = SPIFFS.open(filename);
    if (!file) {
        return "file open error";
    }

    const String header =
        "--" + String(boundary) +
        "\r\n"
        "Content-Disposition: form-data; name=\"model\"\r\n\r\nwhisper-1\r\n"
        "--" +
        String(boundary) +
        "\r\n"
        "Content-Disposition: form-data; name=\"file\"; filename=\"" +
        file.name() +
        "\"\r\n"
        "Content-Type: application/octet-stream\r\n\r\n";
    const String footer = "\r\n--" + String(boundary) + "--\r\n";

    // header
    client.printf("POST %s HTTP/1.1\n", API_PATH);
    client.printf("Host: %s\n", API_HOST);
    client.println("Accept: */*");
    client.printf("Authorization: Bearer %s\n", API_KEY);
    client.printf("Content-Length: %d\n",
                  header.length() + file.size() + footer.length());
    client.printf("Content-Type: multipart/form-data; boundary=%s\n", boundary);
    client.println();
    client.print(header.c_str());
    client.flush();

    // body
    uint8_t buf[512];
    while (file.available()) {
        const auto sz = file.read(buf, sizeof(buf));
        client.write(buf, sz);
        client.flush();
    }
    client.flush();
    file.close();

    // footer
    client.print(footer.c_str());
    client.flush();

    // wait response
    while (client.available() == 0) {
    }

    bool isBody = false;
    String body = "";
    while (client.available()) {
        const auto line = client.readStringUntil('\r');
        if (isBody) {
            body += line;
        } else if (line.equals("\n")) {
            isBody = true;
        }
    }
    client.stop();

    StaticJsonDocument<200> doc;
    deserializeJson(doc, body);
    const auto text = doc["text"].as<String>();
    if (text.length() <= 0) {
        return "No text found in response";
    }
    return text;
}
