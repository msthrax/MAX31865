#ifndef ADAFRUIT_MAX31865_H
#define ADAFRUIT_MAX31865_H

#include "PT100_LookupTable.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#define MAX31865_CONFIG_REG 0x00
#define MAX31865_CONFIG_BIAS 0x80
#define MAX31865_CONFIG_MODEAUTO 0x40
#define MAX31865_CONFIG_MODEOFF 0x00
#define MAX31865_CONFIG_1SHOT 0x20
#define MAX31865_CONFIG_3WIRE 0x10
#define MAX31865_CONFIG_24WIRE 0x00
#define MAX31865_CONFIG_FAULTSTAT 0x02
#define MAX31865_CONFIG_FILT50HZ 0x01
#define MAX31865_CONFIG_FILT60HZ 0x00

#define MAX31865_RTDMSB_REG 0x01
#define MAX31865_RTDLSB_REG 0x02
#define MAX31865_HFAULTMSB_REG 0x03
#define MAX31865_HFAULTLSB_REG 0x04
#define MAX31865_LFAULTMSB_REG 0x05
#define MAX31865_LFAULTLSB_REG 0x06
#define MAX31865_FAULTSTAT_REG 0x07

#define MAX31865_FAULT_HIGHTHRESH 0x80
#define MAX31865_FAULT_LOWTHRESH 0x40
#define MAX31865_FAULT_REFINLOW 0x20
#define MAX31865_FAULT_REFINHIGH 0x10
#define MAX31865_FAULT_RTDINLOW 0x08
#define MAX31865_FAULT_OVUV 0x04

#define RTD_A 3.9083e-3
#define RTD_B -5.775e-7

typedef struct MAX31865_SpiDriver_Struct {
  void (*Max31865_CS_Low)(void);
  void (*Max31865_CS_High)(void);
  void (*Max31865_Spi_WriteByte)(uint8_t);
  uint8_t (*Max31865_Spi_ReadByte)(void);
  void (*Max31865_Delay_1ms)(void);
  void (*Max31865_Delay_65ms)(void);
  void (*Max31865_Delay)(uint32_t);
} MAX31865_SpiDriver_Typedef;

typedef enum max31865_numwires {
  MAX31865_2WIRE = 0,
  MAX31865_3WIRE = 1,
  MAX31865_4WIRE = 0
} max31865_numwires_t;

bool MAX31865_Begin(MAX31865_SpiDriver_Typedef * max31865_SpiDriver, max31865_numwires_t x);

uint8_t MAX31865_ReadFault(MAX31865_SpiDriver_Typedef * max31865_SpiDriver);
void MAX31865_ClearFault(MAX31865_SpiDriver_Typedef * max31865_SpiDriver);
uint16_t MAX31865_ReadRTD(MAX31865_SpiDriver_Typedef * max31865_SpiDriver);

void MAX31865_SetWires(MAX31865_SpiDriver_Typedef * max31865_SpiDriver, max31865_numwires_t wires);
void MAX31865_AutoConvert(MAX31865_SpiDriver_Typedef * max31865_SpiDriver, bool b);
void MAX31865_Enable50Hz(MAX31865_SpiDriver_Typedef * max31865_SpiDriver, bool b);
void MAX31865_EnableBias(MAX31865_SpiDriver_Typedef * max31865_SpiDriver, bool b);

float MAX31865_Temperature(MAX31865_SpiDriver_Typedef * max31865_SpiDriver, float RTDnominal, float refResistor);

#endif
