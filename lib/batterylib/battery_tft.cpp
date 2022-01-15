#include <battery_tft.hpp>

int vref = 1100;
float curv = 0;

void setupBattADC() {
    esp_adc_cal_characteristics_t adc_chars;
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize((adc_unit_t)ADC_UNIT_1, (adc_atten_t)ADC1_CHANNEL_6, (adc_bits_width_t)ADC_WIDTH_BIT_12, 1100, &adc_chars);
    //Check type of calibration value used to characterize ADC
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        Serial.printf("-->[BATT] ADC eFuse Vref:%u mV\n", adc_chars.vref);
        vref = adc_chars.vref;
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        Serial.printf("-->[BATT] ADC Two Point --> coeff_a:%umV coeff_b:%umV\n", adc_chars.coeff_a, adc_chars.coeff_b);
    } else {
        Serial.printf("-->[BATT] ADC Default Vref: %u mV\n", vref);
    }
}

void Battery_TFT::init(bool debug) {
    this->debug = debug;
    /*
    ADC_EN is the ADC detection enable port
    If the USB port is used for power supply, it is turned on by default.
    If it is powered by battery, it needs to be set to high level
    */
    pinMode(ADC_EN, OUTPUT);
    digitalWrite(ADC_EN, HIGH);
    delay(10);                         // suggested by @ygator user in issue #2
    setupBattADC();
    delay(10);                         // suggested by @ygator user in issue #2
}

float Battery_TFT::getVoltage () {
    return curv;
}

void Battery_TFT::update() {
    digitalWrite(ADC_EN, HIGH);
    delay(10);  // suggested by @ygator user in issue #2
    uint16_t v = analogRead(ADC_PIN);
    curv = ((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);
    digitalWrite(ADC_EN, LOW);  // for possible issue: https://github.com/Xinyuan-LilyGO/TTGO-T-Display/issues/6
}

bool Battery_TFT::isCharging() {
    return curv > BATTERY_MAX_V + (BATTCHARG_MIN_V - BATTERY_MAX_V ) / 2;
}

void Battery_TFT::printValues() {
    if (!debug) return;
    Serial.printf("-->[BATT] Battery voltage  \t: %.3fv vref: %i Charge:%i\n", curv, vref, getCharge());  //Output voltage and current of Bat
}

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TFTBATTERY)
Battery_TFT battery;
#endif
