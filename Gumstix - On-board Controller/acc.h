#ifndef _ACC_H
#define _ACC_H

#define FILENAME "/dev/i2c-3"
#define ADDRESSBUS 29
#define XREG 41
#define YREG 43
#define ZREG 45

/* smbus_access read or write markers */
#define I2C_SMBUS_READ	1
/* SMBus transaction types (size parameter in the above functions) 
   Note: these no longer correspond to the (arbitrary) PIIX4 internal codes! */
#define I2C_SMBUS_BYTE_DATA	    2
/* ----- commands for the ioctl like i2c_command call:
 * note that additional calls are defined in the algorithm and hw 
 *	dependent layers - these can be listed here, or see the 
 *	corresponding header files.
 */
#define I2C_FUNCS	0x0705	/* Get the adapter functionality */
#define I2C_SLAVE_FORCE	0x0706	/* Change slave address			*/
				/* Attn.: Slave address is 7 or 10 bits */
				/* This changes the address, even if it */
				/* is already taken!			*/

#define I2C_SMBUS	0x0720	/* SMBus-level access */

/* 
 * Data for SMBus Messages 
 */
#define I2C_SMBUS_BLOCK_MAX	32	/* As specified in SMBus standard */	
#define I2C_SMBUS_I2C_BLOCK_MAX	32	/* Not specified but we use same structure */
union i2c_smbus_data {
	__u8 byte;
	__u16 word;
	__u8 block[I2C_SMBUS_BLOCK_MAX + 2]; /* block[0] is used for length */
	                                            /* and one more for PEC */
};
/* This is the structure as used in the I2C_SMBUS ioctl call */
struct i2c_smbus_ioctl_data {
	char read_write;
	__u8 command;
	int size;
	union i2c_smbus_data *data;
};

// Funcs
void init_acc ();
void get_acc (int *x, int *y);
void get_clean_acc(int *x, int *y);
void calibrate();
void movement_end_check();
void get_pos(int *x, int *y, int *x_acc, int *y_acc);

#endif //_ACC_H
