#include "web.h"
#include "main.h"
#include "globals.h"

#include <EthernetWebServer.h>
#include <SD.h>

const size_t MAX_CHUNK_SIZE = 1024;
char buffer[MAX_CHUNK_SIZE];

EthernetWebServer server(80);

void initWebServer()
{
    server.onNotFound(
        []()
        {
            if (!sendFile(server.uri()))
            {
                server.send(404, "text/plain", "FileNotFound");
            }
        });

    server.begin();
}

void handleWebClient()
{
    server.handleClient();
}

bool sendFile(String fileName)
{
    fileName = fileName.substring(1);

    if (fileName.endsWith("/"))
    {
        fileName += "index.html";
    }

    Serial.println("handleFileRead: " + fileName);

    File file = getFileContents(fileName);
    if (file)
    {
        String contentType = getContentType(fileName);

        server.setContentLength(file.size());
        server.send(200, contentType, "");

        while(true)
        {
            size_t chunkSize = min(file.available(), MAX_CHUNK_SIZE);
            if(chunkSize == 0) {
                break;
            }

            size_t fetched = file.readBytes(buffer, chunkSize);

            Serial.print("Sending ");
            Serial.print(fetched);
            Serial.println(" bytes.");

            server.client().writeFully(buffer, fetched);
        }

        file.close();
        server.client().close();

        return true;
    }

    return false;
}

String getContentType(const String &filename)
{
    if (server.hasArg("download"))
    {
        return "application/octet-stream";
    }
    else if (filename.endsWith(".htm"))
    {
        return "text/html";
    }
    else if (filename.endsWith(".html"))
    {
        return "text/html";
    }
    else if (filename.endsWith(".css"))
    {
        return "text/css";
    }
    else if (filename.endsWith(".js"))
    {
        return "application/javascript";
    }
    else if (filename.endsWith(".png"))
    {
        return "image/png";
    }
    else if (filename.endsWith(".gif"))
    {
        return "image/gif";
    }
    else if (filename.endsWith(".jpg"))
    {
        return "image/jpeg";
    }
    else if (filename.endsWith(".ico"))
    {
        return "image/x-icon";
    }
    else if (filename.endsWith(".xml"))
    {
        return "text/xml";
    }
    else if (filename.endsWith(".pdf"))
    {
        return "application/x-pdf";
    }
    else if (filename.endsWith(".zip"))
    {
        return "application/x-zip";
    }
    else if (filename.endsWith(".gz"))
    {
        return "application/x-gzip";
    }

    return "text/plain";
}