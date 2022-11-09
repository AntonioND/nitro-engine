#ifdef __cplusplus
extern "C" {
#endif

#ifndef __NF_DEFINES_H__
#define __NF_DEFINES_H__





// NightFox LIB - Definiciones General
// Requiere DevkitARM
// Codigo por Cesar Rincon "NightFox"
// http://www.nightfoxandco.com/
// Version 20140413





// Definicion de los datos del usuario
// Informacion obtenida de PALIB source code
// Referencias usadas de http://nds.metawiki.com/Firmware

/*

FW offset*	RAM address	Bytes		Description
0x000020				2			user settings offset / 8 (default 0x7FC0 => 0x3FE00)
0x00002A				2			CRC16 (with initial value 0) of 0x2C with config length
0x00002C				2			config length (0x138)
0x000036				6			MAC address
0x00003C				4			enabled channels
0x000065				1			?
0x000082				1			?
0x000100				1			?
0x000146				14			?
0x000162				1			?
0x03Fx00	0x023FFC80	1			version (5)
0x03Fx02	0x027FFC82	1			favorite color (0-15)
0x03Fx03	0x027FFC83	1			birthday month (1-12)
0x03Fx04	0x027FFC84	1			birthday day (1-31)
0x03Fx06	0x027FFC86	20			name, UTF-16
0x03Fx1A	0x027FFC9A	1/2			length of name in characters
0x03Fx1C	0x027FFC9C	52			message, UTF-16
0x03Fx50	0x027FFCD0	1/2			length of message in characters
0x03Fx52	0x027FFCD2	1			alarm hour
0x03Fx53	0x027FFCD3	1			alarm minute
0x03Fx56	0x027FFCD6	1			0x80=enable alarm, bit 0..6=enable?
			0x027FFCD8	12			touch-screen calibration
			0x027FFCE4	bit 0..2	language
			0x027FFCE4	bit 3		GBA mode screen selection. 0=upper, 1=lower
			0x027FFCE4	bit 6		auto/manual mode. 0=manual, 1=auto
									WIFI power calibration
0x03Fx70				1/2			update counter (used to check latest)
0x03Fx72							CRC16 of 0x03FF00, 0x70 bytes

*/

#define NF_UDATA_COLOR *(u8*)(0x027FFC82)
#define NF_UDATA_BDAY_MONTH *(u8*)(0x027FFC83)
#define NF_UDATA_BDAY_DAY *(u8*)(0x027FFC84)
#define NF_UDATA_ALARM_HOUR *(u8*)(0x027FFCD2)
#define NF_UDATA_ALARM_MINUTE *(u8*)(0x027FFCD3)
#define NF_UDATA_NAME *(u8*)(0x027FFC86)
#define NF_UDATA_NAME_SIZE *(u8*)(0x027FFC9A)
#define NF_UDATA_MSG *(u8*)(0x027FFC9C)
#define NF_UDATA_MSG_SIZE *(u8*)(0x027FFCD0)
#define NF_UDATA_LANG *(u8*)(0x027FFCE4)





#endif

#ifdef __cplusplus
}
#endif
