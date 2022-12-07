
#include <TFTWidgets.h>
#include <Arduino.h>

String baseform = "["
  "{"
  "'name':'name',"
  "'label':'Name',"
  "'type':'text',"
  "'default':''"
  "},"
  "{"
  "'name':'xposition',"
  "'label':'X-Pos',"
  "'type':'number',"
  "'default':'0',"
  "'min':0,'max':10"
  "},"
  "{"
  "'name':'yposition',"
  "'label':'Y-Pos',"
  "'type':'number',"
  "'default':'0',"
  "'min':0,'max':13"
  "},"
  "{"
  "'name':'width',"
  "'label':'Breite',"
  "'type':'number',"
  "'default':'2',"
  "'min':2,'max':12"
  "},"
  "{"
  "'name':'height',"
  "'label':'Höhe',"
  "'type':'number',"
  "'default':'2',"
  "'min':2,'max':14"
  "},"
  "{"
  "'name':'background',"
  "'label':'Füllung',"
  "'type':'color',"
  "'default':'65535'"
  "},"
  "{"
  "'name':'border',"
  "'label':'Rahmen',"
  "'type':'color',"
  "'default':'0'"
  "},"
  "{"
  "'name':'fColor',"
  "'label':'Schrift',"
  "'type':'color',"
  "'default':'0'"
  "},"
  "{"
  "'name':'label',"
  "'label':'Beschriftung',"
  "'type':'text',"
  "'default':''"
  "},"
  "{"
  "'name':'topic',"
  "'label':'Thema',"
  "'type':'text',"
  "'default':''"
  "},"
  "{"
  "'name':'valname',"
  "'label':'Wert-Bez.',"
  "'type':'text',"
  "'default':'wert'"
  "},"
  "{"
  "'name':'unitname',"
  "'label':'Einheit-Bez.',"
  "'type':'text',"
  "'default':'einheit'"
  "}"
  "]";


Widget::Widget(const char json[]) {
  setBaseProperties(json);
}

void Widget::setBaseProperties(String properties) {
  StaticJsonDocument <1500>doc;
  DeserializationError   error = deserializeJson(doc,properties);
  if (error ) {
    Serial.print("JSON widget: ");
    Serial.println(error.c_str());
  }
  if (doc.containsKey("xposition")) _posx = doc["xposition"];
  if (doc.containsKey("yposition")) _posy = doc["yposition"];
  if (doc.containsKey("width")) _width = doc["width"];
  if (doc.containsKey("height")) _height = doc["height"];
  if (doc.containsKey("background")) _bgColor = doc["background"];
  if (doc.containsKey("border")) _borderColor = doc["border"];
  if (doc.containsKey("fColor")) _fontColor = doc["fColor"];
  if (doc.containsKey("label")) strlcpy(_label,doc["label"],50);
  if (doc.containsKey("topic")) strlcpy(_topic,doc["topic"],128);
  if (doc.containsKey("valname")) strlcpy(_valName,doc["valname"],128);
  if (doc.containsKey("unitname")) strlcpy(_unitName,doc["unitname"],128);
  if (doc.containsKey("name")) strlcpy(_name,doc["name"],128);
}

void Widget::drawBase(Adafruit_ILI9341 * tft, uint16_t hl, HA h_align, bool withLabel){
  uint8_t sz,len;
  tft->fillRect(_x,_y,_w,_h,_bgColor);
  tft->drawRect(_x,_y,_w,_h,_borderColor);
  if (_selected) {
    tft->drawRect(_x+1,_y+1,_w-2,_h-2,ILI9341_RED);
    tft->drawRect(_x+2,_y+2,_w-4,_h-4,ILI9341_RED);
    tft->drawRect(_x+3,_y+3,_w-6,_h-6,ILI9341_RED);
  }
  if (withLabel) {
    len = strlen(_label);
    sz = _w/(len * 6);
    if (sz==0) sz = 1;
    tft->setTextSize(1);
    tft->setTextColor(_fontColor);
    tft->setTextWrap(false);
    tft->setFont(_font);
    centerText(tft, _label, _x+2, _y+2, _w-4, hl, h_align, VA::ALIGN_MIDDLE);
  }
}

void Widget::centerText(Adafruit_ILI9341 * tft, char text[], int16_t x, int16_t y, int16_t w, int16_t h, HA h_align, VA v_align) {
  int16_t x1,y1,xt,yt,len;
  uint16_t w1,h1;
  String tmp;
  tmp = unicodeToDisplay(String(text));
  len = tmp.length();
  tft->getTextBounds(tmp,x,y+h,&x1,&y1,&w1,&h1);
  while (w1 > w) {
    len--;
    tmp.remove(len);
    tft->getTextBounds(tmp,x,y+h,&x1,&y1,&w1,&h1);
  }
  //Horizontale Ausrichtung
  switch (h_align) {
    case HA::ALIGN_LEFT : xt=x; break;
    case HA::ALIGN_RIGHT: xt=x+w-w1; break;
    case HA::ALIGN_CENTER: xt=x+w/2-w1/2; break;
    default: xt=x;
  }
  //vertikale Ausrichtung
  //korrigierte Höhe ohne Unterlängen
  //korrigierte Höhe ohne Unterlängen
  int16_t hs = y+h-y1;
  switch (v_align) {
    case VA::ALIGN_TOP: yt=y+hs;
    case VA::ALIGN_MIDDLE: yt=y+h/2+hs/2; break;
    case VA::ALIGN_BOTTOM: yt=y+h; break;
    case VA::ALIGN_FIX: yt=y; break;
    default: yt=y;
  }
  tft->setCursor(xt,yt);
  tft->print(tmp);
}

String Widget::unicodeToDisplay(String text, bool ascii){
  String res = "";
  uint8_t i = 0;
  char c;
  while (i<text.length()) {
    c=text[i];
    if (c==195) { //UTF8 deutsche Umlaute
      i++;
      switch (text[i]) {
        case 164: c=ascii?132:130; break; //ä
        case 182: c=ascii?148:131; break; //ö
        case 188: c=ascii?129:132; break; //ü
        case 159: c=ascii?225:133; break; //ß
        case 132: c=ascii?142:127; break; //Ä
        case 150: c=ascii?153:128; break; //Ö
        case 156: c=ascii?154:129; break; //Ü
        default: c=0;
      }
    } else if (c == 194) { //UTF8 Zeichen für Grad Symbol
      i++;
      if (text[i] == 176) c=ascii?248:134; else c=0;
    } else if (c > 128) { //normale Zeichen werden nicht verändert
      c=0;
    }
    if (c>0) res.concat(c);
    i++;
  }
  return res;
}

void Widget::centerValue(Adafruit_ILI9341 * tft, int16_t value, int16_t x, int16_t y, int16_t w, int16_t h, const char * unit){
  char buf[15];
  if (strlen(unit)>0)  sprintf(buf,"%i %s",value,unit); else sprintf(buf,"%i",value);
  centerText(tft,buf,x,y,w,h,HA::ALIGN_CENTER,VA::ALIGN_MIDDLE);
}

void Widget::centerValue(Adafruit_ILI9341 * tft, float value, int16_t x, int16_t y, int16_t w, int16_t h, uint8_t decimals = 2, const char * unit = ""){
  char buf[15];
  char fmt[10];
  if (decimals > 6) decimals = 6;
  if (strlen(unit)>0) {
    sprintf(fmt,"%%.%if %%s",decimals);
    sprintf(buf,fmt,value,unit);
  } else {
    sprintf(fmt,"%%.%if",decimals);
    sprintf(buf,fmt,value);
  }
  centerText(tft,buf,x,y,w,h,HA::ALIGN_CENTER,VA::ALIGN_MIDDLE);
}

bool Widget::isInside(int16_t x, int16_t y){
  return ((x>=_x) && (x<=(_x+_w)) && (y >= _y) && (y <= (_y+_h)));
}

bool Widget::hasTopic(char topic[]) {
  return (strcmp(topic,getTopic()) == 0);
}

String Widget::getBaseEditForm(){
  return baseform;
}

String Widget::getBaseProperties() {
  char buffer[500];
  StaticJsonDocument<500> doc;
  doc["xposition"]=_posx;
  doc["yposition"]=_posy;
  doc["width"]=_width;
  doc["height"]=_height;
  doc["background"]=_bgColor;
  doc["fColor"]=_fontColor;
  doc["border"]=_borderColor;
  doc["label"]=_label;
  doc["topic"]=_topic;
  doc["valname"]=_valName;
  doc["unitname"]=_unitName;
  doc["name"]=_name;
  uint16_t n = serializeJson(doc,buffer);
  return String(buffer);
}

void Widget::select(int16_t x, int16_t y, Adafruit_ILI9341 * tft){
  bool itsMe = isInside(x,y);
  if (_selected && !itsMe) {
    _selected = false;
    draw(tft);
  }
  if (!_selected && itsMe) {
    _selected = true;
    draw(tft);
  }
}
bool Widget::isSelected() {
  return _selected;
}

String Widget::getGeometrie(){
  char buffer[300];
  StaticJsonDocument<300> doc;
  doc["xposition"]=_posx;
  doc["yposition"]=_posy;
  doc["width"]=_width;
  doc["height"]=_height;
  uint16_t n = serializeJson(doc,buffer);
  return String(buffer);
}
