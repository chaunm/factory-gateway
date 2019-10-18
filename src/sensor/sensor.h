/*
 * sensor.h
 *
 *  Created on: Oct 14, 2019
 *      Author: chaunm
 */

#ifndef SENSOR_H_
#define SENSOR_H_

#include "typesdef.h"
#define NUMBER_OF_SENSORS		30

enum {
	TEMP_HIGH_REG = 16,
	TEMP_LOW_REG,
	HUMI_HIGH_REG,
	HUMI_LOW_REG,
	ALARM_REG,
	NUMBER_OF_REGS
};

enum {
	MIN_TEMP_PARAM = 1,
	MAX_TEMP_PARAM,
	MIN_HUMI_PARAM,
	MAX_HUMI_PARAM,
	NUMBER_OF_PARAMS
};

typedef enum {
	TYPE_TEMP = 0,
	TYPE_HUMI
} sensor_t;

typedef struct tagSENSORREG {
	BYTE reg;
	BYTE value;
}SENSORREG, *PSENSORREG;

typedef struct tagSENSOR {
	WORD address;
	WORD Humi;
	WORD Temp;
	BYTE timeout;
	BYTE regs[NUMBER_OF_REGS];
	WORD params[NUMBER_OF_PARAMS];
} SENSOR, *PSENSOR;

void SensorInit();
VOID SensorUpdateReg(WORD address, BYTE reg, BYTE value);
VOID SensorUpdateParam(WORD address, WORD param, WORD value);
WORD GetSensorAddress(BYTE nIndex);
VOID SensorSendStates(WORD address);
VOID SensorSendSingleState(WORD address, sensor_t type);

#endif /* SENSOR_H_ */
