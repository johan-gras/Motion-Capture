#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/types.h>
#include "acc.h"

#define DISCRIMINATION_WINDOWS 3
#define COUNT_NULL 25

int acc_x[2] = {0}, acc_y[2] = {0};
int vel_x[2] = {0}, vel_y[2] = {0}, pos_x[2] = {0}, pos_y[2] = {0};
int count_null_x = 0, count_null_y = 0;
int reference_x = 0, reference_y = 0;

union i2c_smbus_data data;
struct i2c_smbus_ioctl_data argsX, argsY, argsZ;
unsigned long funcs;
int file;

// Accelerometer init
void init_acc (){
	printf("Accelerometer Init...\n");

	// Variable Init
	argsX.read_write = I2C_SMBUS_READ;
	argsX.size = I2C_SMBUS_BYTE_DATA;
	argsX.data = &data;
	argsX.command = XREG;

	argsY.read_write = I2C_SMBUS_READ;
	argsY.size = I2C_SMBUS_BYTE_DATA;
	argsY.data = &data;
	argsY.command = YREG;

	argsZ.read_write = I2C_SMBUS_READ;
	argsZ.size = I2C_SMBUS_BYTE_DATA;
	argsZ.data = &data;
	argsZ.command = ZREG;

	// Calibration
	calibrate();
}

// Calibration
void calibrate() 
{
	unsigned int count = 0;
	int x, y;
	printf("Calibration...\n");

	do{  
		get_acc(&x, &y);
		reference_x += x;
		reference_y += y;
		count++;
	} while(count!=0x0400);

	reference_x=reference_x>>10;
	reference_y=reference_y>>10;

	printf("Ref_x : %d         Ref_y : %d\n", reference_x, reference_y);
}

// Get accelerometer values from the i2c bus
void get_acc (int *x, int *y)
{
	file = open(FILENAME, O_RDWR);
	ioctl(file, I2C_FUNCS, &funcs);
	ioctl(file, I2C_SLAVE_FORCE, ADDRESSBUS);

	ioctl(file,I2C_SMBUS,&argsX);
	*x = (int8_t)data.byte;

	ioctl(file,I2C_SMBUS,&argsY);
	*y = (int8_t)data.byte;

	close(file);
}

// Get cleaner accelerations values
void get_clean_acc(int *x, int *y){
	unsigned char count = 0;
	int sample_x, sample_y;
    
    // Averaging
    do {
	    get_acc(&sample_x, &sample_y);
	    *x += sample_x;
	    *y += sample_y;
	    count++;                                       
    } while (count!=0x40);
    *x = *x>>6;
    *y = *y>>6;

    // Eliminating zero reference
    *x -= reference_x;
    *y -= reference_y;

    // Discrimination window
    if (*x <= DISCRIMINATION_WINDOWS && *x >= -DISCRIMINATION_WINDOWS)
    	*x = 0;
    if (*y <= DISCRIMINATION_WINDOWS && *y >= -DISCRIMINATION_WINDOWS)
    	*y = 0;
}

// Force the velocity to zero if no movement
void movement_end_check(){
	if (acc_x[1] == 0)
		count_null_x++;
	else
		count_null_x = 0;

	if (acc_y[1] == 0)
		count_null_y++;
	else
		count_null_y = 0;

	if (count_null_x >= COUNT_NULL){
		vel_x[0] = 0;
		vel_x[1] = 0;
	}

	if (count_null_y >= COUNT_NULL){
		vel_y[0] = 0;
		vel_y[1] = 0;
	}
}

// Calculate the relative position
void get_pos(int *x, int *y, int *x_acc, int *y_acc){
	// Get clean acc values
    get_clean_acc(&acc_x[1], &acc_y[1]);

    // Double integration
    vel_x[1] = vel_x[0] + acc_x[0] + ((acc_x[1] - acc_x[0])>>1);
    vel_y[1] = vel_y[0] + acc_y[0] + ((acc_y[1] - acc_y[0])>>1);

    pos_x[1] = pos_x[0] + vel_x[0] + ((vel_x[1] - vel_x[0])>>1);
    pos_y[1] = pos_y[0] + vel_y[0] + ((vel_y[1] - vel_y[0])>>1);

    // New is old
    acc_x[0] = acc_x[1];
    acc_y[0] = acc_y[1];
    vel_x[0] = vel_x[1];
    vel_y[0] = vel_y[1];
    pos_x[0] = pos_x[1];
    pos_y[0] = pos_y[1];

    // Return data
    *x = pos_x[0];
    *y = pos_y[0];
    *x_acc = acc_x[0];
    *y_acc = acc_y[0];

    movement_end_check();    
}
