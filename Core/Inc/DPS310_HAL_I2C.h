/*
 * DPS310_HAL_I2C.h
 *
 *  Created on: Dec 29, 2023
 *      Author: ohya
 */

#ifndef INC_DPS310_HAL_I2C_H_
#define INC_DPS310_HAL_I2C_H_

#include <DPS310.h>

#include "i2c.h"

class DPS310_HAL_I2C: public DPS310 {
public:
	DPS310_HAL_I2C(I2C_HandleTypeDef *hi2c, I2C_ADDRESS address);

	uint8_t readByte(REGISTOR reg);
	uint8_t readByte(uint8_t reg);
	void writeByte(REGISTOR reg, uint8_t value);
	void readBytes(REGISTOR reg, uint8_t *pData, uint8_t size = 1);

	float convertRaw2Pressure(uint8_t rawData[6]);
	float convertRaw2Pressure(std::array<uint8_t, 6> rawData);

private:
	I2C_HandleTypeDef *hi2c;
	uint8_t i2cAddress;
};

#endif /* INC_DPS310_HAL_I2C_H_ */
