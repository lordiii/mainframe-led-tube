#include <web.h>
#include <main.h>
#include <globals.h>
#include <EthernetWebServer.h>
#include <SD.h>

char buffer[WEB_CHUNK_SIZE];

EthernetWebServer server(80);

void initWebServer() {
    server.on(
            "/effects",
            HTTP_GET,
            []() {
                String response = "[";

                for (int i = 0; i < effectCount; i++) {
                    response.append('"');
                    response.append(effects[i].name);
                    response.append('"');

                    if (i + 1 != effectCount) {
                        response.append(',');
                    }
                }

                response.append(']');

                server.send(200, "application/json", response);
                server.client().close();
            }
    );

    server.on(
            "/current",
            HTTP_GET,
            []() {
                if (state->current != nullptr) {
                    server.send(200, "text/plain", state->current->name);
                } else {
                    server.send(200, "text/plain", "off");
                }
            }
    );

    server.on(
            "/set-effect",
            HTTP_POST,
            []() {
                if (server.authenticate("tube-ctl", "supersecretpassword")) {
                    String effectName = server.arg("effect");

                    Effect *effect = nullptr;
                    for (int i = 0; i < effectCount; i++) {
                        effect = &effects[i];

                        if (effectName.equals(effect->name)) {
                            break;
                        } else {
                            effect = nullptr;
                        }
                    }

                    setCurrentEffect(effect);

                    if (effect != nullptr) {
                        displayEffect(effect->name);
                    }

                    server.send(200, "text/plain", "");
                } else {
                    server.requestAuthentication();
                }

                server.client().close();
            }
    );

    server.on(
            "/leds",
            HTTP_GET,
            sendLEDs
    );

    server.on(
            "/",
            HTTP_GET,
            []() {
                if (!sendFile("/index.html")) {
                    server.send(404, "text/plain", "FileNotFound");
                }
            }
    );

    server.onNotFound(
            []() {
                if (!sendFile(server.uri().c_str())) {
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

bool sendFile(const char *fileName) {
    File file = getFileContents(fileName);

    if (file) {
        const char *contentType = getContentType(fileName);

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

const char *getContentType(const char *filename) {
    if (server.hasArg("download")) {
        return "application/octet-stream";
    } else if (strEndsWith(filename, ".htm") || strEndsWith(filename, ".html")) {
        return "text/html";
    } else if (strEndsWith(filename, ".css")) {
        return "text/css";
    } else if (strEndsWith(filename, ".js")) {
        return "application/javascript";
    } else if (strEndsWith(filename, ".png")) {
        return "image/png";
    } else if (strEndsWith(filename, ".gif")) {
        return "image/gif";
    } else if (strEndsWith(filename, ".jpg")) {
        return "image/jpeg";
    } else if (strEndsWith(filename, ".ico")) {
        return "image/x-icon";
    } else if (strEndsWith(filename, ".xml")) {
        return "text/xml";
    } else if (strEndsWith(filename, ".pdf")) {
        return "application/x-pdf";
    } else if (strEndsWith(filename, ".zip")) {
        return "application/x-zip";
    } else if (strEndsWith(filename, ".gz")) {
        return "application/x-gzip";
    }

    return "text/plain";
}

File getFileContents(const char *fileName) {
    if (SD.begin(BUILTIN_SDCARD)) {

        File dataFile = SD.open(fileName, FILE_READ);

        if (dataFile) {
            return dataFile;
        } else {
            return nullptr;
        }
    } else {
        return nullptr;
    }
}

bool strEndsWith(const char *str, const char *suffix) {
    if (!str || !suffix) {
        return false;
    }

    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);

    if (lensuffix > lenstr) {
        return false;
    }

    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}