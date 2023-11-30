#include "web.h"
#include "main.h"
#include "globals.h"

#include <EthernetWebServer.h>
#include <SD.h>

const size_t MAX_CHUNK_SIZE = 1000;

EthernetWebServer server(80);

char *buffer = (char *)malloc(MAX_CHUNK_SIZE);

void initWebServer()
{
    server.on(F("/index.html"), []() {
        File file = getFileContents("index.html");

        if(!file) {
            handleNotFound();
        }

        server.send(200, "text/html");

        while(file.available()) {
            size_t chunkSize = min(MAX_CHUNK_SIZE, file.available());
            
            file.readBytes(buffer, chunkSize);

            server.sendContent(buffer, chunkSize);

            free(buffer);
        }

        server.sendContent(RETURN_NEWLINE);
    });

    server.on(F("/"), handleRoot);
    server.onNotFound(handleNotFound);

    server.begin();
}

void handleWebClient()
{
    server.handleClient();
}

void handleRoot()
{
    String html = F("Hello from HelloServer running on ");
    html += String("Teensy_41");
    server.send(200, F("text/plain"), html);
}

void handleNotFound()
{
    String message = F("File Not Found\n\n");

    message += F("URI: ");
    message += server.uri();
    message += F("\nMethod: ");
    message += (server.method() == HTTP_GET) ? F("GET") : F("POST");
    message += F("\nArguments: ");
    message += server.args();
    message += F("\n");

    for (uint8_t i = 0; i < server.args(); i++)
    {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }

    server.send(404, F("text/plain"), message);
}