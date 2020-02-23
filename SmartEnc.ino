#include "funcee.h"
#include "wifi.h"
#include "etc.h"

/* Time Track For Interval */
unsigned long previousMillis = 0;    // will store last time DHT was updated

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#include <OneWire.h>
#include <DallasTemperature.h>

/* DHT Sensor Declare */
#include <DHTesp.h>
DHTesp dht; /* With DHT Esp Lib */
float dhtHum = 0.0;
float dhTemp = 0.0;

/* Global Objects & Vars */
/* Set the LCD address to 0x27 for a 16 chars and 2 line display */
LiquidCrystal_I2C lcd(0x27, 16, 2);

/* Setup a oneWire instance to communicate with any OneWire devices */
/* GPIO where the DS18B20 is connected to */
OneWire oneWire(0); /* D3 is GPIO pin number. */
DallasTemperature sensors(&oneWire);
float currenTemp = 0.00;

/* Handle GPIO */
#define RELAY_ON LOW
#define RELAY_OFF HIGH
const int8_t RELAY_1_PIN = 12;
const int8_t RELAY_2_PIN = 13;
const int8_t BUZZER_PIN = 2;

/* State Management */
bool S1 = false; /* RELAY_1 State; */
bool S2 = false; /* RELAY_2 State; */

/* Temp Settings */
#define EEPINDX sizeof(float) /* Settings Will Be Saved As float; */
float relay_1_max = 37.77f; /* INDEX 0 */
float relay_1_min = 36.90f; /* INDEX 1 */
float relay_2_max = 37.77f; /* INDEX 2 */
float relay_2_min = 36.90f; /* INDEX 3 */
float buzzer_max = 38.50f;  /* INDEX 4 */
float buzzer_min = 35.00f;  /* INDEX 5 */

/* ON OFF MODE */
bool relay_1_mode = RELAY_ON;
bool relay_2_mode = RELAY_ON;

/* Http Request Handlers */
#include "http_request_handlers.h"

void setup()
{

    /* EEPROM FUNCS */
    EEPROM.begin(512);
    EEPROM_readMe( EEPINDX*0, relay_1_max);
    EEPROM_readMe( EEPINDX*1, relay_1_min);
    EEPROM_readMe( EEPINDX*2, relay_2_max);
    EEPROM_readMe( EEPINDX*3, relay_2_min);
    EEPROM_readMe( EEPINDX*4, buzzer_max);
    EEPROM_readMe( EEPINDX*5, buzzer_min);
    EEPROM_readMe( EEPINDX*5+sizeof(bool), relay_1_mode);
    EEPROM_readMe( EEPINDX*5+(sizeof(bool)*2), relay_2_mode);


    /* GPIO OUTPUT */
    pinMode(RELAY_1_PIN, OUTPUT);
    pinMode(RELAY_2_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);

    /* DHT Setup with pin 14 and DHT11 Sensor */
    dht.setup(14, DHTesp::DHT11);
    
    /* Start DS18B20 Sendor */
    sensors.begin();
        
	  /* initialize the LCD */
	  lcd.begin();
	  /* Turn on the blacklight and print a message. */
	  lcd.backlight();
    lcd.print("A29E");
    lcd.createChar(0, UP_ARROW);
    lcd.createChar(1, DOWN_ARROW);
    lcd.createChar(2, DEG_SYMBLE);

    /* Server Handling Codes. */
    wifiSetup();
    server.on("/temp", HTTP_GET, geTemp);
    server.on("/dhtemp", HTTP_GET, getDHT);
    server.on("/geth", HTTP_GET, geTH);
    server.on("/getconf", HTTP_GET, getConfig);
    server.on("/setconf", HTTP_GET, setConfig);
    server.begin();
}









bool updaTemp();/* updaTemp Function Declaration */
void Display(); /* Display Function Declaration */
void loop()
{


        /* get time betwen */
    unsigned long currentMillis = millis();
    /* Read DHT Sensor At Its Interval */
    if (currentMillis - previousMillis >= dht.getMinimumSamplingPeriod()) {
        dhtHum = dht.getHumidity();
        dhTemp = dht.getTemperature();
        previousMillis = millis();
    }
  
    /* Update DS18B20 Temprature Sensor And Check ERROR */
    if ( updaTemp() ){
        digitalWrite( BUZZER_PIN, HIGH );
        lcd.clear();
        lcd.print("DS18B20 Sensor");
        lcd.setCursor(0, 1); // top left
        lcd.print("   Not Connected");
        delay(500);
        return;
    }

    /* --------------------ENABLE GPIO IF----------------------- */

    /* is Relay 1 Less Then Eql Minimum  */
    if ( currenTemp <= relay_1_min ) {
        digitalWrite(RELAY_1_PIN, relay_1_mode);
        S1 = !relay_1_mode;
    }
    /* is Relay 1 Gretter Then Eql Maximum */
    if ( currenTemp >= relay_1_max ) {
        digitalWrite(RELAY_1_PIN, !relay_1_mode);
        S1 = relay_1_mode;
    }
    
    /* is Relay 2 Less Then Eql Minimum */
    if ( currenTemp <= relay_2_min ) {
        digitalWrite(RELAY_2_PIN, relay_2_mode);
        S2 = !relay_2_mode;
    }
    /* is Relay 2 Gretter Then Eql Maximum */
    if ( currenTemp >= relay_2_max ) {
        digitalWrite(RELAY_2_PIN, !relay_2_mode);
        S2 = relay_2_mode;
    }

    /* Logic For Buzzer ON & OFF */
    if ( currenTemp >= buzzer_max || currenTemp <= buzzer_min ) {
        digitalWrite( BUZZER_PIN, HIGH );
    } else {
        digitalWrite( BUZZER_PIN, LOW );
    }


    /* Display Info To LCD and Delay */
    Display();
    delay( 100 );
}








/* Update Temprature Variable With Current Temprarture */
bool updaTemp()
{
    /* Request For Temprature Via ! Wire */
    sensors.requestTemperatures();
    float new_temp = sensors.getTempCByIndex(0);
    /* Check Sensor Error */
    if ( new_temp == -127.00 ) {
        return true;
    }
    /* Else */
    currenTemp = new_temp;
    return false;
}








/* Write To Display */
void Display()
{
    /* Writing To Display */
    lcd.setCursor(0, 0);
    lcd.print( currenTemp );
    lcd.write( byte( 2 ) );

    lcd.setCursor(7, 0);
    if( relay_1_mode == RELAY_ON ){
        lcd.write( byte( 0 ) );
    } else {
        lcd.write( byte( 1 ) );
    }
    lcd.print( " " );
    if( relay_2_mode == RELAY_ON ){
        lcd.write( byte( 0 ) );
    } else {
        lcd.write( byte( 1 ) );
    }
    lcd.print( " | " );
    
    /* Relay 1 State Monitor */
    if ( S1 ) {
        lcd.write( byte( 0 ) );
    } else {
        lcd.write( byte( 1 ) );
    }
    lcd.print( " ");
    /* Relay 2 State Monitor */
    if ( S2 ) {
        lcd.write( byte( 0 ) );
    } else {
        lcd.write( byte( 1 ) );
    }
    
    /* Go To Secend Line */
    lcd.setCursor(0, 1);
    lcd.print( dhTemp );
    lcd.write( byte( 2 ) );
    lcd.print( "    " );
    lcd.print( dhtHum );
    lcd.print( "%" );
}
