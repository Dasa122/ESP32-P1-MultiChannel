/*
 * Multi-channel P1 meter polling.
 * Shares one UART (Serial1) across 4 meters via individual enable pins.
 * Each channel has its own telegram buffer, CRC, and MeterData struct.
 */

void meterPollCh(uint8_t ch) {
    if (ch >= P1_NUM_CHANNELS) return;

    consoleOut("polling channel " + String(ch));

    // Already enabled by caller, just read
    if (read_into_array_ch(ch)) {
        ledblink(3, 300);

        if (bootTest) {
            if (strlen(teleGram_ch[ch]) > 50) {
                strcat(teleGram_ch[ch], "\n this is a test telegram, retrieved at boot ch=");
                strcat(teleGram_ch[ch], String(ch).c_str());
                testFilesave();
                consoleOut("saved a test telegram for ch " + String(ch));
            } else {
                strcpy(teleGram_ch[ch], "test at boot failed");
                consoleOut("test telegram failed ch " + String(ch));
            }
            return;
        }

        decodeTelegramCh(ch);
        sendMqttCh(ch, false);  // electricity
        sendMqttCh(ch, true);   // gas

    } else {
        consoleOut("no telegram received on ch " + String(ch));
    }

    consoleOut("meterPollCh " + String(ch) + " done");
}

bool read_into_array_ch(uint8_t ch) {
    int byteCounter = 0;
    char inByte[2];
    polled_ch[ch] = false;

    waitSerial1Available(5);
    empty_Serial1();

    if (waitSerial1Available(5)) {
        memset(teleGram_ch[ch], 0, sizeof(teleGram_ch[ch]));
        delayMicroseconds(250);
        Serial.println("available on Serial1 ch" + String(ch) + ": " + String(Serial1.available()));

        while (Serial1.available()) {
            Serial1.readBytes(inByte, 1);
            byteCounter++;

            if (inByte[0] == '/') {
                consoleOut("ch" + String(ch) + " found start at " + String(byteCounter));
                strncat(teleGram_ch[ch], inByte, 1);

                for (int x = 0; x < 750; x++) {
                    Serial1.readBytes(inByte, 1);
                    strncat(teleGram_ch[ch], inByte, 1);
                    if (inByte[0] == '!') {
                        consoleOut("ch" + String(ch) + " found the end sign");
                        Serial1.readBytes(readCRC_ch[ch], 4);
                        strcat(teleGram_ch[ch], readCRC_ch[ch]);
                        polled_ch[ch] = true;
                        return true;
                    }
                }
                consoleOut("ch" + String(ch) + " no endsign found");
                return false;
            }

            if (byteCounter > 2000) {
                consoleOut("ch" + String(ch) + " byteCounter over 2000");
                return false;
            }
        }
        consoleOut("ch" + String(ch) + " no startsign found");
        return false;
    }

    consoleOut("ch" + String(ch) + " got no data from serial1");
    return false;
}

void decodeTelegramCh(uint8_t ch) {
    if (!polled_ch[ch]) return;

    int lengte = strlen(teleGram_ch[ch]);
    consoleOut("ch" + String(ch) + " teleGram length = " + String(lengte));

    // Terminate before CRC
    teleGram_ch[ch][lengte - 4] = '\0';

    int calculatedCRC = CRC16(0x0000, (unsigned char *)teleGram_ch[ch], lengte - 4);
    consoleOut("ch" + String(ch) + " calculated crc = " + String(calculatedCRC));
    consoleOut("ch" + String(ch) + " strtol of readCRC = " + String(strtol(readCRC_ch[ch], NULL, 16)));

    if (strtol(readCRC_ch[ch], NULL, 16) == calculatedCRC) {
        consoleOut("ch" + String(ch) + " crc is correct, extracting values..");
        parseTelegramCh(ch);
        polled_ch[ch] = true;
        meters[ch].valid = true;
        eventSend(2);
        sprintf(timeStamp, "%02d/%02d %02d:%02d", day(), month(), hour(), minute());
    } else {
        consoleOut("ch" + String(ch) + " crc is wrong, not processed..");
        polled_ch[ch] = false;
    }
}

void parseTelegramCh(uint8_t ch) {
    const char* p = teleGram_ch[ch];
    MeterData& m = meters[ch];
    consoleOut("parsing teleGram ch" + String(ch));

    while (*p) {
        if (!strncmp(p, OBIS_SMR, strlen(OBIS_SMR)))
            m.smr = (uint8_t)parseValue(p, OBIS_SMR);
        else if (!strncmp(p, OBIS_CON_LT, strlen(OBIS_CON_LT)))
            m.con_lt = parseValue(p, OBIS_CON_LT);
        else if (!strncmp(p, OBIS_CON_HT, strlen(OBIS_CON_HT)))
            m.con_ht = parseValue(p, OBIS_CON_HT);
        else if (!strncmp(p, OBIS_RET_LT, strlen(OBIS_RET_LT)))
            m.ret_lt = parseValue(p, OBIS_RET_LT);
        else if (!strncmp(p, OBIS_RET_HT, strlen(OBIS_RET_HT)))
            m.ret_ht = parseValue(p, OBIS_RET_HT);
        else if (!strncmp(p, OBIS_POWER_C1, strlen(OBIS_POWER_C1)))
            m.pwr_con[0] = (uint16_t)(parseValue(p, OBIS_POWER_C1) * 1000.0f);
        else if (!strncmp(p, OBIS_POWER_R1, strlen(OBIS_POWER_R1)))
            m.pwr_ret[0] = (uint16_t)(parseValue(p, OBIS_POWER_R1) * 1000.0f);
        else if (!strncmp(p, OBIS_POWER_C2, strlen(OBIS_POWER_C2)))
            m.pwr_con[1] = (uint16_t)(parseValue(p, OBIS_POWER_C2) * 1000.0f);
        else if (!strncmp(p, OBIS_POWER_R2, strlen(OBIS_POWER_R2)))
            m.pwr_ret[1] = (uint16_t)(parseValue(p, OBIS_POWER_R2) * 1000.0f);
        else if (!strncmp(p, OBIS_POWER_C3, strlen(OBIS_POWER_C3)))
            m.pwr_con[2] = (uint16_t)(parseValue(p, OBIS_POWER_C3) * 1000.0f);
        else if (!strncmp(p, OBIS_POWER_R3, strlen(OBIS_POWER_R3)))
            m.pwr_ret[2] = (uint16_t)(parseValue(p, OBIS_POWER_R3) * 1000.0f);
        else if (!strncmp(p, OBIS_GAS, strlen(OBIS_GAS)))
            m.gas = parseGasValue(p);

        while (*p && *p != '\n') p++;
        if (*p == '\n') p++;
    }
}

/*
 * Per-channel MQTT send.
 * Each channel publishes to its own out-topic.
 * Uses format from Mqtt_Format setting.
 */
void sendMqttCh(uint8_t ch, bool gas) {
    if (Mqtt_Format == 0) return;
    if (ch >= P1_NUM_CHANNELS) return;

    char Mqtt_send[26] = {0};
    strcpy(Mqtt_send, Mqtt_outTopic_Ch[ch]);

    bool reTain = false;
    char toMQTT[512] = {0};
    MeterData& m = meters[ch];

    switch (Mqtt_Format) {
        case 1:
            if (!gas) {
                snprintf(toMQTT, sizeof(toMQTT),
                    "{\"idx\":%d,\"nvalue\":0,\"svalue\":\"%.2f;%.2f;%.2f;%.2f;%.2f;%.2f\"}",
                    el_Idx, m.con_lt * 1000, m.con_ht * 1000,
                    m.ret_lt * 1000, m.ret_ht * 1000,
                    m.pwr_con[0], m.pwr_ret[0]);
            } else {
                snprintf(toMQTT, sizeof(toMQTT),
                    "{\"idx\":%d,\"nvalue\":0,\"svalue\":\"%.3f;\"}",
                    gas_Idx, m.gas);
            }
            break;
        case 2:
            snprintf(toMQTT, sizeof(toMQTT),
                "{\"ch\":%d,\"econ_lt\":%.2f,\"econ_ht\":%.2f,\"eret_lt\":%.2f,\"eret_ht\":%.2f,\"actualp_con\":%.2f,\"actualp_ret\":%.2f,\"gas\":%.3f}",
                ch, m.con_lt, m.con_ht, m.ret_lt, m.ret_ht,
                m.pwr_con[0], m.pwr_ret[0], m.gas);
            break;
        case 3:
            snprintf(toMQTT, sizeof(toMQTT),
                "field1=%.3f&field2=%.3f&field3=%.3f&field4=%.3f&field5=%.0f&field6=%.0f&field7=%.3f&status=MQTTPUBLISH&ch=%d",
                m.con_lt, m.con_ht, m.ret_lt, m.ret_ht,
                m.pwr_con[0], m.pwr_ret[0], m.gas, ch);
            reTain = false;
            break;
    }

    if (mqttConnect()) MQTT_Client.publish(Mqtt_send, toMQTT, reTain);
    consoleOut("MQTT sent on ch" + String(ch) + " to " + String(Mqtt_send));
}
