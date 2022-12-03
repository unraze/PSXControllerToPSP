#ifndef __MAIN_H__
#define __MAIN_H__

#include <pspkernel.h>
#include <systemctrl.h>
#include <systemctrl_se.h> 
#include <pspdebug.h>
#include <pspctrl.h>
#include <string.h>
#include <psppower.h>
#include "syscon.h"
#include "sio.h"


int main_thread();
int getRunning();
void setRunning(int r);
int getTHID();
int getMask();
u8 getAX();
u8 getAY();

#endif