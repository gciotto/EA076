/* ###################################################################
 **     Filename    : main.c
 **     Project     : ea076-exp2
 **     Processor   : MKL25Z128VLK4
 **     Version     : Driver 01.01
 **     Compiler    : GNU C Compiler
 **     Date/Time   : 2016-03-15, 18:39, # CodeGen: 0
 **     Abstract    :
 **         Main module.
 **         This module contains user's application code.
 **     Settings    :
 **     Contents    :
 **         No public methods
 **
 ** ###################################################################*/
/*!
 ** @file main.c
 ** @version 01.01
 ** @brief
 **         Main module.
 **         This module contains user's application code.
 */         
/*!
 **  @addtogroup main_module main module documentation
 **  @{
 */         
/* MODULE main */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "DataCmd.h"
#include "BitsIoLdd1.h"
#include "Enable.h"
#include "BitIoLdd1.h"
#include "RS.h"
#include "BitIoLdd2.h"
#include "timer.h"
#include "TimerIntLdd1.h"
#include "TU1.h"
#include "LED.h"
#include "BitIoLdd3.h"
#include "Button.h"
#include "BitIoLdd4.h"
#include "Conversor.h"
#include "AdcLdd1.h"
#include "UTIL1.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"
uint16_t value;
float tensao;
float temperatura;
bool waiting;
static uint8_t dst[8];


/* User includes (#include below this line is not maintained by Processor Expert) */

void write_all_letters(char start_letter, char end_letter);
void write_all_letters_with_button(char start_letter, char end_letter);
void send_string(char *string);
void send_cmd(char cmd, int interval);
void send_data(char data);
void wait_n_interruptions (int n_interruption);
void init_LCD();


/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
	/* Write your local variable definition here */

	/*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
	PE_low_level_init();
	/*** End of Processor Expert internal initialization.                    ***/

	/* Write your code here */
	/* For example: for(;;) { } */

	/*** Don't write any code pass this line, or it will be deleted during code generation. ***/
	/*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
#ifdef PEX_RTOS_START
	PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
#endif
	/*** End of RTOS startup code.  ***/
	init_LCD();
	write_all_letters('A', 'Z');

	while (!Button_GetVal());
	while (Button_GetVal());

	send_cmd(0x01, 26);
	write_all_letters('a', 'z');

	while (!Button_GetVal());
	while (Button_GetVal());

	send_cmd(0x01, 26);
	write_all_letters_with_button('a', 'z');

	//Potenciometro
	while (!Button_GetVal());
	while (Button_GetVal());
	
	//Enquanto o botao nao for pressionado
	while (!Button_GetVal()){
		char buff[8];

		//Espera o conversor terminar de ler o valor
		Conversor_Measure(TRUE);
		
		//Conversor pega o valor lido pela entrada analogica
		Conversor_GetValue16(&value);
		
		//conta de transformacao para a tensao real (value*MAX_TENSAO / (2^16 - 1))
		tensao = value*3300/65536;

		//transforma a tensao em uma string
		sprintf(buff, "%4d mV", (int) tensao);

		// limpa LCD
		send_cmd(0x01, 26);
		
		// manda para o LCD
		send_string(buff);

		// Espera de 100ms para visualizacao do resultado no LCD.
		wait_n_interruptions(1666);

		//deve-se implementar uma logica para sair dessa funcao quando apertar o botao (!!!feito!!!)
	}
	
	// Espera botao ser 'despressionado'
	while (Button_GetVal());
	
	//Sensor de temperatura

	//Enquanto o botao nao for pressionado
	while (!Button_GetVal()){
		char buff[8];
		
		//Espera o conversor terminar de ler o valor
		Conversor_Measure(TRUE);
		
		//Conversor pega o valor lido pela entrada analogica
		Conversor_GetValue16(&value);
		
		//conta de transformacao para a tensao real
		tensao = value*3300/65536;
		
		//transforma a tensao em temperatura
		temperatura = (tensao - 600)/10;
		
		//transforma temperatura em string
		sprintf(buff, "%4d oC", (int) temperatura);

		send_cmd(0x01, 26);
		
		//manda para o LCD
		send_string(buff);

		wait_n_interruptions(16666);
		
		//deve-se implementar uma logica para sair dessa funcao quando apertar o botao (!!!feito!!!)
	}
	
	// Espera botao ser 'despressionado'
	while (Button_GetVal());
	
	/*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
	for(;;){}
	/*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */

void wait_n_interruptions (int n_interruption) {

	autorizacao = 0;
	wait_interval = n_interruption;

	while (!autorizacao || wait_interval > 0);

}

void write_all_letters_with_button(char start_letter, char end_letter) {

	char caracter = start_letter;
	int char_count = 0;

	while (caracter <= end_letter) {

		if (char_count < 16)
			send_data(caracter);

		caracter++;
		char_count++;

		if (char_count > 15) {
			send_cmd(0x01, 26);
			write_all_letters(caracter - 16, caracter - 1);
		}

		while (!Button_GetVal());
		while (Button_GetVal());
	}

}

void write_all_letters(char start_letter, char end_letter) {

	char caracter = start_letter;
	int line = 1, i;
	int char_count = 0;

	while (caracter <= end_letter) {
		send_data(caracter);
		caracter++;
		char_count++;

		if (!( char_count % 16 ) ) {
			for (i = 0; i < 24; i++)
				send_data(' ');

		}
	}

}

void send_string(char *string) {

	if (!string) return;

	int i = 0;
	while (string[i] != '\0')
		send_data(string[i++]);

}

void init_LCD() {

	send_cmd(0x3F, 1);
	send_cmd(0x0F, 1);
	send_cmd(0x01, 26);
	send_cmd(0x06, 1);

}

void send_cmd(char cmd, int interval) {

	DataCmd_PutVal(cmd);

	RS_ClrVal();

	Enable_SetVal();

	Enable_ClrVal();


	wait_n_interruptions(interval);

}

void send_data(char data) {

	DataCmd_PutVal(data);
	RS_SetVal();

	Enable_SetVal();

	Enable_ClrVal();

	wait_n_interruptions(1);

}


/*!
 ** @}
 */
/*
 ** ###################################################################
 **
 **     This file was created by Processor Expert 10.3 [05.09]
 **     for the Freescale Kinetis series of microcontrollers.
 **
 ** ###################################################################
 */
