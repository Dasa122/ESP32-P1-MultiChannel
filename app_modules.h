#pragma once

#include "src/core/HELPERS.ino"
#include "src/core/ISR.ino"

#include "src/meter/AAA-PARSE-TELEGRAM.ino"
#include "src/meter/AAA_READ_SERIAL.ino"
#include "src/meter/SERIAL.ino"

#include "src/storage/SPIFFS_RW.ino"

#include "src/time/TIJD_CALC.ino"
#include "src/time/TIJD_GET.ino"

#include "src/network/EXTERNAL.ino"
#include "src/network/MQTT.ino"
#include "src/network/PORTAL_WIFI.ino"
#include "src/network/Start_WiFi.ino"

#include "src/config/CONFIG_BASIS.ino"
#include "src/config/CONFIG_GEO.ino"
#include "src/config/MQTT_CONFIG.ino"

#include "src/web/ABOUT.ino"
#include "src/web/AAA_REPORT.ino"
#include "src/web/AA_CONSOLE.ino"
#include "src/web/ASYSERVER.ino"
#include "src/web/handleforms.ino"

#include "src/system/ACTIONS.ino"
#include "src/system/Reboot__AP.ino"
#include "src/system/test.ino"
