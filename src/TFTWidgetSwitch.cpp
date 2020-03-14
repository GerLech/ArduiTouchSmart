#include <TFTWidgetSwitch.h>
#include <ArduinoJson.h>

String switchextraform = "["
  "{"
  "'name':'onColor',"
  "'label':'Farbe für EIN',"
  "'type':'color'"
  "},"
  "{"
  "'name':'offColor',"
  "'label':'Farbe für AUS',"
  "'type':'color'"
  "},"
  "{"
  "'name':'switchCol',"
  "'label':'Farbe Schalter',"
  "'type':'color'"
  "},"
  "{"
  "'name':'borderCol',"
  "'label':'Rand Schalter',"
  "'type':'color'"
  "},"
  "{"
  "'name':'onLabel',"
  "'label':'Text für EIN',"
  "'type':'text'"
  "},"
  "{"
  "'name':'offLabel',"
  "'label':'Text für AUS',"
  "'type':'text'"
  "}"
  "]";

TFTWidgetSwitch::TFTWidgetSwitch(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const char name[], const char properties[], uint16_t yOffset, const GFXfont * font):Widget(properties){
  strlcpy(_name,name,15);
  _font = font;
  _yOffset = yOffset;
  _posx = x; _posy = y; _width =w; _height = h;
  init(properties);
}

TFTWidgetSwitch::TFTWidgetSwitch(const char properties[], uint16_t yOffset, const GFXfont * font):Widget(properties){
  _font = font;
  _yOffset = yOffset;
  init(properties);
}

void TFTWidgetSwitch::init(const char properties[]){
  _x=_posx * BLOCKSIZE; _y=_posy * BLOCKSIZE + _yOffset;
  if (_width<4) _width=4;
  if (_height>4) _height=4;
  if (_height<3) _height=3;
  _w = _width*BLOCKSIZE; _h = _height*BLOCKSIZE;
  StaticJsonDocument <1500>doc;
  Serial.println(properties);
  deserializeJson(doc, properties);
  if (strcmp(_label,"")==0) strlcpy(_label,"Switch",50);
  if (doc.containsKey("onColor")) _onColor = doc["onColor"];
  if (doc.containsKey("offColor")) _offColor = doc["offColor"];
  if (doc.containsKey("default")) _value = doc["default"];
  if (doc.containsKey("switchCol")) _swColor = doc["switchCol"];
  if (doc.containsKey("borderCol")) _swbrdColor = doc["borderCol"];
  if (doc.containsKey("onLabel")) strlcpy(_onLabel,doc["onLabel"],5);
  if (doc.containsKey("offLabel")) strlcpy(_offLabel,doc["offLabel"],5);
  _status = ST_NEW;
  _dx=(_w-38)/2;
  _dy=(_h-22)/2+21;
}

void TFTWidgetSwitch::draw(Adafruit_ILI9341 * tft) {
  drawBase(tft,20,HA::ALIGN_CENTER);
  if (_dx>20) {
    tft->setFont(_font);
    tft->setTextColor(_offColor);
    centerText(tft,_offLabel,_x+1,_y+21,_dx-3,_h-22,HA::ALIGN_RIGHT,VA::ALIGN_MIDDLE);
    tft->setTextColor(_onColor);
    centerText(tft,_onLabel,_x+_w-_dx+3,_y+21,_dx-3,_h-22,HA::ALIGN_LEFT,VA::ALIGN_MIDDLE);
    tft->fillRoundRect(_x+_dx,_y+_dy-9,36,18,9,_swColor);
    tft->drawRoundRect(_x+_dx,_y+_dy-9,36,18,9,_swbrdColor);
  }
  drawValue(tft);
}

bool TFTWidgetSwitch::hasTopic(char topic[]) {
  return (strcmp(topic,getTopic()) == 0);
}


void TFTWidgetSwitch::update(Adafruit_ILI9341 * tft, const char data[], bool display) {
  char buf[200];
  strlcpy(buf,data,200);
  StaticJsonDocument <200>doc;
  deserializeJson(doc, buf);
  if (doc.containsKey(_valName)) _value = (strcmp(doc[_valName],"ON")==0)?1:0;
  if (display) {
    deleteOld(tft);
    drawValue(tft);
  }
}

void TFTWidgetSwitch::deleteOld(Adafruit_ILI9341 * tft){
  tft->fillRoundRect(_x+1+_dx,_y+_dy-8,34,16,8,_swColor);
}


void TFTWidgetSwitch::drawValue(Adafruit_ILI9341 * tft){
  if (_value == 0){
    tft->fillCircle(_x+_dx+9,_y+_dy,8,_offColor);
    tft->drawCircle(_x+_dx+9,_y+_dy,8,_swColor);
  } else {
    tft->fillCircle(_x+_dx+27,_y+_dy,8,_onColor);
    tft->drawCircle(_x+_dx+27,_y+_dy,8,_swColor);
  }
  tft->drawRoundRect(_x+_dx,_y+_dy-9,36,18,9,_swbrdColor);
}


bool TFTWidgetSwitch::touchEvent(Adafruit_ILI9341 * tft, int16_t x, int16_t y, EV event) {
  if (isInside(x,y)) {
    Serial.printf("Pos. x=%i y=%i event = %i\n",x,y,event);
    if (event == EV::EVT_CLICK) {
      uint8_t v = 0;
      v=(x<(_x+_w/2))?0:1;
      if (v != _value) {
        _value = v;
        _status = ST_CHANGED;
        Serial.printf("Event x=%i y=%i value= %i\n",x,y,_value);
        deleteOld(tft);
        drawValue(tft);
      }
    }
    return true;
  } else {
    return false;
  }
};

int16_t TFTWidgetSwitch::getValue() {
  return _value;
}
void TFTWidgetSwitch::setStatus(uint8_t status) {
  _status = status;
}

bool TFTWidgetSwitch::toPublish() {
  return ((_status == ST_CHANGED) || (_status == ST_NEW)) ;
}

String TFTWidgetSwitch::getProperties() {
  char buffer[1000];
  StaticJsonDocument<1000> doc;
  deserializeJson(doc,getBaseProperties());
  doc["type"] = WT_SWITCH;
  doc["onColor"] = _onColor;
  doc["offColor"] = _offColor;
  doc["onLabel"] = _onLabel;
  doc["offLabel"] = _offLabel;
  doc["switchCol"] = _swColor;
  doc["borderCol"] = _swbrdColor;
  serializeJson(doc,buffer);
  return String(buffer);
}

void TFTWidgetSwitch::setProperties(String properties){
  setBaseProperties(properties);
  init(properties.c_str());
}

String TFTWidgetSwitch::getExtraEditForm() {
  return switchextraform;
}

const char * TFTWidgetSwitch::getMessage() {
 if (_status == ST_NEW) return "";
 if (_value == 0) {
   return "OFF";
 } else {
   return "ON";
 }
}
