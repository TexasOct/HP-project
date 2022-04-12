//
// Created by texas on 12/04/2022.
//

#include "../include/WebServer.h"
#include <ESP8266WebServer.h>
#include "FSys.h"

ESP8266WebServer server(80);

void webStart()
{
    server.on("/" , webPage);
    server.onNotFound([]() {server.send( 404 , "text/plain" , "File not found" );});
    server.begin();
    Serial.println("HTTP Server Start");
}

void webPage()
{
    Serial.println("webPage");
    File file;
    openHtml( file );
    size_t sent = server.streamFile(file, "text/html");
    file.close();
}

void clientHandle(){
    server.handleClient();
}
