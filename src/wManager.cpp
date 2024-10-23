#include "wManager.h"
#include <WiFiManager.h>
#include <ESPmDNS.h>

void init_WifiManager(){
    //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wm;
    wm.setDarkMode(true);
    wm.setShowInfoUpdate(false);
    wm.setTitle("Bitcorn Hole Board");

    WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP

    // reset settings - wipe stored credentials for testing
    // these are stored by the esp library
    // wm.resetSettings();

    // Automatically connect using saved credentials,
    // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
    // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
    // then goes into a blocking loop awaiting configuration and will return success result

    bool res;
    // res = wm.autoConnect(); // auto generated AP name from chipid
    // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
    String mac = WiFi.macAddress();
    mac.replace(":","");
    String unicName = wmAPNAME + mac.substring(8);
    Serial.println("WIFI Manager starting...");
    wm.setHostname(unicName);    
    res = wm.autoConnect(unicName.c_str(), wmPASSWD); // password protected ap
    MDNS.begin(unicName);
    if(!res) {        
        Serial.println("Communication with WiFi module failed!");
        ESP.restart();
    } 
    else {
        //if you get here you have connected to the WiFi    
        Serial.println("connected...yeey :)");        
        Serial.print("If you network allow mDNS\nYour API endpoint is: http://");
        Serial.print(unicName+".local");
        Serial.println("/readSensor");
        
    }
}