#pragma once

#include "core/HELPERS.ino"
#include "core/ISR.ino"

#include "meter/AAA-PARSE-TELEGRAM.ino"
#include "meter/AAA_READ_SERIAL.ino"
#include "meter/SERIAL.ino"

#include "storage/SPIFFS_RW.ino"

#include "time/TIJD_CALC.ino"
#include "time/TIJD_GET.ino"

#include "network/EXTERNAL.ino"
#include "network/MQTT.ino"
#include "network/PORTAL_WIFI.ino"
#include "network/Start_WiFi.ino"

#include "config/CONFIG_BASIS.ino"
#include "config/CONFIG_GEO.ino"
#include "config/MQTT_CONFIG.ino"

#include "web/ABOUT.ino"
#include "web/AAA_REPORT.ino"
#include "web/AA_CONSOLE.ino"
#include "web/ASYSERVER.ino"
#include "web/handleforms.ino"

#include "system/ACTIONS.ino"
#include "system/Reboot__AP.ino"
#include "system/test.ino"
