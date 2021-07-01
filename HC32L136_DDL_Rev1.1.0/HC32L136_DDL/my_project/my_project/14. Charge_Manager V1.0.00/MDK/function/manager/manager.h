#ifndef __MANAGER_H
#define __MANAGER_H
#include "ddl.h"

typedef enum {
    out_5v  = 1,
    out_9v  = 2,
    out_12v = 3,
    out_20v = 4,
    qc3_0   = 5,
} charge_mode_t;

void charge_mode_select (charge_mode_t mode);

void volt_inc_200mv (void) ;

void volt_dec_200mv (void);

#endif

