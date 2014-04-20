#include "board.h"
#include "drv_max31855.h"
#include "drv_max31855.h"
static uint32_t raw_read(int sensor)
{
  int i;
  uint32_t d = 0;
    
  if(sensor > 1)
      sensor = 0;

  GPIOB->BSRRH = GPIO_Pin_12; // SCLK low
  delayMicroseconds(10);
  GPIOE->BSRRH = (sensor + 1) << 13; // CS low (PE13 or 14)
  delayMicroseconds(10);

  for (i=31; i>=0; i--)
  {
    GPIOB->BSRRH = GPIO_Pin_12; // SCLK low
    delayMicroseconds(1);
    d <<= 1;
    if (GPIOB->IDR & GPIO_Pin_13) { // read serial Data
      d |= 1;
    }

    GPIOB->BSRRL = GPIO_Pin_12; // SCLK high
    delayMicroseconds(1);
  }

  GPIOE->BSRRL = (sensor + 1) << 13; // CS high
  return d;
}

float read_celsius(int sensor)
{
    uint32_t raw_data;
    int16_t temp;
    float centigrade;
    raw_data = raw_read(sensor);
    
    
    if (raw_data & 0x7) {
    // uh oh, a serious problem!
    return 0;
    }
    
    raw_data >>= 18;
    
    temp = raw_data & 0x3FFF;
    
    if(raw_data & 0x2000)
        temp |= 0xC000;
    
    centigrade = raw_data;
    
    centigrade *= 0.25f;
    
    return centigrade;
}
