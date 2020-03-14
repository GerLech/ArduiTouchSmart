
#include <SPI.h>
#include "Adafruit_GFX.h" //Grafik Bibliothek
#include "Adafruit_ILI9341.h" // Display Treiber
#include <ArduinoJson.h>
#include <WebServer.h>
#include <WebConfig.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "XPT2046_Touchscreen.h"
#include "TouchEvent.h"
#include "TFTForm.h"
#include "SPIFFS.h"
#include "FS.h"
#include "ArduiTouchSmart.h"
#include "fonts/AT_Standard9pt7b.h"

//Verwendete Pins am Display
#define TFT_CS   5
#define TFT_DC   4
#define TFT_MOSI 23
#define TFT_CLK  18
#define TFT_RST  22
#define TFT_MISO 19
#define TFT_LED  15
#define TOUCH_CS 14
#define TOUCH_IRQ 2

#define GMT_OFFSET_SEC 3600

//Konfigurationswerte
String param = "["
  "{"
  "'name':'ssid',"
  "'label':'SSID des WLAN',"
  "'type':'select',"
  "'default':''"
  "},"
  "{"
  "'name':'pwd',"
  "'label':'WLAN Passwort',"
  "'type':'password',"
  "'default':''"
  "},"
  "{"
  "'name':'broker',"
  "'label':'MQTT Broker',"
  "'type':'text',"
  "'default':'raspberrypi4'"
  "},"
  "{"
  "'name':'hostname',"
  "'label':'Hostname',"
  "'type':'text',"
  "'default':'ArduiSmart'"
  "},"
  "{"
  "'name':'ntpserver',"
  "'label':'NTP Server',"
  "'type':'text',"
  "'default':''"
  "},"
  "{"
  "'name':'summertime',"
  "'label':'Sommerzeit',"
  "'type':'check',"
  "'default':'0'"
  "}"
  "]";

int8_t subemnu1[12] = {
    CMD_SCREENOFF,
    CMD_PLAY,
    CMD_PAGEPLUS,
    CMD_PAGEMINUS,
    CMD_WDGPLUS,
    CMD_WDGMINUS,
    CMD_EDITPAGE,
    CMD_EDIT,
    CMD_EXCHANGE,
    CMD_CONF,
    CMD_LEFT,
    CMD_RIGHT
  };
  int8_t submenu2[12] = {
    CMD_SCREENOFF,
    CMD_PLAY,
    CMD_SAVE,
    CMD_READ,
    -1,
    -1,
    -1,
    CMD_RESET,
    CMD_INFO,
    CMD_WLAN,
    CMD_CALIBRATE,
    CMD_BACK
  };

WebConfig conf;
WebServer server;
WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
XPT2046_Touchscreen touch(TOUCH_CS, TOUCH_IRQ);
TouchEvent tevent(touch);
TFTForm tftconf(&tft, &AT_Standard9pt7b);
ArduiTouchSmart ats = ArduiTouchSmart(&tft,&tftconf,&AT_Standard9pt7b, true, true);

//globale Variablen
boolean connected;
String clientId; //ID für MQTT
uint32_t nextConnect = 0; //Nächster Zeitpunkt für MQTT Verbindungsversuch
uint32_t lt;
uint32_t ltime = 0;
bool confMode = false;
bool ledOn = true;
bool calibrate = false;
bool wlan = false;
bool info = false;
bool reset = false;
uint8_t calLevel = 0;
//configuration values
uint16_t xMin = 208;
uint16_t yMin = 233;
uint16_t xMax = 3709;
uint16_t yMax = 3880;
char ssid[50];
char password[30];
char broker[128] = "raspberrypi4";
char hostname[30] = "MySmarthome";
char ntpserver[128] = "de.pool.ntp.org";
bool summertime = false;
void showInfo() {
  float num;
  uint8_t z;
  info = true;
  tft.fillScreen(ILI9341_DARKGREY);
  tft.setFont(&AT_Standard9pt7b);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(5,25); tft.print("WLAN SSID");
  tft.setCursor(120,25); tft.print(ssid);
  tft.setCursor(5,45); tft.print("Verbunden");
  if (connected)  { tft.setCursor(120,45); tft.print("ja");}
  else  { tft.setCursor(120,45); tft.print("nein");}
  tft.setCursor(5,65); tft.print("MQTT Broker");
  tft.setCursor(120,65); tft.print(broker);
  tft.setCursor(5,85); tft.print("Hostname");
  tft.setCursor(120,85); tft.print(hostname);
  tft.setCursor(5,105); tft.print("Freier RAM");
  num = ESP.getFreeHeap()/1024;
  tft.setCursor(120,105); tft.printf("%.2f kByte",num);
  tft.setCursor(5,125); tft.print("Rev. Nr.");
  z = ESP.getChipRevision();
  tft.setCursor(120,125); tft.print(z);
  tft.setCursor(5,145); tft.print("CPU Freq.");
  z = ESP.getCpuFreqMHz();
  tft.setCursor(120,145); tft.printf("%i MHz",z);
  tft.setCursor(5,165); tft.print("Flash");
  num= ESP.getFlashChipSize()/1024/1024;
  tft.setCursor(120,165);tft.printf("%.3f MByte" );
}

void showCalibrate() {
  tft.fillScreen(ILI9341_WHITE);
  tft.setFont(&AT_Standard9pt7b);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(10,100);
  calLevel++;
  if (calLevel < 5) {
    tft.print("Klicken Sie an den");
    tft.setCursor(10,120);
    switch (calLevel) {
      case 1 : tft.print("oberen Rand"); break;
      case 2 : tft.print("rechten Rand"); break;
      case 3 : tft.print("unteren Rand"); break;
      case 4 : tft.print("linken Rand"); break;
    }
    tevent.autocalibrate(calLevel);
  } else {
    if (calLevel == 5) {
      tft.print("Kalibrierung fertig");
      tevent.getMinMax(&xMin,&yMin,&xMax,&yMax);
      tft.setCursor(10,120);
      tft.printf("xMin %i yMin %i",xMin,yMin);
      tft.setCursor(10,140);
      tft.printf("xMax %i yMax %i",xMax,yMax);
    } else {
      calLevel = 0;
      calibrate = false;
      char buf[500];
      StaticJsonDocument<500> doc;
      doc["xMin"]=xMin;
      doc["yMin"]=yMin;
      doc["xMax"]=xMax;
      doc["yMax"]=yMax;
      serializeJson(doc,buf);
      File f = SPIFFS.open("/calibration.jsn","w+");
      if (f) {
        f.print(buf);
        f.close();
      }
      ats.endEdit("");
    }
  }

}

void showWlanForm(){
  char buf[300];
  StaticJsonDocument<300> doc;
  doc["ssid"]=ssid;
  doc["pwd"]=password;
  doc["broker"]=broker;
  doc["hostname"]=hostname;
  doc["ntpserver"]=ntpserver;
  doc["summertime"]=summertime;
  serializeJson(doc,buf);
  tftconf.setDescription(param);
  tftconf.setValues(buf);
  int n = WiFi.scanNetworks();
  Serial.printf("%i Netzwerke\n",n );
  if (n>0) {
    int8_t ix = tftconf.findName("ssid");
    Serial.printf("index %i für ssid\n",ix);
    if (ix>=0) {
      tftconf.clearOptions("ssid");
      for (uint8_t i = 0; i<n; i++) tftconf.addOption(ix,WiFi.SSID(i));
    }
  }
  wlan = true;
  tftconf.showForm();
}

void displayLed(bool on) {
  digitalWrite(TFT_LED, !on);
  ledOn = on;
}

void onAllEvents(int16_t x, int16_t y, EV event)
{
  if (event == EV::EVT_CLICK) {
    if (!ledOn) {
      displayLed(true);
      return;
    }
    if (tftconf.isActive()) {
      tftconf.handleClick(x, y);
    } else if (calibrate) {
      showCalibrate();
    } else if (info) {
      info = false;
      ats.endEdit("");
    } else {
      ats.touchEvent(x, y, EV::EVT_CLICK);
    }
  } else {
    ats.touchEvent(x,y,event);
  }
}

void onConfSave(String data) {
  if (reset) {
    ESP.restart();
    tft.fillScreen(ILI9341_BLACK);
  }
  if (wlan) {
    Serial.println("Setup fertig");
    File f = SPIFFS.open("/wlansetup.jsn","w+");
    if (f) {
      Serial.print("Setup:");
      Serial.println(data);
      f.print(data);
      f.close();
    }
    wlan = false;
    ats.endEdit("");
  } else {
    ats.endEdit(data);
  }
}

void onConfCancel() {
  reset = false;
  ats.endEdit("");
}

void externEdit(uint8_t cmd) {
  switch (cmd) {
    case CMD_SCREENOFF:   displayLed(false); break;
    case CMD_CALIBRATE: calLevel = 0;
      calibrate = true;
      showCalibrate();
      break;
    case CMD_WLAN: showWlanForm(); break;
    case CMD_INFO: showInfo(); break;
    case CMD_RESET: tftconf.showDialog("Wollen Sie die Anwendung neu starten? Ungespeicherte Daten gehen verloren!");
      reset = true;
      break;
    default: ats.endEdit(""); break;
  }
}
//Falls möglich am lokalen WLAN anmelden
boolean initWiFi() {
    boolean connected = false;
    //Stationsmodus
    WiFi.mode(WIFI_STA);
    //wenn eine SSID konfiguriert wurde versuchen wir uns anzumelden
    if (strlen(ssid) != 0) {
      Serial.print("Verbindung zu ");
      Serial.print(ssid);
      Serial.println(" herstellen");
      //Verbindungsaufbau starten
      WiFi.begin(ssid,password);
      uint8_t cnt = 0;
      //10 Sekunden auf erfolgreiche Verbindung warten
      while ((WiFi.status() != WL_CONNECTED) && (cnt<20)){
        delay(500);
        Serial.print(".");
        cnt++;
      }
      Serial.println();
      //Wenn die Verbindung erfolgreich war, wird die IP-Adresse angezeigt
      if (WiFi.status() == WL_CONNECTED) {
        Serial.print("IP-Adresse = ");
        Serial.println(WiFi.localIP());
        connected = true;
      }
    }
    /*
    if (!connected) {
      //keine Verbindung, wir starten einen Access Point
      //damit wir auf die Konfiguration zugreifen können
      WiFi.mode(WIFI_AP);
      WiFi.softAP(conf.getApName(),"",1);
    }
    */
    return connected;
}

//Verbindung zum Broker überprüfen und falls nötig wiederherstellen
void checkMQTT() {
  const char * thema;
  if (!client.connected() && (millis() > nextConnect)) {
    //Wir haben keine Verbindung, also Wiederherstellung erforderlich
    Serial.print("Versuche MQTT Verbindung zu ");
    Serial.print(broker);
    Serial.print(" mit ID ");
    Serial.println(clientId);
    client.setServer(broker,1883);
    if (client.connect(clientId.c_str())) {
      Serial.println("Verbunden");
      //Nach erfolgreicher Verbindung werden die Themen abonniert
      client.subscribe("#");
      if (client.connected()) {
        nextConnect=0;
      } else {
        Serial.println("Fehler");
        nextConnect = millis()+5000;
      }
    } else {
      Serial.print("Fehlgeschlagen, rc=");
      Serial.print(client.state());
      Serial.println(" Nächster Versuch in 5 s");
      nextConnect = millis()+5000;
    }
  }
}

//HTTP Request bearbeiten und Formular anzeigen
void handleRoot() {
  conf.handleFormRequest(&server);
}


//Empfangene Daten vom MQTT Broker auswerten
void gotData(char* topic, byte* payload, unsigned int length) {
  //Das Thema wird zur Protokollierung ausgegeben
  Serial.println(topic);
  if (length<200) {
    char buffer[200];
    memcpy(&buffer[0], payload, length);
    buffer[length]=0;
    Serial.println(buffer);
    ats.updateTopic(topic,buffer);
  } else {
    Serial.println("MQTT Message too long");
  }

}

bool publish(const char * thema, const char * message) {
  client.publish(thema,message,true);
}

void topicChange(const char * newTopic, const char * oldTopic) {
  //client.unsubscribe(oldTopic);
  client.subscribe(newTopic);
}

void setupArduiTouch() {
  if(!SPIFFS.begin(true)){
      Serial.println("SPIFFS Mount Failed");
  }
  StaticJsonDocument<500> doc;
  File f;
  String data = "";
  if (SPIFFS.exists("/wlansetup.jsn")) {
    Serial.println("Setup gefunden");
    f = SPIFFS.open("/wlansetup.jsn","r");
    if (f) {
      data = f.readString();
      f.close();
      if (data.length() > 0);
      {
        DeserializationError error;
        error = deserializeJson(doc, data);
        if (error ) {
          Serial.println(data);
          Serial.print("JSON Description: ");
          Serial.println(error.c_str());
        } else {
          Serial.print("WLAN Setup: ");
          Serial.println(data);
          if (doc.containsKey("ssid")) strlcpy(ssid, doc["ssid"],30);
          if (doc.containsKey("pwd")) strlcpy(password, doc["pwd"],30);
          if (doc.containsKey("broker")) strlcpy(broker, doc["broker"],128);
          if (doc.containsKey("hostname")) strlcpy(hostname, doc["hostname"],30);
          if (doc.containsKey("ntpserver")) strlcpy(ntpserver, doc["ntpserver"],30);
          if (doc.containsKey("summertime")) summertime = doc["summertime"];
          Serial.printf("Sommerzeit %i \n",summertime);
        }
      }
    }
  }
  //Die JSON Zeichenkette für das Konfigurations-Formular
  //wird gesetzt
  /*
  conf.setDescription(param);
  //Falls vorhanden wird die Konfiguration aus dewm SPIFFS geladen
  conf.readConfig();
  */
  //Als MQTT Client-Id wird die MAC-Adresse verwendet
  clientId=WiFi.macAddress();
  clientId.replace(":","");
  //Die MQTT Verbindungsparameter werden gesetzt
  client.setServer(broker,1883);
  //Callback Funktion wenn Daten empfangen wurden
  client.setCallback(gotData);
  //Die Netzwerk-Verbindung wird hergestellt
  connected = initWiFi();
  uint16_t ofs = (summertime)?3600:0;
  if (connected)     configTime(GMT_OFFSET_SEC, ofs, ntpserver);
  pinMode(TFT_LED,OUTPUT);
  displayLed(true);
  tft.begin();
  tft.fillScreen(ILI9341_BLACK);
  touch.begin();
  tevent.setResolution(tft.width(),tft.height());
  tevent.setDrawMode(false);
  if (SPIFFS.exists("/calibration.jsn")){
    Serial.println("Kalibrierung gefunden");
    f = SPIFFS.open("/calibration.jsn","r");
    if (false) {
      data = f.readString();
      f.close();
      if (data.length() > 0);
      {
        deserializeJson(doc, data);
        Serial.print("calibration: ");
        Serial.println(data);
        xMin = doc["xMin"];
        yMin = doc["yMin"];
        xMax = doc["xMax"];
        yMax = doc["yMax"];
        tevent.calibrate(xMin,yMin,xMax,yMax);
      }
    }
  }
  tevent.registerOnAllEvents(onAllEvents);
  //Konfigurations editor
  tftconf.registerOnSave(onConfSave);
  tftconf.registerOnCancel(onConfCancel);
  //Der Web Server wird gestartet
  server.on("/",handleRoot);
  server.begin(80);
  ats.registerOnPublish(publish);
  ats.registerOnTopicChange(topicChange);
  ats.registerOnExternCommand(externEdit);
  ats.addPage();
  ats.loadAllPages();
  ats.addMenu(subemnu1);
  ats.addMenu(submenu2);
}

void loopArduiTouch() {
  if (connected) {
    //wenn eine Verbindung besteht
    //wird die Verbindung zum Broker getestet
    checkMQTT();
    if (client.connected()) {
      //Auf neue Nachrichten vom Broker prüfen
      client.loop();
    }
  }
  if ((millis()-lt) > 100) {
    tevent.pollTouchScreen();
    ats.timeEvent(connected);
    lt=millis();
  }

  //Der Web Server prüft ob Anfragen vorliegen
  server.handleClient();
}
