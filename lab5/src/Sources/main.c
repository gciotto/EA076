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
#include "C1.h"
#include "BitIoLdd5.h"
#include "C2.h"
#include "BitIoLdd6.h"
#include "C3.h"
#include "BitIoLdd7.h"
#include "L1.h"
#include "BitIoLdd8.h"
#include "L2.h"
#include "BitIoLdd9.h"
#include "L3.h"
#include "BitIoLdd10.h"
#include "L4.h"
#include "BitIoLdd11.h"
#include "EInt1.h"
#include "ExtIntLdd1.h"
#include "WAIT1.h"
#include "EE241.h"
#include "GI2C1.h"
#include "CI2C1.h"
#include "ConversorDA.h"
#include "DacLdd1.h"
#include "KSDK1.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"

#define M_PI 3.14

union Temperatura {
	float temp_as_float;
	byte temp_as_bytes[4];
};

bool waiting;
const char buttons_map [4][3] = {{'1', '2', '3'},{'4', '5', '6'},{'7', '8', '9'},{'#', '0', '*'}};
/* User includes (#include below this line is not maintained by Processor Expert) */

void write_all_letters(char start_letter, char end_letter);
void write_all_letters_with_button(char start_letter, char end_letter);
void send_string(char *string);
void send_cmd(char cmd, int interval);
void send_data(char data);
void wait_n_interruptions (int n_interruption);
void init_LCD();
void check_key_L1(int *result);
void check_key_L2(int *result);
void check_key_L3(int *result);
void check_key_L4(int *result);
void get_keys(int **keys_matrix);
uint16_t getDigitalTension();
char read_keys();
int test_memory();
void do_memory_with_sensor();
void generate_square_wave(int T, float V);
void generate_triangular_wave(int T, float V);
void generate_sinoidal_wave(int T, float V);
void generate_dente_serra_wave(int T, float V);

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
	/* Write your local variable definition here */

	/*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
	PE_low_level_init();
	/*** End of Processor Expert internal initialization.                    ***/

	/* Write your code here */

	init_LCD();
	isReady = 0;
	tick_counter = 0;
	EInt1_Enable();
	buttons[0] = (int*) malloc (3*sizeof(int));
	buttons[1] = (int*) malloc (3*sizeof(int));
	buttons[2] = (int*) malloc (3*sizeof(int));
	buttons[3] = (int*) malloc (3*sizeof(int));

	uint8 data, cont = 30, result;

	send_string("LCD works");

	for (;;) {

		for (;;) {
			char wave = read_keys();

			switch (wave) {
			case '1':
				send_cmd(0x01, ceil(1535 / INTERRUPT_PERIOD));
				send_string("Onda Quad.");
				generate_square_wave(250000, 3.3);
				send_cmd(0x01, ceil(1535 / INTERRUPT_PERIOD));
				send_string("Fim onda Quad.");
				break;
			case '2':
				send_cmd(0x01, ceil(1535 / INTERRUPT_PERIOD));
				send_string("Onda Triang.");
				generate_triangular_wave(250000, 3.3);
				send_cmd(0x01, ceil(1535 / INTERRUPT_PERIOD));
				send_string("Fim onda Triang.");
				break;
			case '3':
				send_cmd(0x01, ceil(1535 / INTERRUPT_PERIOD));
				send_string("Onda Senoi.");
				generate_sinoidal_wave(250000, 3.3);
				send_cmd(0x01, ceil(1535 / INTERRUPT_PERIOD));
				send_string("Fim onda Senoi.");
				break;
			}

		}

	}


  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */

void generate_square_wave(int T, float V) {

	int level = 0, count, max_level = ceil (V/3.3 * 4095);
	char p = 0 ;
	p  = read_keys();

	ConversorDA_SetValue(&level);
	/* Gera onda ateh usuario pressionar tecla 0 */
	while (p != '0') {

		count = 0;
		/* Quantidade de iteracoes a serem executadas. */
		int aux = T / (2 * INTERRUPT_PERIOD);
		while ( count < aux && p !='0') {

			if (p != '0' && p != '#'&& p != '*') p  = read_keys();
			count++;
			/* Espera uma interrupcao de 1us */
			wait_n_interruptions(1);
		}

		/* Ajusta amplitude */
		level = (level == 0 ? max_level : 0);
		ConversorDA_SetValue(&level);

		/* Suporte as teclas do LCD. */
		if (p != '0' && p != '#'&& p != '*') p  = read_keys();

		/* Aumenta periodo */
		if (p == '*') {
			T += 25000;
			p = 0;
		} /* Diminui periodo */
		else if (p == '#') {
			T -= 25000;
			p = 0;
		}
	}
}

void generate_sinoidal_wave(int T, float V) {

	float level = 0, x, x_3, x_5, x_7, x0, max_level = V/3.3 * 4095;
	int signal = 1, count, period_count = (T / INTERRUPT_PERIOD);

	unsigned int level_int = 0;
	ConversorDA_SetValue(&level_int);
	
	char p  = read_keys();
	while (p != '0') {

		/* Reinicializa variaveis importantes */
		level = 0;
		signal = 1;
		count = 0;
		x = 0;
		x0 = 0;
		period_count = (T / INTERRUPT_PERIOD);
		
		while ( count < period_count && p != '0') {

			if (p != '0' && p != '#'&& p != '*') p  = read_keys();

			x_3 = (x + x0)*(x + x0)*(x + x0);
			x_5 = x_3 * x_3 / (x + x0);
			x_7 = x_5 * x_5 / x_3;
			
			level = max_level/2 + (max_level/2 - 1) * signal * 
					(x + x0 - x_3/6 + x_5/120 - x_7/5040);

			level_int = ceil(level);
			ConversorDA_SetValue(&level_int);

			count++;

			/* Avanca x */
			x = x + (2*INTERRUPT_PERIOD*M_PI)/T;

			/* Ver explicacao no relatorio */
			if (count < period_count/2) {
				signal = 1;
				x0 = 0;
			}
			else {
				x0 = -M_PI;
				signal = -1;
			}

			wait_n_interruptions(1); /* Espera 1us */
		}

		/* Suporte as teclas do LCD. */
		if (p != '0' && p != '#'&& p != '*') p  = read_keys();

		/* Aumenta periodo */
		if (p == '*') {
			T += 25000;
			p = 0;
		} /* Diminui periodo */
		else if (p == '#') {
			T -= 25000;
			p = 0;
		}
	}

	level_int = 0;
	ConversorDA_SetValue(&level_int);
}

void generate_dente_serra_wave(int T, float V) {

	float level, max_level = V/3.3 * 4095;
	int level_int = 0;
	ConversorDA_SetValue(&level_int);

	/* Gera onda ateh usuario pressionar tecla 0 */
	char p  = read_keys();
	while (p != '0') {

		level = 0;
		while ( level < max_level && p != '0') {

			if (p != '0' && p != '#'&& p != '*') p  = read_keys();

			/* max_level * INTERRUPT_PERIOD / T eh o coeficiente angular */
			level += max_level * INTERRUPT_PERIOD / T;

			if (level > max_level)	level = max_level;

			level_int = ceil(level);
			ConversorDA_SetValue(&level_int);

			wait_n_interruptions(1); /* Espera 1us */
		}

		/* Suporte as teclas do LCD. */
		if (p != '0' && p != '#'&& p != '*') p  = read_keys();

		/* Aumenta periodo */
		if (p == '*') {
			T += 25000;
			p = 0;
		} /* Diminui periodo */
		else if (p == '#') {
			T -= 25000;
			p = 0;
		}
	}

	level = 0;
	ConversorDA_SetValue(&level);
}

void generate_triangular_wave(int T, float V) {

	float level, max_level = V/3.3 * 4095;
	int level_int = 0;
	ConversorDA_SetValue(&level_int);

	/* Gera onda ateh usuario pressionar tecla 0 */
	char p  = read_keys();
	while (p != '0') {

		level = 0;
		while ( level < max_level && p != '0') {

			if (p != '0' && p != '#'&& p != '*') p  = read_keys();

			/* max_level * (2*INTERRUPT_PERIOD) / T eh o coeficiente angular */
			level += max_level * (2*INTERRUPT_PERIOD) / T;

			if (level > max_level)	level = max_level;

			level_int = ceil(level);
			ConversorDA_SetValue(&level_int);

			wait_n_interruptions(1); /* Espera 1us */
		}

		/* Suporte as teclas do LCD. */
		if (p != '0' && p != '#'&& p != '*') p  = read_keys();

		/* Aumenta periodo */
		if (p == '*') {
			T += 25000;
			p = 0;
		} /* Diminui periodo */
		else if (p == '#') {
			T -= 25000;
			p = 0;
		}

		while ( level > 0 && p != '0') {

			if (p != '0' && p != '#'&& p != '*') p  = read_keys();

			level -= max_level * (2*INTERRUPT_PERIOD) / T ;

			if (level < 0)	level = 0;

			level_int = ceil(level);
			ConversorDA_SetValue(&level_int);

			wait_n_interruptions(1); /* Espera 1us */
		}

		/* Suporte as teclas do LCD. */
		if (p != '0' && p != '#'&& p != '*') p  = read_keys();

		/* Aumenta periodo */
		if (p == '*') {
			T += 25000;
			p = 0;
		} /* Diminui periodo */
		else if (p == '#') {
			T -= 25000;
			p = 0;
		}
	}

	level = 0;
	ConversorDA_SetValue(&level);
}
int test_memory() {

	int address;
	byte data_byte = 0, data_read;
	//for (address = 0; address < 2048; address++) {
	for (address = 0; address < 2; address++) {

		for (data_byte = 1; data_byte != 0; data_byte = data_byte << 1) {

			EE241_WriteByte(address, data_byte);
			EE241_ReadByte(address, &data_read);

			if (data_byte != data_read)
				return 0;
		}

	}

	return 1;

}

void get_keys(int **keys_matrix) {

	check_key_L1(keys_matrix[0]);

	check_key_L2(keys_matrix[1]);

	check_key_L3(keys_matrix[2]);

	check_key_L4(keys_matrix[3]);

}

void check_key_L1(int* result) {

	memset(result, 0, 3*sizeof(int));	

	L1_ClrVal();
	L2_SetVal();
	L3_SetVal();
	L4_SetVal();

	if (C1_GetVal() == 0)
		result[0] = 1;

	if (C2_GetVal() == 0)
		result[1] = 1;

	if (C3_GetVal() == 0)
		result[2] = 1;

	L1_SetVal();

}

void check_key_L2(int* result) {

	memset(result, 0, 3*sizeof(int));	

	L1_SetVal();
	L2_ClrVal();
	L3_SetVal();
	L4_SetVal();

	if (C1_GetVal() == 0)
		result[0] = 1;

	if (C2_GetVal() == 0)
		result[1] = 1;

	if (C3_GetVal() == 0)
		result[2] = 1;

	L2_SetVal();
}

void check_key_L3(int* result) {

	memset(result, 0, 3*sizeof(int));		

	L1_SetVal();
	L2_SetVal();
	L3_ClrVal();
	L4_SetVal();

	if (C1_GetVal() == 0)
		result[0] = 1;

	if (C2_GetVal() == 0)
		result[1] = 1;

	if (C3_GetVal() == 0)
		result[2] = 1;

	L3_SetVal();
}

void check_key_L4(int* result) {

	memset(result, 0, 3*sizeof(int));	

	L1_SetVal();
	L2_SetVal();
	L3_SetVal();
	L4_ClrVal();

	if (C1_GetVal() == 0)
		result[0] = 1;

	if (C2_GetVal() == 0)
		result[1] = 1;

	if (C3_GetVal() == 0)
		result[2] = 1;

	L4_SetVal();
}



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
		//while (1) {
		send_data(caracter);
		caracter++;
		char_count++;

		if (!( char_count % 16 ) ) {
			//			char_count = 0;
			//			  if (line) {
			//			  	send_cmd(0xC0, 10);
			//			  	line = 0;	
			//			  }
			//			   else  {
			//				   send_cmd(0x80, 10);
			//				   line = 1;
			//			   }
			//
			//			line = !line;
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

	send_cmd(0x3F, ceil(40 / INTERRUPT_PERIOD));
	send_cmd(0x0F, ceil(40 / INTERRUPT_PERIOD));	
	send_cmd(0x01, ceil(1536 / INTERRUPT_PERIOD));
	send_cmd(0x06, ceil(40 / INTERRUPT_PERIOD));

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

	wait_n_interruptions(ceil(40 / INTERRUPT_PERIOD));

}

char read_keys() {

	int count = 0;
	char result = 0;

	L1_ClrVal();
	L2_ClrVal();
	L3_ClrVal();
	L4_ClrVal();

	if (isReady) {
		int i,j;
		for (i = 0; i < 4; i++)
			for (j = 0; j < 3; j++) {

				if (buttons[i][j]) { 

					if (!( count % 16 ) && count != 0 ) {
						send_cmd(0x01, 26);
						count  =0;
					}

					result = buttons_map[i][j];

					//send_data(buttons_map[i][j]);
					count++;
				}
			}

		isReady = 0;
	}

	return result;
}

uint16_t getDigitalTension(){
	uint16_t value;

	Conversor_Measure(TRUE);

	//Conversor pega o valor lido pela entrada analogica
	Conversor_GetValue16(&value);

	return value;
}

void do_memory_with_sensor() {

	if (test_memory())
		send_string("Ok - Memoria");
	else send_string("Erro - Memoria");

	while(!read_keys());

	int restart = 0;
	char button_pressed;
	for(;;) {

		send_cmd(0x1, 26);
		send_string("6 para iniciar...");

		/* Espera  botao 6 */
		while (restart == 0 && read_keys() != '6');

		send_cmd(0x1, 26);
		send_string("Grav. iniciada.");

		/* Variaveis de contagem. */
		float sum = 0, min = 5000, max = 0;
		int count = 0;
		EE241_Address initial_address = 0;

		/* Amostragem dos valores. */
		while (read_keys() != '7') {

			uint16_t tension = getDigitalTension();
			float tension_f;
			union Temperatura temp;

			/* Transforma a tensao em temperatura */
			tension_f = (float)tension * 3300/65536;
			temp.temp_as_float = (tension_f - 600)/10;

			EE241_WriteBlock(initial_address, temp.temp_as_bytes, sizeof(float));
			temp.temp_as_float = 0;
			/* Conferir resultado. */
			memset(temp.temp_as_bytes, 0, sizeof(float));
			EE241_ReadBlock(initial_address, temp.temp_as_bytes, sizeof(float));

			/* Atualiza contadores */
			sum += temp.temp_as_float;
			initial_address += sizeof(float);
			count++;

			/* Atualiza maximo e minimo */
			if (temp.temp_as_float > max)
				max = temp.temp_as_float;

			if (temp.temp_as_float < min)
				min = temp.temp_as_float;

			/* Espera 1s; */
			//wait_n_interruptions(16667);
			wait_n_interruptions(1667);			
		}

		send_cmd(0x1, 26);
		send_string("Grav. finalizada.");

		restart = 0;
		while (!restart) {

			button_pressed = read_keys();	

			if (button_pressed == '6')
				restart = 1;

			/* Media, minimo ou maximo */
			if (button_pressed == '3' || button_pressed == '4' || 
					button_pressed == '5') {

				float data_print;
				char *text;

				char buffer[6];
				memset(buffer, 0, 6);

				/* Seleciona valor a ser impresso */
				if (button_pressed == '3') {
					data_print = sum/count;
					text = "Media = ";

				} else if (button_pressed == '4') {
					data_print = max;
					text = "Max. = ";

				} else {
					data_print = min;
					text = "Min. = ";

				}

				/* Calcula parte decimal da temperatura */
				float decimal = data_print - (int) data_print;
				/* Imprime resultado com duas casas decimais. */
				sprintf(buffer, "%d.%02d", (int) data_print, (int) (decimal * 100));

				/* Manda para o LCD. */
				send_cmd(0x1, 26);
				send_string(text);
				send_string(buffer);

			}

			/* Imprimir posicoes de memoria */
			if (button_pressed == '8') {

				/* Imprime primeira posicao. */
				union Temperatura temp;
				char buffer_data[6], buffer_address[6];

				memset(buffer_data, 0, 6);
				memset(buffer_address, 0, 6);

				EE241_Address aux_address = 0x0;

				temp.temp_as_float = 0;
				memset(temp.temp_as_bytes, 0, sizeof(float));
				EE241_ReadBlock(aux_address, temp.temp_as_bytes, sizeof(float));

				float decimal = temp.temp_as_float - (int) temp.temp_as_float;
				sprintf(buffer_data, "%d.%02d", (int) temp.temp_as_float, (int) (decimal * 100));

				sprintf(buffer_address, "0x%x", aux_address);

				send_cmd(0x1, 26);

				send_string(buffer_address);
				send_string(" - ");
				send_string(buffer_data);


				/* Imprime circularmente posicoes ate # for pressionado. */
				do {

					button_pressed = read_keys();

					if (button_pressed == '*') {

						/* Avanca na memoria */
						aux_address += sizeof(float);

						if (aux_address >= initial_address)
							aux_address = 0;

						/* Le float da memoria. */
						temp.temp_as_float = 0;
						memset(temp.temp_as_bytes, 0, sizeof(float));
						EE241_ReadBlock(aux_address, temp.temp_as_bytes, sizeof(float));

						/* Controi strings para impressao. */
						float decimal = temp.temp_as_float - (int) temp.temp_as_float;
						sprintf(buffer_data, "%d.%02d", (int) temp.temp_as_float, (int) (decimal * 100));
						sprintf(buffer_address, "0x%x", aux_address);

						/* Imprime posicao atual de memoria. */
						send_cmd(0x1, 26);
						send_string(buffer_address);
						send_string(" - ");
						send_string(buffer_data);

					}

				} while (button_pressed != '#');
			}

		}
	}

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
