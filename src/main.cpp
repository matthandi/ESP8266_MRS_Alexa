#include <Arduino.h>
#include <EEPROM.h>

#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <Espalexa.h>

// structure for managing different types of ssid/pw/mqttbroker triple data
typedef struct EEPROM_Data{
    char SSID[3][32];
    char PW[3][32];
    char mqttBroker[3][16];
    char ClientID[16];
    char mqttPath[32];
}EEPROM_Data_td;

EEPROM_Data_td eepromData;

// prototypes
boolean connectWifi();

//callback functions
void firstLightChanged(uint8_t brightness);
void secondLightChanged(uint8_t brightness);
void thirdLightChanged(uint8_t brightness);
void betaChanged(EspalexaDevice* d);

// Change this!!
char* ssid = "";
char* password = "";

boolean wifiConnected = false;

Espalexa espalexa;

void setup()
{
	Serial.begin(115200);

	/*
	* PW will be read from the EEPROM
	*  - first time uncomment the lines for writing - write the PW once and comment the lines again :)
	*/
	EEPROM.begin(512);
	// for writing eeprom-data - uncomment one of the following lines
	//#include <..\secrets\configdata.h>
	EEPROM.get(0,eepromData);

	ssid = eepromData.SSID[0];
	password = eepromData.PW[0];
	// setup wifi mode
	//WiFi.mode(WIFI_STA);
	//wifiMulti.addAP(eepromData.SSID[0], eepromData.PW[0]);
	//wifiMulti.addAP(eepromData.SSID[1], eepromData.PW[1]);
	//wifiMulti.addAP(eepromData.SSID[2], eepromData.PW[2]);

	// Initialise wifi connection
	wifiConnected = connectWifi();
	
	if(wifiConnected){
		
		// Define your devices here. 
		espalexa.addDevice("Light 1", firstLightChanged); //simplest definition, default state off
//		espalexa.addDevice("Light 2", secondLightChanged, 255); //third parameter is beginning state (here fully on)
		espalexa.addDevice("Beta", betaChanged, EspalexaDeviceType::dimmable, 127);

		espalexa.begin();
		
	} else
	{
		while (1) {
		Serial.println("Cannot connect to WiFi. Please check data and reset the ESP.");
		delay(2500);
		}
	}
}
 
void loop()
{
	espalexa.loop();
	delay(1);
}

void betaChanged(EspalexaDevice* d) {
  if (d == nullptr) return;

  uint8_t brightness = d->getValue();
  uint8_t percent = d->getPercent();
  uint8_t degrees = d->getDegrees(); //for heaters, HVAC, ...

  Serial.print("B changed to ");
  Serial.print(percent);
  Serial.println("%");
}

//our callback functions
void firstLightChanged(uint8_t brightness) {
		Serial.print("Device 1 changed to ");
		
		//do what you need to do here

		//EXAMPLE
		if (brightness) {
		Serial.print("ON, brightness ");
		Serial.println(brightness);
		}
		else  {
		Serial.println("OFF");
		}
}

void secondLightChanged(uint8_t brightness) {
	//do what you need to do here
}

void thirdLightChanged(uint8_t brightness) {
  	//do what you need to do here
}

// connect to wifi – returns true if successful or false if not
boolean connectWifi(){
	boolean state = true;
	int i = 0;
	
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
	Serial.println("");
	Serial.println("Connecting to WiFi");

	// Wait for connection
	Serial.print("Connecting...");
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
		if (i > 20){
		state = false; break;
		}
		i++;
	}
	Serial.println("");
	if (state){
		Serial.print("Connected to ");
		Serial.println(ssid);
		Serial.print("IP address: ");
		Serial.println(WiFi.localIP());
	}
	else {
		Serial.println("Connection failed.");
	}
	return state;
}