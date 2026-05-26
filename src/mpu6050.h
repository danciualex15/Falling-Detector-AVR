#include <avr/io.h>
#include "twi.h"
#include <stdio.h>

void mpu6050_init(void);
uint8_t mpu6050_get_accel(int16_t *ax, int16_t *ay, int16_t *az);
void MPU6050_Init_FallDetect(void);