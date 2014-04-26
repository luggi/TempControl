/*
 * cli.h
 *
 *  Created on: 6 Apr 2014
 *      Author: Hydra
 */

#ifndef CLI_H_
#define CLI_H_

extern uint8_t cliMode;

void cliProcess(void);
char *ftoa(float x, char *floatString);

#endif                          /* CLI_H_ */
