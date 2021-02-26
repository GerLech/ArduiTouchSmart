#include <TFTWidgetColorbar.h>
#include <ArduinoJson.h>

String colorbarextraform = "["
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
  "'name':'scaleCol',"
  "'label':'Farbe Skala',"
  "'type':'color'"
  "},"
  "{"
  "'name':'scaleTics',"
  "'label':'Skalenteile',"
  "'type':'number'"
  "},"
  "{"
  "'name':'barWidth',"
  "'label':'Balkenbreite',"
  "'type':'number'"
  "},"
  "{"
  "'name':'decimals',"
  "'label':'Kommastellen',"
  "'type':'number'"
  "}"
  "]";

TFTWidgetColorbar::TFTWidgetColorbar(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const char name[], const char properties[], uint16_t yOffset, const GFXfont * font):Widget(properties){
  strlcpy(_name,name,15);
  _font = font;
  _yOffset = yOffset;
  _posx = x; _posy = y; _width = w; _height = h;
  init(properties);
}

TFTWidgetColorbar::TFTWidgetColorbar(const char properties[], uint16_t yOffset, const GFXfont * font):Widget(properties){
  _font = font;
  _yOffset = yOffset;
  init(properties);
}

void TFTWidgetColorbar::init(const char properties[]){
  if (_width<3) _width=3;
  if (_height<5) _height=5;
  _w = _width*BLOCKSIZE;
  _h = _height*BLOCKSIZE;
  _x = _posx*BLOCKSIZE;
  _y = _posy * BLOCKSIZE + _yOffset;
  Serial.print("New Colorbar:");
  Serial.println(properties);
  StaticJsonDocument <1500>doc;
  DeserializationError   error = deserializeJson(doc,properties);
  if (error ) {
    Serial.println("JSON new Colorbar: ");
    Serial.println(properties);
    Serial.println(error.c_str());
  }
  if (strcmp(_label,"")==0) strlcpy(_label,"Colorbar",50);
  if (doc.containsKey("minCol")) _minColor = doc["minCol"];
  if (doc.containsKey("maxCol")) _maxColor = doc["maxCol"];
  if (doc.containsKey("min")) _min = doc["min"];
  if (doc.containsKey("max")) _max = doc["max"];
  if (doc.containsKey("default")) _value = doc["default"];
  if (doc.containsKey("decimals")) _decimals = doc["decimals"];
  if (doc.containsKey("barWidth")) _barwidth = doc["barWidth"];
  if (doc.containsKey("scaleCol")) _scColor = doc["scaleCol"];
  if (doc.containsKey("scaleTics")) _scaleTics = doc["scaleTics"];
  if (doc.containsKey("unit")) strlcpy(_unit,doc["unit"],5);
  if (_value > _max) _value = _max;
  if (_value < _min) _value = _min;
  if (_barwidth > (_w/4-2) ) _barwidth = (_w/4-2);

  _rmin = ((_minColor >> 11) & 0x1F);
  _gmin = ((_minColor >> 5) & 0x3F);
  _bmin = (_minColor & 0x1F);
  _rmax = ((_maxColor >> 11) & 0x1F);
  _gmax = ((_maxColor >> 5) & 0x3F);
  _bmax = (_maxColor & 0x1F);
  _bx = _x+_w/4-_barwidth;
  _by = _y+30;
  _bh = _h-65;
  while  ((_scaleTics > 1) && (_bh/_scaleTics < 20)) _scaleTics = _scaleTics/2;
  if (_scaleTics == 0) _scaleTics = 1;
}

void TFTWidgetColorbar::draw(Adafruit_ILI9341 * tft) {
  if (_scaleTics < 1) _scaleTics = 1;
  float dv = (_max-_min)/_scaleTics;
  int16_t xt = _bx+_barwidth+9;
  int16_t yt;
  drawBase(tft,20);
  tft->drawRect(_bx-1,_by-1,_barwidth+2,_bh+2,_scColor);

  tft->setFont(NULL);
  tft->setTextColor(_scColor);
  for (uint8_t i = 0; i<=_scaleTics; i++){
    yt = _by+_bh-i*_bh/_scaleTics;
    tft->drawLine(xt-8,yt,xt-3,yt,_scColor);
    tft->setCursor(xt,yt-4);
    tft->print(_min+i*dv);
  }
  drawValue(tft);
}

void TFTWidgetColorbar::update(Adafruit_ILI9341 * tft, const char data[], bool display) {
  char buf[200];
  strlcpy(buf,data,200);
  StaticJsonDocument <200>doc;
  DeserializationError   error = deserializeJson(doc,buf);
  if (error ) {
    Serial.println("JSON: ");
    Serial.println(buf);
    Serial.println(error.c_str());
  }
  //deserializeJson(doc, data);
  Serial.println(_valName);
  if (doc.containsKey(_unitName)) strlcpy(_unit,doc[_unitName],5);
  if (doc.containsKey(_valName)) _value = doc[_valName];
  if (_value < _min) _value = _min;
  if (_value > _max) _value = _max;
  if (display) {
   deleteOld(tft);
   drawValue(tft);
 }
}

void TFTWidgetColorbar::deleteOld(Adafruit_ILI9341 * tft){
  tft->fillRect(_bx,_by,_barwidth,_bh,_bgColor);
  tft->fillRect(_x+1,_y+_h-21,_w-2,20,_bgColor);
}


void TFTWidgetColorbar::drawValue(Adafruit_ILI9341 * tft){
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
  tft->fillRect(_bx,_by+_bh-v*_bh/100,_barwidth,v*_bh/100,c);
  tft->setFont(_font);
  tft->setTextColor(_fontColor);
  centerValue(tft,_value,_x+1,_y+_h-21,_w-2,20,_decimals,_unit);

}
String TFTWidgetColorbar::getProperties() {
  char buffer[1000];
  StaticJsonDocument<1000> doc;
  DeserializationError   error = deserializeJson(doc,getBaseProperties());
  if (error ) {
    Serial.println("JSON set colorbar properties: ");
    Serial.println(error.c_str());
  }
  doc["type"] = WT_COLORBAR;
  doc["minCol"] = _minColor;
  doc["maxCol"] = _maxColor;
  doc["min"] = _min;
  doc["max"] = _max;
  doc["decimals"] = _decimals;
  doc["barWidth"] = _barwidth;
  doc["scaleCol"] = _scColor;
  doc["scaleTics"] = _scaleTics;
  doc["unit"] = _unit;
  serializeJson(doc,buffer);
  return String(buffer);
}

void TFTWidgetColorbar::setProperties(String properties){
  setBaseProperties(properties);
  init(properties.c_str());
}

String TFTWidgetColorbar::getExtraEditForm() {
  return colorbarextraform;
}
