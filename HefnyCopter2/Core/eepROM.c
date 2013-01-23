/*
 * eepROM.c
 *
 * Created: 23-Jul-12 3:38:49 PM
 *  Author: M.Hefny
 */ 

#include <avr/io.h>  
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <avr/eeprom.h>
#include <util/atomic.h>
#include <avr/interrupt.h> 
#include <avr/pgmspace.h>


#include "../Include/typedefs.h"
#include "../Include/IO_config.h"
#include "../Include/GlobalValues.h"
#include "../Include/eepROM.h"
#include "../Include/Beeper.h"
 /*
 PI
 PG:50
 PLimit:100
 IGain:25
 ILimit:50
 
 YAW:
 PG:50
 PLimit:20
 IGain:50
 ILimit:10
 
 
 Stick Scaling:
 R:30
 P:30
 Y:50
 Thr:90
 
 */

static config_t const defaultConfig PROGMEM = 
{
	.signature = HEFNYCOPTER2_SIGNATURE,
	.IsCalibrated=0,
	.RX_mode=RX_mode_UARTMode,
	.BoardOrientationMode = QuadFlyingMode_PLUS,
	.QuadFlyingMode = QuadFlyingMode_PLUS,
	.GyroParams[0] = {20,10,0,0,0,20,0},	// pitch roll gyro.
	.GyroParams[1] = {10,50,1,35,-1,20,200},	// Yaw gyro
	.AccParams[0]  = {7,40,1,20,2,50,990},		// ACC_PITCH & Y
	.AccParams[1]  = {20,30,0,0,0,30,600},	// ACC_Z
	.RX_mode = 0, //RX_MODE_CPPM,
	//.Sensor_zero[GYRO_ROLL_Index] = 548,
	//.Sensor_zero[GYRO_PITCH_Index] = 548,
	//.Sensor_zero[GYRO_Z_Index] = 570,
	//.Sensor_zero[ACC_PITCH_Index] = 610,
	//.Sensor_zero[ACC_ROLL_Index] = 610,
	//.Sensor_zero[ACC_Z_Index] = 700,
	//.SelfLevelMode = IMU_SelfLevelMode,
	.AutoDisarm = 3,  // auto disarm
	//.LCDContrast = 32,
	.ThrottleMin = 150,
	.StickScaling = 4,
	.LVA = 0,
	.VoltageAlarm=0, // off		[40 means 4 volt]
	.Acc_Pitch_Trim=0,
	.Acc_Roll_Trim=0,
};


void Initial_EEPROM_Config_Load(void)
{
	// load up last settings from EEPROM
	if(eeprom_read_byte((uint8_t*) EEPROM_DATA_START_POS )!=HEFNYCOPTER2_SIGNATURE)
	{
		Save_Default_Config_to_EEPROM();
		
	} else {
		// read eeprom
		eeprom_read_block(&Config, (void*) EEPROM_DATA_START_POS, sizeof(config_t)); 
	}
}

void Save_Default_Config_to_EEPROM (void)
{
	
	// copy default config to config.
	Set_EEPROM_Default_Config();
	
	
	// Init values.
	for (uint8_t i = 0; i < RXChannels; i++)
	{
		Config.RX_Mid[0][i] = PWM_MID;
		Config.RX_Min[0][i] = PWM_LOW;
		Config.RX_Mid[1][i] = PWM_MID;
		Config.RX_Min[1][i] = PWM_LOW;
	}	
	
	// write to eeProm
	Save_Config_to_EEPROM();
}

void Set_EEPROM_Default_Config(void)
{
	memcpy_P ( &Config, &defaultConfig, sizeof(config_t));
}

void Save_Config_to_EEPROM(void)
{
	// write to eeProm
	cli();
	eeprom_write_block_changes( (const void*) &Config, (void*) EEPROM_DATA_START_POS, sizeof(config_t));	//current_config CONFIG_STRUCT
	sei();
	
	Beeper_Beep(BEEP_LONG,1);	
}


void Load_Config_from_EEPROM(void)
{
	// write to eeProm
	
	eeprom_write_block_changes( (const void*) &Config, (void*) EEPROM_DATA_START_POS, sizeof(config_t));	//current_config CONFIG_STRUCT
	
}

void eeprom_write_block_changes( const uint8_t * src, void * dest, size_t size )
{ 
	size_t len;

	for(len=0;len<size;len++)
	{
		eeprom_write_byte_changed( dest,  *src );

		src++;
		dest++;
	}
}


void eeprom_write_byte_changed( uint8_t * addr, uint8_t value )
{ 
	if(eeprom_read_byte(addr) != value)
	{
		eeprom_write_byte( addr, value );
	}
}
