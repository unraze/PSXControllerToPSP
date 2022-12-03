#ifndef __SIO_H__
#define __SIO_H__

void resetFBuf();
void sioPutchar(int ch);
void sioPutString(const char *data, int len);
void sioInit(int baud);
void sioShutdown();
int sioReadChar(void);

#endif