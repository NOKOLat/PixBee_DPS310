/*
 * DPS310_HAL_I2C.cpp
 *
 *  Created on: Dec 29, 2023
 *      Author: ohya
 */

#include <DPS310_HAL_I2C.h>

DPS310_HAL_I2C::DPS310_HAL_I2C(I2C_HandleTypeDef *hi2c, I2C_ADDRESS address)
:hi2c(hi2c),i2cAddress((uint8_t)address){
	DPS310();
}

uint8_t DPS310_HAL_I2C::readByte(REGISTOR reg){
	uint8_t res = 0;
	HAL_I2C_Mem_Read(hi2c, i2cAddress<<1, (uint8_t)reg, 1, &res, 1, 100);
	return res;
}

uint8_t DPS310_HAL_I2C::readByte(uint8_t reg){
	uint8_t res = 0;
	HAL_I2C_Mem_Read(hi2c, i2cAddress<<1, (uint8_t)reg, 1, &res, 1, 100);
	return res;
}


void DPS310_HAL_I2C::writeByte(REGISTOR reg, uint8_t value){
	HAL_I2C_Mem_Write(hi2c, i2cAddress<<1, (uint8_t)reg, 1, &value, 1, 100);
}

void DPS310_HAL_I2C::readBytes(REGISTOR reg, uint8_t *pData, uint8_t size){
	HAL_I2C_Mem_Read(hi2c, i2cAddress<<1, (uint8_t)reg, 1, pData, size, 1000);
}

float DPS310_HAL_I2C::convertRaw2Pressure(uint8_t rawData[6]){
	uint8_t *tmp = rawData;
	uint32_t rawPressure = (tmp[0]<<16 & 0xff0000) + (tmp[1]<<8&0xff00) + tmp[2];
	uint32_t rawTemperature = (tmp[3]<<16 & 0xff0000) + (tmp[4]<<8&0xff00) + tmp[5];
	float scaledRawPressure = rawPressure / (float)scaleFactor[(uint8_t)pressureOversamplingRate];
	float scaledRawTemperature = rawTemperature / (float)scaleFactor[(uint8_t)temperatureOversamplingRate];

	return c00 + scaledRawPressure*(c01+scaledRawPressure*(c20+scaledRawPressure*c30))+scaledRawTemperature*c01 + scaledRawTemperature*scaledRawPressure*(c11+scaledRawPressure*c21);
}
float DPS310_HAL_I2C::convertRaw2Pressure(std::array<uint8_t, 6> rawPressure){
	return convertRaw2Pressure(rawPressure.data());
}
