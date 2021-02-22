#ifndef _CX_UTIL_H
#define _CX_UTIL_H

void i2cInit(void);
void i2cScan(void);

void uart2Init(void);
int uart2Send(const char* data);

#endif