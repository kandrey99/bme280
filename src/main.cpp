#include <Arduino.h>
#include <TFT_eSPI.h>
#include <Esp.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <display/display.h>
#include ".config.h"

extern "C"
{
    #include "user_interface.h"
    extern struct rst_info rst_info;
}

extern const char* ssid;
extern const char* password;
extern const char* mqtt_server;
extern const char* mqtt_clientId;
extern const char* mqtt_user;
extern const char* mqtt_password;

const uint32_t sleep_time = 5e6;

TFT_eSPI tft = TFT_eSPI();
Display display = Display(&tft);
WiFiClient wifiClient;
PubSubClient client(wifiClient);
Adafruit_BME280 bme;

void callback(char* topic, byte* payload, unsigned int length) 
{
    if (strcmp(topic, "bme1/rotation") == 0)
    {
        tft.fillScreen(TFT_BLACK);
        uint8_t rotation = *((uint8_t*)payload);
        tft.setRotation(rotation);
    }
    if (strcmp(topic, "bme1/brightness") == 0)
    {
        uint8_t brightness = *((uint8_t*)payload);
    }
}

void setup_wifi()
{
    delay(100);        
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to SSID "); Serial.println(ssid);
    for (int i = 0; i < 10; i++)
    {        
        if (WiFi.status() == WL_CONNECTED)
        {            
            Serial.println("WiFi connected");
            Serial.print("Local IP address: "); Serial.println(WiFi.localIP());
            break;
        }
        delay(500);
        Serial.print(".");        
    }
    Serial.println();
}

void reconnect()
{ 
    if (!client.connected())
    {
        Serial.print("Connecting to MQTT server "); Serial.println(mqtt_server);
        if (client.connect(mqtt_clientId, mqtt_user, mqtt_password))
        {
            Serial.println("MQTT connected");
            Serial.print("Local IP address: "); Serial.println(WiFi.localIP());            
            client.subscribe("bme1/rotation");
            client.subscribe("bme1/brightness");
        }
        else
        {
            Serial.print("MQTT connection failed, rc="); Serial.print(client.state()); Serial.println(", try again later");
        }
    }
} 

void do_work()
{
    if (!client.connected()) reconnect();

    client.loop();

    String temperature = (String)bme.readTemperature();
    String humidity = (String)bme.readHumidity();
    String pressure = (String)(bme.readPressure() * 0.00750063755419211);

    client.publish("bme1/temperature", temperature.c_str());
    client.publish("bme1/humidity", humidity.c_str());
    client.publish("bme1/pressure", pressure.c_str());
    
    tft.setTextFont(1);    
    tft.setCursor(0, 0);

    tft.setTextColor(TFT_CYAN, TFT_BLACK);    
    display.setFont(&term_8x15pxFontInfo);

    /* print temperature */
    display.print("T "); display.print(temperature.c_str()); display.print(" t"); display.println("C");
    tft.cursor_y += 10;

    /* print humidity */
    display.print("H "); display.print(humidity.c_str()); display.println(" h");
    tft.cursor_y += 10;

    /* print pressure */
    display.print("P "); display.print(pressure.c_str()); display.println(" p");

    /* print system information */
    tft.setTextFont(1);
    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    tft.setCursor(0, 90);
    tft.print("ssid: "); tft.println(ssid);
    tft.print("ip: "); tft.println(WiFi.localIP().toString());
    tft.print("mqtt: "); tft.println(mqtt_server);
    tft.print("client: "); tft.print(mqtt_clientId);    
}

void setup(void) {
    Serial.begin(115200);

    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
    bme.begin(0x76);

    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);


    //ESP.deepSleep(sleep_time, RF_NO_CAL);
}

void loop(void) 
{
    do_work();
    delay(5000);
}
