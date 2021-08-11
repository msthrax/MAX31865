#include "MAX31865.h"

/* Low Layer ------------------------------------------------------------ */
void MAX31865_ReadRegisterN(MAX31865_SpiDriver_Typedef * max31865_SpiDriver, uint8_t addr, uint8_t buffer[], uint8_t n) {
  addr &= 0x7F; // make sure top bit is not set
          
  max31865_SpiDriver->Max31865_CS_Low();
//  max31865_SpiDriver->Max31865_Delay(2);
  max31865_SpiDriver->Max31865_Spi_WriteByte(addr);
  for(size_t for_i_MAX31865_ReadRegisterN = 0; for_i_MAX31865_ReadRegisterN < n; for_i_MAX31865_ReadRegisterN++) {
    buffer[for_i_MAX31865_ReadRegisterN] = max31865_SpiDriver->Max31865_Spi_ReadByte();
  }
  max31865_SpiDriver->Max31865_CS_High();
//  max31865_SpiDriver->Max31865_Delay(2);
}

uint8_t MAX31865_ReadRegister8(MAX31865_SpiDriver_Typedef * max31865_SpiDriver, uint8_t addr) {
  uint8_t ret = 0;
  MAX31865_ReadRegisterN(max31865_SpiDriver, addr, &ret, 1);

  return ret;
}

uint16_t MAX31865_ReadRegister16(MAX31865_SpiDriver_Typedef * max31865_SpiDriver, uint8_t addr) {
  uint8_t buffer[2] = {0, 0};
  MAX31865_ReadRegisterN(max31865_SpiDriver, addr, buffer, 2);

  uint16_t ret = buffer[0];
  ret <<= 8;
  ret |= buffer[1];

  return ret;
}

void MAX31865_WriteRegister8(MAX31865_SpiDriver_Typedef * max31865_SpiDriver, uint8_t addr, uint8_t data) {
  addr |= 0x80; // make sure top bit is set

  max31865_SpiDriver->Max31865_CS_Low();
//  max31865_SpiDriver->Max31865_Delay(2);
  max31865_SpiDriver->Max31865_Spi_WriteByte(addr);
  max31865_SpiDriver->Max31865_Spi_WriteByte(data);
  max31865_SpiDriver->Max31865_CS_High();
//  max31865_SpiDriver->Max31865_Delay(2);
}
/* ------------------------------------------------------------------------- */

/*!
    @brief Initialize the SPI interface and set the number of RTD wires used
    @param wires The number of wires in enum format. Can be MAX31865_2WIRE,
    MAX31865_3WIRE, or MAX31865_4WIRE
    @return True
*/
/**************************************************************************/
bool MAX31865_Begin(MAX31865_SpiDriver_Typedef * max31865_SpiDriver, max31865_numwires_t wires) {
  MAX31865_SetWires(max31865_SpiDriver, wires);
  MAX31865_EnableBias(max31865_SpiDriver, false);
  MAX31865_AutoConvert(max31865_SpiDriver, false);
  MAX31865_ClearFault(max31865_SpiDriver);

  // Serial.print("config: ");
  // Serial.println(MAX31865_ReadRegister8(MAX31865_CONFIG_REG), HEX);
  return true;
}

/**************************************************************************/
/*!
    @brief Read the raw 8-bit FAULTSTAT register
    @return The raw unsigned 8-bit FAULT status register
*/
/**************************************************************************/
uint8_t MAX31865_ReadFault(MAX31865_SpiDriver_Typedef * max31865_SpiDriver) {
  return MAX31865_ReadRegister8(max31865_SpiDriver, MAX31865_FAULTSTAT_REG);
}

/**************************************************************************/
/*!
    @brief Clear all faults in FAULTSTAT
*/
/**************************************************************************/
void MAX31865_ClearFault(MAX31865_SpiDriver_Typedef * max31865_SpiDriver) {
  uint8_t t = MAX31865_ReadRegister8(max31865_SpiDriver, MAX31865_CONFIG_REG);
  t &= ~0x2C;
  t |= MAX31865_CONFIG_FAULTSTAT;
  MAX31865_WriteRegister8(max31865_SpiDriver, MAX31865_CONFIG_REG, t);
}

/**************************************************************************/
/*!
    @brief Enable the bias voltage on the RTD sensor
    @param b If true bias is enabled, else disabled
*/
/**************************************************************************/
void MAX31865_EnableBias(MAX31865_SpiDriver_Typedef * max31865_SpiDriver, bool b) {
  uint8_t t = MAX31865_ReadRegister8(max31865_SpiDriver, MAX31865_CONFIG_REG);
  if (b) {
    t |= MAX31865_CONFIG_BIAS; // enable bias
  } else {
    t &= ~MAX31865_CONFIG_BIAS; // disable bias
  }
  MAX31865_WriteRegister8(max31865_SpiDriver, MAX31865_CONFIG_REG, t);
}

/**************************************************************************/
/*!
    @brief Whether we want to have continuous conversions (50/60 Hz)
    @param b If true, auto conversion is enabled
*/
/**************************************************************************/
void MAX31865_AutoConvert(MAX31865_SpiDriver_Typedef * max31865_SpiDriver, bool b) {
  uint8_t t = MAX31865_ReadRegister8(max31865_SpiDriver, MAX31865_CONFIG_REG);
  if (b) {
    t |= MAX31865_CONFIG_MODEAUTO; // enable autoconvert
  } else {
    t &= ~MAX31865_CONFIG_MODEAUTO; // disable autoconvert
  }
  MAX31865_WriteRegister8(max31865_SpiDriver, MAX31865_CONFIG_REG, t);
}

/**************************************************************************/
/*!
    @brief Whether we want filter out 50Hz noise or 60Hz noise
    @param b If true, 50Hz noise is filtered, else 60Hz(default)
*/
/**************************************************************************/

void MAX31865_Enable50Hz(MAX31865_SpiDriver_Typedef * max31865_SpiDriver, bool b) {
  uint8_t t = MAX31865_ReadRegister8(max31865_SpiDriver, MAX31865_CONFIG_REG);
  if (b) {
    t |= MAX31865_CONFIG_FILT50HZ;
  } else {
    t &= ~MAX31865_CONFIG_FILT50HZ;
  }
  MAX31865_WriteRegister8(max31865_SpiDriver, MAX31865_CONFIG_REG, t);
}

/**************************************************************************/
/*!
    @brief How many wires we have in our RTD setup, can be MAX31865_2WIRE,
    MAX31865_3WIRE, or MAX31865_4WIRE
    @param wires The number of wires in enum format
*/
/**************************************************************************/
void MAX31865_SetWires(MAX31865_SpiDriver_Typedef * max31865_SpiDriver, max31865_numwires_t wires) {
  uint8_t t = MAX31865_ReadRegister8(max31865_SpiDriver, MAX31865_CONFIG_REG);
  if (wires == MAX31865_3WIRE) {
    t |= MAX31865_CONFIG_3WIRE;
  } else {
    // 2 or 4 wire
    t &= ~MAX31865_CONFIG_3WIRE;
  }
  MAX31865_WriteRegister8(max31865_SpiDriver, MAX31865_CONFIG_REG, t);
}

/**************************************************************************/
/*!
    @brief Read the temperature in C from the RTD through calculation of the
    resistance. Uses
   http://www.analog.com/media/en/technical-documentation/application-notes/AN709_0.pdf
   technique
    @param RTDnominal The 'nominal' resistance of the RTD sensor, usually 100
    or 1000
    @param refResistor The value of the matching reference resistor, usually
    430 or 4300
    @returns Temperature in C
*/
/**************************************************************************/
float MAX31865_Temperature(MAX31865_SpiDriver_Typedef * max31865_SpiDriver, float RTDnominal, float refResistor) {
  return PT100_LookupTable[MAX31865_ReadRTD(max31865_SpiDriver)];
  
  float Z1, Z2, Z3, Z4, Rt, temp;
  
  Rt = MAX31865_ReadRTD(max31865_SpiDriver);
  Rt /= 32768;
  Rt *= refResistor;

  // Serial.print("\nResistance: "); Serial.println(Rt, 8);

  Z1 = -RTD_A;
  Z2 = RTD_A * RTD_A - (4 * RTD_B);
  Z3 = (4 * RTD_B) / RTDnominal;
  Z4 = 2 * RTD_B;

  temp = Z2 + (Z3 * Rt);
  temp = (sqrt(temp) + Z1) / Z4;

  if (temp >= 0)
    return temp;

  // ugh.
  Rt /= RTDnominal;
  Rt *= 100; // normalize to 100 ohm

  float rpoly = Rt;

  temp = -242.02;
  temp += 2.2228 * rpoly;
  rpoly *= Rt; // square
  temp += 2.5859e-3 * rpoly;
  rpoly *= Rt; // ^3
  temp -= 4.8260e-6 * rpoly;
  rpoly *= Rt; // ^4
  temp -= 2.8183e-8 * rpoly;
  rpoly *= Rt; // ^5
  temp += 1.5243e-10 * rpoly;

  return temp;
}

/**************************************************************************/
/*!
    @brief Read the raw 16-bit value from the RTD_REG in one shot mode
    @return The raw unsigned 16-bit value, NOT temperature!
*/
/**************************************************************************/
uint16_t MAX31865_ReadRTD(MAX31865_SpiDriver_Typedef * max31865_SpiDriver) {
  
  
  MAX31865_ClearFault(max31865_SpiDriver);
  MAX31865_EnableBias(max31865_SpiDriver, true);
  max31865_SpiDriver->Max31865_Delay(20);
  uint8_t t = MAX31865_ReadRegister8(max31865_SpiDriver, MAX31865_CONFIG_REG);
  t |= MAX31865_CONFIG_1SHOT;
  MAX31865_WriteRegister8(max31865_SpiDriver, MAX31865_CONFIG_REG, t);
  max31865_SpiDriver->Max31865_Delay(75);

  uint16_t rtd = MAX31865_ReadRegister16(max31865_SpiDriver, MAX31865_RTDMSB_REG);

  MAX31865_EnableBias(max31865_SpiDriver, false); // Disable bias current again to reduce selfheating.

  // remove fault
  rtd >>= 1;

  return rtd;
}
