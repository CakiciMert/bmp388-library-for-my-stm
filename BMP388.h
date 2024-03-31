/*
 * BMP388.h
 *
 *  Created on: Jan 4, 2024
 *      Author: Tremila
 */

#ifndef INC_BMP388_H_
#define INC_BMP388_H_

#include "stm32f4xx_hal.h"

#define BMP388_I2C 	   &hi2c1

#define BMP388_ADDR    (0x76) //sdo gnd



///////REGISTER ADRESSSES
#define BMP388_ID (0x00)
#define ERR_REG (0x02)
#define STATUS 	(0x03)
#define DATA_0 	   0x04 //8 bit
#define DATA_1 	   0x05 //8 bit
#define DATA_2     0x06 //4 bit
#define DATA_3 	   0x07 //8 bit
#define DATA_4 	   0x08 //8 bit
#define DATA_5 	   0x09 //4 bit
#define SENSORTIME_0 (0x0C)
#define SENSORTIME_1 (0x0D)
#define SENSORTIME_2 (0x0E)
#define EVENT (0x10)
#define INT_STATUS (0x11)
#define FIFO_LENGTH_1 (0x12)
#define FIFO_LENGTH_2 (0x13)
#define FIFO_DATA (0x14)
#define FIFO_WTM_0 (0x15)
#define FIFO_WTM_1 (0x16)
#define FIFO_CONFIG_1 (0x17)
#define FIFO_CONFIG_2 (0x18)
#define INT_CTRL (0x19)
#define PWR_CTRL (0x1B)
#define IF_CONF (0x1A)
#define OSR (0x1C)
#define ODR (0x1D)
#define CONFIG (0x1F)
#define CMD (0x7E)

/////////////////////////////////////////////////////////////////////////////////////////
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"


/* Declarations and definitions ----------------------------------------------*/

#define BMP388_CHIP_ID				0x50

// BMP 388 commands
#define BMP388_CMD_RDY				0x10
#define BMP388_SOFTRESET			0xB6

// Over sampling macros
#define BMP388_NO_OVERSAMPLING		0b00000000
#define BMP388_OVERSAMPLING_2X      0b00000001
#define BMP388_OVERSAMPLING_4X      0b00000010
#define BMP388_OVERSAMPLING_8X      0b00000011
#define BMP388_OVERSAMPLING_16X     0b00000100
#define BMP388_OVERSAMPLING_32X     0b00000101

// Filter setting macros
#define BMP388_IIR_FILTER_DISABLE     0b00000000
#define BMP388_IIR_FILTER_COEFF_1     0b00000001
#define BMP388_IIR_FILTER_COEFF_3     0b00000010
#define BMP388_IIR_FILTER_COEFF_7     0b00000011
#define BMP388_IIR_FILTER_COEFF_15    0b00000100
#define BMP388_IIR_FILTER_COEFF_31    0b00000101
#define BMP388_IIR_FILTER_COEFF_63    0b00000110
#define BMP388_IIR_FILTER_COEFF_127   0b00000111

// output data rate macros
#define BMP388_ODR_200_HZ             0b00000000
#define BMP388_ODR_100_HZ             0b00000001
#define BMP388_ODR_50_HZ              0b00000010
#define BMP388_ODR_25_HZ              0b00000011
#define BMP388_ODR_12_5_HZ            0b00000100
#define BMP388_ODR_6_25_HZ            0b00000101
#define BMP388_ODR_3_1_HZ             0b00000110
#define BMP388_ODR_1_5_HZ             0b00000111
#define BMP388_ODR_0_78_HZ            0b00001000
#define BMP388_ODR_0_39_HZ            0b00001001
#define BMP388_ODR_0_2_HZ             0b00001010
#define BMP388_ODR_0_1_HZ             0b00001011
#define BMP388_ODR_0_05_HZ            0b00001100
#define BMP388_ODR_0_02_HZ            0b00001101
#define BMP388_ODR_0_01_HZ            0b00001110
#define BMP388_ODR_0_006_HZ           0b00001111
#define BMP388_ODR_0_003_HZ           0b00010000
#define BMP388_ODR_0_001_HZ           0b00010001

#define BMP388_CALIBDATA_LEN	21


#endif /* INC_BMP388_H_ */

struct BMP388_calib_data {
    float par_t1;
    float par_t2;
    float par_t3;
    float par_p1;
    float par_p2;
    float par_p3;
    float par_p4;
    float par_p5;
    float par_p6;
    float par_p7;
    float par_p8;
    float par_p9;
    float par_p10;
    float par_p11;

    float t_lin;
};


void initialize_calib_data(struct BMP388_calib_data *calib_data);




void TrimParameterReadout();

void BMP388_Init();

float BMP388_compensate_temperature(uint32_t uncomp_temp, struct BMP388_calib_data *calib_data);

float BMP388_compensate_pressure(uint32_t uncomp_press, struct BMP388_calib_data *calib_data);

float BMP388_Calibration();

float Get_Altitude(float press);






