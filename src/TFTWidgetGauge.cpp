//#include <TFTWidgets.h>
#include <TFTWidgetGauge.h>
#include <ArduinoJson.h>

String gaugeextraform = "["
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
  "'name':'pointer',"
  "'label':'Zeiger',"
  "'type':'color'"
  "},"
  "{"
  "'name':'bow',"
  "'label':'Skalenbogen',"
  "'type':'color'"
  "},"
  "{"
  "'name':'decimals',"
  "'label':'Kommastellen',"
  "'type':'number'"
  "}"
  "]";

TFTWidgetGauge::TFTWidgetGauge(uint8_t x, uint8_t y, uint8_t w, const char name[], const char properties[], uint16_t yOffset, const GFXfont * font):Widget(properties){
  strlcpy(_name,name,15);
  _font = font;
  _yOffset = yOffset;
  _posx = x; _posy = y; _width = w; _height = w;
  init(properties);
}

TFTWidgetGauge::TFTWidgetGauge(const char properties[], uint16_t yOffset, const GFXfont * font):Widget(properties){
  _font = font;
  _yOffset = yOffset;
  init(properties);
}

void TFTWidgetGauge::init(const char properties[]) {
  _x=_posx * BLOCKSIZE; _y=_posy * BLOCKSIZE + _yOffset;
  if (_width<4) _width = 4;
  if (_width>12) _width = 12;
  _w = _width * BLOCKSIZE;
  switch (_width) {
    case 4: _h = 3*BLOCKSIZE; break;
    case 5: _h = 4*BLOCKSIZE; break;
    case 6: _h = 4*BLOCKSIZE; break;
    case 7: _h = 5*BLOCKSIZE; break;
    case 8: _h = 6*BLOCKSIZE; break;
    case 9: _h = 6*BLOCKSIZE; break;
    case 10: _h = 7*BLOCKSIZE; break;
    case 11: _h = 7*BLOCKSIZE; break;
    case 12: _h = 8*BLOCKSIZE; break;
    default:   _h = _width* BLOCKSIZE; break;
  }

  switch (_width) {
    case 4: _radius = 25; _margin=7; _textwidth=40; break;
    case 5: _radius = 35; _margin=15; _textwidth=50; break;
    case 6: _radius = 38; _margin=15; _textwidth=60; break;
    case 7: _radius = 45; _margin=15; _textwidth=70; break;
    case 8: _radius = 50; _margin=20; _textwidth=80; break;
    case 9: _radius = 55; _margin=20; _textwidth=90; break;
    case 10: _radius = 65; _margin=20; _textwidth=100; break;
    case 11: _radius = 70; _margin=20; _textwidth=100; break;
    case 12: _radius = 80; _margin=20; _textwidth=100; break;
  }
  Serial.print("New Gauge:");
  Serial.println(properties);
  StaticJsonDocument <1500>doc;
  DeserializationError   error = deserializeJson(doc,properties);
  if (error ) {
    Serial.println("JSON new gauge: ");
    Serial.println(properties);
    Serial.println(error.c_str());
  }
  if (strcmp(_label,"")==0) strlcpy(_label,"Gauge",50);
  if (doc.containsKey("pointer")) _pointerColor = doc["pointer"];
  if (doc.containsKey("bow")) _bowColor = doc["bow"];
  if (doc.containsKey("min")) _min = doc["min"];
  if (doc.containsKey("max")) _max = doc["max"];
  if (doc.containsKey("default")) _value = doc["default"];
  if (doc.containsKey("decimals")) _decimals = doc["decimals"];
  if (doc.containsKey("unit")) strlcpy(_unit,doc["unit"],5);
  if (_value > _max) _value = _max;
  if (_value < _min) _value = _min;
  _mid = (_max - _min)/2+_min;
}

void TFTWidgetGauge::draw(Adafruit_ILI9341 * tft) {
  char buf[10];
  drawBase(tft,20);
  uint8_t w = _w/2-_radius-2;
  if (w > 23) {  //4 characters minimum
    tft->setFont(NULL);
    tft->setTextColor(_fontColor);
    sprintf(buf,"%i",_min);
    centerText(tft,buf,_x,_y+_h-_margin-8,w,8,HA::ALIGN_RIGHT,VA::ALIGN_FIX);
    sprintf(buf,"%i",_mid);
    centerText(tft,buf,_x+_w/2-w/2,_y+_h-_margin-_radius*1.1-8,w,8,HA::ALIGN_CENTER,VA::ALIGN_FIX);
    sprintf(buf,"%i",_max);
    centerText(tft,buf,_x+_w-w,_y+_h-_margin-8,w,8,HA::ALIGN_LEFT,VA::ALIGN_FIX);
  }
  drawValue(tft);
}

void TFTWidgetGauge::update(Adafruit_ILI9341 * tft, const char data[], bool display) {
  char buf[200];
  strlcpy(buf,data,200);
  StaticJsonDocument <200>doc;
  DeserializationError   error = deserializeJson(doc,buf);
  if (error ) {
    Serial.println("JSON update gauge: ");
    Serial.println(error.c_str());
  }
  if (doc.containsKey(_unitName)) strlcpy(_unit,doc[_unitName],5);
  if (doc.containsKey(_valName)) _value = doc[_valName];
  if (_value < _min) _value = _min;
  if (_value > _max) _value = _max;
  if (display)  {
    deleteOld(tft);
    drawValue(tft);
  }
}

void TFTWidgetGauge::deleteOld(Adafruit_ILI9341 * tft){
  tft->fillRect(_x+(_w/2)-(_textwidth/2),_y+_h-(2*_margin)-14,_textwidth,14,_bgColor);
  tft->drawLine(_x+(_w/2), _y+_h-_margin,_x+(_w/2)+_xl, _y+_h-_margin+_yl,_bgColor);
}


void TFTWidgetGauge::drawValue(Adafruit_ILI9341 * tft){
  float winkel = (_value-_mid)*M_PI/(_max-_min);
  _xl=sin(winkel)*_radius*1.1;
  _yl=-cos(winkel)*_radius*1.1;
  tft->startWrite();
  tft->drawCircleHelper(_x+(_w/2),_y+_h-_margin, _radius,1,_bowColor);
  tft->drawCircleHelper(_x+(_w/2),_y+_h- _margin, _radius-1,2,_bowColor);
  tft->endWrite();
  tft->drawCircle(_x+(_w/2), _y+_h-_margin,2, _bowColor);
  tft->drawLine(_x+(_w/2), _y+_h-_margin,_x+(_w/2)+_xl, _y+_h-_margin+_yl,_pointerColor);
  tft->setFont(_font);
  tft->setTextColor(_fontColor);
  centerValue(tft,_value,_x+(_w/2)-(_textwidth/2),_y+_h-(2*_margin)-14,_textwidth,14,_decimals,_unit);

}
String TFTWidgetGauge::getProperties() {
  char buffer[1000];
  StaticJsonDocument<1000> doc;
  DeserializationError   error = deserializeJson(doc,getBaseProperties());
  if (error ) {
    Serial.println("JSON set gauge properties: ");
    Serial.println(error.c_str());
  }
  doc["type"] = WT_GAUGE;
  doc["pointer"] = _pointerColor;
  doc["bow"] = _bowColor;
  doc["min"] = _min;
  doc["max"] = _max;
  doc["decimals"] = _decimals;
  doc["unit"] = _unit;
  serializeJson(doc,buffer);
  return String(buffer);
}

void TFTWidgetGauge::setProperties(String properties){
  setBaseProperties(properties);
  init(properties.c_str());
}

String TFTWidgetGauge::getExtraEditForm() {
  return gaugeextraform;
}
