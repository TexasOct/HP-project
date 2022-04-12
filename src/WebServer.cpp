//
// Created by texas on 12/04/2022.
//

#include "WebServer.h"
#include <ESP8266WebServer.h>
#include "FSys.h"

ESP8266WebServer server(80);

void webPage()
{
    Serial.println("webPage");
    File file;
    openHtml( file );
    size_t sent = server.streamFile(file, "text/html");
    file.close();
    return;
}

void clientHandle(){
    server.handleClient();
}
