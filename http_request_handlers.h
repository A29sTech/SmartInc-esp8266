
void geTemp(AsyncWebServerRequest* req) {
    req->send(200, "text/plain", String( currenTemp ));
}

void getDHT(AsyncWebServerRequest* req){
    String json_res = "{ \"temp\": \"" 
                      + String(dhTemp)
                      + "\", \"hum\": \""
                      + String(dhtHum)
                      + "\" }";
    req->send( 200, "application/json", json_res );
}

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

void getConfig(AsyncWebServerRequest* req) {
    String json_res = "{ \"max_temp\": \"" 
                      + String( max_temp )
                      + "\", \"max_diff\": \""
                      + String( max_diff )
                      + "\", \"max_diff\": \""
                      + String( max_diff )
                      + "\", \"min_diff\": \""
                      + String( min_diff )
                      + "\", \"diff_r1\": \""
                      + String( diff_r1 )
                      + "\", \"diff_r2\": \""
                      + String( diff_r2 )
                      + "\" }";
    req->send(200, "application/json", json_res);
}

void setConfig( AsyncWebServerRequest* req ) {

    if ( req->params() == 1 ) {
      
        AsyncWebParameter* param = req->getParam(0);
        char key = param->name()[0];
        float value = param->value().toFloat();

        if ( value > 0.0 && value < 100.00 ) {
            //
        } else {
            req->send(200, "text/plain", "ERROR");
            return;
        }
        
        switch( key )
        {
            //
            case '0':
                max_temp = value;
                EEPROM_writeMe(0, max_temp);
                break;

            case '1':
                max_diff = value;
                EEPROM_writeMe(EEPINDX*1 , max_diff);
                break;

            case '2':
                min_diff = value;
                EEPROM_writeMe(EEPINDX*2, min_diff);
                break;

            case '3':
                diff_r1 = value;
                EEPROM_writeMe(EEPINDX*3, diff_r1);
                break;
                
            case '4':
                diff_r2 = value;
                EEPROM_writeMe(EEPINDX*4, diff_r2);
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
