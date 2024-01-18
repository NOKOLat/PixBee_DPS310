/*
 * DPS310.h
 *
 *  Created on: Dec 28, 2023
 *      Author: ohya
 */

#ifndef INC_DPS310_H_
#define INC_DPS310_H_

#include <stdint.h>
#include <array>

class DPS310 {
public:
	enum class REGISTOR{
		PSR_B2,
		PSR_B1,
		PSR_B0,
		TMP_B2,
		TMP_B1,
		TMP_B0,
		PRS_CFG,
		TMP_CFG,
		MEAS_CFG,
		CFG_REG,
		INT_STS,
		FIFO_STS,
		RESET,
		ProductID,
		COFE = 0x10,
		COFE_SRCE = 0x28,
	};

	//RATE_x mean x measurements per second.
	enum class MESURMENT_RATE{
		RATE_1,
		RATE_2,
		RATE_4,
		RATE_8,
		RATE_16,
		RATE_32,
		RATE_64,
		RATE_128,
	};

	enum class OVERSAMPLING_RATE{
		RATE_1,
		RATE_2,
		RATE_4,
		RATE_8,
		RATE_16,
		RATE_32,
		RATE_64,
		RATE_128,
	};

	enum class MODE{
		STANDBY,
		COMMAND_PRESSURE,
		COMMAND_TEMPERATURE,
		CONTINOUS_PRESSURE = 0b101,
		CONTINOUS_TEMPERATURE,
		CONTINOUS_PRESSURE_TEMPERATURE
	};

	enum class INTERRUPT_STATUS{
		PRESSURE = 1,
		TEMEPRATURE = 2,
		FIFO = 4,
	};

	enum class FIFO_STATUS{
		IS_FULL = 2,
		IS_EMPTY = 1,
	};

	enum class I2C_ADDRESS{
		HIGH = 0x77,
		LOW = 0x76
	};
	DPS310();

	void setIsNewDataReady(bool isReady = true);
	float getPressure();
	float getTemperature();

	void setPressureOversamplingRate(OVERSAMPLING_RATE rate);
	void setPressureMesurmentRate(MESURMENT_RATE rate);

	void useInternalTemperatureSensor(bool isInternal = true);
	void setTemperatureOversamplingRate(OVERSAMPLING_RATE rate);
	void setTemperatureMesurmentRate(MESURMENT_RATE rate);

	bool isFIFOFull();
	bool isFIFOEmpty();

	//For MEAS_CFG
	bool isCoefficientsAvailable();
	bool isSensorInitialized();
	bool isPressureReady();
	bool isTemperatureReady();
	void setMesurmentMode(MODE mode);

	//configure the CFG_REG
	void setInteruptActiveLevel(bool isActiveHigh = true);
	void enableFIFOfullIT(bool isEnable = true);
	void enableTemperatureMeasurementCpltIT(bool isEnable = true);
	void enablePressureMeasurementCpltIT(bool isEnable = true);
	void enableTemperatureResultBitShift(bool enableShift = true);
	void enablePressureResultBitShift(bool enableShift = true);
	void enableFIFO(bool isEnable = true);
	/*
	 * set SPI mode
	 * @param1 arg:
	 * 	0 for 4-wire interface
	 *  1 for 3-wire interface
	 */
	void setSPIMode(uint8_t arg = 0);

	uint8_t getInterruptStatus();
	uint8_t getFIFOStatus();

	void clearFIFOFlush();
	void reset();

	void updateCoefficient();

	bool isCoeffiTempSrcInternal();

	virtual uint8_t readByte(REGISTOR reg){return 0;}
	virtual void writeByte(REGISTOR reg, uint8_t value){}
	virtual void readBytes(REGISTOR reg, uint8_t *pData, uint8_t size = 1){}

protected:
	uint16_t c0;
	uint16_t c1;
	uint32_t c00;
	uint32_t c10;
	uint16_t c01;
	uint16_t c11;
	uint16_t c20;
	uint16_t c21;
	uint16_t c30;

	bool isPressureResultShift = false;
	bool isTemperatureResultShift = false;
	OVERSAMPLING_RATE pressureOversamplingRate = OVERSAMPLING_RATE::RATE_1;
	OVERSAMPLING_RATE temperatureOversamplingRate = OVERSAMPLING_RATE::RATE_1;

	const std::array<uint32_t, 8> scaleFactor = {
			524288,
			1572864,
			3670016,
			7864320,
			253952,
			516096,
			1040384,
			2088960
	};

	uint8_t setBit(uint8_t arg, uint8_t position, bool value = true);
	uint8_t resetBit(uint8_t arg, uint8_t position);

	uint32_t getTwosComplement(uint32_t value, uint8_t length);

};

#endif /* INC_DPS310_H_ */
