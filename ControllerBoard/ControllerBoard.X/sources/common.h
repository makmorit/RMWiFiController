#ifndef __COMMON_H
#define __COMMON_H

#include <xc.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//
// for __delay_ms, __delay_us
//
#include <htc.h>

#ifndef _XTAL_FREQ
    #define MHz 000000
    #define _XTAL_FREQ 20MHz
#endif

#endif // __COMMON_H
