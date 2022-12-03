/*
	PSP SYSCON CTRL override example

	1.disable power switch 0.5sec after HOLD off to avoid unnecessary suspend.
	2.swap UP / DN buttons
	3.force HOLD OFF to controll VSH in HOLD ON
*/
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "syscon.h"
#include "main.h"

int oldpwr = 0;

void syscon_ctrl(sceSysconPacket *packet)
{
	u32 newButtons;

	switch(packet->rx_response)
	{
	case 0x08:
		if(getAX() != 0x80){
			packet->rx_data[4] = getAX();
		}
		if(getAY() != 0x80){
			packet->rx_data[5] = getAY();
		}
	case 0x07:
		newButtons = syscon_get_dword(packet->rx_data);	// active low
		newButtons = ~newButtons;						// active high

		newButtons |= getMask();

		// store modified buttons
		syscon_put_dword(packet->rx_data,~newButtons);
		syscon_make_checksum(&packet->rx_sts);
	}
}

