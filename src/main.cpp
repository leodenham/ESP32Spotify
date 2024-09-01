#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "creds.hpp"
#include "helpers.h"
#include "screen.hpp"

Album current_album;


void refresh_auth_token() {
  if (!refresh_token){
    Serial.println("No refresh token");
    return;
  }
  HTTPClient http;
  http.begin("https://accounts.spotify.com/api/token");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Authorization", "Basic " + base64_encode(String(CLIENT_ID) + ":" + String(CLIENT_SECRET)));
  String httpRequestData = "grant_type=refresh_token&refresh_token=" + String(refresh_token);

  int response = http.POST(httpRequestData);
  if (response < 0){
    Serial.println("Failed Refresh Token");
  }
  String refreshPayload = http.getString();
  JsonDocument refreshParsed;
  deserializeJson(refreshParsed, refreshPayload);
  String newRefreshToken = refreshParsed["refresh_token"].as<String>();
  if (newRefreshToken != "null"){
    Serial.println("New refresh token:");
    Serial.println(newRefreshToken);
    refresh_token = newRefreshToken;
  }
  access_token = refreshParsed["access_token"].as<String>();
  refresh_recieved = millis();
  http.end(); 
}

String get_current_playing_album() {
  if (access_token == "") {
    Serial.println("No access token");
    return;
  }
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    return;
  }
  
  HTTPClient http;
  http.setTimeout(5000);  // Set a timeout of 5 seconds
  http.begin("https://api.spotify.com/v1/me/player/currently-playing");
  http.addHeader("Authorization", "Bearer " + access_token);
  int httpResponseCode = http.GET();


  if (httpResponseCode < 0){
    Serial.println("Error fetching");
  }
  
  if (httpResponseCode == 204){
    Serial.println("No currently playing track");
  }

  String payload = http.getString();
  JsonDocument responseParsed;
  deserializeJson(responseParsed, payload);
  http.end();

  JsonObject item = responseParsed["item"];
  if (!item){
    Serial.println("No item");
  }
  JsonObject album = item["album"];
  if (!album){
    Serial.println("No album");
  }
  String album_id = album["id"];
  String album_name = album["name"];
  JsonArray images = album["images"];


  int min_index = -1;
  for (JsonObject image : images) {
    if (image["width"] < 64){
      break;
    }
    min_index++;
  }

  String image_url = images[min_index]["url"];
  int image_width = images[min_index]["width"].as<int>();
  int image_height = images[min_index]["height"].as<int>();

  Image album_image = {
    .url = image_url,
    .height = image_height, 
    .width = image_width
  };

  Album new_album = {
    .id = album_id, 
    .name = album_name, 
    .image = album_image
  };

  current_album = new_album;
  http.end();
}


void get_current_album_art() {
  HTTPClient http;
  if (!current_album.id){
    Serial.println("No current album");
  }
  http.begin(current_album.image.url);
  int httpResponseCode = http.GET();

  if (httpResponseCode == HTTP_CODE_OK) {
    WiFiClient *stream = http.getStreamPtr();

    uint8_t buffer[128] = { 0 };
    int len = stream->available();
    int x = 0, y = 0;  // Starting coordinates for the image

    while (len > 0) {
      int c = stream->readBytes(buffer, sizeof(buffer));
      // Process the buffer to display the image
      // Assuming you have a function to draw JPEG from buffer:
      drawJPEG(buffer, c, x, y);
      len = stream->available();
    }
  }
}



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
  dma_display->setBrightness8(255); //0-255
  dma_display->clearScreen();
  dma_display->fillScreen(myTEAL);
  
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED){
    delay(200);
    Serial.printf(".");
  }
  Serial.printf("\nConnected to wifi at IP: %s\n", WiFi.localIP().toString().c_str());
  delay(2000);

  refresh_auth_token();
  get_current_playing();
}


void loop() {
  delay(1000);
  if (millis() - refresh_recieved > 59*60*1000) { // Refresh every 55 minutes
    refresh_auth_token();
  }
}
