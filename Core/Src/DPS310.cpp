/*
 * DPS310.cpp
 *
 *  Created on: Dec 28, 2023
 *      Author: ohya
 */

#include <DPS310.h>

DPS310::DPS310() {
	// TODO Auto-generated constructor stub

}

void DPS310::setIsNewDataReady(bool isReady){

}

float DPS310::getPressure(){
	uint8_t tmp[6];
	readBytes(REGISTOR::PSR_B2, tmp, 6);
	uint32_t rawPressure = (tmp[0]<<16 & 0xff0000) + (tmp[1]<<8&0xff00) + tmp[2];
	uint32_t rawTemperature = (tmp[3]<<16 & 0xff0000) + (tmp[4]<<8&0xff00) + tmp[5];
	rawPressure = getTwosComplement(rawPressure, 24);
	rawTemperature = getTwosComplement(rawTemperature, 24);
	float scaledRawPressure = rawPressure / (float)scaleFactor[(uint8_t)pressureOversamplingRate];
	float scaledRawTemperature = rawTemperature / (float)scaleFactor[(uint8_t)temperatureOversamplingRate];

	return c00 + scaledRawPressure*(c10+scaledRawPressure*(c20+scaledRawPressure*c30))+scaledRawTemperature*(c01 + scaledRawPressure*(c11+scaledRawPressure*c21));
}
float DPS310::getTemperature(){
	uint8_t tmp[3];
	readBytes(REGISTOR::TMP_B2, tmp, 3);
	uint32_t rawTemperature = (tmp[0]<<16 & 0xff0000) + (tmp[1]<<8&0xff00) + tmp[2];
	rawTemperature = getTwosComplement(rawTemperature, 24);
	float scaledRawTemperature = rawTemperature / (float)scaleFactor[(uint8_t)temperatureOversamplingRate];

	return c0*0.5+c1*scaledRawTemperature;
}

void DPS310::setPressureOversamplingRate(OVERSAMPLING_RATE rate){
	uint8_t tmp = readByte(REGISTOR::PRS_CFG);
	tmp = (tmp&0xf0)|((uint8_t)rate);
	writeByte(REGISTOR::PRS_CFG, tmp);
	pressureOversamplingRate = rate;
	if(rate>=OVERSAMPLING_RATE::RATE_16 && !isPressureResultShift){
		enablePressureResultBitShift();
	}
}
void DPS310::setPressureMesurmentRate(MESURMENT_RATE rate){
	uint8_t tmp = readByte(REGISTOR::PRS_CFG);
	tmp = (tmp&0x0f)|((uint8_t)rate<<4);
	writeByte(REGISTOR::PRS_CFG, tmp);
}

void DPS310::useInternalTemperatureSensor(bool isInternal){
	uint8_t tmp = readByte(REGISTOR::TMP_CFG);
	tmp = setBit(tmp, 7, !isInternal);
	writeByte(REGISTOR::TMP_CFG, tmp);
}
void DPS310::setTemperatureOversamplingRate(OVERSAMPLING_RATE rate){
	uint8_t tmp = readByte(REGISTOR::TMP_CFG);
	tmp = (tmp&0xf0)|((uint8_t)rate);
	writeByte(REGISTOR::TMP_CFG, tmp);
	pressureOversamplingRate = rate;
	if(rate>=OVERSAMPLING_RATE::RATE_16 && !isPressureResultShift){
		enablePressureResultBitShift();
	}
}
void DPS310::setTemperatureMesurmentRate(MESURMENT_RATE rate){
	uint8_t tmp = readByte(REGISTOR::TMP_CFG);
	tmp = (tmp&0b10001111)|((uint8_t)rate<<4);
	writeByte(REGISTOR::TMP_CFG, tmp);
}


//For MEAS_CFG
bool DPS310::isCoefficientsAvailable(){
	return readByte(REGISTOR::MEAS_CFG)>>7 & 1;
}

bool DPS310::isSensorInitialized(){
	return readByte(REGISTOR::MEAS_CFG)>>6 & 1;
}
bool DPS310::isPressureReady(){
	return readByte(REGISTOR::MEAS_CFG)>>5 & 1;
}
bool DPS310::isTemperatureReady(){
	return readByte(REGISTOR::MEAS_CFG)>>4 & 1;
}
void DPS310::setMesurmentMode(MODE mode){
	writeByte(REGISTOR::MEAS_CFG, (uint8_t)mode);
}

//configure the CFG_REG
void DPS310::setInteruptActiveLevel(bool isActiveHigh){
	uint8_t tmp = readByte(REGISTOR::CFG_REG);
	tmp = setBit(tmp,7, isActiveHigh);
	writeByte(REGISTOR::CFG_REG, tmp);
}
void DPS310::enableFIFOfullIT(bool isEnable){
	uint8_t tmp = readByte(REGISTOR::CFG_REG);
	tmp = setBit(tmp,7, isEnable);
	writeByte(REGISTOR::CFG_REG, tmp);
}
void DPS310::enableTemperatureMeasurementCpltIT(bool isEnable){
	uint8_t tmp = readByte(REGISTOR::CFG_REG);
	tmp = setBit(tmp,5, isEnable);
	writeByte(REGISTOR::CFG_REG, tmp);
}
void DPS310::enablePressureMeasurementCpltIT(bool isEnable){
	uint8_t tmp = readByte(REGISTOR::CFG_REG);
	tmp = setBit(tmp,4, isEnable);
	writeByte(REGISTOR::CFG_REG, tmp);

}
void DPS310::enableTemperatureResultBitShift(bool enableShift){
	uint8_t tmp = readByte(REGISTOR::CFG_REG);
	tmp = setBit(tmp,3, enableShift);
	writeByte(REGISTOR::CFG_REG, tmp);
	isTemperatureResultShift = enableShift;
}
void DPS310::enablePressureResultBitShift(bool enableShift){
	uint8_t tmp = readByte(REGISTOR::CFG_REG);
	tmp = setBit(tmp,2, enableShift);
	writeByte(REGISTOR::CFG_REG, tmp);
	isPressureResultShift = enableShift;
}
void DPS310::enableFIFO(bool isEnable){
	uint8_t tmp = readByte(REGISTOR::CFG_REG);
	tmp = setBit(tmp,1, isEnable);
	writeByte(REGISTOR::CFG_REG, tmp);
}
/*
 * set SPI mode
 * @param1 arg:
 * 	0 for 4-wire interface
 *  1 for 3-wire interface
 */
void DPS310::setSPIMode(uint8_t arg){
	if(arg > 1){
		return;
	}

	uint8_t tmp = readByte(REGISTOR::CFG_REG);
	tmp = setBit(tmp,0, arg==0);
	writeByte(REGISTOR::CFG_REG, tmp);
}

uint8_t DPS310::getInterruptStatus(){
	return readByte(REGISTOR::INT_STS);
}
uint8_t DPS310::getFIFOStatus(){
	return readByte(REGISTOR::FIFO_STS);
}
bool DPS310::isFIFOFull(){
	return getFIFOStatus() & (uint8_t)FIFO_STATUS::IS_FULL;
}
bool DPS310::isFIFOEmpty(){
	return getFIFOStatus() & (uint8_t)FIFO_STATUS::IS_EMPTY;
}


void DPS310::clearFIFOFlush(){
	writeByte(REGISTOR::RESET, 1<<7);
}
void DPS310::reset(){
	writeByte(REGISTOR::RESET, 0b1001);
}

void DPS310::updateCoefficient(){
	uint8_t tmp[18] = {};
	readBytes(REGISTOR::COFE, tmp, 18);
	c0 = (tmp[0]<<4 & 0xff0) + (tmp[1]>>4 & 0x0f);
	c1 = (tmp[1]<<8 & 0xf00) + (tmp[2]);

	c00 = (tmp[3]<<12 & 0xff000) + (tmp[4]<<4 & 0xff0) + (tmp[5]>>4 & 0x0f);
	c10 = (tmp[5]<<16 & 0xf0000) + (tmp[6]<<8 & 0xff00) + tmp[7];
	c01 = (tmp[8]<<8 & 0xff00) + (tmp[9]);
	c11 = (tmp[10]<<8 & 0xff00) + (tmp[11]);
	c20 = (tmp[12]<<8 & 0xff00) + (tmp[14]);
	c21 = (tmp[14]<<8 & 0xff00) + (tmp[15]);
	c30 = (tmp[16]<<8 & 0xff00) + (tmp[17]);

	c0 = getTwosComplement(c0,12);
	c1 = getTwosComplement(c1,12);
	c00 = getTwosComplement(c00,20);
	c01 = getTwosComplement(c01,20);
	c11 = getTwosComplement(c11,16);
	c20 = getTwosComplement(c20,16);
	c21 = getTwosComplement(c21,16);
	c30 = getTwosComplement(c30,16);
}

bool DPS310::isCoeffiTempSrcInternal(){
	return !(readByte(REGISTOR::COFE_SRCE)>>7);
}

uint8_t DPS310::setBit(uint8_t arg, uint8_t position, bool value){
	if(value){
		return arg |= 1<<position;
	}else{
		return arg &= ~(1<<position);
	}
}
uint8_t DPS310::resetBit(uint8_t arg, uint8_t position){
	return arg &= ~(1<<position);
}

uint32_t DPS310::getTwosComplement(uint32_t value, uint8_t length){
	if (value & ((uint32_t)1 << (length - 1)))
	{
		return value - ((uint32_t)1 << length);
	}
	return value;

//	return value - ((uint32_t)1<<length);
}

