#include <TFTWidgetChart.h>
#include <ArduinoJson.h>

String chartextraform = "["
"{"
"'name':'chartType',"
"'label':'Darstellung',"
"'type':'select',"
"'default':'0',"
"'options':["
"{'v':'0','l':'Punkte'},"
"{'v':'1','l':'Linie'},"
"{'v':'2','l':'Balken'}"
"]"
"},"
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
"'name':'minGw',"
"'label':'Grenzwert unten',"
"'type':'float'"
"},"
"{"
"'name':'maxGw',"
"'label':'Grenzwert oben',"
"'type':'float'"
"},"
"{"
"'name':'showLim',"
"'label':'Grenzwerte',"
"'type':'check'"
"},"
"{"
"'name':'timerange',"
"'label':'Zeitbereich',"
"'type':'number'"
"},"
"{"
"'name':'xRes',"
"'label':'Auflösung-X',"
"'type':'float'"
"},"
"{"
"'name':'yRes',"
"'label':'Auflösung-Y',"
"'type':'float'"
"},"
"{"
"'name':'nullCol',"
"'label':'Nullinie',"
"'type':'color'"
"},"
"{"
"'name':'gridCol',"
"'label':'Achsen',"
"'type':'color'"
"},"
"{"
"'name':'valCol',"
"'label':'Kurve',"
"'type':'color'"
"},"
"{"
"'name':'gwCol',"
"'label':'Grenzwerte',"
"'type':'color'"
"}"
"]";


TFTWidgetChart::TFTWidgetChart(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const char name[], const char properties[], uint16_t yOffset, const GFXfont * font):Widget(properties){
  strlcpy(_name,name,15);
  _font = font;
  _yOffset = yOffset;
  _posx = x; _posy = y; _width = w; _height = h;
  _cnt = 0;
  init(properties);
}

TFTWidgetChart::TFTWidgetChart(const char properties[], uint16_t yOffset, const GFXfont * font):Widget(properties){
  _font = font;
  _yOffset = yOffset;
  _cnt = 0;
  init(properties);
}

void TFTWidgetChart::init(const char properties[]){
  _x=_posx * BLOCKSIZE; _y=_posy * BLOCKSIZE + _yOffset;
  if (_width<8) _width=8; if (_width>12) _width=12;
  if (_height<8) _height=8; if (_height>14) _height=14;
  _w = _width*BLOCKSIZE; _h = _height*BLOCKSIZE;
  Serial.print("New Chart:");
  Serial.println(properties);
  StaticJsonDocument <1500>doc;
  DeserializationError   error = deserializeJson(doc,properties);
  if (error ) {
    Serial.println("JSON new chart: ");
    Serial.println(properties);
    Serial.println(error.c_str());
  }
  if (strcmp(_label,"")==0) strlcpy(_label,"Chart",50);
  if (doc.containsKey("unit")) strlcpy(_unit,doc["unit"],5);
  if (doc.containsKey("chartType")) _chartType = doc["chartType"];
  if (doc.containsKey("min")) _min = doc["min"];
  if (doc.containsKey("max")) _max = doc["max"];
  if (doc.containsKey("minGw")) _minGw = doc["minGw"];
  if (doc.containsKey("maxGw")) _maxGw = doc["maxGw"];
  if (doc.containsKey("showLim")) _showLim = doc["showLim"];
  if (doc.containsKey("timerange")) _timerange = doc["timerange"];
  if (doc.containsKey("xRes")) _xRes = doc["xRes"];
  if (doc.containsKey("yRes")) _yRes = doc["yRes"];
  if (doc.containsKey("nullCol")) _nullCol = doc["nullCol"];
  if (doc.containsKey("gridCol")) _gridCol = doc["gridCol"];
  if (doc.containsKey("gwCol")) _gwCol = doc["gwCol"];
  if (doc.containsKey("valCol")) _valCol = doc["valCol"];
  if (_value > _max) _value = _max;
  if (_value < _min) _value = _min;
  _xmax = _x + _w -10;
  _xmin = _x + 30;
  _ymax = _y + _h - 20;
  _ymin = _y + 24;
  _yScale = (_h - 40)/(_max - _min);
  _x0 = _x+30;
  _y0 = _y+24 + _max * _yScale;
  _mw = 4;
  _maxCnt = (_w - 40)/_mw;
  if (_maxCnt < 44) {
    _mw = 3;
    _maxCnt = (_w - 40)/_mw;
  }
  _xScale = 1.0 * _maxCnt * _mw / _timerange;
  _intervall = _timerange * 60 / _maxCnt;
  _seconds = 0;
  _average = 0;
  _cntMw = 0;
}

void TFTWidgetChart::draw(Adafruit_ILI9341 * tft) {
  Serial.println("Start draw");
  drawBase(tft,20,HA::ALIGN_CENTER,false);
  tft->setFont(NULL);
  tft->setTextColor(_fontColor);
  tft->setCursor(_x+32,_y + 8);
  tft->print(unicodeToDisplay(_label,true));
  drawCurve(tft);
  drawValue(tft);
}


void TFTWidgetChart::drawGrid(Adafruit_ILI9341 * tft) {
  char buf[10];
  uint8_t len;
  uint8_t h,m;
  uint16_t t;
  int16_t y1 = _y + 24;
  int16_t x1 = _x0;
  int16_t e;
  tft->setFont(NULL);
  tft->setTextColor(_gridCol);
  //y grid
  float tmp = _max;
  while (tmp >= _min) {
    tft->drawLine(_x0-2,y1,_xmax,y1,_gridCol);
    if (_yRes<0.1) {
      sprintf(buf,"%1.2f",tmp);
    } else if (_yRes<1) {
      sprintf(buf,"%1.1f",tmp);
    } else {
      sprintf(buf,"%1.0f",tmp);
    }
    len = strlen(buf);
    tft->setCursor(_x+28 - len * 6,y1-4);
    tft->print(buf);
    y1 += _yRes * _yScale;
    tmp -= _yRes;
  }
  //unit
  tft->setCursor(_x+2,_y + 8);
  tft->print(unicodeToDisplay(_unit,true));
  //zero line
  if ((_y0 >= (_ymin-10)) && (_y0 <= (_ymax+10))) tft->drawLine(_x0-2,_y0,_x0+_w - 40,_y0,_nullCol);
  //x grid
  tmp = 0; e=0;
  while (tmp <= (_timerange + 1)) {
    tft->drawLine(x1,_ymin,x1,_ymax+2,_gridCol);
    t = tmp;
    h = t/60;
    m = t % 60;
    sprintf(buf,"%i:%02i",h,m);
    len = strlen(buf);
    if ((x1 - len * 3) > e) {
      tft->setCursor(x1 - len * 3,_ymax+6);
      tft->print(buf);
      e = x1+len*3;
    }
    x1 += _xRes * _xScale;
    tmp += _xRes;
  }
  if (_showLim == 1) {
    y1 = _y0 - _maxGw * _yScale;
    if ((y1 >= (_ymin-10)) && (y1 <= (_ymax+10))) tft->drawLine(_x0-2,y1,_x0+_w - 40,y1,_gwCol);
    y1 = _y0 - _minGw * _yScale;
    if ((y1 >= (_ymin-10)) && (y1 <= (_ymax+10))) tft->drawLine(_x0-2,y1,_x0+_w - 40,y1,_gwCol);
  }
}

void TFTWidgetChart::update(Adafruit_ILI9341 * tft, const char data[], bool display) {
  char buf[200];
  strlcpy(buf,data,200);
  StaticJsonDocument <200>doc;
  DeserializationError   error = deserializeJson(doc,buf);
  if (error ) {
    Serial.println("JSON update chart: ");
    Serial.println(error.c_str());
  }
  if (doc.containsKey(_valName)) {
    _value = doc[_valName];
    _average += _value;
    _cntMw++;
    if (display) drawValue(tft);
  }
}

void TFTWidgetChart::deleteOld(Adafruit_ILI9341 * tft){
  tft->fillRect(_x+3,_y+21,_w-6,_h-24,_bgColor);
}


void TFTWidgetChart::drawValue(Adafruit_ILI9341 * tft){
  char buf[20];
  sprintf(buf,"%1.1f %s",_value,_unit);
  String s = unicodeToDisplay(buf,true);
  uint8_t l = s.length();
  tft->fillRect(_x+_w-60,_y+3,57,17,_bgColor);
  tft->setFont(NULL);
  tft->setTextColor(_gridCol);
  tft->setCursor(_x+_w-4-l*6,_y+8);
  tft->print(s);
}

void TFTWidgetChart::drawCurve(Adafruit_ILI9341 * tft) {
  int16_t x1,x2,y1,y2,ya,yh;
  deleteOld(tft);
  drawGrid(tft);
  x1 = _x0;
  y1 = _y0+_values[0];
  if (y1 < _ymin) y1 = _ymin;
  if (y1 > _ymax) y1 = _ymax;
  for (uint8_t i = 1; i<_cnt; i++){
    x2 = _x0 + i * _mw;
    y2 = _y0 + _values[i];
    if (y2 < _ymin) y2 = _ymin;
    if (y2 > _ymax) y2 = _ymax;
    switch (_chartType) {
      case CTPOINT: tft->fillCircle(x2,y2,1,_valCol); break;
      case CTLINE: tft->drawLine(x1,y1,x2,y2,_valCol); break;
      case CTBAR: if (y2 >= _y0) {
        yh = y2 - _y0; ya = _y0;
        if (_y0 < _ymin) { ya = _ymin; yh -= (_ymin - _y0);}
        if ((ya + yh) > _ymax) yh = _ymax - ya;
        if ((_y0  <= _ymax) && (yh >= 0)) tft->fillRect(x1,ya,_mw,yh, _valCol);
      } else {
        yh = _y0 - y2; ya = y2;
        if (_y0 > _ymax) {yh -= (_y0 - _ymax); ya = _ymax - yh;}
        if (ya < _ymin) yh = ya - _ymin;
        if ((_y0  >= _ymin) && (yh >= 0)) tft->fillRect(x1,ya,_mw,yh, _valCol);
      }
      break;
    }
    x1 = x2; y1 = y2;
  }
}

String TFTWidgetChart::getProperties() {
  char buffer[2000];
  StaticJsonDocument<2000> doc;
  DeserializationError   error = deserializeJson(doc,getBaseProperties());
  if (error ) {
    Serial.println("JSON set chart properties: ");
    Serial.println(error.c_str());
  }
  doc["type"] = WT_CHART;
  doc["unit"] = _unit;
  doc["chartType"] = String(_chartType);
  doc["min"] = _min;
  doc["max"] = _max;
  doc["minGw"] = _minGw;
  doc["maxGw"] = _maxGw;
  doc["timerange"] = _timerange;
  doc["xRes"] = _xRes;
  doc["yRes"] = _yRes;
  doc["nullCol"] = _nullCol;
  doc["gridCol"] = _gridCol;
  doc["valCol"] = _valCol;
  doc["gwCol"] = _gwCol;
  doc["showLim"] = _showLim;
  serializeJson(doc,buffer);
  //Serial.println(String(buffer));
  return String(buffer);
}

void TFTWidgetChart::setProperties(String properties){
  setBaseProperties(properties);
  init(properties.c_str());
}

String TFTWidgetChart::getExtraEditForm() {
  return chartextraform;
}

void TFTWidgetChart::everySecond(Adafruit_ILI9341 * tft, boolean display){
  int16_t v = 0;
  _seconds++;
  if (_seconds >= _intervall) {
    if (_cntMw > 0) {
      v= -1.0 * _average  * _yScale / _cntMw;
    } else {
      if (_cnt > 0) {
        v = _values[0];
      } else {
        v = 0;
      }
    }
    _average = 0;
    _cntMw = 0;
    //Serial.printf("MW = %i\n",v);
    for (uint8_t i = 49; i > 0; i--) {
      _values[i] = _values[i-1];
    }
    if (_cnt < _maxCnt) _cnt++;
    //Serial.printf("CNT = %i\n",_cnt);
    _values [0] = v;
    if (display) {
      drawCurve(tft);
    }
    _seconds = 0;
  }
}
