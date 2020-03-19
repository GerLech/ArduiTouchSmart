# ArduiTouchSmart
Translation will follow!
Diese Bibliothek unterstützt eine grafische Benutzeroberfläche zum Anzeigen von Messwerten in verschiedener Darstellung. Es können aber auch Bedienelemente wie Schalter und Regler angezeigt werden. Die Bibliothek ermöglicht es, die Ober-fläche zu konfigurieren und die Konfiguration im Flash-Filesystem abzuspeichern. Zum Ausfüllen von Formularen wird die Bibliothek TFTForm benötigt.
## ArduiTouchSmart(Adafruit_ILI9341 * tft, TFTForm * conf, const GFXfont * font, bool header, bool footer)
Das ist der Konstruktor der Klasse. Es wird ein Zeiger auf eine Instanz der Adafruit_ILI9341 Display Klasse, ein weiterer Zeiger auf eine Instanz der TFTForm Klasse und ein Zeiger auf einen GFX Zeichensatz benötigt. Der Zeiger auf den Zeichensatz kann auch auf NULL gesetzt werden, dann wird immer der interne 5x7 Zeichensatz benutzt. Mit dem Parameter header wird festgelegt ob eine Menüzeile am oberen Bildschirmrand angezeigt wird oder nicht. Mit dem Parameter footer wird festgelegt ob eine Statuszeile am unteren Bildschirmrand angezeigt wird oder nicht.

## Error addPage()
Diese Funktion fügt eine neue Seite hinzu. Es wird eine Fehlernummer zurückgege-ben. Die folgenden Fehlermeldungen sind möglich. NO_ERROR (0) kein Fehler oder NO_MORE_PAGES (4) wenn die maximale Anzahl der Seiten (20) erreicht wurde.

## void removePage(uint8_t index)
Diese Funktion entfernt die Seite mit der Seitennummer index+1. Alle nachfolgen-den Seiten werden nachgerückt. Ist die Seite, die gelöscht werden soll, die einzige, so wird nur der Inhalt aber nicht die Seite selbst gelöscht. Auf diese Weise existiert immer zumindest eine Seite.

## uint8_t getPageCount()
Die Funktion returniert die Anzahl der definierten Seiten.

## uint8_t getCurrentPage()
Die Funktion returniert den Index der Seite, die gerade angezeigt wird. Das heißt bei der Seite 1 liefert die Funktion 0, da der Listenindex mit 0 beginnt.
Error addWidget(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t type, const char name[], const char style[] )
Auf der gerade angezeigten Seite wird ein Widget hinzugefügt. Die Position des Widgets wird mit den Parametern x und y angegeben, die Breite und Höhe mit den Parametern width und height. Für alle Abmessungen wird ein Raster von 20x20 Bildpunkten zugrunde gelegt. Das bedeutet der Bereich für x ist zwischen 0 und 12, der Bereich für y zwischen 0 und 16 oder zwischen 0 und 14, wenn eine Kopf- und eine Fußzeile angezeigt wird. Der Parameter type gibt die Art des Widgets an. Die folgenden Werte sind möglich. 
+ WT_SIMPLE (0): Einfache Messwertausgabe als Zahl.
+ WT_GAUGE (1): Darstellung des Messwerts als Zeigerinstrument.
+ WT_COLORBAR (2): Darstellung des Messwerts als vertikalen Farbbalken.
+ WT_BUTTON (3): Knopf benutzbar als Taster oder Schalter.
+ WT_SLIDER (4): Schieberegler zum Senden von analogen Werten.
+ WT_SWITCH (5): Schiebeschalter zum Ein- und Ausschalten.
+ WT_COLORSLIDER (6): Drei Schieberegler für rot, grün und blau.  

Der Parameter name setzt den Namen des Widgets. Der letzte Parameter style enthält weitere Einstellwerte für das Widget im JSON Format. Dieser Parameter kann auch auf NULL gesetzt werden, dann werden die Standard Einstellungen be-nutzt.
Es wird eine Fehlernummer zurückgegeben. Die folgenden Fehlermeldungen sind möglich. NO_ERROR(0) kein Fehler, NO_MORE_Widgets(1) wenn die maxima-le Anzahl der Widgets pro Seite (20) erreicht wurde oder WIDGET_OUTSIDE(2), wenn das Widget auf Grund seiner Abmessungen und Position über den Rand der Seite hinausgeht.

## Error addWidget(const char style[] )
Auf der gerade angezeigten Seite wird ein Widget hinzugefügt. Mit dieser Funktion werden alle Daten für das neue Widget als JSON String übergeben. Also auch Art, Größe und Position. Der Parameter style enthält den JSON String.
Es wird eine Fehlernummer zurückgegeben. Die folgenden Fehlermeldungen sind möglich. NO_ERROR(0) kein Fehler, NO_MORE_Widgets(1) wenn die maxima-le Anzahl der Widgets pro Seite (20) erreicht wurde, WIDGET_OUTSIDE(2), wenn das Widget auf Grund seiner Abmessungen und Position über den Rand der Seite hinausgeht oder NO_TYPE(5), wenn der JSON String keine Angaben zur Widget-Art enthält..

## void removeWidget(uint8_t index)
Das Widget mit dem angegebenen Index wird von der gerade angezeigten Seite entfernt. Nachfolgende Widgets werden vorgerückt sodass keine Lücke bleibt.

## void drawWidget(uint8_t index)
Das Widget mit dem angegebenen Index auf der gerade angezeigten Seite wird neu gezeichnet.

## void updateValue(uint8_t index, const char data[])
Der Messwert des Widgets mit dem angegebenen Index auf der gerade angezeigten Seite wird aktualisiert. Der Parameter data enthält die neuen Werte im JSON For-mat. Hier kann direkt eine empfangene MQTT Nachricht weitergegeben werden.

## int16_t findTopic(char topic[])
Diese Funktion returniert den Index eines Widgets auf der gerade angezeigten Seite, dem das im Parameter topic angegebene Thema zugeordnet wurde. Wird kein entsprechendes Widget gefunden, so wird -1 zurückgegeben.

## void drawPage()
Die aktuelle Seite wird neu angezeigt.

## int16_t getSelectedIndex()
Im Editier-Modus können Widgets durch Anklicken ausgewählt werden. Diese Funk-tion liefert den Index des ausgewählten Widgets auf der gerade angezeigten Seite. Ist kein Widget ausgewählt, so ist der Rückgabewert -1.

## uint8_t getWidgetCount()
Diese Funktion gibt die Anzahl der Widgets auf der gerade angezeigten Seite zurück.

## void updateTopic(char topic[], const char data[])
Diese Funktion aktualisiert alle Widgets auf allen Seiten, denen das im Parameter topic angegebene Thema (ohne Präfix) zugewiesen wurde. Der Parameter data enthält einen JSON-String mit den Daten die als MQTT Nachricht empfangen wur-den.

## bool touchEvent(int16_t x, int16_t y, EV event)
Damit Klicks auf den Touch-Screen reagiert werden kann, müssen bei einem Klick mit dieser Funktion die Koordinaten des Berührungspunktes in den Parametern x und y und die Art des Ereignisses im Parameter event übergeben werden

## const char * getTopic(uint8_t index)
Diese Funktion returniert das Thema, das dem Widget mit dem angegebenen Index auf der gerade angezeigten Seite zugewiesen wurde.  

## void registerOnPublish(bool (\*callback)(const char * topic, const char * message))
Diese Funktion registriert die Callback-Funktion bool onPublish(const char* topic, const char * message), die immer dann aufgerufen wird, wenn eine Nachricht von einem Widget an den MQTT Broker gesendet wer-den soll. Der Parameter topic enthält das Thema ohne Präfix und der Parameter message die Nachricht, die gesendet werden soll.

## void registerOnExternCommand(void(\*callback)(uint8_t button))
Diese Funktion registriert die Callback-Funktion onExternCommand(uint8_t button), die immer dann aufgerufen wird, wenn im Menü ein externes Komman-do angeklickt wird. Der Parameter button enthält die Nummer des angetippten Menü-Knopfs. Die folgenden externen Kommandos stehen zur Verfügung.

+  CMD_SCREENOFF (0): Abschalten der Hintergrundbeleuchtung des Dis-plays
+  CMD_LIST (8): Alias Liste für ESP_NOW Themen konfigurieren.
+  CMD_CALIBRATE (15): Touchscreen kalibrieren.
+  CMD_WLAN (16): Die Netzwerk Einstellungen ändern.
+  CMD_RESET (18): Den Controller neu starten.
+  CMD_INFO (19): Allgemeine Informationen anzeigen.
+  CMD_CONDITION (25): Automatisierungs-Regeln konfigurieren.
+  CMD_FOLDER (26): SPIFFS Verzeichnis anzeigen.

## void timeEvent(bool connected, bool showTime)
Diese Funktion sorgt dafür, dass Widgets, die Bedienelemente wie Schalter und Regler enthalten, darauf überprüft werden, ob MQTT-Nachrichten gesendet wer-den sollen. Der Parameter connected muss auf wahr gesetzt werden, wenn eine Netzwerkverbindung existiert. Wenn der Parameter showTime wahr ist, wird im Hauptmenü in der Mitte die aktuelle Uhrzeit angezeigt. Diese Funktion sollte in der loop Funktion des Sketches aufgerufen werden.

## void setFormat(bool header, bool footer)
Mit dieser Funktion können Kopf und Fußzeilen ein- und ausgeschaltet werden. Der Parameter header steht für die Kopfzeile mit dem Menü, der Parameter footer für die Fußzeile. Die Seite wird am Ende der Funktion nicht neu gezeichnet. Dazu muss die Funktion drawPage() aufgerufen werden.

## void endEdit(String data)
Diese Funktion muss aufgerufen werden, wenn die Bearbeitung eines Formulars mit der Klasse TFTForm abgeschlossen wurde. Der Parameter data enthält einen JSON-String mit den geänderten Daten oder einen Leerstring, wenn die Bearbeitung abgebrochen wurde. Die Funktion gibt die Kontrolle an die ArduiTouchSmart-Klasse zurück. Die aktuelle Seite wird neu gezeichnet.

## void setEditForm(uint8_t form)
Mit dieser Funktion kann der ArduiTouchSmart Klasse mitgeteilt werden, dass ein externes Formular angezeigt wird mit dem Parameter form = FRM_EDITEXTERN (5) oder dass das externe Formular geschlossen wurde mit form = FRM_NONE (0). Das ist notwendig, damit die ArduiTouchSmart Klasse in dieser Zeit nicht auf den Bildschirm zugreift. Wird das externe Formular über ein externes Kommando aufge-rufen, ist eine Verwendung dieser Funktion nicht nötig.

## void nextPage()
Diese Funktion blättert eine Seite vorwärts bis die letzte Seite erreicht wurde.

## void previousPage()
Diese Funktion blättert eine Seite zurück bis die erste Seite erreicht wurde.

## uint8_t savePage(uint8_t pagenum)
Die Seite, deren Seitennummer im Parameter pagenum angegeben wurde, wird mit allen Widgets im Flash Filesystem gespeichert. Zu beachten ist, dass die Seiten-nummer mit 0 beginnt!

## uint8_t saveAllPages()
Es werden alle Seiten mit allen Widgets im Flash Filesystem gespeichert.

## uint8_t loadAllPages()
Zuerst werden alle Seiten mit all ihren Widgets entfernt. Anschließend werden alle Seiten mit ihren Widgets, die im Filesystem gefunden werden, geladen und ange-legt.

## void setError(const char error[])
Der String, der im Parameter error steht, wird in der Mitte der Fußzeile an-gez6eigt.

## void addMenu(const int8_t menu[12])
Ein Untermenü wird hinzugefügt. Der Parameter menu ist eine Liste von zwölf in-ternen oder externen Kommandos, die im Menü angezeigt werden sollen. Sollte ein Menü-Feld leer bleiben, ist als Kommando -1 zu verwenden.  Folgende Kommandos können verwendet werden.

+  CMD_SCREENOFF (0): Abschalten der Hintergrundbeleuchtung des Dis-plays
+  CMD_PLAY (1): Vom Konfigurations-Modus in den Run Modus wechseln.
+  CMD_PAGEPLUS (2): Eine Seite hinzufügen.
+  CMD_PAGEMINUS (3): Eine Seite entfernen.
+  CMD_WDGPLUS (4): Ein Widget hinzufügen.
+  CMD_WDGMINUS (5): Ein Widget entfernen.
+  CMD_EDITPAGE (6): Die allgemeinen Einstellungen eines Widgets ändern.
+  CMD_EDIT (7): Die speziellen Einstellungen eines Widgets ändern.
+  CMD_LIST (8): Alias Liste für ESP_NOW Themen konfigurieren.
+  CMD_CONF (9): Eine Menüebene nach unten gehen.
+  CMD_LEFT (10): Eine Seite zurückblättern.
+  CMD_RIGHT (11): Eine Seite vorwärtsblättern.
+  CMD_PAUSE (12): Vom Run-Modus in den Konfigurations-Modus wechseln.
+  CMD_SAVE (13): Alle Seiten und Widgets ins Flash Filesystem speichern.
+  CMD_READ (14): Alle Seiten und Widgets aus dem Flash Filesystem laden.
+  CMD_CALIBRATE (15): Touchscreen kalibrieren.
+  CMD_WLAN (16): Die Netzwerk Einstellungen ändern.
+  CMD_BACK (17): Eine Menüebene nach oben gehen.
+  CMD_RESET (18): Den Controller neu starten.
+  CMD_INFO (19): Allgemeine Informationen anzeigen.
+  CMD_EXCHANGE (20): Zwei Seiten austauschen.
+  CMD_UP (22): Eine Menüebene nach oben gehen.
+  CMD_DOWN (23): Eine Menüebene nach unten gehen.
+  CMD_CONDITION (25): Automatisierungs-Regeln konfigurieren.
+  CMD_FOLDER (26): SPIFFS Verzeichnis anzeigen.
