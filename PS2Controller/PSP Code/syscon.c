/*
	PSP SYSCON hook handling / support routine

	1.hook CTRL functions.
	2.hook battery functions.
	3.disable CALLBACKs by generic status.

*/
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include "main.h"
#include "syscon.h"

#define DUMP_SYSCON_PACKET 0

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
extern void syscon_ctrl(sceSysconPacket *packet);
extern const char *syscon_battery(sceSysconPacket *packet);

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
static int syscon_callback_disable = 0;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void syscon_put_dword(u8 *ptr,u32 value)
{
	ptr[0] = value;
	ptr[1] = value>>8;
	ptr[2] = value>>16;
	ptr[3] = value>>24;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
u32 syscon_get_dword(u8 *ptr)
{
	return ptr[0] | (ptr[1]<<8) | (ptr[2]<<16)| (ptr[3]<<24);
}

/////////////////////////////////////////////////////////////////////////////
// make syscon packet checksum
/////////////////////////////////////////////////////////////////////////////
void syscon_make_checksum(void *data)
{
	int i;
	u8 sum;
	u8 *ptr = (u8 *)data;
	int size = ptr[1];

	sum = 0;
	for(i=0;i<size;i++)
		sum += *ptr++;
	*ptr = sum^0xff;
}

#if DUMP_SYSCON_PACKET
/////////////////////////////////////////////////////////////////////////////
// dump SYSCON driver access packet
/////////////////////////////////////////////////////////////////////////////
void syscon_dump_data(const char *name,sceSysconPacket *packet)
{
	int i;

	Kprintf("syscon dump CMD[%02X]:'%s'(%08X)\n",packet->tx_cmd,name,(u32)packet);

	Kprintf("00=[%02X %02X %02X %02X] 04=[%02X %02X]\n",
		packet->unk00[0],
		packet->unk00[1],
		packet->unk00[2],
		packet->unk00[3],
		packet->unk04[0],
		packet->unk04[0]);

	Kprintf("STS[%02X] 07[%02X] 08[%02X %02X %02X %02X]\n",
		packet->status,
		packet->unk07,
		packet->unk08[0],
		packet->unk08[1],
		packet->unk08[2],
		packet->unk08[3]);

	if(packet->tx_len>2)
	{
		
		Kprintf("TX:CMD=%02X,LEN=%02X,DATA=",
			packet->tx_cmd,
			packet->tx_len);
		for(i=0;i<packet->tx_len-2;i++)
			Kprintf("%02X,",packet->tx_data[i]);
		Kprintf("SUM=%02X\n",packet->tx_data[i]);
	}

	Kprintf("\nRX:STS=%02X,LEN=%02X,RES=%02X,DATA=",
		packet->rx_sts,
		packet->rx_len,
		packet->rx_response);
//	if(packet->rx_response==packet->tx_cmd)
	{
		for(i=0;i<packet->rx_len-3;i++)
			Kprintf("%02X,",packet->rx_data[i]);
		Kprintf("SUM=%02X",packet->rx_data[i]);
	}
	Kprintf("\n");

	Kprintf("+28[%08X] CB %08X,R28=%08X,ARG2=%08X\n",
		packet->unk28,
		packet->callback,
		packet->callback_r28,
		packet->callback_arg2);

	Kprintf("\n");
}
#endif

/////////////////////////////////////////////////////////////////////////////
// Disable callbacks by generic status
/////////////////////////////////////////////////////////////////////////////
void sysconLockResponse(void)
{
	syscon_callback_disable++;
}

/////////////////////////////////////////////////////////////////////////////
// Enable callbacks by generic status
/////////////////////////////////////////////////////////////////////////////
void sysconUnLockResponse(void)
{
	if(syscon_callback_disable)
		syscon_callback_disable--;
}

/////////////////////////////////////////////////////////////////////////////
// Unknown entry or parameter,Do not used in syscon.prx
/////////////////////////////////////////////////////////////////////////////
static void syscon_debug_callback0(sceSysconPacket *packet)
{
#if DUMP_SYSCON_PACKET
	syscon_dump_data("syscon_debug_callback0",packet);
#endif
	while(1);
}

/////////////////////////////////////////////////////////////////////////////
// SYSCON debug callback before transmit
/////////////////////////////////////////////////////////////////////////////
static void syscon_transmit_callback(sceSysconPacket *packet)
{
#if 0
	switch(packet->tx_cmd)
	{
	case 0x0b: // change WDT CLEAR? to NOP
		packet->tx_cmd = 0x00;
		packet->tx_len = 0x00;
		syscon_make_checksum(&packet->tx_cmd);
		break;
	}
#endif
}

/////////////////////////////////////////////////////////////////////////////
// SYSCON debug callback after receive data
/////////////////////////////////////////////////////////////////////////////
static void syscon_receive_callback(sceSysconPacket *packet)
{
	const char *name;

	// modify response data
	switch(packet->tx_cmd)
	{
	case 0x00:	name="NOP"; break;
	case 0x01:	name="pspSyscon_driver_7ec5a957"; break;
	case 0x02:	name="pspSyscon_driver_34c36ff9"; break;

	case 0x05:	name="pspSyscon_driver_3b657a27"; break;
	case 0x07:	name="Get Controller";
		syscon_ctrl(packet);
		name = NULL;
		break;
	case 0x08:	name="Get Controller with analog stick";
		syscon_ctrl(packet);
		name = NULL;
		break;
	case 0x09:	name="pspSysconReadClock"; break;
	case 0x0a:	name="pspSysconReadAlarm"; break;
	case 0x0b:	name="pspSyscon_driver_fc32141a";
		// called 5sec cycles , WDT clear ?
		name = NULL;
		break;

	case 0x0c:	name="pspSyscon_driver_882f0aab"; break;

	case 0x0e:	name="pspSyscon_driver_f775bc34"; break;
	case 0x0f:	name="pspSyscon_driver_a9aef39f"; break;

	case 0x11:	name="pspSyscon_driver_7bcc5eae"; break;

	case 0x1c:	name="pspSysconMsOn"; break;
	case 0x1d:	name="pspSysconMsOff"; break;
	case 0x1e:	name="pspSysconWlanOn"; break;
	case 0x1f:	name="pspSysconWlanOff"; break;
	case 0x20:	name="pspSysconWriteClock"; break;

	case 0x22:	name="pspSysconWriteAlarm"; break;

	case 0x23:	name="pspSyscon_driver_65eb6096 (RTC)"; break;
	case 0x24:	name="pspSyscon_driver_eb277c88 (RTC)"; break;
	case 0x25:	name="pspSysconSendSetParam"; break;
	case 0x26:	name="pspSysconReceiveSetParam"; break;

	case 0x32:	name="pspSysconResetDevice"; break;
	case 0x33:	name="_pspSysconCtrlAStickPower"; break;
	case 0x34:	name="pspSysconCtrlHRPower"; break;
	case 0x35:	name="pspSysconPowerStandby"; break;
	case 0x36:	name="pspSysconPowerSuspend"; break;

	case 0x40:	name="pspSyscon_driver_e7e87741"; break;
	case 0x41:	name="pspSyscon_driver_fb148fb6"; break;
	case 0x42:	name="pspSysconCtrlVoltage"; break;

	case 0x45:	name="pspSysconCtrlPower"; break;
	case 0x46:	name="pspSysconGetPowerStatus"; break;
	case 0x47:	name="pspSysconCtrlLED"; break;
	case 0x48:	name="pspSysconResolverPowerdown"; break;
	case 0x49:	name="pspSyscon_driver_Unkonow_3de38336"; break;
	case 0x4a:	name="pspSysconGetPowerError"; break;
	case 0x4b:	name="pspSysconCtrlLeptonPower"; break;
	case 0x4c:	name="pspSysconCtrlMsPower"; break;
	case 0x4d:	name="pspSysconCtrlWlanPower"; break;

	case 0x4e:	name="pspSyscon_driver_806d4d6c/eab13fbe/c5075828"; break;
	case 0x4f:	name="pspSyscon_driver_d8471760/eab13fbe/c5075828"; break;

	default:
		// 60-7F : battery command
		if(packet->tx_cmd >=0x60 && packet->tx_cmd <0x80)
		{
			name = syscon_battery(packet);
		}
		else
			name = "Unknown Syscon Command";
	}

	// make receive error when disable callback by rx_sts
	// because "packet->rx_sts" in this function can't override change secnse of rx_sts.
	if(syscon_callback_disable)
	{
		// make check sum error
		packet->rx_response = 0xff; // SYSCON_RES_81;
		// syscon_make_checksum(&packet->rx_sts);
	}

#if DUMP_SYSCON_PACKET
	// show data
	if(name)
	{
		syscon_dump_data(name,packet);
	}
#endif
}

/////////////////////////////////////////////////////////////////////////////
// This doesn't exist probably.
/////////////////////////////////////////////////////////////////////////////
static void syscon_debug_callback3(sceSysconPacket *packet)
{
#if DUMP_SYSCON_PACKET
	syscon_dump_data("syscon_debug_callback3",packet);
#endif
	while(1);
}

/////////////////////////////////////////////////////////////////////////////
// SYSCON debug callback entries
/////////////////////////////////////////////////////////////////////////////
static sceSysconDebugHandlers syscon_debug_callbacks =
{
	syscon_debug_callback0,
	syscon_transmit_callback,
	syscon_receive_callback,
	syscon_debug_callback3
};

/////////////////////////////////////////////////////////////////////////////
// dump SYSCON driver access packet
/////////////////////////////////////////////////////////////////////////////
void install_syscon_hook(void)
{
	sceSysconSetDebugHandlers = (void*)sctrlHENFindFunction("sceSYSCON_Driver", "sceSyscon_driver", 0x5EE92F3C);
	sceSysconSetDebugHandlers(&syscon_debug_callbacks);
}

void uninstall_syscon_hook(void)
{
	sceSysconSetDebugHandlers = (void*)sctrlHENFindFunction("sceSYSCON_Driver", "sceSyscon_driver", 0x5EE92F3C);
	sceSysconSetDebugHandlers(NULL);
}
