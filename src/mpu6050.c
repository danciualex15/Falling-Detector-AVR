#include "mpu6050.h"
#define MPU6050_ADDR 0x68
#define PWR_MGMT_1   0x6B
#define ACCEL_CONFIG 0x1C

void mpu6050_init(void) {
    twi_start();
    twi_write(MPU6050_ADDR << 1); // write Address (0xD0)
    twi_write(0x6B);              // PWR_MGMT_1 register
    twi_write(0x00);              // write 0 to wake up
    twi_stop();
}

void MPU6050_Init_FallDetect(void) {
    twi_start();
    twi_write(MPU6050_ADDR << 1); // device Address + Write (0xD0)
    twi_write(0x6B);              // point to PWR_MGMT_1 register
    twi_write(0x00);              // write 0x00 to wake it up
    twi_stop();                   // end transaction

    twi_start();                  // start a NEW transaction
    twi_write(MPU6050_ADDR << 1); // device Address + Write (0xD0)
    twi_write(ACCEL_CONFIG);      // point to ACCEL_CONFIG register (0x1C)
    twi_write(0x10);              // write 0x10 to set +/- 8g range
    twi_stop();                   // end transaction
}

uint8_t mpu6050_get_accel(int16_t *ax, int16_t *ay, int16_t *az) {
    uint8_t h, l;
    if (twi_start_timeout() == 1) return 1;
    if (twi_write(MPU6050_ADDR << 1) == 1) return 1;
    if (twi_write(0x3B) == 1) return 1;

    twi_start();
    twi_write(MPU6050_ADDR << 1);
    twi_write(0x3B);
    
    twi_start();
    twi_write((MPU6050_ADDR << 1) | 1);

    twi_read_ack(&h); twi_read_ack(&l);
    *ax = (h << 8) | l;

    twi_read_ack(&h); twi_read_ack(&l);
    *ay = (h << 8) | l;

    twi_read_ack(&h); twi_read_nack(&l);
    *az = (h << 8) | l;

    twi_stop();

    return 0;
}