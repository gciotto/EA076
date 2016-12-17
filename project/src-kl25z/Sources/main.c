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
#include "Bluetooth.h"
#include "ASerialLdd1.h"
#include "WAIT2.h"
#include "STM32.h"
#include "ASerialLdd2.h"
#include "I2CAccel.h"
#include "Accelerometer.h"
#include "GI2C2.h"
#include "WAIT3.h"
#include "WAIT4.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"

#define M_PI 3.14

#define CLEAR_COMMAND 153000 / INTERRUPT_PERIOD

union Temperatura {
	float temp_as_float;
	byte temp_as_bytes[4];
};

union Periodo {
	byte periodo_as_byte[4];
	int periodo_as_int;
};

bool waiting;
const char buttons_map [4][3] = {{'1', '2', '3'},{'4', '5', '6'},{'7', '8', '9'},{'#', '0', '*'}};
/* User includes (#include below this line is not maintained by Processor Expert) */

void init_buttons();

void check_key_L1(int *result);
void check_key_L2(int *result);
void check_key_L3(int *result);
void check_key_L4(int *result);
void get_keys(int **keys_matrix);
uint16_t getDigitalTension();
char read_keys();
void generate_square_wave(int T, float P);
void generate_triangular_wave(int T, float P);
void generate_sinoidal_wave(int T, float P);
void generate_sinoidal_wave_fast(int T, float P);


/* LCD related FUNCTIONS and VARIABLES */
void write_all_letters(char start_letter, char end_letter);
void write_all_letters_with_button(char start_letter, char end_letter);
void send_string(char *string);
void send_cmd(char cmd, int interval);
void send_data(char data);
void wait_n_interruptions (int n_interruption);
void init_LCD();

/* EEPROM related FUNCTIONS and VARIABLES */
float sum, min, max;
uint16_t count;
void start_EEPROM();
int test_memory();
void do_memory_with_sensor();
void do_memory_with_sensor_with_bluetooth();

/* BLUETOOTH related FUNCTIONS and VARIABLES */
int change_bluetooth_name(char *name);
int change_bluetooth_baudrate(int baudrate);
void send_char_bluetooth(char c);
void init_UART_Bluetooth();
void init_Bluetooth();
int send_string_bluetooth(char* name);

/* STM32 related FUNCTIONS and VARIABLES */
void init_UART_STM32();
int send_string_STM32(char* name);
int send_float_STM32(float f);
int get_float_STM32(float *f);
int get_int_STM32(int *f);
char get_char_STM32();

/* ADC related FUNCTIONS and VARIABLES */
uint16_t getDigitalTension();
float getTemperature();

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
	/* Write your local variable definition here */

	/*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
	PE_low_level_init();
	/*** End of Processor Expert internal initialization.                    ***/

	/* Write your code here */
	tick_counter = 0;
	init_LCD();
	init_buttons();

	Accelerometer_CalibrateZ1g();
	
	init_Bluetooth();
	init_UART_STM32();

	send_cmd(0x1, CLEAR_COMMAND);

	for (;;) {
		float amplitude, x, y, z;
		int periodo, level;

		while (!hasReceivedCharFromSTM32);

		char rcv_byte;
		STM32_RecvChar(&rcv_byte);

		send_data(rcv_byte);

		hasReceivedCharFromSTM32 = 0;

		switch (rcv_byte) {
		case 'C':
			send_cmd(0x01, CLEAR_COMMAND);
			send_string("STM32 CONNECTED!");

			send_string_STM32("OK");

			break;
		case 'D':
			send_cmd(0x01, CLEAR_COMMAND);
			send_string("STM32 DISCONN.!");


			break;

		case 0x10:

			send_cmd(0x01, CLEAR_COMMAND);
			send_string("EEPROM STARTED!");

			wait_n_interruptions(500000/INTERRUPT_PERIOD);

			send_string_STM32("OK");

			start_EEPROM();

			break;

		case 0x12:

			send_cmd(0x01, CLEAR_COMMAND);
			send_string("STATS REQUESTED!");

			send_string_STM32("SEND");
			send_float_STM32(max);
			send_float_STM32(min);
			send_float_STM32(sum/count);

			break;

		case 0x13:

			send_cmd(0x01, CLEAR_COMMAND);
			send_string("VALUES REQUESTED");

			send_string_STM32("SEND");

			uint8_t count_a[2];
			count_a[0] = (uint8_t) count & 0xFF;
			count_a[1] = (uint8_t) ((count & 0xFF00) >> 8);

			while (!isReadyToSendToSTM32);
			isReadyToSendToSTM32 = 0;

			STM32_SendChar(count_a[0]);

			while (!isReadyToSendToSTM32);
			isReadyToSendToSTM32 = 0;
			STM32_SendChar(count_a[1]);

			int i;
			EE241_Address aux_address = 0x0;
			union Temperatura aux;

			for (i = 0; i < count; i++) {

				EE241_ReadBlock(aux_address, aux.temp_as_bytes, sizeof(float));

				while (!hasReceivedCharFromSTM32);

				hasReceivedCharFromSTM32 = 0;
				STM32_RecvChar(&rcv_byte);

				if (rcv_byte == '>') {
					send_float_STM32(aux.temp_as_float);
					aux_address += sizeof(float);
				}
			}

			break;

		case 0x50:

			send_cmd(0x01, CLEAR_COMMAND);
			send_string("ADC TEMP REQUEST.");

			send_string_STM32("SEND");

			send_float_STM32(getTemperature());

			break;

		case 0x51:

			send_cmd(0x01, CLEAR_COMMAND);
			send_string("ADC POT. REQUEST.");

			uint16_t tension = getDigitalTension();
			float tension_f = (float)tension * 3.3/65536;

			send_string_STM32("SEND");

			send_float_STM32(tension_f);

			break;


		case 0x53:

			send_cmd(0x01, CLEAR_COMMAND);
			send_string("ADC BLUET. REQ.");

			send_string_STM32("SEND");

			char buffer[24];
			float temp, pressure, pot;

			get_float_STM32(&temp);
			get_float_STM32(&pot);
			get_float_STM32(&pressure);

			send_string_bluetooth("RELATORIO DO STM32 ADC:\n\r");
			memset (buffer, 0, 24);

			/* Calcula parte decimal da temperatura */
			float decimal = temp - (int) temp;
			/* Imprime resultado com duas casas decimais. */
			sprintf(buffer, "TEMP(oC) = %d.%02d\n\r", (int) temp, (int) (decimal * 100));
			send_string_bluetooth(buffer);

			memset (buffer, 0, 24);
			/* Calcula parte decimal da temperatura */
			decimal = pot - (int) pot;
			/* Imprime resultado com duas casas decimais. */
			sprintf(buffer, "TENSION (V) = %d.%02d\n\r", (int) pot, (int) (decimal * 100));
			send_string_bluetooth(buffer);

			memset (buffer, 0, 24);
			/* Calcula parte decimal da temperatura */
			decimal = pressure - (int) pressure;
			/* Imprime resultado com duas casas decimais. */
			sprintf(buffer, "PRESS. = %d.%02d\n\r", (int) pressure, (int) (decimal * 100));
			send_string_bluetooth(buffer);
			break;

		case 0x60:


			send_cmd(0x01, CLEAR_COMMAND);
			send_string("SENO REQUESTED!");

			send_string_STM32("SEND");

			while (!hasReceivedCharFromSTM32);

			hasReceivedCharFromSTM32 = 0;
			STM32_RecvChar(&rcv_byte);

			if (rcv_byte == 'P') {
				send_string_STM32("OK");
				get_int_STM32(&periodo);

			}

			while (!hasReceivedCharFromSTM32);

			hasReceivedCharFromSTM32 = 0;
			STM32_RecvChar(&rcv_byte);

			if (rcv_byte == 'A') {
				send_string_STM32("OK");
				get_float_STM32(&amplitude);
			}
			generate_sinoidal_wave_fast(periodo*1000, amplitude);

			break;



		case 0x61:
			send_cmd(0x01, CLEAR_COMMAND);
			send_string("TRIANG REQUESTED!");

			send_string_STM32("SEND");
			while (!hasReceivedCharFromSTM32);

			hasReceivedCharFromSTM32 = 0;
			STM32_RecvChar(&rcv_byte);

			if (rcv_byte == 'P') {
				send_string_STM32("OK");
				get_int_STM32(&periodo);

			}

			while (!hasReceivedCharFromSTM32);

			hasReceivedCharFromSTM32 = 0;
			STM32_RecvChar(&rcv_byte);

			if (rcv_byte == 'A') {
				send_string_STM32("OK");
				get_float_STM32(&amplitude);
			}
			generate_triangular_wave_fast(periodo*1000, amplitude);

			break;

		case 0x62:

			send_cmd(0x01, CLEAR_COMMAND);
			send_string("STEP REQUESTED!");

			send_string_STM32("SEND");
			while (!hasReceivedCharFromSTM32);

			hasReceivedCharFromSTM32 = 0;
			STM32_RecvChar(&rcv_byte);

			if (rcv_byte == 'P') {
				send_string_STM32("OK");
				get_int_STM32(&periodo);

			}

			while (!hasReceivedCharFromSTM32);

			hasReceivedCharFromSTM32 = 0;
			STM32_RecvChar(&rcv_byte);

			if (rcv_byte == 'A') {
				send_string_STM32("OK");
				get_float_STM32(&amplitude);
			}
			generate_square_wave(periodo*1000, amplitude);

			break;

		case 0x63:
			send_cmd(0x01, CLEAR_COMMAND);
			send_string("STEP REQUESTED!");

			send_string_STM32("SEND");
			get_float_STM32(&amplitude);
			level = 4095*amplitude/3.3;

			ConversorDA_SetValue(&level);

			break;

		case 0x80:

			send_cmd(0x01, CLEAR_COMMAND);
			send_string("ACCEL. REQUESTED!");

			Accelerometer_Enable();

			send_string_STM32("OK");

			do {

				while (!hasReceivedCharFromSTM32);
				hasReceivedCharFromSTM32 = 0;

				STM32_RecvChar(&rcv_byte);

				if (rcv_byte == '>') {


					x = (float) Accelerometer_GetX();
					y = (float) Accelerometer_GetY();
					z = (float) Accelerometer_GetZ();

					send_float_STM32(x);


					while (!hasReceivedCharFromSTM32);
					hasReceivedCharFromSTM32 = 0;
					STM32_RecvChar(&rcv_byte);

					send_float_STM32(y);

					while (!hasReceivedCharFromSTM32);
					hasReceivedCharFromSTM32 = 0;
					STM32_RecvChar(&rcv_byte);

					send_float_STM32(z);
				}

			} while (rcv_byte != 0x81 );

			send_string_STM32("OK");

			Accelerometer_Disable();

			send_cmd(0x01, CLEAR_COMMAND);
			send_string("ACCEL. STOPPED!");

		}

	}




	/* For example: for(;;) { } */

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

void start_EEPROM() {

	send_cmd(0x1, CLEAR_COMMAND);
	send_string("Gravando..");

	/* Variaveis de contagem. */
	char recv_char;
	sum = 0;
	min = 500;
	max = 0;
	count = 0;

	int mustStop = 0, button;
	EE241_Address initial_address = 0;

	/* Amostragem dos valores. */
	while (!mustStop) {

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

		button = read_keys();

		if (hasReceivedCharFromSTM32) {

			STM32_RecvChar(&recv_char);
			hasReceivedCharFromSTM32 = 0;

			send_string_STM32("OK");
		}

		if (button == '7' || recv_char == 0x11) {
			mustStop = 1;
		}

		/* Espera 1s; */
		wait_n_interruptions(1000000 / INTERRUPT_PERIOD);			
	}

	send_cmd(0x1, CLEAR_COMMAND);
	send_string("Finalizada.");

}

void init_UART_STM32() {

	hasReceivedCharFromSTM32 = 0;
	isReadyToSendToSTM32 = 1;
}

char get_char_STM32() {

	char result = 0;

	if (hasReceivedCharFromSTM32) {
		hasReceivedCharFromSTM32 = 0;
		STM32_RecvChar(&result);
	}

	return result;
}

void init_Bluetooth() {

	init_UART_Bluetooth();
	change_bluetooth_name("C16");
	change_bluetooth_baudrate(4);
}

void init_UART_Bluetooth() {

	hasReceivedChar = 0;
	isReadyToSend = 1;
}

void init_buttons() {

	isReady = 0;
	EInt1_Enable();
	buttons[0] = (int*) malloc (3*sizeof(int));
	buttons[1] = (int*) malloc (3*sizeof(int));
	buttons[2] = (int*) malloc (3*sizeof(int));
	buttons[3] = (int*) malloc (3*sizeof(int));

}

int send_string_Discovery(char* name) {

	int result, i;

	for (i = 0; i < strlen(name); i++) {

		while(!SendFlagDiscovery);

		SendFlagDiscovery = 0;
		result = STM32_SendChar(name[i]);

		if (result != ERR_OK)
			return result;
	}
	return ERR_OK;
}


int change_bluetooth_baudrate(int baudrate) {

	char command[255], response[255], baudrate_as_string[8];

	memset(command, 0, 255);
	memset(baudrate_as_string, 0, 8);
	memset(response, 0, 255);

	strcpy(command, "AT+BAUD");
	sprintf(baudrate_as_string, "%d", baudrate);

	strcat(command, baudrate_as_string);

	int result = send_string_bluetooth(command);
	if (result == ERR_OK) {

		int tam = 0;
		while (strcmp(response, "OK9600")) {

			char rcv_byte;
			if (hasReceivedChar) {
				Bluetooth_RecvChar(&rcv_byte);

				hasReceivedChar = 0;

				response[tam++] = rcv_byte;
				send_data(rcv_byte);
			}
		}
	}

}

int change_bluetooth_name(char *name) {

	char command[255], response[255];

	memset(command, 0, 255);
	memset(response, 0, 255);

	strcpy(command, "AT+NAME");

	strcat(command, name);

	int result = send_string_bluetooth(command);
	if (result == ERR_OK) {

		int tam = 0;
		while (strcmp(response, "OKsetname")) {

			char rcv_byte;
			if (hasReceivedChar) {
				Bluetooth_RecvChar(&rcv_byte);

				hasReceivedChar = 0;

				response[tam++] = rcv_byte;
				send_data(rcv_byte);
			}
		}
	}

}

int send_float_STM32(float f) {

	union Temperatura aux;
	int i;
	aux.temp_as_float = f;

	for (i = 0; i < 4; i++) {

		while(!isReadyToSendToSTM32);

		isReadyToSendToSTM32 = 0;
		STM32_SendChar(aux.temp_as_bytes[i]);
	}

	return 1;
}

int get_int_STM32(int *f) {

	union Periodo aux;
	int i;

	for (i = 0; i < 4; i++) {

		while(!hasReceivedCharFromSTM32);

		hasReceivedCharFromSTM32 = 0;
		STM32_RecvChar(&aux.periodo_as_byte[i]);
	}

	*f = aux.periodo_as_int;
	return 1;


}

int get_float_STM32(float *f) {

	union Temperatura aux;
	int i;

	for (i = 0; i < 4; i++) {

		while(!hasReceivedCharFromSTM32);

		hasReceivedCharFromSTM32 = 0;
		STM32_RecvChar(&aux.temp_as_bytes[i]);
	}

	*f = aux.temp_as_float;
	return 1;


}

int send_string_STM32(char* name) {

	int result, i;

	for (i = 0; i < strlen(name); i++) {

		while(!isReadyToSendToSTM32);

		isReadyToSendToSTM32 = 0;
		result = STM32_SendChar(name[i]);

		if (result != ERR_OK)
			return result;

	}

	return ERR_OK;
}

int send_string_bluetooth(char* name) {

	int result, i;

	for (i = 0; i < strlen(name); i++) {

		while(!isReadyToSend);

		isReadyToSend = 0;
		result = Bluetooth_SendChar(name[i]);

		if (result != ERR_OK)
			return result;

	}

	return ERR_OK;
}


void generate_square_wave(int T, float P) {

	int level = 0, count;
	char p = 0, stm = 0, amp = 0 ;
	p  = read_keys();

	ConversorDA_SetValue(&level);
	while (p != '0' && stm != 100) {

		count = 0;
		int aux = T / (4 * INTERRUPT_PERIOD);
		while ( count < aux && p !='0') {

			if (p != '0' && p != '#'&& p != '*') {
				p  = read_keys();
			}

			if (stm != 100 && stm != 101 && stm != 102 && stm != 103 && stm != 104)
				stm = get_char_STM32();


			count++;
			wait_n_interruptions(1);
		}

		if (p != '0' && p != '#'&& p != '*') {
			p  = read_keys();
		}
		if (stm != 100 && stm != 101 && stm != 102 && stm != 103 && stm != 104) stm = get_char_STM32();

		if (p == '*' || stm == 101) {
			send_string_STM32("OK");
			T += 25000;
			p = 0;
			stm = 0;
		}
		else if (stm == 102 ) {
			send_string_STM32("OK");
			T -= 25000;
			p = 0;
			stm = 0;

		}
		else if (stm == 103) {
			send_string_STM32("OK");
			P = P + 0.1;
			p = 0;
			stm = 0;
		}
		else if (stm == 104) {
			send_string_STM32("OK");
			P = P - 0.1;
			p = 0;
			stm = 0;
		}

		level = (level == 0 ? 4095*P/3.3 : 0);
		ConversorDA_SetValue(&level);

	}
	isReady = 0;
}

void generate_triangular_wave_fast(int T, float P) {

	int count, mustRecalculate = 1, i, level;

	//float *samples = (float*) malloc (count * sizeof(float)), x;
	float samples[500], x;
	char stm = 0;

	while (stm != 100) {

		if (mustRecalculate) {

			x = 0;
			count = floor(T / ( 250*INTERRUPT_PERIOD ));

			for (i = 0; i < count; i++) {

				samples[i] = ( P / 3.3 ) * 4095 * x;

				x = x + ( INTERRUPT_PERIOD * 250 ) / T;
			}

			mustRecalculate = 0;
		}

		i = 0;
		while (!mustRecalculate && stm != 100) {

			level = (int)samples[i];

			ConversorDA_SetValue(&level);

			wait_n_interruptions(250);
			i = (i+1) % count;

			if (stm != 100 && stm != 101 && stm != 102 && stm != 103 && stm != 104) 
				stm = get_char_STM32();

			if (stm == 101) {
				send_string_STM32("OK");
				T += 25000;
				stm = 0;
				mustRecalculate = 1;
			}
			else if (stm == 102) {
				send_string_STM32("OK");
				T -= 25000;
				stm = 0;
				mustRecalculate = 1;
			}
			else if (stm == 103) {
				send_string_STM32("OK");
				P = P + 0.1;
				stm = 0;
				mustRecalculate = 1;
			}
			else if (stm == 104) {
				send_string_STM32("OK");
				P = P - 0.1;
				stm = 0;
				mustRecalculate = 1;
			}
		}
	}

	free(samples);
}

void generate_sinoidal_wave_fast(int T, float P) {

	int count, mustRecalculate = 1, i, level, level_ant;

	//float samples = (float*) malloc (count * sizeof(float)), x, x_3, x_5, x_7;
	float samples[500], x, x_3, x_5, x_7; 
	char stm = 0;

	while (stm != 100) {

		if (mustRecalculate) {

			x = 0;
			count = floor(T / (100*INTERRUPT_PERIOD));

			for (i = 0; i < count / 2; i++) {

				x_3 = x * x * x;
				x_5 = x_3 * x_3 / x;
				x_7 = x_5 * x_5 / x_3;

				samples[i] = ( P / 3.3 ) * ((4095 / 2) + (4095 / 2) * (x - x_3/6 + x_5/120 - x_7/5040));

				if (samples[i] > 4095) samples[i] = 4095;
				
				x = x + (100*INTERRUPT_PERIOD*2*M_PI)/T;
			}

			for (; i < count; i++) {

				x_3 = (x - M_PI) * (x - M_PI) * (x - M_PI);
				x_5 = x_3 * x_3 / (x - M_PI);
				x_7 = x_5 * x_5 / x_3;

				samples[i] = ( P / 3.3 ) * ((4095 / 2) - (4095 / 2) * ((x - M_PI) - x_3/6 + x_5/120 - x_7/5040));

				if (samples[i] > 4095) samples[i] = 4095;
				
				x = x + (100*INTERRUPT_PERIOD*2*M_PI)/T;
			}

			mustRecalculate = 0;
		}

		i = 0;
		while (!mustRecalculate && stm != 100) {

			level_ant = level;
			level = round(samples[i]);

			if (level - level_ant < 500)
				ConversorDA_SetValue(&level);

			i = (i+1) % count;

			wait_n_interruptions(100);

			if (stm != 100 && stm != 101 && stm != 102 && stm != 103 && stm != 104) 
				stm = get_char_STM32();

			if (stm == 101) {
				send_string_STM32("OK");
				T += 25000;
				stm = 0;
				mustRecalculate = 1;
			}
			else if (stm == 102) {
				send_string_STM32("OK");
				T -= 25000;
				stm = 0;
				mustRecalculate = 1;
			}
			else if (stm == 103) {
				send_string_STM32("OK");
				P = P + 0.1;
				stm = 0;
				mustRecalculate = 1;
			}
			else if (stm == 104) {
				send_string_STM32("OK");
				P = P - 0.1;
				stm = 0;
				mustRecalculate = 1;
			}
		}
	}


}

void generate_sinoidal_wave(int T, float P) {

	float level = 0, level_ant, x, x_3, x_5, x_7, origin, aux;
	int signal = 1, count, period_count = (T / (INTERRUPT_PERIOD));

	unsigned int level_int;

	char p  = read_keys(), stm = 0;

	level_int = ceil(level);
	ConversorDA_SetValue(&level_int);

	while (p != '0' && stm != 100) {

		level = 0;
		level_ant = 0;
		signal = 1;
		count = 0;
		x = 0;
		origin = 0;
		period_count = ceil(T / (13.75 * INTERRUPT_PERIOD));
		while ( count < period_count && p != '0') {

			if (p != '0' && p != '#'&& p != '*') {
				p  = read_keys();
			}

			if (stm != 100 && stm != 101 && stm != 102 && stm != 103 && stm != 104)
				stm = get_char_STM32();

			level_ant = level;
			x_3 = (x + origin)*(x + origin)*(x + origin);
			x_5 = x_3 * x_3 / (x + origin);
			x_7 = x_5 * x_5 / x_3;
			level = 4095*P/(3.3*2) + 4095*P/(3.3*2) * signal * (x + origin - x_3/6 + x_5/120 - x_7/5040);


			if (level - level_ant < 100) {
				level_int = round(level);
				ConversorDA_SetValue(&level_int);
			}

			count++;

			x = x + (2*13.75*INTERRUPT_PERIOD*M_PI)/T;

			if (count <= period_count/2) {
				signal = 1;
				origin = 0;
			}
			else {
				origin = -M_PI;
				signal = -1;
			}

			wait_n_interruptions(1);
		}

		if (p != '0' && p != '#'&& p != '*') {
			p  = read_keys();
		}
		if (stm != 100 && stm != 101 && stm != 102 && stm != 103 && stm != 104) stm = get_char_STM32();

		if (p == '*' || stm == 101) {
			send_string_STM32("OK");
			T += 25000/2;
			p = 0;
			stm = 0;
		}
		else if (stm == 102 ) {
			send_string_STM32("OK");
			T -= 25000/2;
			p = 0;
			stm = 0;

		}
		else if (stm == 103) {
			send_string_STM32("OK");
			P = P + 0.1;
			p = 0;
			stm = 0;
		}
		else if (stm == 104) {
			send_string_STM32("OK");
			P = P - 0.1;
			p = 0;
			stm = 0;
		}

	}

	level_int = 0;
	ConversorDA_SetValue( &level_int);

}

void generate_triangular_wave(int T, float P) {

	float level, const_f = 4095 * (2*INTERRUPT_PERIOD) / T;
	int level_int = 0;
	ConversorDA_SetValue(&level);

	int aux = T / (2 * INTERRUPT_PERIOD), count = 0;


	char p  = read_keys(), stm = 0;
	while (p != '0' && stm != 100) {

		const_f = 4095 * (P/3.3) * (5*INTERRUPT_PERIOD) / T;

		level = 0;
		while ( level < 4095 * (P/3.3) && p != '0') {

			if (p != '0' && p != '#'&& p != '*') {
				p  = read_keys();
			}

			if (stm != 100 && stm != 101 && stm != 102 && stm != 103 && stm != 104)
				stm = get_char_STM32();

			level += const_f;

			if (level > 4095)
				level = 4095;

			level_int = ceil(level);
			ConversorDA_SetValue(&level_int);

			wait_n_interruptions(1);
		}

		if (p != '0' && p != '#'&& p != '*') {
			p  = read_keys();
		}
		if (stm != 100 && stm != 101 && stm != 102 && stm != 103 && stm != 104) stm = get_char_STM32();

		if (p == '*' || stm == 101) {
			send_string_STM32("OK");
			T += 25000;
			p = 0;
			stm = 0;
		}
		else if (stm == 102 ) {
			send_string_STM32("OK");
			T -= 25000;
			p = 0;
			stm = 0;

		}
		else if (stm == 103) {
			send_string_STM32("OK");
			P = P + 0.1;
			p = 0;
			stm = 0;
		}
		else if (stm == 104) {
			send_string_STM32("OK");
			P = P - 0.1;
			p = 0;
			stm = 0;
		}

		/*
		level = 4095;
		while ( level > 0 && p != '0') {

			if (p != '0' && p != '#'&& p != '*') p  = read_keys();

			level -= 4095 * (2*INTERRUPT_PERIOD) / T ;

			if (level < 0)
				level = 0;

			level_int = ceil(level);
			ConversorDA_SetValue(&level_int);

			wait_n_interruptions(1);

			count++;
		}

		if (p != '0' && p != '#'&& p != '*') p  = read_keys();

		if (p == '*') {
			T += 25000/4;
			p = 0;
			send_data('+');
		}
		else if (p == '#') {
			T -= 25000/4;
			p = 0;
		}

		 */
	}

	level = 0;
	ConversorDA_SetValue(&level);

	isReady = 0;

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

	wait_n_interruptions(ceil(60 / INTERRUPT_PERIOD));

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

float getTemperature() {

	float tension_f;
	uint16_t tension = getDigitalTension();

	/* Transforma a tensao em temperatura */
	tension_f = (float)tension * 3300/65536;

	return (tension_f - 600)/10;

}

char read_bluetooth() {

	char c = 0;

	if (hasReceivedChar) {
		Bluetooth_RecvChar(&c);
		hasReceivedChar = 0;
	}

	return c;

}


void do_memory_with_sensor() {

	if (test_memory())
		send_string("Ok - Memoria");
	else send_string("Erro - Memoria");

	while(!read_keys());

	int restart = 0;
	char button_pressed, char_pressed;
	for(;;) {

		send_cmd(0x1, 1550 / INTERRUPT_PERIOD);
		send_string("6 p/ iniciar");

		/* Espera  botao 6 */
		while (restart == 0 && read_keys() != '6');

		send_cmd(0x1, 1550 / INTERRUPT_PERIOD);
		send_string("Gravando..");

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
			wait_n_interruptions(100000 / INTERRUPT_PERIOD);			
		}

		send_cmd(0x1, 1550 / INTERRUPT_PERIOD);
		send_string("Finalizada.");

		restart = 0;
		while (!restart) {

			button_pressed = read_keys();
			char_pressed = read_bluetooth();


			if (button_pressed == '6' || char_pressed == 'S')
				restart = 1;

			/* Media, minimo ou maximo */
			if (button_pressed == '3' || button_pressed == '4' || 
					button_pressed == '5') {

				float data_print;
				char *text;

				char buffer[16];
				memset(buffer, 0, 16);

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
				send_cmd(0x1, 1550 / INTERRUPT_PERIOD);
				send_string(text);
				send_string(buffer);

			}

			if (char_pressed == 'R') {

				char buffer[150];
				memset(buffer, 0, 150);

				float decimal_max = max - (int) max, decimal_min = min - (int)min , decimal_avg  = sum/count - (int)(sum/count);

				int decimal_max_i = (int) (100*decimal_max), decimal_min_i = (int) (100*decimal_min), decimal_avg_i = (int) (100*decimal_avg);  
				sprintf(buffer, "\n\rRELATORIO:\n\rMEDIA: %d.%02d\n\rMAX.: %d.%02d\n\rMIN.:%d.%02d\n\r", (int) (sum/count),  decimal_avg_i,
						(int)max, decimal_max_i, (int)min, decimal_min_i);
				send_string_bluetooth(buffer);
			}

			if (char_pressed == 'L') {

				EE241_Address aux_address = 0x0;

				send_string_bluetooth("\n\rValores na memoria:\n\r");

				while (aux_address <= initial_address) {

					union Temperatura temp;
					char buffer_data[6], buffer_address[6];

					memset(buffer_data, 0, 6);
					memset(buffer_address, 0, 6);

					temp.temp_as_float = 0;
					memset(temp.temp_as_bytes, 0, sizeof(float));
					EE241_ReadBlock(aux_address, temp.temp_as_bytes, sizeof(float));

					float decimal = temp.temp_as_float - (int) temp.temp_as_float;
					sprintf(buffer_data, "%d.%02d", (int) temp.temp_as_float, (int) (decimal * 100));

					sprintf(buffer_address, "0x%x - ", aux_address);

					send_string_bluetooth(buffer_address);
					send_string_bluetooth(buffer_data);
					send_string_bluetooth("\n\r");

					aux_address += sizeof(float);
				}

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
