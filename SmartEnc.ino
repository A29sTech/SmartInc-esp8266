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
float updaTemp(){
    sensors.requestTemperatures();
    currenTemp = sensors.getTempCByIndex(0);
    return currenTemp;
}


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
float max_temp = 37.90f;
float max_diff = 1.0f;
float min_diff = 10.0f;
float diff_r1 = 1.0f;
float diff_r2 = 0.5f;

/* Http Request Handlers */
#include "http_request_handlers.h"

void setup()
{

    /* EEPROM FUNCS */
    EEPROM.begin(512);
    EEPROM_readMe( EEPINDX*0, max_temp);
    EEPROM_readMe( EEPINDX*1 , max_diff);
    EEPROM_readMe( EEPINDX*2, min_diff);
    EEPROM_readMe( EEPINDX*3, diff_r1);
    EEPROM_readMe( EEPINDX*4, diff_r2);


    /* GPIO OUTPUT */
    pinMode(RELAY_1_PIN, OUTPUT);
    pinMode(RELAY_2_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);

    /* DHT Setup with pin 14 and DHT11 Sensor */
    dht.setup(14, DHTesp::DHT11);
        
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

void loop()
{
    /* get time betwen */
    unsigned long currentMillis = millis();
    /* Read DHT Sensor At Its Interval */
    if (currentMillis - previousMillis >= dht.getMinimumSamplingPeriod()) {
        dhtHum = dht.getHumidity();
        dhTemp = dht.getTemperature();
    }
    /* Update DS18B20 Temprature Sensor */
    updaTemp();
    /* Check If Temptrature Sensor Is Connected or not */
    if( currenTemp == -127.00) {
        digitalWrite( BUZZER_PIN, HIGH );
        lcd.clear();
        lcd.print("DS18B20 Sensor");
        lcd.setCursor(0, 1); // top left
        lcd.print("   Not Connected");
    } else {
        /* OutPut Control Structures */
        if ( currenTemp >= max_temp ) {
            digitalWrite( RELAY_1_PIN, RELAY_OFF );
            digitalWrite( RELAY_2_PIN, RELAY_OFF );
            S1 = false; /* Set Relay 1 State To OFF */
            S2 = false; /* Set Relay 2 State To OFF */

            /* Buzzer Control HIGH */
            if  ( currenTemp > (max_temp + max_diff) ) {
                digitalWrite( BUZZER_PIN, HIGH );
            } else {
                digitalWrite( BUZZER_PIN, LOW );
            } 
        } else {
            /* Relay Control LOW */
            if ( currenTemp <= (max_temp - diff_r1)) {
                digitalWrite(RELAY_1_PIN, RELAY_ON);
                S1 = true; /* Set Relay 1 State To ON */
            }
            if ( currenTemp <= (max_temp - diff_r2)) {
                digitalWrite(RELAY_2_PIN, RELAY_ON);
                S2 = true; /* Set Relay 2 State To ON */
            }
            /* Buzzer Control LOW */
            if ( currenTemp < (max_temp - min_diff)){
                digitalWrite( BUZZER_PIN, HIGH );
            } else {
                digitalWrite( BUZZER_PIN, LOW );
            }
        }

            /* Writing To Display */
            lcd.setCursor(0, 0);
            lcd.print( currenTemp );
            lcd.write( byte( 2 ) );
            lcd.print( " " );
            if( S1 == true || S2 == true ){
                lcd.write( byte( 0 ) );
            } else {
                lcd.write( byte( 1 ) );
            }
            lcd.print( "  " );
            lcd.print( max_temp );
            lcd.print( "#" );
            /* Go To Secend Line */
            lcd.setCursor(0, 1);
            lcd.print( dhTemp );
            lcd.write( byte( 2 ) );
            lcd.print( "    " );
            lcd.print( dhtHum );
            lcd.print( "%" );
    }
    delay( 1000 );
}
