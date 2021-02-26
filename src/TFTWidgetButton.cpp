#include <TFTWidgetButton.h>
#include <ArduinoJson.h>

String buttonextraform = "["
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
  "'name':'btnbrdCol',"
  "'label':'Rand des Knopfes',"
  "'type':'color'"
  "},"
  "{"
  "'name':'isSwitch',"
  "'label':'Schalter',"
  "'type':'check'"
  "}"
  "]";

TFTWidgetButton::TFTWidgetButton(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const char name[], const char properties[], uint16_t yOffset, const GFXfont * font):Widget(properties){
  strlcpy(_name,name,15);
  _yOffset = yOffset;
  _font = font;
  _posx = x; _posy = y; _width = w; _height = h;
  init(properties);
}

TFTWidgetButton::TFTWidgetButton(const char properties[], uint16_t yOffset, const GFXfont * font):Widget(properties) {
  _font = font;
  _yOffset = yOffset;
  init(properties);
}

void TFTWidgetButton::init(const char properties[]) {
  if (_width<2) _width=4;
  if (_height>2) _height=3;
  if (_height<2) _height=2;
  _w = _width*BLOCKSIZE;
  _h = _height*BLOCKSIZE;
  _x = _posx*BLOCKSIZE;
  _y = _posy * BLOCKSIZE + _yOffset;
  Serial.print("New Button:");
  Serial.println(properties);
  StaticJsonDocument <1500>doc;
  DeserializationError   error = deserializeJson(doc,properties);
  if (error ) {
    Serial.println("JSON new button: ");
    Serial.println(properties);
    Serial.println(error.c_str());
  }
  if (strcmp(_label,"")==0) strlcpy(_label,"Button",50);
  if (doc.containsKey("onColor")) _onColor = doc["onColor"];
  if (doc.containsKey("offColor")) _offColor = doc["offColor"];
  if (doc.containsKey("default")) _value = doc["default"];
  if (doc.containsKey("btnbrdCol")) _brdColor = doc["btnbrdCol"];
  if (doc.containsKey("isSwitch")) _switch = doc["isSwitch"];
  uint16_t posx = doc["xposition"];
  Serial.println(posx);
  _status = ST_NEW;
  _bh=26;
  _bw=_w-20;
}

void TFTWidgetButton::draw(Adafruit_ILI9341 * tft) {
  drawBase(tft,20,HA::ALIGN_CENTER,false);
  drawValue(tft);
}

bool TFTWidgetButton::hasTopic(char topic[]) {
  return (strcmp(topic,getTopic()) == 0);
}

void TFTWidgetButton::update(Adafruit_ILI9341 * tft, const char data[], bool display) {
  char buf[200];
  strlcpy(buf,data,200);
  StaticJsonDocument <200>doc;
  DeserializationError   error = deserializeJson(doc,buf);
  if (error ) {
    Serial.println("JSON update button: ");
    Serial.println(error.c_str());
  }
  if (doc.containsKey(_valName)) _value = (strcmp(doc[_valName],"ON")==0)?1:0;
  if (display) drawValue(tft);
}


void TFTWidgetButton::drawValue(Adafruit_ILI9341 * tft){
  if (_value == 0){
    tft->fillRoundRect(_x+10,_y+(_h-_bh)/2,_bw,_bh,_bh/2,_offColor);
  } else {
    tft->fillRoundRect(_x+10,_y+(_h-_bh)/2,_bw,_bh,_bh/2,_onColor);
  }
  tft->setFont(_font);
  tft->setTextColor(_fontColor);
  tft->drawRoundRect(_x+10,_y+(_h-_bh)/2,_bw,_bh,_bh/2,_brdColor);
  centerText(tft,_label,_x+12,_y+(_h-_bh)/2,_bw-4,_bh,HA::ALIGN_CENTER,VA::ALIGN_MIDDLE);
}


bool TFTWidgetButton::touchEvent(Adafruit_ILI9341 * tft, int16_t x, int16_t y, EV event) {
  if (isInside(x,y)) {
    Serial.printf("Pos. x=%i y=%i event = %i\n",x,y,event);
    if ((_switch==1)&&(event == EV::EVT_CLICK)) {
      uint8_t v = 0;
      _value = (_value == 0)?1:0;
      _status = ST_CHANGED;
      drawValue(tft);
    }
    if (_switch==0) {
      if ((_value == 0) && (event==EV::EVT_DOWN)) {
        _value = 1;
        _status = ST_CHANGED;
        drawValue(tft);
      }
      if ((_value == 1) && (event==EV::EVT_UP)) {
        _value = 0;
        _status = ST_CHANGED;
        drawValue(tft);
      }
    }
    return true;
  } else {
    return false;
  }
};

int16_t TFTWidgetButton::getValue() {
  return _value;
}
void TFTWidgetButton::setStatus(uint8_t status) {
  _status = status;
}

bool TFTWidgetButton::toPublish() {
  return ((_status == ST_CHANGED) ) ;
}

String TFTWidgetButton::getProperties() {
  char buffer[1000];
  StaticJsonDocument<1000> doc;
  DeserializationError   error = deserializeJson(doc,getBaseProperties());
  if (error ) {
    Serial.println("JSON set button properties: ");
    Serial.println(error.c_str());
  }
  doc["type"] = WT_BUTTON;
  doc["onColor"] = _onColor;
  doc["offColor"] = _offColor;
  doc["btnbrdCol"] = _brdColor;
  doc["isSwitch"] = _switch;
  serializeJson(doc,buffer);
  return String(buffer);
}

void TFTWidgetButton::setProperties(String properties){
  setBaseProperties(properties);
  init(properties.c_str());
}

String TFTWidgetButton::getExtraEditForm() {
  return buttonextraform;
}

const char * TFTWidgetButton::getMessage() {
  if (_status == ST_NEW) {
    return"";
  }
  if (_value == 0) {
    return "OFF";
  } else {
    return "ON";
  }
}
