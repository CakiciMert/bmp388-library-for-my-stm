/*
 * BMP388.c
 *
 *  Created on: Jan 4, 2024
 *      Author: Tremila
 */
#include "BMP388.h"
#include <math.h>
extern I2C_HandleTypeDef hi2c2;

uint8_t BMP388_WIA;

uint32_t raw_pressure, raw_temperature,time;
float altitude, pressure, temperature;
uint8_t bmp388_id;
HAL_StatusTypeDef result;


uint16_t NVM_PAR_T1;
uint16_t NVM_PAR_T2;
int8_t NVM_PAR_T3;
int16_t NVM_PAR_P1;
int16_t NVM_PAR_P2;
int8_t NVM_PAR_P3;
int8_t NVM_PAR_P4;
uint16_t NVM_PAR_P5;
uint16_t NVM_PAR_P6;
int8_t NVM_PAR_P7;
int8_t NVM_PAR_P8;
int16_t NVM_PAR_P9;
int8_t NVM_PAR_P10;
int8_t NVM_PAR_P11;
struct BMP388_calib_data calib_data;

void initialize_calib_data(struct BMP388_calib_data *calib_data) {//// BURDA KESİNLİKLE POW KULLANMADAN YAZIN BEN ÜŞENDİM(BITWISE OPERATIONS BAKIP 2 NIN NIN POWERI ŞEKLİNDE YAZMA VAR)
    calib_data->par_t1 = (float)NVM_PAR_T1 * pow(2, 8);
    calib_data->par_t2 = (float)NVM_PAR_T2 * pow(2, -30);
    calib_data->par_t3 = (float)NVM_PAR_T3 * pow(2, -48);
    calib_data->par_p1 = (float)(NVM_PAR_P1 - pow(2, 14)) * pow(2, -20);
    calib_data->par_p2 = (float)(NVM_PAR_P2 - pow(2, 14)) * pow(2, -29);
    calib_data->par_p3 = (float)NVM_PAR_P3 * pow(2, -32);
    calib_data->par_p4 = (float)NVM_PAR_P4 * pow(2, -37);
    calib_data->par_p5 = (float)NVM_PAR_P5 * pow(2, 3);
    calib_data->par_p6 = (float)NVM_PAR_P6 * pow(2, -6);
    calib_data->par_p7 = (float)NVM_PAR_P7 * pow(2, -8);
    calib_data->par_p8 = (float)NVM_PAR_P8 * pow(2, -15);
    calib_data->par_p9 = (float)NVM_PAR_P9 * pow(2, -48);
    calib_data->par_p10 = (float)NVM_PAR_P10 * pow(2, -48);
    calib_data->par_p11 = (float)NVM_PAR_P11 * pow(2, -65);



}

void TrimParameterReadout()
{
	uint8_t buffer[24];
	HAL_I2C_Mem_Read(BMP388_I2C, BMP388_ADDR << 1, 0x31, 1, buffer, 21, 10);

	NVM_PAR_T1 = (buffer[1] << 8) | buffer[0];
	NVM_PAR_T2 = (buffer[3] << 8) | buffer[2];
	NVM_PAR_T3 =  buffer[4];
	NVM_PAR_P1 = (buffer[6] << 8) | buffer[5];
	NVM_PAR_P2 = (buffer[8] << 8) | buffer[7];
	NVM_PAR_P3 =  buffer[9];
	NVM_PAR_P4 =  buffer[10];
	NVM_PAR_P5 = (buffer[12] << 8) | buffer[11];
	NVM_PAR_P6 = (buffer[14] << 8) | buffer[13];
	NVM_PAR_P7 =  buffer[15];
	NVM_PAR_P8 =  buffer[16];
	NVM_PAR_P9 = (buffer[18] << 8) | buffer[17];
	NVM_PAR_P10 =  buffer[19];
	NVM_PAR_P11 =  buffer[20];

	initialize_calib_data(&calib_data);

}
uint8_t dataToWrite;

void BMP388_Init()
{



	//BMP388_ReadBytes(bmp, CHIP_ID, &chip_id, 1)

	result=HAL_I2C_Mem_Read(BMP388_I2C, BMP388_ADDR << 1, BMP388_ID, I2C_MEMADD_SIZE_8BIT, &bmp388_id, 1, 100);

	if(result != HAL_OK && bmp388_id == BMP388_CHIP_ID)// DEĞİLSE RESET AT sonra eklicem
	{

		//TrimParameterReadout();

	}
	TrimParameterReadout();
	//HAL_I2C_Mem_Read(BMP388_I2C, BMP388_ADDR, BMP388_ID, 1, &BMP388_WIA, 1, 10);

	//dataToWrite=0xB6;
	//HAL_I2C_Mem_Write(BMP388_I2C, BMP388_ADDR, RESET, 1, &dataToWrite, 1, 10);


	///osr phase   temp soldaki
	dataToWrite=0x00;
	dataToWrite = dataToWrite | (BMP388_OVERSAMPLING_2X << 3) | BMP388_OVERSAMPLING_8X;    // temp os si 2  pres os si 8 yaptık
	HAL_I2C_Mem_Write(BMP388_I2C, BMP388_ADDR << 1, OSR, 1, &dataToWrite, 1, 100);




	dataToWrite=0x00;
	dataToWrite = dataToWrite | (BMP388_IIR_FILTER_COEFF_1 << 1); //osrs temperature

	HAL_I2C_Mem_Write(BMP388_I2C, BMP388_ADDR << 1, CONFIG, I2C_MEMADD_SIZE_8BIT, &dataToWrite, 1, 100);

	dataToWrite=0x00;
	dataToWrite = dataToWrite | BMP388_ODR_12_5_HZ;
	HAL_I2C_Mem_Write(BMP388_I2C, BMP388_ADDR << 1, ODR, I2C_MEMADD_SIZE_8BIT, &dataToWrite, 1, 100);

	dataToWrite=0x00;
	dataToWrite = dataToWrite | (BMP388_OVERSAMPLING_8X << 4);// NORMAL MODE ile bunun değeri aynı ondna bu kullanıyom
	dataToWrite = dataToWrite | (1U << 1);//temp open
	dataToWrite = dataToWrite | (1U );//press open
	HAL_I2C_Mem_Write(BMP388_I2C, BMP388_ADDR << 1, PWR_CTRL, I2C_MEMADD_SIZE_8BIT, &dataToWrite, 1, 100);

}





//initialize_calib_data(&calib_data);
//The code below is taken from datasheet (https://www.mouser.com/pdfdocs/BST-BMP388-DS001-01.pdf | BMP388 Data sheet p.54)
float BMP388_compensate_temperature(uint32_t uncomp_temp, struct BMP388_calib_data *calib_data)
{


float partial_data1;
float partial_data2;
partial_data1 = (float)(uncomp_temp - calib_data->par_t1);
partial_data2 = (float)(partial_data1 * calib_data->par_t2);
/* Update the compensated temperature in calib structure since this is
* needed for pressure calculation */
calib_data->t_lin = partial_data2 + (partial_data1 * partial_data1) * calib_data->par_t3;
/* Returns compensated temperature */
return calib_data->t_lin;
}

float BMP388_compensate_pressure(uint32_t uncomp_press, struct BMP388_calib_data *calib_data)
{
/* Variable to store the compensated pressure */
float comp_press;
/* Temporary variables used for compensation */
float partial_data1;
float partial_data2;
float partial_data3;
float partial_data4;
float partial_out1;
float partial_out2;
/* Calibration data */
partial_data1 = calib_data->par_p6 * calib_data->t_lin;
partial_data2 = calib_data->par_p7 * (calib_data->t_lin * calib_data->t_lin);
partial_data3 = calib_data->par_p8 * (calib_data->t_lin * calib_data->t_lin * calib_data->t_lin);
partial_out1 = calib_data->par_p5 + partial_data1 + partial_data2 + partial_data3;
partial_data1 = calib_data->par_p2 * calib_data->t_lin;
partial_data2 = calib_data->par_p3 * (calib_data->t_lin * calib_data->t_lin);
partial_data3 = calib_data->par_p4 * (calib_data->t_lin * calib_data->t_lin * calib_data->t_lin);
partial_out2 = (float)uncomp_press *
(calib_data->par_p1 + partial_data1 + partial_data2 + partial_data3);
partial_data1 = (float)uncomp_press * (float)uncomp_press;
partial_data2 = calib_data->par_p9 + calib_data->par_p10 * calib_data->t_lin;
partial_data3 = partial_data1 * partial_data2;
partial_data4 = partial_data3 + ((float)uncomp_press * (float)uncomp_press * (float)uncomp_press) * calib_data->par_p11;
comp_press = partial_out1 + partial_out2 + partial_data4;
return comp_press;
}
////////////The code up is taken from datasheet (https://www.mouser.com/pdfdocs/BST-BMP388-DS001-01.pdf | BMP388 Data sheet p.55)


// Call this function once after reading the trim parameters to initialize the calibration data


// ... [BMP388_compensate_temperature and BMP388_compensate_pressure functions] ...
uint8_t raw_data[12];
float BMP388_GetData()
{
	//TrimParameterReadout();

    result=HAL_I2C_Mem_Read(BMP388_I2C, BMP388_ADDR<< 1 , DATA_0, 1, raw_data, 6, 10);
    raw_pressure = (uint32_t) raw_data[2] << 16 | (uint32_t)raw_data[1] << 8 | (uint32_t)raw_data[0];
    raw_temperature = (uint32_t)raw_data[5] << 16 | (uint32_t)raw_data[4] << 8 | (uint32_t)raw_data[3];
    //time = (uint32_t)raw_data[10] << 16 | (uint32_t)raw_data[9] << 8 | (uint32_t)raw_data[8];
    TrimParameterReadout();//// bundan emin değilim
    temperature = BMP388_compensate_temperature(raw_temperature, &calib_data);

    pressure = BMP388_compensate_pressure(raw_pressure, &calib_data) / 256.0;


    altitude = Get_Altitude(pressure);
    return altitude;
}

float BMP388_Calibration()// duzgun calibration yazarsınız
{
	int total = 0;


	for(int i=0;i<100;i++)
	{

		total += BMP388_GetData();
	}



	return total/100.0;
}

float Get_Altitude(float press)
{
	return 44307.694*(1.0-pow(press/101325.0,0.1903));/////101325.0 bu sayı ground pressure ayarlıocanız onu
}
