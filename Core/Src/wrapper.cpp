/*
 * wrapper.cpp
 *
 *  Created on: Oct 10, 2023
 *      Author: ohya
 */

#include "wrapper.hpp"
/* Pre-Processor Begin */
#include <string>

#include "DPS310/DPS310_HAL_I2C.h"

#include <usart.h>
#include <gpio.h>
/* Pre-Processor End */

/* Enum, Struct Begin */

/* Enum, Struct End */

/* Function Prototype Begin */
void print(std::string str);
DPS310_HAL_I2C dps310(&hi2c2,DPS310::I2C_ADDRESS::HIGH);
/* Function Prototype End */

/* Variable Begin */

/* Variable End */

void init(void){
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOH, GPIO_PIN_1, GPIO_PIN_SET);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);

	print("\n<<<start DPS310 test>>>\n");
//	dps310.reset();
	uint8_t productID = 0;
	while(productID==0){
		HAL_Delay(100);
		productID = dps310.readByte(DPS310::REGISTOR::ProductID);
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
	}
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);

	print("product id : " + std::to_string(productID));
	dps310.useInternalTemperatureSensor(dps310.isCoeffiTempSrcInternal());
	dps310.setTemperatureMesurmentRate(DPS310::MESURMENT_RATE::RATE_128);
	dps310.setTemperatureOversamplingRate(DPS310::OVERSAMPLING_RATE::RATE_1);
	dps310.setPressureMesurmentRate(DPS310::MESURMENT_RATE::RATE_128);
	dps310.setPressureOversamplingRate(DPS310::OVERSAMPLING_RATE::RATE_1);
	while(!dps310.isCoefficientsAvailable());
	dps310.updateCoefficient();

	dps310.setMesurmentMode(DPS310::MODE::CONTINOUS_PRESSURE_TEMPERATURE);

	print("----register check----\n");
	for(uint8_t n=0; n<0x0D; n++){
		uint8_t tmp = dps310.readByte((DPS310::REGISTOR)n);
		print("register " + std::to_string(n) + " : " + std::to_string(tmp));
		if(hi2c2.ErrorCode){
			print("i2c error code"+std::to_string(hi2c2.ErrorCode));
		}
	}
	print("///end init()///");

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);
}

void loop(void){
	if(dps310.isTemperatureReady()){
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
		float pressure = dps310.getTemperature();
		uint32_t pressureInt = pressure;
		print("temperature : " + std::to_string(pressureInt) + "c\n");
	}
//	if(dps310.isPressureReady()){
//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
//		float pressure = dps310.getPressure();
//		uint32_t pressureInt = pressure/100;
//		print("pressure : " + std::to_string(pressureInt) + "hPa");
//	}
}

void print(std::string str){
	str += "\r\n";
	HAL_UART_Transmit(&huart3, (uint8_t*)str.c_str(), str.size(), 100);
}

/* Function Body Begin */
