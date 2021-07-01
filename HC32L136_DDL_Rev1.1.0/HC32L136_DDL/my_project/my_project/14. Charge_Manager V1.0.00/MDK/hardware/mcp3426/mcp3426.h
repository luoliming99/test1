#ifndef __MCP3426_H
#define __MCP3426_H
#include "ddl.h"

void mcp3426_init (void);

int16_t mcp3426_read_adc (uint8_t chan);

#endif

