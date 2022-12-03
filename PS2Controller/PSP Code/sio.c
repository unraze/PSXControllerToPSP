#include <pspkernel.h>
#include <systemctrl.h>
#include <systemctrl_se.h> 
#include <pspsdk.h>
#include <pspintrman_kernel.h>
#include <pspintrman.h>
#include <pspsyscon.h>
#include <pspctrl.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pspreg.h>

// no changes here...the same good old consts
#define PSP_UART4_DIV1 0xBE500024
#define PSP_UART4_DIV2 0xBE500028
#define PSP_UART4_CTRL 0xBE50002C
#define PSP_UART_CLK   96000000
#define SIO_CHAR_RECV_EVENT  0x01

#define PSP_UART4_FIFO 0xBE500000
#define PSP_UART4_STAT 0xBE500018

#define PSP_UART_TXFULL  0x20
#define PSP_UART_RXEMPTY 0x10


static SceUID sio_eventflag = -1;


// CIRCULAR FIFO FUNCTIONS-------------------------
unsigned short int w = 0;
unsigned short int r = 0;
unsigned short int buf[255]; // 0..255 = 256 bytes


void fWrite(unsigned short int b){
   buf[w] = b;
   w++;
   if (w==r) r++; // CHECK THIS... "r++" should not be interrupted...consider about stopping ALL interrupts in intr handler
}

int fRead(){
   if (w==r) return -1;
   int b = buf[r];
   r++; // CHECK THIS... "r++" should not be interrupted...consider about stopping ALL interrupts in intr handler
   return b;
}

void resetFBuf(){
   // disable SIO interrupt...prevents resetFBuf() from being interrupted
   // by SIO events that would result in a data inconsistency.
   sceKernelDisableIntr(PSP_HPREMOTE_INT);
   w = 0;
   r = 0;
   sceKernelEnableIntr(PSP_HPREMOTE_INT); // re-enable interrupt
}

// END CIRCULAR FIFO FUNCTIONS-------------------------

void sioPutchar(int ch){
   // as you see this is _blocking_...not an issue for
   // normal use as everithing doing I/O
   // should run in its own thread..in addition, HW FIFO isn't
   // working at all by now, so queue should not be that long :)
	while(_lw(PSP_UART4_STAT) & PSP_UART_TXFULL){
		sceKernelDelayThread(100);
	}
   _sw(ch, PSP_UART4_FIFO);
}


void sioPutString(const char *data, int len){
   unsigned int k1 = pspSdkSetK1(0);

      int i;
      for(i = 0; i < len; i++)
      {
            sioPutchar(data[i]);
      }

   pspSdkSetK1(k1);
}

int sioGetChar(void){
   if(_lw(PSP_UART4_STAT) & PSP_UART_RXEMPTY)
   {   
         return -1;
   }

   return _lw(PSP_UART4_FIFO);
}

void sioSetBaud(int baud){ // no need to export this....always call sioInit()...

   int div1, div2; // low, high bits of divisor value

   div1 = PSP_UART_CLK / baud;
   div2 = div1 & 0x3F;
   div1 >>= 6;

   _sw(div1, PSP_UART4_DIV1);
   _sw(div2, PSP_UART4_DIV2);
   _sw(0x60, PSP_UART4_CTRL); // ?? someone do it with 0x70
}

void _sioInit(void)
{
   /* Shut down the remote driver */
   sceHprmEnd();
   /* Enable UART 4 */
   sceSysregUartIoEnable(4);
   /* Enable remote control power */
   sceSysconCtrlHRPower(1);
}

int intr_handler(void *arg)
{
   // disable interrupt...we don't want SIO to call intr_handler again while it's already running
   // don't know if it's really needed here, but i remember this was a must in pc programming
   // MAYBE i'm better use "int intrs = pspSdkDisableInterrupts();" (disable ALL intrs) to handle reader/writer conflicts

   sceKernelDisableIntr(PSP_HPREMOTE_INT);

   /* Read out the interrupt state and clear it */
   u32 stat = _lw(0xBE500040);
   _sw(stat, 0xBE500044);

   if(!(_lw(PSP_UART4_STAT) & PSP_UART_RXEMPTY)) {
      fWrite(_lw(PSP_UART4_FIFO));
      sceKernelSetEventFlag(sio_eventflag, SIO_CHAR_RECV_EVENT); // set "we got something!!" flag
   }

   sceKernelEnableIntr(PSP_HPREMOTE_INT); // re-enable interrupt
   // MAYBE i'm better use "pspSdkEnableInterrupts(intrs);"
   return -1;
}

void sioInit(int baud){

   unsigned int k1 = pspSdkSetK1(0);

      _sioInit();

      sio_eventflag = sceKernelCreateEventFlag("SioShellEvent", 0, 0, 0);
                  
      sceKernelRegisterIntrHandler(PSP_HPREMOTE_INT, 1, intr_handler, NULL, NULL);
      sceKernelEnableIntr(PSP_HPREMOTE_INT);
      sceKernelDelayThread(2000000);
      sioSetBaud(baud);

   pspSdkSetK1(k1);
}


void sioShutdown(){
	unsigned int k1 = pspSdkSetK1(0);

	sceSysconCtrlHRPower(0);
	sceKernelDeleteEventFlag(sio_eventflag);
	sceKernelReleaseIntrHandler(PSP_HPREMOTE_INT);
	sceKernelDisableIntr(PSP_HPREMOTE_INT);

	pspSdkSetK1(k1);
}


int sioReadChar(void) // maybe a "char * sioReadString(int numberOfCharachtersToWaitFor)" could be useful as well
{
   unsigned int k1 = pspSdkSetK1(0);
   
        int ch;
        u32 result;
      SceUInt timeout;

        timeout = 100000;
       
      ch = fRead();
       
       if(ch == -1)
       {
                sceKernelWaitEventFlag(sio_eventflag, SIO_CHAR_RECV_EVENT, PSP_EVENT_WAITOR|PSP_EVENT_WAITCLEAR, &result, &timeout); //timeout could be null=forever
                ch = fRead();
        }
   pspSdkSetK1(k1);
        return ch;
}
