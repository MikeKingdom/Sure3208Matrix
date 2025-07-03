#include <Arduino.h>
#include "HT1632.h"
#include "font_8x5.h"
#include "images.h"

#define _TASK_SLEEP_ON_IDLE_RUN
#define _TASK_STATUS_REQUEST
#include <TaskScheduler.h>

#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <config.h>

#define DATA 16
#define WR 17
#define CS1 18
#define CS2 19
#define CS3 21
#define CS4 22

#define NUM_DISPLAYS 4

// Debug and Test options
#define _DEBUG_

#ifdef _DEBUG_
#define _PP(a) Serial.print(a);
#define _PL(a) Serial.println(a);
#else
#define _PP(a)
#define _PL(a)
#endif

#define CONNECT_TIMEOUT 30
#define CONNECT_OK 0
#define CONNECT_FAILED (-99)

int textX = 0;
int textWidth = 0;
char text[256] = "Hello, how are you?";

WebServer server(80);

Scheduler scheduler;

void connectInit();
Task tConnect(TASK_SECOND, TASK_FOREVER, &connectInit, &scheduler, true);

void connectCheck();

void handleScrollAPI();

void scroll();
Task tScroll(TASK_MILLISECOND * 80, TASK_FOREVER, &scroll, &scheduler, true);

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ; // wait for serial attach
  HT1632.begin(CS1, CS2, CS3, CS4, WR, DATA);

  textWidth = HT1632.getTextWidth(text, FONT_8X5_END, FONT_8X5_HEIGHT);

  Serial.println("Initialized");
}

void loop()
{
  scheduler.execute();
  server.handleClient();
}

void connectInit()
{
  _PP(millis());
  _PL(F(": connectInit."));
  _PL(F("WiFi parameters: "));
  _PP(F("SSID: "));
  Serial.println(ssid);
  _PP(F("PWD : "));
  Serial.println(password);

  WiFi.mode(WIFI_STA);
  // WiFi.hostname("clock");
  WiFi.begin(ssid, password);
  yield();

  tConnect.yield(&connectCheck);
}

void connectCheck()
{
  _PP(millis());
  _PL(F(": connectCheck."));

  if (WiFi.status() == WL_CONNECTED)
  {
    _PP(millis());
    _PP(F(": Connected to AP. Local IP: "));
    _PL(WiFi.localIP());
    
    // Setup web server routes
    server.on("/api/scroll", HTTP_POST, handleScrollAPI);
    server.begin();
    _PL(F("Web server started"));
    
    tConnect.disable();
  }
  else
  {
    if (tConnect.getRunCounter() % 15 == 0)
    {
      _PP(millis());
      _PL(F(": Re-requesting connection to AP..."));

      WiFi.disconnect(true);
      yield();

      WiFi.hostname("cyd_clock");
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, password);
      yield();
    }

    if (tConnect.getRunCounter() == CONNECT_TIMEOUT)
    {
      tConnect.getInternalStatusRequest()->signal(CONNECT_FAILED);
      tConnect.disable();

      _PP(millis());
      _PL(F(": connectOnDisable."));
      _PP(millis());
      _PL(F(": Unable to connect to WiFi."));
    }
  }
}

void handleScrollAPI()
{
  _PL(F("Received POST request to /api/scroll"));
  
  if (server.hasArg("plain"))
  {
    String body = server.arg("plain");
    _PP(F("Request body: "));
    _PL(body);
    
    // Create JSON document with appropriate size
    JsonDocument doc;
    
    // Parse JSON
    DeserializationError error = deserializeJson(doc, body);
    
    if (error)
    {
      _PP(F("JSON parsing failed: "));
      _PL(error.c_str());
      server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid JSON format\"}");
      return;
    }
    
    // Check if "text" field exists
    if (!doc.containsKey("text"))
    {
      server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing 'text' field\"}");
      return;
    }
    
    // Get the text value
    const char* newText = doc["text"];
    
    if (newText == nullptr)
    {
      server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"'text' field must be a string\"}");
      return;
    }
    
    // Check text length
    if (strlen(newText) >= sizeof(text))
    {
      server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Text too long\"}");
      return;
    }
    
    // Update the text
    strcpy(text, newText);
    textWidth = HT1632.getTextWidth(text, FONT_8X5_END, FONT_8X5_HEIGHT);
    textX = 0; // Reset scroll position
    
    _PP(F("Text updated to: "));
    _PL(text);
    
    server.send(200, "application/json", "{\"status\":\"success\",\"message\":\"Text updated successfully\"}");
    return;
  }
  
  server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"No body in request\"}");
}

void scroll()
{
  for (int i = 0; i < NUM_DISPLAYS; i++) {
    HT1632.renderTarget(i);
    HT1632.clear();
    HT1632.drawText(text, OUT_SIZE * (NUM_DISPLAYS - i) - textX, 0, FONT_8X5, FONT_8X5_END, FONT_8X5_HEIGHT);
    HT1632.render();
  }
  textX = (textX + 1) % (textWidth + OUT_SIZE * NUM_DISPLAYS);
}