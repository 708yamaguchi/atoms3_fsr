#ifndef ADS7828_h
#define ADS7828_h

#include "Arduino.h"
#include <Wire.h>

#define ADC_CH0 (0x8C) // CH1
#define ADC_CH1 (0xCC) // CH2
#define ADC_CH2 (0x9C) // CH3
#define ADC_CH3 (0xDC) // CH4
#define ADC_CH4 (0xAC) // CH5
#define ADC_CH5 (0xEC) // CH6
#define ADC_CH6 (0xBC) // CH7
#define ADC_CH7 (0xFC) // CH8

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
    byte channel_to_addr[8] = {ADC_CH0, ADC_CH1, ADC_CH2, ADC_CH3,
                               ADC_CH4, ADC_CH5, ADC_CH6, ADC_CH7};
    void readI2C(byte register_addr, uint8_t data_len, byte buffer[]);
};

#endif

