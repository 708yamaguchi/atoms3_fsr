#ifndef ADS7828_h
#define ADS7828_h

#include "Arduino.h"
#include <Wire.h>

#define ADC_CH0_INTERNAL (0x8C) // CH1
#define ADC_CH1_INTERNAL (0xCC) // CH2
#define ADC_CH2_INTERNAL (0x9C) // CH3
#define ADC_CH3_INTERNAL (0xDC) // CH4
#define ADC_CH4_INTERNAL (0xAC) // CH5
#define ADC_CH5_INTERNAL (0xEC) // CH6
#define ADC_CH6_INTERNAL (0xBC) // CH7
#define ADC_CH7_INTERNAL (0xFC) // CH8

#define ADC_CH0_EXTERNAL (0x84) // CH1
#define ADC_CH1_EXTERNAL (0xC4) // CH2
#define ADC_CH2_EXTERNAL (0x94) // CH3
#define ADC_CH3_EXTERNAL (0xD4) // CH4
#define ADC_CH4_EXTERNAL (0xA4) // CH5
#define ADC_CH5_EXTERNAL (0xE4) // CH6
#define ADC_CH6_EXTERNAL (0xB4) // CH7
#define ADC_CH7_EXTERNAL (0xF4) // CH8


class ADS7828
{
  // user-accessible "public" interface
  public:
  // methods
	ADS7828(byte addr);
    int getValue(uint8_t channel);
 private:
    byte adc_addr;
    byte adc_buff[2];
    byte channel_to_addr[8] = {ADC_CH0_INTERNAL, ADC_CH1_INTERNAL,
                               ADC_CH2_INTERNAL, ADC_CH3_INTERNAL,
                               ADC_CH4_INTERNAL, ADC_CH5_INTERNAL,
                               ADC_CH6_INTERNAL, ADC_CH7_INTERNAL};
    void readI2C(byte register_addr, uint8_t data_len, byte buffer[]);
};

#endif

