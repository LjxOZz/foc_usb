#ifndef _FOC_H_
#define _FOC_H_

#include "main.h"

#define sqrt3 		1.7320508f


void Svpwm_ABC(float Vd, float Vq, float theta, float Udc, uint16_t Tpwm);

#endif

