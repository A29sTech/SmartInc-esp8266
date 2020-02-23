
/* Return Current Tamprature Only */
void geTemp(AsyncWebServerRequest* req) {
    req->send(200, "text/plain", String( currenTemp ));
}

/* Return DHT Sensor Readings */
void getDHT(AsyncWebServerRequest* req){
    String json_res = "{ \"temp\": \"" 
                      + String(dhTemp)
                      + "\", \"hum\": \""
                      + String(dhtHum)
                      + "\" }";
    req->send( 200, "application/json", json_res );
}

/* Return All Data Readed B Sensor */
void geTH(AsyncWebServerRequest* req){
    String json_res = "{ \"temp\": \"" 
                      + String(dhTemp)
                      + "\", \"hum\": \""
                      + String(dhtHum)
                      + "\", \"temprature\": \""
                      + String(currenTemp)
                      + "\" }";
    req->send( 200, "application/json", json_res );
}

/* Retrun All Configration Saved On EEPROM */
void getConfig(AsyncWebServerRequest* req) {
    String json_res = "{ \"relay_1_max\": \"" 
                      + String( relay_1_max )
                      + "\", \"relay_1_min\": \""
                      + String( relay_1_min )
                      + "\", \"relay_2_max\": \""
                      + String( relay_2_max )
                      + "\", \"relay_2_min\": \""
                      + String( relay_2_min )
                      + "\", \"buzzer_max\": \""
                      + String( buzzer_max )
                      + "\", \"buzzer_min\": \""
                      + String( buzzer_min )/*  */
                      + "\", \"relay_1_mode\": \""
                      + String( !relay_1_mode )
                      + "\", \"relay_2_mode\": \""
                      + String( !relay_2_mode )
                      + "\" }";
    req->send(200, "application/json", json_res);
}

/* Set A Configration Value */
void setConfig( AsyncWebServerRequest* req ) {

    if ( req->params() == 1 ) {
      
        AsyncWebParameter* param = req->getParam(0);
        char key = param->name()[0];
        float value = param->value().toFloat();

        if ( value >= 0.0f && value < 100.00 ) {
            //
        } else {
            req->send(200, "text/plain", "ERROR");
            return;
        }
        
        switch( key )
        {
            //
            case '0':
                relay_1_max = value;
                EEPROM_writeMe(EEPINDX*0, relay_1_max);
                break;

            case '1':
                relay_1_min = value;
                EEPROM_writeMe(EEPINDX*1 , relay_1_min);
                break;

            case '2':
                relay_2_max = value;
                EEPROM_writeMe(EEPINDX*2, relay_2_max);
                break;

            case '3':
                relay_2_min = value;
                EEPROM_writeMe(EEPINDX*3, relay_2_min);
                break;
                
            case '4':
                buzzer_max = value;
                EEPROM_writeMe(EEPINDX*4, buzzer_max);
                break;

            case '5':
                buzzer_min = value;
                EEPROM_writeMe(EEPINDX*5, buzzer_min);
                break;
            case '6':
                if ( value > 0.0f ) {
                    relay_1_mode = RELAY_ON;
                } else {
                    relay_1_mode = RELAY_OFF;
                }
                EEPROM_writeMe(EEPINDX*5+sizeof(bool), relay_1_mode);
                break;
            case '7':
                if ( value > 0.0f ) {
                    relay_2_mode = RELAY_ON;
                } else {
                    relay_2_mode = RELAY_OFF;
                }
                EEPROM_writeMe(EEPINDX*5+(sizeof(bool)*2), relay_2_mode);
                break;
          
            default:
                req->send(200, "text/plain", "ERROR");
                return;
                break;
        }
        req->send(200, "text/html", "OK");
        
    } else req->send(200, "text/html", "ERROR");
    
}


void configui( AsyncWebServerRequest* req ) {
    
}
