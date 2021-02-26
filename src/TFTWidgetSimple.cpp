#include <TFTWidgetSimple.h>
#include <ArduinoJson.h>

String simpleextraform = "["
  "{"
  "'name':'unit',"
  "'label':'Einheit',"
  "'type':'text',"
  "'default':''"
  "},"
  "{"
  "'name':'min',"
  "'label':'Minimalwert',"
  "'type':'float'"
  "},"
  "{"
  "'name':'max',"
  "'label':'Maximalwert',"
  "'type':'float'"
  "},"
  "{"
  "'name':'minCol',"
  "'label':'Minimalwert',"
  "'type':'color'"
  "},"
  "{"
  "'name':'maxCol',"
  "'label':'Maximalwert',"
  "'type':'color'"
  "},"
  "{"
  "'name':'decimals',"
  "'label':'Kommastellen',"
  "'type':'number'"
  "}"
  "]";

TFTWidgetSimple::TFTWidgetSimple(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const char name[], const char properties[], uint16_t yOffset, const GFXfont * font):Widget(properties){
  strlcpy(_name,name,15);
  _font = font;
  _yOffset = yOffset;
  _posx = x; _posy = y; _width = w; _height = h;
  init(properties);
}

TFTWidgetSimple::TFTWidgetSimple(const char properties[], uint16_t yOffset, const GFXfont * font):Widget(properties){
  _font = font;
  _yOffset = yOffset;
  init(properties);
}

void TFTWidgetSimple::init(const char properties[]){
  _x=_posx * BLOCKSIZE; _y=_posy * BLOCKSIZE + _yOffset;
  if (_width<4) _width=4; if (_width>12) _width=12;
  if (_height<3) _height=3; if (_height>5) _height=5;
  _w = _width*BLOCKSIZE; _h = _height*BLOCKSIZE;
  Serial.print("New Simple:");
  Serial.println(properties);
  StaticJsonDocument <1500>doc;
  DeserializationError   error = deserializeJson(doc,properties);
  if (error ) {
    Serial.println("JSON new simple: ");
    Serial.println(properties);
    Serial.println(error.c_str());
  }
  if (strcmp(_label,"")==0) strlcpy(_label,"Simple",50);
  if (doc.containsKey("minCol")) _minColor = doc["minCol"];
  if (doc.containsKey("maxCol")) _maxColor = doc["maxCol"];
  if (doc.containsKey("min")) _min = doc["min"];
  if (doc.containsKey("max")) _max = doc["max"];
  if (doc.containsKey("default")) _value = doc["default"];
  if (doc.containsKey("decimals")) _decimals = doc["decimals"];
  if (doc.containsKey("unit")) strlcpy(_unit,doc["unit"],5);

  _rmin = ((_minColor >> 11) & 0x1F);
  _gmin = ((_minColor >> 5) & 0x3F);
  _bmin = (_minColor & 0x1F);
  _rmax = ((_maxColor >> 11) & 0x1F);
  _gmax = ((_maxColor >> 5) & 0x3F);
  _bmax = (_maxColor & 0x1F);
}

void TFTWidgetSimple::draw(Adafruit_ILI9341 * tft) {
  drawBase(tft,20);
  drawValue(tft);
}

void TFTWidgetSimple::update(Adafruit_ILI9341 * tft, const char data[], bool display) {
  char buf[200];
  strlcpy(buf,data,200);
  StaticJsonDocument <200>doc;
  DeserializationError   error = deserializeJson(doc,buf);
  if (error ) {
    Serial.println("JSON update simple: ");
    Serial.println(error.c_str());
  }
  if (doc.containsKey(_unitName)) strlcpy(_unit,doc[_unitName],5);
  if (doc.containsKey(_valName)) _value = doc[_valName];
  if (display) {
    deleteOld(tft);
    drawValue(tft);
  }
}

void TFTWidgetSimple::deleteOld(Adafruit_ILI9341 * tft){
  tft->fillRect(_x+1,_y+21,_w-2,_h-22,_bgColor);
}


void TFTWidgetSimple::drawValue(Adafruit_ILI9341 * tft){
  uint8_t r,g,b;
  uint16_t c;
  uint16_t v = (_value-_min)*100/(_max-_min); //value between 0 and 1
  if (_rmin < _rmax) {
    r=(v * (_rmax - _rmin)/100 + _rmin);
  } else {
    r=(_rmin - v *(_rmin-_rmax)/100);
  }
  if (_gmin < _gmax) {
    g=(v * (_gmax - _gmin)/100 + _gmin);
  } else {
    g=(_gmin - v *(_gmin-_gmax)/100);
  }
  if (_bmin < _bmax) {
    b=(v * (_bmax - _bmin)/100 + _bmin);
  } else {
    b=(_bmin - v *(_bmin-_bmax)/100);
  }
  c = ((r & 0x1F) << 11) + ((g & 0x3F) <<5) + (b & 0x1F);
  if (_h > BLOCKSIZE) {
    tft->setFont(&AT_Bold12pt7b);
  } else {
    tft->setFont(_font);
  }
  tft->setTextColor(c);
  centerValue(tft,_value,_x+1,_y+21,_w-2,_h-22,_decimals,_unit);

}

String TFTWidgetSimple::getProperties() {
  char buffer[1000];
  StaticJsonDocument<1000> doc;
  DeserializationError   error = deserializeJson(doc,getBaseProperties());
  if (error ) {
    Serial.println("JSON set simple properties: ");
    Serial.println(error.c_str());
  }
  doc["type"] = WT_SIMPLE;
  doc["minCol"] = _minColor;
  doc["maxCol"] = _maxColor;
  doc["min"] = _min;
  doc["max"] = _max;
  doc["decimals"] = _decimals;
  doc["unit"] = _unit;
  serializeJson(doc,buffer);
  return String(buffer);
}

void TFTWidgetSimple::setProperties(String properties){
  setBaseProperties(properties);
  init(properties.c_str());
}

String TFTWidgetSimple::getExtraEditForm() {
  return simpleextraform;
}
