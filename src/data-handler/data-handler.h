/*
 * data-handler.h
 *
 *  Created on: Oct 13, 2019
 *      Author: chaunm
 */

#ifndef DATA_HANDLER_H_
#define DATA_HANDLER_H_
#include "uart-data.h"

void HandleRequestConfig(const char* thingId, const char* param, WORD value);

#endif /* DATA_HANDLER_H_ */
