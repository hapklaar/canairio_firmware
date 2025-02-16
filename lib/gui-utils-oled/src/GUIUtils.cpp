#include "GUIUtils.hpp"

#include "GUIIcons.h"

#//include <battery.hpp>

/******************************************************************************
*   D I S P L A Y  M E T H O D S
******************************************************************************/

void GUIUtils::displayInit() {
#ifdef WEMOSOLED  // display via i2c for WeMOS OLED board & TTGO18650
    U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, 4, 5, U8X8_PIN_NONE);
#elif HELTEC   // display via i2c for Heltec board
    U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, 15, 4, 16);
#elif TTGO_TQ  // display via i2c for TTGO_TQ
    U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, 4, 5, U8X8_PIN_NONE);
#elif ESP32DEVKIT
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, U8X8_PIN_NONE, U8X8_PIN_NONE);
#elif ESP32PICOD4
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, U8X8_PIN_NONE, U8X8_PIN_NONE);
#elif M5PICOD4
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, U8X8_PIN_NONE, U8X8_PIN_NONE);
#else  // display via i2c for TTGO_T7 (old D1MINI) board
    U8G2_SSD1306_64X48_ER_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, U8X8_PIN_NONE, U8X8_PIN_NONE);
#endif
    u8g2.setBusClock(100000);
    u8g2.begin();
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.setContrast(255);
    u8g2.setFontRefHeightExtendedText();
    u8g2.setDrawColor(1);
    u8g2.setFontPosTop();
    u8g2.setFontDirection(0);
    u8g2.setFontMode(0);
    this->u8g2 = u8g2;
    dw = u8g2.getDisplayWidth();
    dh = u8g2.getDisplayHeight();

    // init battery (only for some boards)
    // batteryInit();

    Serial.println("-->[OGUI] display config ready.");
}

void GUIUtils::showWelcome() {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_5x8_tf);
    u8g2.drawStr(0, 0, "CanAirIO");
    u8g2.sendBuffer();
    u8g2.setFont(u8g2_font_4x6_tf);
    u8g2.drawStr(dw - 18, 1, getFirmwareVersionCode().c_str());
    u8g2.drawLine(0, 9, dw - 1, 9);
    lastDrawedLine = 10;
    // only for first screen
    u8g2.sendBuffer();
}

void GUIUtils::setPowerSave() {
    u8g2.setPowerSave(1);
}
void GUIUtils::showMain() {
    u8g2.clearBuffer();
    u8g2.sendBuffer();
}

void GUIUtils::showProgress(unsigned int progress, unsigned int total) {
    u8g2.setFont(u8g2_font_4x6_tf);
    char output[12];
    sprintf(output, "%03d%%", (progress / (total / 100)));
    u8g2.drawStr(0, lastDrawedLine, output);
    u8g2.sendBuffer();
}

void GUIUtils::welcomeAddMessage(String msg) {
    if (lastDrawedLine >= dh - 6) {
        delay(2000);
        showWelcome();
    }
    u8g2.setFont(u8g2_font_4x6_tf);
#ifdef TTGO_TQ
    if (lastDrawedLine < 32) {
        u8g2.drawStr(0, lastDrawedLine, msg.c_str());
        lastDrawedLine = lastDrawedLine + 7;
        u8g2.sendBuffer();
    } else {
        u8g2.drawStr(72, lastDrawedLine - 20, msg.c_str());
        lastDrawedLine = lastDrawedLine + 7;
        u8g2.sendBuffer();
    }
#else
    u8g2.drawStr(0, lastDrawedLine, msg.c_str());
    lastDrawedLine = lastDrawedLine + 7;
    u8g2.sendBuffer();
#endif
    delay(100);
}

// TODO: This metod failed on redraw or clear the space first
void GUIUtils::welcomeRepeatMessage(String msg) {
    lastDrawedLine = lastDrawedLine - 7;
    welcomeAddMessage("               ");
    lastDrawedLine = lastDrawedLine - 7;
    welcomeAddMessage(msg);
}

void GUIUtils::displayCenterBig(String msg) {
#ifndef EMOTICONS
#ifdef TTGO_TQ
    u8g2.setCursor(0, 1);
    u8g2.setFont(u8g2_font_inb30_mn);
#else
    u8g2.setCursor(0, 0);
    u8g2.setFont(u8g2_font_inb24_mn);
#endif
    u8g2.print(msg.c_str());
#else
#ifdef TTGO_TQ
    u8g2.setCursor(52, 00);
    u8g2.setFont(u8g2_font_9x18B_tf);
    u8g2.print(msg.c_str());
#else
    if (dw > 64) {
        if (_deviceType <= AQI_COLOR::AQI_PM) {  // PM
            u8g2.setCursor(dw - 64, 6);
            u8g2.setFont(u8g2_font_inb24_mn);
        } else {  // CO2
            u8g2.setCursor(dw - 62, 10);
            u8g2.setFont(u8g2_font_inb19_mn);
        }
    } else {
        if (_deviceType <= AQI_COLOR::AQI_PM) {  // PM
            u8g2.setCursor(dw - 28, 7);
            u8g2.setFont(u8g2_font_9x18B_tf);
        } else {  // CO2
            u8g2.setCursor(dw - 27, 8);
            u8g2.setFont(u8g2_font_7x13B_tf);
        }
    }
    u8g2.print(msg.c_str());
    u8g2.setCursor(94, 36);
    // u8g2.setFont(u8g2_font_4x6_tf);
    u8g2.setFont(u8g2_font_6x13_tf);
    String unit = _unit_symbol;
    u8g2.print(unit);
#endif
#endif
}

void GUIUtils::displayBottomLine(String msg) {
    u8g2.setFont(u8g2_font_4x6_tf);
#ifdef TTGO_TQ
    u8g2.setCursor(115, 16);
    u8g2.print(msg.c_str());
#else
#ifndef EMOTICONS
    u8g2.setCursor(0, 29);
    u8g2.print(msg.c_str());
#endif
#endif
}

void GUIUtils::displayEmoticonLabel(int cursor, String msg) {
    u8g2.setFont(u8g2_font_unifont_t_emoticons);
    u8g2.drawGlyph(76, 12, cursor);
    u8g2.setFont(u8g2_font_4x6_tf);
    u8g2.setCursor(77, 17);
    u8g2.print(msg);
}

void GUIUtils::displayBigEmoticon(String msg) {
#ifdef EMOTICONS
#ifdef TTGO_TQ
    u8g2.setFont(u8g2_font_6x12_tf);
    u8g2.setCursor(40, 14);
    u8g2.print(msg);
#else
    u8g2.setFont(u8g2_font_5x7_tf);  //5x7 5x7 6x10 4x6 5x7
    u8g2.setCursor(29, 28);          //(35, 26);; (25, 29); (30, 29); (29, 28); (25, 30)(30, 29)
    u8g2.print(msg);                 //4 8 7 6 7 6
#endif
#endif
}

void GUIUtils::displayBigLabel(int cursor, String msg) {
#ifdef EMOTICONS
#ifdef TTGO_TQ
    u8g2.setFont(u8g2_font_5x7_tf);  //5x7 5x7 6x10 4x6 5x7
    u8g2.setCursor(cursor, 16);      //70 94 88 82 90 90
    u8g2.print(msg);
#else
    u8g2.setFont(u8g2_font_4x6_tf);
    u8g2.setCursor(35, 20);
    u8g2.print(msg);
#endif  //4 8 7 6 7 6
#endif
}

void GUIUtils::displaySensorAverage(int average) {
#ifndef EMOTICONS
#ifdef TTGO_TQ
    if (average < 13) {
        displayEmoticonLabel(0x0024, "GOOD");
    } else if (average < 36) {
        displayEmoticonLabel(0x0062, "MODERATE");
    } else if (average < 56) {
        displayEmoticonLabel(0x0032, "UNH SEN G");
    } else if (average < 151) {
        displayEmoticonLabel(0x0051, "UNHEALTY");
    } else if (average < 251) {
        displayEmoticonLabel(0x0053, "VERY UNH");
    } else {
        displayEmoticonLabel(0x0057, "HAZARDOUS");
    }
#endif
#else
    if (_deviceType <= AQI_COLOR::AQI_PM) {  //PM sensors and PAX
        if (average < 13) {
#ifdef TTGO_TQ
            u8g2.drawXBM(1, 0, 32, 32, SmileFaceGood);
            displayBigEmoticon("GOOD");
            displayBigLabel(66, "/green");
#else
            u8g2.drawXBM(0, 1, 32, 32, SmileFaceGood);
            displayBigEmoticon("  GOOD");
            displayBigLabel(0, " green");
#endif
        } else if (average < 36) {
#ifdef TTGO_TQ
            u8g2.drawXBM(1, 0, 32, 32, SmileFaceModerate);
            displayBigEmoticon("MODERATE");
            displayBigLabel(90, "/yel");
#else
            u8g2.drawXBM(0, 1, 32, 32, SmileFaceModerate);
            displayBigEmoticon("MODERATE");
            displayBigLabel(0, "yellow");
#endif
        } else if (average < 56) {
#ifdef TTGO_TQ
            u8g2.drawXBM(1, 0, 32, 32, SmileFaceUnhealthySGroups);
            displayBigEmoticon("UNH SEN");
            displayBigLabel(84, "/oran");
#else
            u8g2.drawXBM(0, 1, 32, 32, SmileFaceUnhealthySGroups);
            displayBigEmoticon("UNH SEN");
            displayBigLabel(0, "orange");
#endif
        } else if (average < 151) {
#ifdef TTGO_TQ
            u8g2.drawXBM(1, 0, 32, 32, SmileFaceUnhealthy);
            displayBigEmoticon("UNHEALT");
            displayBigLabel(84, "/red");  //OK
#else
            u8g2.drawXBM(0, 1, 32, 32, SmileFaceUnhealthy);
            displayBigEmoticon("UNHEALT");
            displayBigLabel(0, "  red");
#endif
        } else if (average < 251) {
#ifdef TTGO_TQ
            u8g2.drawXBM(1, 0, 32, 32, SmileFaceVeryUnhealthy);
            displayBigEmoticon("V UNHEA");
            displayBigLabel(84, "/viol");  //OK
#else
            u8g2.drawXBM(0, 1, 32, 32, SmileFaceVeryUnhealthy);
            displayBigEmoticon("V UNHEA");
            displayBigLabel(0, "violet");
#endif
        } else {
#ifdef TTGO_TQ
            u8g2.drawXBM(1, 0, 32, 32, SmileFaceHazardous);
            displayBigEmoticon("HAZARD");
            displayBigLabel(78, "/brown");
#else
            u8g2.drawXBM(0, 1, 32, 32, SmileFaceHazardous);
            displayBigEmoticon("HAZARD");
            displayBigLabel(0, " brown");
#endif
        }
        char output[4];
        sprintf(output, "%03d", average);
        displayCenterBig(output);
    } else {  //PM sensors
        if (average < 600) {
#ifdef TTGO_TQ
            u8g2.drawXBM(1, 0, 32, 32, SmileFaceGood);
            displayBigEmoticon("GOOD");
            displayBigLabel(66, "/green");
#else
            u8g2.drawXBM(0, 1, 32, 32, SmileFaceGood);
            displayBigEmoticon("  GOOD");
            displayBigLabel(0, " green");
#endif
        } else if (average < 800) {
#ifdef TTGO_TQ
            u8g2.drawXBM(1, 0, 32, 32, SmileFaceModerate);
            displayBigEmoticon("MODERATE");
            displayBigLabel(90, "/yel");
#else
            u8g2.drawXBM(0, 1, 32, 32, SmileFaceModerate);
            displayBigEmoticon("MODERATE");
            displayBigLabel(0, "yellow");
#endif
        } else if (average < 1000) {
#ifdef TTGO_TQ
            u8g2.drawXBM(1, 0, 32, 32, SmileFaceUnhealthy);
            displayBigEmoticon("UNHEALT");
            displayBigLabel(84, "/red");  //OK
#else
            u8g2.drawXBM(0, 1, 32, 32, SmileFaceUnhealthy);
            displayBigEmoticon("UNHEALT");
            displayBigLabel(0, "  red");
#endif
        } else if (average < 1400) {
#ifdef TTGO_TQ
            u8g2.drawXBM(1, 0, 32, 32, SmileFaceVeryUnhealthy);
            displayBigEmoticon("V UNHEA");
            displayBigLabel(84, "/viol");  //OK
#else
            u8g2.drawXBM(0, 1, 32, 32, SmileFaceVeryUnhealthy);
            displayBigEmoticon("V UNHEA");
            displayBigLabel(0, "violet");
#endif
        } else {
#ifdef TTGO_TQ
            u8g2.drawXBM(1, 0, 32, 32, SmileFaceHazardous);
            displayBigEmoticon("HAZARD");
            displayBigLabel(78, "/brown");
#else
            u8g2.drawXBM(0, 1, 32, 32, SmileFaceHazardous);
            displayBigEmoticon("HAZARD");
            displayBigLabel(0, " brown");
#endif
        }
        char output[4];
        sprintf(output, "%04d", average);
        displayCenterBig(output);
    }
#endif
}

void GUIUtils::displayMainValues() {
    displaySensorAverage(_average);
    char output[50];
    if (_deviceType <= AQI_COLOR::AQI_PM) // PM sensors and PAX
        sprintf(output, "%03d E%02d H%02d%% T%02d°C", _mainValue, 0, (int)_humi, (int)_temp);
    else
        sprintf(output, "%04d E%02d H%02d%% T%02d°C", _mainValue, 0, (int)_humi, (int)_temp);
    displayBottomLine(String(output));

#ifdef EMOTICONS
#ifndef TTGO_TQ
    u8g2.setFont(u8g2_font_4x6_tf);
    u8g2.setCursor(48, 0);
    sprintf(output, "%04d", _mainValue);
    // u8g2.print(output);  //TODO: it sometime fails
#endif
#endif
    u8g2.setFont(u8g2_font_6x12_tf);
#ifndef TTGO_TQ
    //u8g2.setCursor(20, 39);
    u8g2.setCursor(2, 39);
#else
#ifdef EMOTICONS
    u8g2.setCursor(40, 23);  // valor RSSI
#else
    u8g2.setCursor(100, 13);  // valor RSSI
#endif
#endif
    if (_rssi == 0) {
        u8g2.print("   ");
    } else {
        _rssi = abs(_rssi);
        sprintf(output, "%02d", _rssi);
        u8g2.print(_rssi);
    }
    if (_batteryCharge == 0) {
        u8g2.print(" ");
    } else {
        u8g2.setFont(u8g2_font_6x12_tf);
        u8g2.print(" ");
        _batteryCharge = abs(_batteryCharge);
        sprintf(output, "%02d", _batteryCharge);
        u8g2.print(_batteryCharge);
        u8g2.print("%");
    }
    isNewData = false;



}

// TODO: separate this function, format/display
void GUIUtils::setSensorData(GUIData data) {
    suspendTaskGUI();
    _deviceType = data.color;
    _humi = data.humi;
    _temp = data.temp;
    _mainValue = data.mainValue;
    _minorValue = data.minorValue;
    _unit_symbol = data.unitSymbol;
    _unit_name = data.unitName;
    _average = _mainValue;
    _rssi = abs(data.rssi);
    isNewData = true;
    resumeTaskGUI();
}

void GUIUtils::setGUIStatusFlags(bool wifiOn, bool bleOn, bool blePair) {
    static uint_least64_t guiTimeStamp = 0;
    if (millis() - guiTimeStamp > 500) {
        guiTimeStamp = millis();
        suspendTaskGUI();
        _wifiOn = wifiOn;
        _bleOn = bleOn;
        _blePair = blePair;
        resumeTaskGUI();
    }
}

void GUIUtils::setInfoData(String info) {
    // TODO: 
}

void GUIUtils::setBatteryStatus(float volts, int charge, bool isCharging) {
     suspendTaskGUI();
    _batteryVolts = volts;
    _batteryCharge = charge;
    _isCharging = isCharging;
    resumeTaskGUI();
}

void GUIUtils::displayGUIStatusFlags() {
#ifdef TTGO_TQ
    if (_bleOn)
        u8g2.drawBitmap(dw - 9, dh - 8, 1, 8, ic_bluetooth_on);
    if (_blePair)
        u8g2.drawBitmap(dw - 9, dh - 8, 1, 8, ic_bluetooth_pair);
    if (_wifiOn)
        u8g2.drawBitmap(dw - 18, dh - 8, 1, 8, ic_wifi_on);
    if (dataOn)
        u8g2.drawBitmap(dw - 35, dh - 8, 1, 8, ic_data_on);
    if (preferenceSave)
        u8g2.drawBitmap(dw - 57, dh - 8, 1, 8, ic_pref_save);
    if (sensorLive)
        u8g2.drawBitmap(dw - 48, dh - 8, 1, 8, ic_sensor_live);

#else
    if (_bleOn)
        u8g2.drawBitmap(dw - 10, dh - 8, 1, 8, ic_bluetooth_on);
    if (_blePair)
        u8g2.drawBitmap(dw - 10, dh - 8, 1, 8, ic_bluetooth_pair);
    if (_wifiOn)
        u8g2.drawBitmap(dw - 20, dh - 8, 1, 8, ic_wifi_on);
    if (dataOn)
        u8g2.drawBitmap(dw - 30, dh - 8, 1, 8, ic_data_on);
    if (preferenceSave)
        u8g2.drawBitmap(10, dh - 8, 1, 8, ic_pref_save);
    if (sensorLive)
        u8g2.drawBitmap(0, dh - 8, 1, 8, ic_sensor_live);

    u8g2.drawLine(0, dh - 10, dw - 1, dh - 10);
#endif
    if (dataOn) dataOn = false;                  // reset trigger for publish data ok.
    if (preferenceSave) preferenceSave = false;  // reset trigger for save preference ok.
    if (sensorLive) sensorLive = false;
}

/// enable trigger for show data ok icon, one time.
void GUIUtils::displayDataOnIcon() {
    dataOn = true;
}

/// enable trigger for sensor live icon, one time.
void GUIUtils::displaySensorLiveIcon() {
    sensorLive = true;
}

/// enable trigger for save preference ok, one time.
void GUIUtils::displayPreferenceSaveIcon() {
    preferenceSave = true;
}

void GUIUtils::pageStart() {
    u8g2.firstPage();
}

void GUIUtils::pageEnd() {
    u8g2.nextPage();
}

void GUIUtils::setBrightness(uint32_t value){

}

void GUIUtils::setWifiMode(bool enable){

}

void GUIUtils::setPaxMode(bool enable){

}

void GUIUtils::setSampleTime(int time){

}

void GUIUtils::setTrackValues(float speed, float distance){

}

void GUIUtils::setTrackTime(int h, int m, int s){

}

void GUIUtils::suspendTaskGUI(){
}

void GUIUtils::resumeTaskGUI(){
}

void GUIUtils::setCallbacks(GUIUserPreferencesCallbacks* pCallBacks){

}

void GUIUtils::loop(){
    static uint_least64_t guiTimeStamp = 0;
    if (millis() - guiTimeStamp > 500) {
        guiTimeStamp = millis();
        gui.pageStart();
        gui.displayMainValues();
        gui.displayGUIStatusFlags();
        gui.pageEnd();
    }
}

/// Firmware version from platformio.ini
String GUIUtils::getFirmwareVersionCode() {
    String VERSION_CODE = "r";
#ifdef REVISION
    int VCODE = REVISION;
#else
    int VCODE = 0;
#endif
    return String(VERSION_CODE + VCODE);
}

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_GUIHANDLER)
GUIUtils gui;
#endif
