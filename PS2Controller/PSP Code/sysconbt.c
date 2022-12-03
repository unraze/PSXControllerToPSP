/*
	PSP SYSCON battery override example

	1.override battery serial code

*/
#include <pspkernel.h>
#include <pspdebug.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "syscon.h"

#define OVERRIDE_BATTERY_SERIAL 0x44556677

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
const char *syscon_battery(sceSysconPacket *packet)
{
	switch(packet->tx_cmd-0x60)
	{
	case 0x00:	return "pspSysconBatteryNop";
	case 0x01:	return "pspSysconBatteryGetStatusCap";
	case 0x02:	return "pspSysconBatteryGetTemp";
	case 0x03:	return "pspSysconBatteryGetVolt";
	case 0x04:	return "pspSysconBatteryGetElec";
	case 0x05:	return "pspSysconBatteryGetRCap";
	case 0x06:	return "pspSysconBatteryGetCap";
	case 0x07:	return "pspSysconBatteryGetFullCap";
	case 0x08:	return "pspSysconBatteryGetIFC";
	case 0x09:	return "pspSysconBatteryGetLimitTime";
	case 0x0a:	return "pspSysconBatteryGetStatus";
	case 0x0b:	return "pspSysconBatteryGetCycle";
	case 0x0c:
		// store ctrl data & rebuild checksum
		syscon_put_dword(packet->rx_data,OVERRIDE_BATTERY_SERIAL);
		syscon_make_checksum(&packet->rx_sts);
		return "pspSysconBatteryGetSerial";
	case 0x0d:	return "pspSysconBatteryGetInfo";
	case 0x0e:	return "pspSysconBatteryGetTempAD";
	case 0x0f:	return "pspSysconBatteryGetVoltAD";
	case 0x10:	return "pspSysconBatteryGetElecAD";
	case 0x11:	return "pspSysconBatteryGetTotalElec";
	case 0x12:	return "pspSysconBatteryGetChargeTime";
	case 0x13:	return "pspSyscon_driver_Unkonow_1165c864";
	case 0x14:	return "pspSyscon_driver_Unkonow_68ef0bef";
	case 0x1f:	return "pspSysconBatteryAuth";
	}

	return "Unknown Battery Command";

}
