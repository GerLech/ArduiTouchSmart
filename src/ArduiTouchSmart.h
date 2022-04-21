//Version 1.1

#ifndef ArduiTouchSmart_h
#define ArduiTouchSmart_h

#include <Arduino.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_GFX.h>
#include <TFTicons.h>


#define MAXPAGES 20
#define MAXMENUS 5

#include <TFTWidgetPage.h>

#define FRM_NONE 0
#define FRM_WIDGETEDIT 1
#define FRM_WIDGETNEW 2
#define FRM_WIDGETDELETE 3
#define FRM_PAGEDELETE 4
#define FRM_EDITEXTERN 5
#define FRM_LOADALL 6
#define FRM_SAVEALL 7
#define FRM_CHANGEPAGE 8
#define FRM_RESET 9


class ArduiTouchSmart{
public:
  ArduiTouchSmart(Adafruit_ILI9341 * tft, TFTForm * conf, const GFXfont * font, bool header, bool footer);
  Error addPage();
  void removePage(uint8_t index);
  uint8_t getPageCount();
  uint8_t getCurrentPage();
  uint8_t getConfMode();


  //This functions are the same as on TFTWidgetPage and will be called with the current page
  Error addWidget(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t type, const char name[], const char style[] );
  Error addWidget(const char style[] );
  void removeWidget(uint8_t index);
  void drawWidget(uint8_t index);
  void updateValue(uint8_t index, const char data[]);
  int16_t findTopic(char topic[]);
  void drawPage();
  int16_t getSelectedIndex();

  uint8_t getWidgetCount();

  void updateTopic(char topic[], const char data[]);
  bool touchEvent(int16_t x, int16_t y, EV event);
  const char * getTopic(uint8_t index);
  void registerOnPublish(bool (*callback)(const char * topic, const char * message));
  void registerOnExternCommand(void(*callback)(uint8_t button));
  void registerOnAlarm(void(*callback)(uint16_t level));
  void timeEvent(bool connected, bool showTime);
  void setFormat(bool header, bool footer);
  void endEdit(String data);
  void nextPage(boolean cyclic = false);
  void previousPage();
  uint8_t savePage(uint8_t pagenum);
  uint8_t saveAllPages();
  uint8_t loadAllPages();
  void setError(const char error[]);
  void addMenu(const int8_t menu[12]);
  void setEditForm(uint8_t form);
private:
  bool toolbarEvent(int16_t x, int16_t y);
  bool footbarEvent(int16_t x, int16_t y);
  void displayTime();
  void drawToolbar();
  void drawFootBar();
  void updatePages();
  void deleteAllWidgets();
  void showPageChangeDialog();
  void changePage(String data);
  void everySecond();
  TFTWidgetPage * _pages[MAXPAGES];
  uint8_t _cnt = 0;
  uint8_t _currentPage = 0;
  int8_t _menu[MAXMENUS][12];
  uint8_t _menuCnt = 0;
  char _timeFormat[30] = "%d.%m. %H:%M:%S";
  Adafruit_ILI9341 * _tft;
  TFTForm * _conf;
  const GFXfont * _font;
  bool _header = true;
  bool _footer = true;
  uint16_t _lastAlarm = 0;
  uint8_t _confMode = 0;
  uint8_t _currentForm = FRM_NONE;
  char _error[25] = "";
  uint8_t _timcnt = 0;
  uint16_t _alarmcnt = 0;
  uint32_t _lts = 0;
  bool(*_onPublish)(const char * topic, const char * message) = NULL;
  void(*_onExternCommand)(uint8_t button) = NULL;
  void(*_onAlarm)(uint16_t level) = NULL;
};

#endif
