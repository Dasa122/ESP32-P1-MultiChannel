#pragma once

void setup();
void loop();
void flush_wifi();
void eventSend(byte what);

void test_actionFlag();

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
void initWebSocket();

void parseTelegram();
float parseValue(const char* p, const char* obis);
float parseGasValue(const char* p);

void meterPoll();
bool read_into_array();
void decodeTelegram();

String putReport(const String& var);
void printStruct(String bestand, int what);

void handleAbout(AsyncWebServerRequest *request);
void start_asyserver();
void confirm();

void zendPageBasis(AsyncWebServerRequest *request);
void zendPageGEOconfig(AsyncWebServerRequest *request);
void zendPageMQTTconfig(AsyncWebServerRequest *request);

bool checkRemote(String url);
void handleNotFound(AsyncWebServerRequest *request);

void testMessage();
void consoleOut(String toLog);
void ledblink(int i, int wacht);
String getChipId(bool sec);
long getValidVal(long valNew, long valOld, long maxDiffer);
char *split(char *str, const char *delim);
double round3(double value);
double round2(double value);
double round1(double value);
double round0(double value);
bool waitSerial1Available(int howMany);
void empty_serial();
void empty_Serial1();
void showDir();
void printFiles();
void writeMonth(int maand);

bool mqttConnect();
void MQTT_Receive_Callback(char *topic, byte *payload, unsigned int length);
void sendMqtt(bool gas);

void loginAdmin(AsyncWebServerRequest *request);

void handleForms(AsyncWebServerRequest *request);
void handlePortalRoot();
void handleForm();
void wifiConnect();
void handlePortalNotFound(AsyncWebServerRequest *request);
int connectWifi();
void handlePortalClose();
void scanWifi();
String makeList(int aantal);
void start_portal();
void start_wifi();
void loginBoth(AsyncWebServerRequest *request, String who);

void handle_Serial();

void SPIFFS_read();
void writeStruct(String whichfile, int mnd);
bool readStruct(String whichfile, int mnd);
void testFilesave();
void wifiConfigsave();
void basisConfigsave();
void mqttConfigsave();
bool file_open_for_read(const char* bestand);

void sun_setrise();
bool isSummertime();
bool getTijd();

void read_test();
void testPrint(int aantal);

void isr();
