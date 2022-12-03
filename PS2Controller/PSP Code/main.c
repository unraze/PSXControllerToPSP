#include "main.h"

PSP_MODULE_INFO("Plugin", 0x1006, 1, 1);

#define	BUTTON_SELECT	0x01
#define	BUTTON_L3		0x02
#define	BUTTON_R3		0x04
#define BUTTON_START	0x08
#define	BUTTON_UP		0x10
#define	BUTTON_RIGHT	0x20
#define BUTTON_DOWN		0x40
#define BUTTON_LEFT		0x80

#define	BUTTON_L2		0x01
#define	BUTTON_R2		0x02
#define	BUTTON_L1		0x04
#define	BUTTON_R1		0x08
#define	BUTTON_TRI		0x10
#define	BUTTON_CIRCLE	0x20
#define	BUTTON_CROSS	0x40
#define	BUTTON_SQUARE	0x80


int gholdmask=0x00;
u8 analogx = 128;
u8 analogy = 128;
int data[10];
int thid,running=1;

int getRunning(){
	return running;
}
void setRunning(int r){
	running = r;
}
int getTHID(){
	return thid;
}
int getMask(){
	return gholdmask;
}
u8 getAX(){
	return analogx;
}
u8 getAY(){
	return analogy;
}


void stuff_keys(int mask){
	gholdmask = mask;
}


int main_thread()
{

	sioInit(19200);
	sioPutchar(0xFE);
	sceKernelDelayThread(100000);


	int ch,numChars,mask,i;
	while(1){
loop:
		sioPutchar(0x34);
		ch = sioReadChar();
		if(ch==-1){
			resetFBuf();
			goto loop;
		}else if(ch==0xEE){
			sceKernelDelayThread(20000);
			resetFBuf();
			goto loop;
		}else if(ch==0xAC){
			numChars = sioReadChar();
			if(numChars==-1){
				resetFBuf();
				goto loop;
			}
			for(i=0; i<numChars; i++){
				data[i] = sioReadChar();
				if(data[i]==-1){
					resetFBuf();
					goto loop;
				}
			}
		}
		resetFBuf();

		int chk = 0;
		for(i = 0; i < 6; i++){
			chk = chk | data[i];
		}
		if(chk == 0){
			sceKernelDelayThread(20000);
			goto loop;
		}

		mask=0;
		if(((~data[0]) & BUTTON_SELECT)!=0){
			mask |= SYSCON_CTRL_SELECT;
		}
		if(((~data[0]) & BUTTON_START)!=0){
			mask |= SYSCON_CTRL_START;
		}
		if(((~data[0]) & BUTTON_UP)!=0){
			mask |= SYSCON_CTRL_ALLOW_UP;
		}
		if(((~data[0]) & BUTTON_RIGHT)!=0){
			mask |= SYSCON_CTRL_ALLOW_RT;
		}
		if(((~data[0]) & BUTTON_DOWN)!=0){
			mask |= SYSCON_CTRL_ALLOW_DN;
		}
		if(((~data[0]) & BUTTON_LEFT)!=0){
			mask |= SYSCON_CTRL_ALLOW_LT;
		} 


		if(((~data[1]) & BUTTON_L1)!=0){
			mask |= SYSCON_CTRL_LTRG;
		}
		if(((~data[1]) & BUTTON_R1)!=0){
			mask |= SYSCON_CTRL_RTRG;
		}
		if(((~data[1]) & BUTTON_TRI)!=0){
			mask |= SYSCON_CTRL_TRIANGLE;
		}
		if(((~data[1]) & BUTTON_CIRCLE)!=0){
			mask |= SYSCON_CTRL_CIRCLE;
		}
		if(((~data[1]) & BUTTON_CROSS)!=0){
			mask |= SYSCON_CTRL_CROSS;
		}
		if(((~data[1]) & BUTTON_SQUARE)!=0){
			mask |= SYSCON_CTRL_RECTANGLE;
		}

		if(((~data[0]) & BUTTON_L3)!=0){
			mask |= SYSCON_CTRL_HOME;
		}
		if(((~data[1]) & BUTTON_L2)!=0){
			mask |= SYSCON_CTRL_VOL_DN;
		}
		if(((~data[1]) & BUTTON_R2)!=0){
			mask |= SYSCON_CTRL_VOL_UP;
		}
		if(data[2] > 188){
			mask |= SYSCON_CTRL_CIRCLE;
		}else if(data[2] < 68){
			mask |= SYSCON_CTRL_RECTANGLE;
		}
		if(data[3] > 188){
			mask |= SYSCON_CTRL_CROSS;
		}else if(data[3] < 68){
			mask |= SYSCON_CTRL_TRIANGLE;
		}

		analogx = data[4];
		analogy = data[5];
		stuff_keys(mask);

		sceKernelDelayThread(25000);
	}

	sceKernelSleepThread();
	return 0;
}

int module_start(SceSize args, void *argp) {
	install_syscon_hook();
	thid = sceKernelCreateThread("HookTest", main_thread, 0x18, 0x1000, 0, NULL);
	if(thid >= 0) sceKernelStartThread(thid, args, argp);
	return 0;
}

int module_stop()
{
        return 0;
} 