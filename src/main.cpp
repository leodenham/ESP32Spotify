#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "creds.hpp"
#include "screen.hpp"

void setup() {
  Serial.begin(9600);
  // Module configuration
  HUB75_I2S_CFG mxconfig(
    PANEL_RES_X,   // module width
    PANEL_RES_Y,   // module height
    PANEL_CHAIN,    // Chain length,
    _pins
  );

  // Display Setup
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(90); //0-255
  dma_display->clearScreen();
  dma_display->fillScreen(myWHITE);
  
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED){
    delay(200);
    Serial.printf(".");
  }
  Serial.printf("\nConnected to wifi at IP: %s\n", WiFi.localIP().toString().c_str());

  HTTPClient http;
  http.begin("http://httpbin.org/get"); // Specify the URL
  int httpCode = http.GET();            // Make the request

  // Check the returning code
  if (httpCode > 0) {
    // Get the request response payload
    String payload = http.getString();
    Serial.println(payload);
  } else {
    Serial.println("Error on HTTP request");
  }

  http.end(); // Free the resources
}

void loop() {
  Serial.println("test");
  delay(1000); // Add a delay to avoid flooding the serial monitor
}
