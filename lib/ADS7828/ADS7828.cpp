#include "ADS7828.h"

ADS7828::ADS7828(byte addr)
{
  adc_addr = addr;
  // Wire.begin();
}

int ADS7828::getValue(uint8_t channel)
{
  byte channel_addr = channel_to_addr[channel];
  readI2C(channel_addr, 2, adc_buff);
  // 12ビットのADC値を計算して返す
  int value = (((int)adc_buff[0]) << 8 ) | adc_buff[1];
  return value;
}

void ADS7828::readI2C(byte register_addr, uint8_t data_len, byte buffer[])
{
  Wire.beginTransmission(adc_addr);
  Wire.write(register_addr);
  Wire.endTransmission(false);

  Wire.requestFrom(adc_addr, data_len);

  int i = 0;
  while(Wire.available())
  {
    buffer[i] = Wire.read();
    i++;
  }
}
