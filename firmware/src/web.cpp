#include <web.h>
#include <main.h>
#include <globals.h>
#include <EthernetWebServer.h>
#include <SD.h>

char buffer[WEB_CHUNK_SIZE];

EthernetWebServer server(80);

void initWebServer() {
    server.on(
            "/leds",
            HTTP_GET,
            sendLEDs
    );

    server.onNotFound(
            []() {
                if (!sendFile(server.uri())) {
                    server.send(404, "text/plain", "FileNotFound");
                }
            });

    server.begin();
}

void handleWebClient() {
    server.handleClient();
}

void sendLEDs() {
    const static int LENGTH = LED_PER_RING * LED_TOTAL_RINGS * LED_BYTES_PER_LED;

    server.setContentLength(LENGTH);
    server.send(200, "application/octet-stream", "");

    auto *ledBuffer = (uint8_t *) drawingMemory;
    int i = 0;
    for (; i < LENGTH; i++) {
        server.client().writeFully(*ledBuffer);
        ledBuffer++;
    }

    server.client().close();
}

bool sendFile(String fileName) {
    fileName = fileName.substring(1);

    if (fileName.endsWith("/")) {
        fileName += "index.html";
    }

    File file = getFileContents(fileName);
    if (file) {
        String contentType = getContentType(fileName);

        server.setContentLength(file.size());
        server.send(200, contentType, "");

        while (true) {
            size_t chunkSize = min(file.available(), WEB_CHUNK_SIZE);
            if (chunkSize == 0) {
                break;
            }

            size_t fetched = file.readBytes(buffer, chunkSize);
            server.client().writeFully(buffer, fetched);
        }

        file.close();
        server.client().close();

        return true;
    }

    return false;
}

String getContentType(const String &filename) {
    if (server.hasArg("download")) {
        return "application/octet-stream";
    } else if (filename.endsWith(".htm") || filename.endsWith(".html")) {
        return "text/html";
    } else if (filename.endsWith(".css")) {
        return "text/css";
    } else if (filename.endsWith(".js")) {
        return "application/javascript";
    } else if (filename.endsWith(".png")) {
        return "image/png";
    } else if (filename.endsWith(".gif")) {
        return "image/gif";
    } else if (filename.endsWith(".jpg")) {
        return "image/jpeg";
    } else if (filename.endsWith(".ico")) {
        return "image/x-icon";
    } else if (filename.endsWith(".xml")) {
        return "text/xml";
    } else if (filename.endsWith(".pdf")) {
        return "application/x-pdf";
    } else if (filename.endsWith(".zip")) {
        return "application/x-zip";
    } else if (filename.endsWith(".gz")) {
        return "application/x-gzip";
    }

    return "text/plain";
}