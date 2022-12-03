.include "m8def.inc"



.equ F_CPU = 16000000
.equ BAUD  = 19200

.equ UBRR_VAL   = ((F_CPU+BAUD*8)/(BAUD*16)-1)
.equ BAUD_REAL  = (F_CPU/(16*(UBRR_VAL+1)))
.equ BAUD_ERROR = ((BAUD_REAL*1000)/BAUD-1000)
 
.if ((BAUD_ERROR>10) || (BAUD_ERROR<-10))
  .error "Systematischer Fehler der Baudrate grösser 1 Prozent und damit zu hoch!"
.endif
 



.def	TEMP1	=	r16
.def	TEMP2	=	r17
.def	TEMP3	=	r18

.equ	BUFFER_SIZE =	6

.equ	DEBUG_PRT	=	PORTC
.equ	DEBUG_PIN	= 	PINC
.equ	DEBUG_DDR	=	DDRC
.equ	LED_RED		=	0
.equ	LED_YELLOW	=	1
.equ	LED_GREEN	=	2

.equ	DATA_PRT	=	PORTB
.equ	DATA_PIN	=	PINB
.equ	DATA_DDR	=	DDRB
.equ	MOSI_PIN	=	3
.equ	MISO_PIN	=	4
.equ	SCK_PIN		=	5
.equ	SS_PIN		=	2
.equ	ACK_PIN		=	1



//RAM
.DSEG
BUFFER1:		.BYTE 	6
BUFFER2:		.BYTE 	6
BUFFER_R_H:		.BYTE	1
BUFFER_R_L:		.BYTE	1
BUFFER_W_H:		.BYTE	1
BUFFER_W_L:		.BYTE	1


//CODE
.CSEG
.org 0x000
       rjmp RESET
.org INT0addr                 ; External Interrupt0 Vector Address
       reti                   
.org INT1addr                 ; External Interrupt1 Vector Address
       reti                   
.org OC2addr                  ; Output Compare2 Interrupt Vector Address
       reti                   
.org OVF2addr                 ; Overflow2 Interrupt Vector Address
       reti                   
.org ICP1addr                 ; Input Capture1 Interrupt Vector Address
       reti                   
.org OC1Aaddr                 ; Output Compare1A Interrupt Vector Address
       rjmp TIMEOUT                   
.org OC1Baddr                 ; Output Compare1B Interrupt Vector Address
       reti                   
.org OVF1addr                 ; Overflow1 Interrupt Vector Address
       reti                   
.org OVF0addr                 ; Overflow0 Interrupt Vector Address
       reti                   
.org SPIaddr                  ; SPI Interrupt Vector Address
       reti                   
.org URXCaddr                 ; USART Receive Complete Interrupt Vector Address
	   rjmp RXD_ISR
.org UDREaddr                 ; USART Data Register Empty Interrupt Vector Address
       reti                   
.org UTXCaddr                 ; USART Transmit Complete Interrupt Vector Address
       reti                   
.org ADCCaddr                 ; ADC Interrupt Vector Address
       reti                   
.org ERDYaddr                 ; EEPROM Interrupt Vector Address
       reti                   
.org ACIaddr                  ; Analog Comparator Interrupt Vector Address
       reti                   
.org TWIaddr                  ; Irq. vector address for Two-Wire Interface
       reti                   
.org SPMRaddr                 ; SPM complete Interrupt Vector Address
       reti                   
.org INT_VECTORS_SIZE




;########################################################################
;########################################################################
;####################            MAIN CODE           ####################
;########################################################################
;########################################################################



/*///////////////////////////////////////////
Hardware Init After Reset/Power On
////////////////////////////////////////// */
RESET:
	//INIT STACK
    ldi		TEMP1,		LOW(RAMEND)
    out		SPL, 		TEMP1
    ldi		TEMP1, 		HIGH(RAMEND)
    out		SPH, 		TEMP1


	//INIT TIMEOUT HANDLER
	ldi     TEMP1,		HIGH( (F_CPU/10000) - 1 )
	out     OCR1AH,		TEMP1
	ldi     TEMP1,		LOW( (F_CPU/10000) - 1 )
	out     OCR1AL, 	TEMP1

	ldi		TEMP1,		0x00
	out		TCCR1A,		TEMP1
	ldi     TEMP1,		( 1 << WGM12 ) | (1 << CS12) | (0 << CS11) | ( 1 << CS10 )
	out     TCCR1B,		TEMP1
 
	ldi     TEMP1, 		(0 << OCIE1A)
	out     TIMSK, 		TEMP1



    //INIT UART AND SPI
    rcall	INIT_UART
	rcall	INIT_SPI

	//INIT BUFFER
	rcall	BUFFER_INIT

	//INIT ACK
	cbi		DATA_DDR,	ACK_PIN
	sbi		DATA_PRT,	ACK_PIN
	

	//INIT DEBUG PRT
	ldi		TEMP1,		(1<<LED_RED) | (1<<LED_YELLOW) | (1<<LED_GREEN)
	out		DEBUG_DDR,	TEMP1

	sei

	rjmp 	MAIN




/* /////////////////////////////////////////////////////////
Main Code
///////////////////////////////////////////////////////// */
MAIN:
	ldi		TEMP1,		(1<<LED_YELLOW)
	out		DEBUG_PRT,	TEMP1


	cbi		DATA_PRT,	SS_PIN

	ldi		TEMP2,		0x00
	
	ldi		TEMP1,		0x01
	rcall	SEND_PS2_BYTE

	ldi		TEMP1,		0x43
	rcall	SEND_PS2_BYTE

	ldi		TEMP1,		0x00
	rcall	SEND_PS2_BYTE

	ldi		TEMP1,		0x01
	rcall	SEND_PS2_BYTE

	ldi		TEMP2,		0x01
	ldi		TEMP1,		0x00
	rcall	SEND_PS2_BYTE

	sbi		DATA_PRT,	SS_PIN


   	rcall	DELAY


	cbi		DATA_PRT,	SS_PIN

	ldi		TEMP2,		0x00
	
	ldi		TEMP1,		0x01
	rcall	SEND_PS2_BYTE

	ldi		TEMP1,		0x44
	rcall	SEND_PS2_BYTE

	ldi		TEMP1,		0x00
	rcall	SEND_PS2_BYTE

	ldi		TEMP1,		0x01
	rcall	SEND_PS2_BYTE

	ldi		TEMP1,		0x03
	rcall	SEND_PS2_BYTE

	ldi		TEMP1,		0x00
	rcall	SEND_PS2_BYTE

	ldi		TEMP1,		0x00
	rcall	SEND_PS2_BYTE

	ldi		TEMP1,		0x00
	rcall	SEND_PS2_BYTE

	ldi		TEMP2,		0x01
	ldi		TEMP1,		0x00
	rcall	SEND_PS2_BYTE

	sbi		DATA_PRT,	SS_PIN


	rcall	DELAY	


	cbi		DATA_PRT,	SS_PIN

	ldi		TEMP2,		0x00
	
	ldi		TEMP1,		0x01
	rcall	SEND_PS2_BYTE

	ldi		TEMP1,		0x43
	rcall	SEND_PS2_BYTE

	ldi		TEMP1,		0x00
	rcall	SEND_PS2_BYTE

	ldi		TEMP1,		0x00
	rcall	SEND_PS2_BYTE

	ldi		TEMP1,		0x5A
	rcall	SEND_PS2_BYTE

	ldi		TEMP1,		0x5A
	rcall	SEND_PS2_BYTE

	ldi		TEMP1,		0x5A
	rcall	SEND_PS2_BYTE

	ldi		TEMP1,		0x5A
	rcall	SEND_PS2_BYTE

	ldi		TEMP2,		0x01
	ldi		TEMP1,		0x5A
	rcall	SEND_PS2_BYTE

	sbi		DATA_PRT,	SS_PIN

	ldi		TEMP1,		(1 << LED_GREEN)
	out		DEBUG_PRT,	TEMP1

	rcall	DELAY
	rcall	POLL_PS2_PORT
	rcall	START_UART

MAIN_LOOP:
   	rcall	DELAY

	rcall	POLL_PS2_PORT

	rjmp 	MAIN_LOOP








//##################################################################//
//##################################################################//
//####################       RANDOM STUFF       ####################//
//##################################################################//
//##################################################################//



/* /////////////////////////////////////////////////////////
ERROR Handler
///////////////////////////////////////////////////////// */
ERROR:
	cli
	ldi		TEMP1,		(1<<LED_RED)
	out		DEBUG_PRT,	TEMP1
ERROR_:
	rjmp	ERROR_



/* /////////////////////////////////////////////////////////
A Timeout Occured, No Controller Is Connected (Anymore)!
///////////////////////////////////////////////////////// */
TIMEOUT:
	ldi		TEMP1,		(1<<LED_YELLOW)
	out		DEBUG_PRT,	TEMP1

	sbic    UCSRA,		RXC
	rcall	RXD_HANDLER

	ldi		TEMP2,		0x01
	cbi		DATA_PRT,	SS_PIN

	ldi		TEMP1,		0x01
	rcall	SEND_PS2_BYTE
	ldi		TEMP1,		0x42
	rcall	SEND_PS2_BYTE
	sbi		DATA_PRT,	SS_PIN
	cpi		TEMP1,		0xFF
	breq	TIMEOUT

	rjmp	RESET_CPU


RESET_CPU:
	WDR

	ori TEMP1, (1<<WDCE)|(1<<WDE)
	out WDTCR, temp1
	ldi TEMP1, (1<<WDE) |(0<<WDP2) |(0<<WDP1) |(0<<WDP0)
	out WDTCR, temp1

wait_for_reset:
	rjmp wait_for_reset
	ret


/* /////////////////////////////////////////////////////////
Wait 1 Second
///////////////////////////////////////////////////////// */
WAIT_1_S:
			push	R17
			push	R18
			push	R19

         	ldi  	R17, ($09*(F_CPU/1000000))
WAIT_1_S_:  ldi  	R18, $BC
WAIT_1_S__:	ldi  	R19, $C4
WAIT_1_S___:dec  	R19
          	brne 	WAIT_1_S___
          	dec  	R18
          	brne 	WAIT_1_S__
          	dec  	R17
          	brne 	WAIT_1_S_

			pop		R19
			pop		R18
			pop		R17
			ret



/* /////////////////////////////////////////////////////////
Perform A Short Delay
///////////////////////////////////////////////////////// */
DELAY:
			push	R18
			push	R19

         	ldi  	R18, $80
DELAY_:		ldi  	R19, $FF
DELAY__:	dec  	R19
          	brne 	DELAY__
          	dec  	R18
          	brne 	DELAY_

			pop		R19
			pop		R18
			ret







//##################################################################//
//##################################################################//
//########################        SPI       ########################//
//##################################################################//
//##################################################################//



/* /////////////////////////////////////////////////////////
Init SPI
///////////////////////////////////////////////////////// */
INIT_SPI:
	ldi		TEMP1,		(1<<SS_PIN)	|	(1<<MOSI_PIN)	|	(1<<SCK_PIN)	|	(0<<MISO_PIN)
	out		DATA_DDR,	TEMP1
	ldi		TEMP1,		(1<<SS_PIN)	|	(1<<MOSI_PIN)	|	(1<<SCK_PIN)	|	(0<<MISO_PIN)
	out		DATA_PRT,	TEMP1

	ldi		TEMP1,		(0<<SPIE)	|	(1<<SPE)	|	(1<<DORD)	|	(1<<MSTR)	|	(1<<CPOL)	|	(1<<CPHA)	|	(1<<SPR1)	|	(0<<SPR0)
	out		SPCR,		TEMP1

	ldi		TEMP1,		(0<<SPI2X)
	out		SPSR,		TEMP1
	ret


/* /////////////////////////////////////////////////////////
Send TEMP1 To SPI Bus And Receive Byte From SPI To TEMP1
///////////////////////////////////////////////////////// */
TRANSPHER_SPI:
	out		SPDR,		TEMP1
TRANSPHER_SPI_:
	sbis	SPSR,		SPIF
	rjmp	TRANSPHER_SPI_
	
	in		TEMP1,		SPDR
	ret	








//##################################################################//
//##################################################################//
//########################        PS2       ########################//
//##################################################################//
//##################################################################//



/* /////////////////////////////////////////////////////////
Send TEMP1 To PS2 Port, Receive From PS2 Port To TEMP1
Wait For ACK If TEMP2[0] Is Cleared
Output If TEMP2[4] Is Set
///////////////////////////////////////////////////////// */
SEND_PS2_BYTE:
	push	TEMP2
	push	TEMP3
	cbi     UCSRB, 		RXCIE

	rcall	TRANSPHER_SPI

	sbrc	TEMP2,		4
	rcall	BUFFER_WRITE

	sbrc	TEMP2,		0
	rjmp	WAIT_FOR_ACK_
 
 	ldi		TEMP3,		0x00
	out		TCNT1H,		TEMP3
	out		TCNT1L,		TEMP3
	ldi		TEMP3,		0b00111100
	out		TIFR,		TEMP3
 	ldi		TEMP3,		(1 << OCIE1A)
	out		TIMSK,		TEMP3

WAIT_FOR_ACK:
	sbic	DATA_PIN,	ACK_PIN
	rjmp	WAIT_FOR_ACK
 	
	cli
 	ldi		TEMP3,		(0 << OCIE1A)
	out		TIMSK,		TEMP3
	sbi     UCSRB, 		RXCIE
	sei	

WAIT_FOR_ACK_:
	sbis	DATA_PIN,	ACK_PIN
	rjmp	WAIT_FOR_ACK_

	ldi		R17,		(F_CPU/100000)
WAIT_FOR_ACK_LOOP:
	ldi		R18,		0x40
WAIT_FOR_ACK_LOOP_:
	dec		R18
	brne	WAIT_FOR_ACK_LOOP_
	dec		R17
	brne	WAIT_FOR_ACK_LOOP

	pop		TEMP3
	pop		TEMP2
	ret


/* /////////////////////////////////////////////////////////
Poll Data From The PS2 Port
///////////////////////////////////////////////////////// */
POLL_PS2_PORT:
	push	TEMP1
	push	TEMP2
	push	TEMP3

	cbi		DATA_PRT,	SS_PIN

	ldi		TEMP2,		0x00
	
	ldi		TEMP1,		0x01
	rcall	SEND_PS2_BYTE

	ldi		TEMP1,		0x42
	rcall	SEND_PS2_BYTE

	andi	TEMP1,		0x0F
	mov		TEMP3,		TEMP1
	lsl		TEMP3

POLL_PS2_PORT_:
	ldi		TEMP1,		0x00
	rcall	SEND_PS2_BYTE

	ldi		TEMP2,		0x10

	dec		TEMP3
	cpi		TEMP3,		0x00
	brne	POLL_PS2_PORT_

	ldi		TEMP2,		0x11
	ldi		TEMP1,		0x00
	rcall	SEND_PS2_BYTE

	sbi		DATA_PRT,	SS_PIN

	pop		TEMP3
	pop		TEMP2
	pop		TEMP1
	ret








//##################################################################//
//##################################################################//
//########################       UART       ########################//
//##################################################################//
//##################################################################//



/* /////////////////////////////////////////////////////////
Init UART
///////////////////////////////////////////////////////// */
INIT_UART:
	sbi		DDRD,		0

    ldi     TEMP1, 		HIGH(UBRR_VAL)
    out     UBRRH, 		TEMP1
    ldi     TEMP1, 		LOW(UBRR_VAL)
    out     UBRRL, 		TEMP1

    ldi     TEMP1, 		(1<<URSEL)|(3<<UCSZ0)
    out     UCSRC, 		TEMP1
 
    sbi     UCSRB,		TXEN   
	cbi     UCSRB, 		RXCIE
    sbi     UCSRB, 		RXEN

	ret


/* /////////////////////////////////////////////////////////
Start UART
///////////////////////////////////////////////////////// */
START_UART:
	sbi     UCSRB, 		RXCIE
	ret



/* /////////////////////////////////////////////////////////
Output TEMP1 To UART
///////////////////////////////////////////////////////// */
SEROUT:
    sbis    UCSRA,		UDRE
    rjmp    SEROUT
    out     UDR, 		TEMP1
    ret    



/* /////////////////////////////////////////////////////////
UART Receive Interrupt
///////////////////////////////////////////////////////// */
RXD_ISR:
	rcall	RXD_HANDLER
	reti

/* /////////////////////////////////////////////////////////
UART Receive Handler
///////////////////////////////////////////////////////// */
RXD_HANDLER:
    push	TEMP1
    in      TEMP1,		SREG
	push	TEMP1
	push	TEMP2
    
	in		TEMP1,		UDR
	
	cpi		TEMP1,		0x34
	breq	RXD_ISR_POLL
	cpi		TEMP1,		0xFE
	breq	RXD_ISR_RESET
	rjmp	RXD_ISR_ERROR


RXD_ISR_RESET:
	rjmp	RESET_CPU

RXD_ISR_POLL:
	sbis	DEBUG_PRT,	LED_GREEN
	rjmp	RXD_ISR_ERROR
	ldi		TEMP1,		0xAC
	rcall	SEROUT
	ldi		TEMP1,		BUFFER_SIZE
	rcall	SEROUT
	ldi		TEMP2,		BUFFER_SIZE
RXD_ISR_POLL_:
	rcall	BUFFER_READ
	rcall	SEROUT
	dec		TEMP2
	brne	RXD_ISR_POLL_
	rjmp	RXD_ISR_

RXD_ISR_ERROR:
	ldi		TEMP1,		0xEE
	rcall	SEROUT
	rjmp	RXD_ISR_

RXD_ISR_:
 	pop		TEMP2
	pop		TEMP1
	out		SREG,		TEMP1
	pop		TEMP1

    ret






;########################################################################
;########################################################################
;####################           BUFFER STUFF         ####################
;########################################################################
;########################################################################



/*///////////////////////////////////////////
Init Buffers
////////////////////////////////////////// */
BUFFER_INIT:
	push 	TEMP1
	push	TEMP2
	push	ZH
	push	ZL

	ldi		ZH,				HIGH(BUFFER1)
	ldi		ZL,				LOW(BUFFER1)
	ldi		TEMP2,			0
CLEAR_RAM_:
	ldi		TEMP1,			0
	ST		Z+,				TEMP1
	inc		TEMP2
	cpi		TEMP2,			((BUFFER_SIZE*2)+4)
	brne	CLEAR_RAM_

	ldi		TEMP1,			HIGH(BUFFER2)
	STS		BUFFER_R_H,		TEMP1
	ldi		TEMP1,			LOW(BUFFER2)
	STS		BUFFER_R_L,		TEMP1

	ldi		TEMP1,			HIGH(BUFFER1)
	STS		BUFFER_W_H,		TEMP1
	ldi		TEMP1,			LOW(BUFFER1)
	STS		BUFFER_W_L,		TEMP1

	pop		ZL
	pop		ZH
	pop		TEMP2
	pop		TEMP1
	ret




/*///////////////////////////////////////////
Read From Buffer To TEMP1
////////////////////////////////////////// */
BUFFER_READ:
	push	r31
	push	r30
	push	r29
	push	r28
	push	TEMP2

	LDS		ZH,				BUFFER_R_H
	LDS		ZL,				BUFFER_R_L

	LD		TEMP1,			Z+
	;ldi		TEMP2,			0xFF
	;ST		Z+,				TEMP2

	ldi		YH,				HIGH(BUFFER2)
	ldi		YL,				LOW(BUFFER2)

	ldi		TEMP2,			LOW(BUFFER_SIZE)
	add		YL,				TEMP2
	ldi		TEMP2,			HIGH(BUFFER_SIZE)
	adc		YH,				TEMP2

	cp		ZL,				YL
	cpc		ZH,				YH

	brne	BUFFER_READ_BUFFER1

	ldi		ZH,				HIGH(BUFFER2)
	ldi		ZL,				LOW(BUFFER2)

	rjmp	BUFFER_READ_END

BUFFER_READ_BUFFER1:
	ldi		YH,				HIGH(BUFFER1)
	ldi		YL,				LOW(BUFFER1)

	ldi		TEMP2,			LOW(BUFFER_SIZE)
	add		YL,				TEMP2
	ldi		TEMP2,			HIGH(BUFFER_SIZE)
	adc		YH,				TEMP2

	cp		ZL,				YL
	cpc		ZH,				YH

	brne	BUFFER_READ_END

	ldi		ZH,				HIGH(BUFFER1)
	ldi		ZL,				LOW(BUFFER1)

	rjmp	BUFFER_READ_END

BUFFER_READ_END:
	STS		BUFFER_R_H,		ZH	
	STS		BUFFER_R_L,		ZL

	pop		TEMP2
	pop		r28
	pop		r29
	pop		r30
	pop		r31

	ret






/*///////////////////////////////////////////
Write TEMP1 To Buffer
////////////////////////////////////////// */
BUFFER_WRITE:
	push	r31
	push	r30
	push	r29
	push	r28
	push	TEMP2

	LDS		ZH,				BUFFER_W_H
	LDS		ZL,				BUFFER_W_L

	ST		Z+,				TEMP1

	ldi		YH,				HIGH(BUFFER2)
	ldi		YL,				LOW(BUFFER2)

	ldi		TEMP2,			LOW(BUFFER_SIZE)
	add		YL,				TEMP2
	ldi		TEMP2,			HIGH(BUFFER_SIZE)
	adc		YH,				TEMP2

	cp		ZL,				YL
	cpc		ZH,				YH

	brne	BUFFER_WRITE_BUFFER1

	LDS		ZH,				BUFFER_R_H
	LDS		ZL,				BUFFER_R_L

	ldi		YH,				HIGH(BUFFER1)
	ldi		YL,				LOW(BUFFER1)

	cp		ZL,				YL
	cpc		ZH,				YH

	breq	BUFFER_WRITE_FLIP

	ldi		ZH,				HIGH(BUFFER2)
	ldi		ZL,				LOW(BUFFER2)

	rjmp	BUFFER_WRITE_END

BUFFER_WRITE_BUFFER1:
	ldi		YH,				HIGH(BUFFER1)
	ldi		YL,				LOW(BUFFER1)

	ldi		TEMP2,			LOW(BUFFER_SIZE)
	add		YL,				TEMP2
	ldi		TEMP2,			HIGH(BUFFER_SIZE)
	adc		YH,				TEMP2

	cp		ZL,				YL
	cpc		ZH,				ZH

	brne	BUFFER_WRITE_END

	LDS		ZH,				BUFFER_R_H
	LDS		ZL,				BUFFER_R_L

	ldi		YH,				HIGH(BUFFER2)
	ldi		YL,				LOW(BUFFER2)

	cp		ZL,				YL
	cpc		ZH,				YH

	breq	BUFFER_WRITE_FLIP

	ldi		ZH,				HIGH(BUFFER1)
	ldi		ZL,				LOW(BUFFER1)

	rjmp	BUFFER_WRITE_END

BUFFER_WRITE_FLIP:
	rcall	BUFFER_FLIP
	rjmp	BUFFER_WRITE_END_

BUFFER_WRITE_END:
	STS		BUFFER_W_H,		ZH	
	STS		BUFFER_W_L,		ZL

BUFFER_WRITE_END_:
	pop		TEMP2
	pop		r28
	pop		r29
	pop		r30
	pop		r31

	ret


/*///////////////////////////////////////////
Flip Buffer
////////////////////////////////////////// */
BUFFER_FLIP:
	push	YL
	push	YH
	push	ZL
	push	ZH
	push	TEMP1

	LDS		ZL,				BUFFER_W_L
	LDS		ZH,				BUFFER_W_H

	ldi		TEMP1,			5
	sub		ZL,				TEMP1
	ldi		TEMP1,			0
	sbc		ZH,				TEMP1


	LDS		YL,				BUFFER_R_L
	LDS		YH,				BUFFER_R_H

	STS		BUFFER_W_L,		YL
	STS		BUFFER_W_H,		YH

	STS		BUFFER_R_L,		ZL
	STS		BUFFER_R_H,		ZH



	pop		TEMP1
	pop		ZH
	pop		ZL
	pop		YH
	pop		YL
	ret
