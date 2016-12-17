/*
 * interface.c

 *
 *  Created on: May 5, 2016
 *      Author: gciotto
 */
#include "interface.h"

const module_t NONE 	= { .type = 0x3F, .name = "NONE", .background_color = LCD_COLOR_LIGHTBLUE, .pushed_background_color = LCD_COLOR_DARKBLUE};
const module_t EEPROM	= { .type = 0x01, .name = "Read EEPROM", .background_color = LCD_COLOR_LIGHTGREEN, .pushed_background_color = LCD_COLOR_DARKGREEN};
const module_t TEMP		= { .type = 0x02, .name = "Accelerom.", .background_color = LCD_COLOR_LIGHTRED, .pushed_background_color = LCD_COLOR_DARKRED };
const module_t ADC_O	= { .type = 0x03, .name = "Use ADC", .background_color = LCD_COLOR_LIGHTCYAN, .pushed_background_color = LCD_COLOR_DARKCYAN };
const module_t DAC_O		= { .type = 0x04, .name = "Use DAC", .background_color = LCD_COLOR_LIGHTMAGENTA, .pushed_background_color = LCD_COLOR_DARKMAGENTA };

void send_UART(uint8_t* b, uint16_t len) {

	while (!isReadyToSend);

	isReadyToSend = 0;
	HAL_UART_Transmit_IT(&huart6, b, len);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {

	isReadyToSend = 1;

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	hasReceivedByte = 1;
}

module_t findModule(uint8_t code) {

	if (code == EEPROM.type) return EEPROM;
	else if (code == TEMP.type) return TEMP;
	else if (code == ADC_O.type) return ADC_O;
	else if (code == DAC_O.type) return DAC_O;

	return NONE;
}



void executeCommand(enum command command) {

	if (isConnected) {

		uint8_t buffer_in[256], *buffer_out;
		uint16_t length;

		xSemaphoreTake(mutex_handler, portMAX_DELAY);

		prepareCommand(command, &buffer_out, &length);
		sendCommand(buffer_out,length);

		vPortFree(buffer_out);

		receiveCommand(buffer_in, &length);
		processCommand(command, buffer_in, length);

		xSemaphoreGive(mutex_handler);
	}

}

void refreshConnectState(TS_StateTypeDef ts_event) {

	uint32_t color_back_pushed, color_back_normal;

	if (!isConnected){
		memcpy(buttons[BUTTON_CONNECT].title, "Conn.\0", 6);
		color_back_pushed = LCD_COLOR_DARKGREEN;
		color_back_normal = LCD_COLOR_LIGHTGREEN;
	}
	else {
		memcpy(buttons[BUTTON_CONNECT].title, "Disc.\0", 6);
		color_back_pushed = LCD_COLOR_DARKRED;
		color_back_normal = LCD_COLOR_LIGHTRED;
	}

	if (isTouched(ts_event, &buttons[BUTTON_CONNECT], BUTTON_CONNECT_WIDTH, BUTTON_HEIGTH)) {

		if (!buttons[BUTTON_CONNECT].hasBeenAcknowledged)
			buttons[BUTTON_CONNECT].buttonPressedHandler((void*) &buttons[BUTTON_CONNECT]);

		if (!buttons[BUTTON_CONNECT].isPushed)
			placeButton(buttons[BUTTON_CONNECT], color_back_pushed, LCD_COLOR_WHITE, BUTTON_CONNECT_WIDTH, BUTTON_HEIGTH);
		buttons[BUTTON_CONNECT].isPushed = 1;
	}
	else {

		if (buttons[BUTTON_CONNECT].isPushed)
			placeButton(buttons[BUTTON_CONNECT], color_back_normal, LCD_COLOR_BLACK, BUTTON_CONNECT_WIDTH, BUTTON_HEIGTH);

		buttons[BUTTON_CONNECT].isPushed = 0;
		buttons[BUTTON_CONNECT].hasBeenAcknowledged = 0;
	}
}


void refreshBoards() {

	for (int i = 0; i < 4; i++) {

		if (isConnected) pboards[i].isEnabled = 1;
		else pboards[i].isEnabled = 0;

		placeBoard(&pboards[i], pboards[i].board.module.background_color, LCD_COLOR_BLACK);
	}
}

void resetBoards() {

	pboards_c = 0;

	for (int i = 0; i < NUMBER_OF_BOARDS; i++){
		pboards[i].board.module = NONE;

		pboards[i].isEnabled = 0;
		pboards[i].isPressed = 0;
	}
}

void buttonConnectPressed(void* source){

	((button_t*)source)->hasBeenAcknowledged = 1;

	uint8_t connect_byte;


	if (!isConnected) {

		connect_byte = CONNECT_CMD;

		uint8_t *connect_answer = pvPortMalloc(3*sizeof(uint8_t));

		setStatus(CONNECTING);
		connect_byte = 'C';
		send_UART(&connect_byte, 1);

		memset(connect_answer, 0, 3);

		hasReceivedByte = 0;
		HAL_StatusTypeDef status = HAL_UART_Receive(&huart6, &connect_answer[0], 2, 4000);

		//if (status == HAL_OK && !strcmp(connect_answer, "OK")) {
		if (1) {

			isConnected = 1;

			setStatus(CONNECTED);

		}
		else {

			isConnected = 0;
			setStatus(DISCONNECTED);
		}

		vPortFree(connect_answer);


	}
	else {

		connect_byte = DISCONNECT_CMD;
		send_UART(&connect_byte, 1);

		setStatus(DISCONNECTED);
		isConnected = 0;

	}

	refreshBoards();
}

void refreshButtonState (TS_StateTypeDef ts_event,  button_t *button, uint16_t width, uint16_t heigth) {

	if (isTouched(ts_event, button, width, heigth)) {

		if (!button->isPushed) {
			paintPushedButton(*button, width, heigth);
			if (!strcmp(button->title, "<Home"))
				osDelay(150);
		}

		if (!button->hasBeenAcknowledged) {
			button->hasBeenAcknowledged = 1;
			button->buttonPressedHandler((void*) button);
		}

		button->isPushed = 1;
	}
	else {

		if (button->isPushed && isConnected)
			unpaintPushedButton(*button, width, heigth);

		button->hasBeenAcknowledged = 0;
		button->isPushed = 0;
	}

}

void paintPushedBoard(pboard_t *board) {

	placeBoard(board, board->board.module.pushed_background_color, LCD_COLOR_WHITE);
}

void unpaintPushedBoard(pboard_t *board) {
	placeBoard(board, board->board.module.background_color, LCD_COLOR_BLACK);
}

void clearEEPROMValues() {


	xSemaphoreTake(mutex_drawer, portMAX_DELAY);

	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_FillRect(212, 45, 260, 160);

	xSemaphoreGive(mutex_drawer);

}

void addEEPROMValueAt(uint16_t address, union EEPROM_data value) {

	uint16_t base_address_x = 212, base_address_y = 45, y_n;

	if (address <= 0x1c)
		y_n = base_address_y + (address / 4)*20;

	else {

		base_address_x = 347;
		y_n = base_address_y + ((address - 0x20) / 4)*20;

	}

	char buffer[32];
	memset(buffer, 0, 32);

	xSemaphoreTake(mutex_drawer, portMAX_DELAY);

	sprintf(buffer, "0x%02x", address);

	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGREEN);
	BSP_LCD_DisplayStringAt(base_address_x, y_n, (uint8_t*) buffer, LEFT_MODE);

	memset(buffer, 0, 32);
	sprintf(buffer, "%02.2f", value.data_as_float);

	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(base_address_x + 60, y_n, (uint8_t*) buffer, LEFT_MODE);

	xSemaphoreGive(mutex_drawer);

}

void setEEPROM_max(union EEPROM_data max) {

	char buffer[32];
	memset(buffer, 0, 32);

	xSemaphoreTake(mutex_drawer, portMAX_DELAY);

	sprintf(buffer, "%02.2f", max.data_as_float);

	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	BSP_LCD_SetTextColor(LCD_COLOR_DARKGREEN);
	BSP_LCD_DisplayStringAt(265, 220, (uint8_t*) buffer, LEFT_MODE);

	xSemaphoreGive(mutex_drawer);
}

void setEEPROM_min(union EEPROM_data max) {

	char buffer[32];
	memset(buffer, 0, 32);

	xSemaphoreTake(mutex_drawer, portMAX_DELAY);

	sprintf(buffer, "%02.2f", max.data_as_float);

	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);
	BSP_LCD_DisplayStringAt(265, 245, (uint8_t*) buffer, LEFT_MODE);

	xSemaphoreGive(mutex_drawer);
}

void setEEPROM_mean(union EEPROM_data max) {

	char buffer[32];
	memset(buffer, 0, 32);

	xSemaphoreTake(mutex_drawer, portMAX_DELAY);

	sprintf(buffer, "%02.2f", max.data_as_float);

	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	BSP_LCD_SetTextColor(LCD_COLOR_DARKGRAY);
	BSP_LCD_DisplayStringAt(405, 220, (uint8_t*) buffer, LEFT_MODE);

	xSemaphoreGive(mutex_drawer);
}

void setEEPROM_status(enum EEPROM_status status) {

	xSemaphoreTake(mutex_drawer, portMAX_DELAY);

	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);

	if (status == STOPPED) {

		BSP_LCD_SetTextColor(LCD_COLOR_RED);
		BSP_LCD_DisplayStringAt(330, 10, (uint8_t*) "STOPPED", LEFT_MODE);

	}
	else {

		BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
		BSP_LCD_DisplayStringAt(330, 10, (uint8_t*) "STARTED", LEFT_MODE);
	}

	xSemaphoreGive(mutex_drawer);

}

void setACCEL_X(union EEPROM_data x) {

	xSemaphoreTake(mutex_drawer, portMAX_DELAY);

	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_FillRect(230, 90, 200, 20);

	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

	char buffer[16];
	memset(buffer, 0, 16);

	sprintf(buffer, "= %.5f g", x.data_as_float/4096);

	BSP_LCD_DisplayStringAt(230, 90, buffer, LEFT_MODE);

	xSemaphoreGive(mutex_drawer);

}

void setACCEL_Y(union EEPROM_data y) {

	xSemaphoreTake(mutex_drawer, portMAX_DELAY);

	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_FillRect(230, 120, 200, 20);

	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

	char buffer[16];
	memset(buffer, 0, 16);

	sprintf(buffer, "= %.5f g", y.data_as_float/4096);

	BSP_LCD_DisplayStringAt(230, 120, buffer, LEFT_MODE);

	xSemaphoreGive(mutex_drawer);

}

void setACCEL_Z(union EEPROM_data z) {

	xSemaphoreTake(mutex_drawer, portMAX_DELAY);

	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_FillRect(230, 150, 200, 20);

	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

	char buffer[16];
	memset(buffer, 0, 16);

	sprintf(buffer, "= %.5f g", z.data_as_float/4096);

	BSP_LCD_DisplayStringAt(230, 150, buffer, LEFT_MODE);

	xSemaphoreGive(mutex_drawer);

}

void setACCEL_status(enum EEPROM_status status) {

	xSemaphoreTake(mutex_drawer, portMAX_DELAY);

	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_FillRect(330, 50, 150, 20);

	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);

	if (status == STARTED) {
		BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
		BSP_LCD_DisplayStringAt(330, 50, "STARTED", LEFT_MODE);
	}
	else {

		BSP_LCD_SetTextColor(LCD_COLOR_RED);
		BSP_LCD_DisplayStringAt(330, 50, "STOPPED", LEFT_MODE);

	}

	xSemaphoreGive(mutex_drawer);

}

void setADC_Temperature(union EEPROM_data temperature) {

	xSemaphoreTake(mutex_drawer, portMAX_DELAY);

	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_FillRect(375, 85, 70, 20);

	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

	char buffer[7];
	memset(buffer, 0, 7);

	sprintf(buffer, "= %1.2f", temperature.data_as_float);

	BSP_LCD_DisplayStringAt(375, 85, buffer, LEFT_MODE);

	xSemaphoreGive(mutex_drawer);

}

void setADC_Pressure(union EEPROM_data pressure) {

	xSemaphoreTake(mutex_drawer, portMAX_DELAY);

	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_FillRect(375, 230, 70, 20);

	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

	char buffer[7];
	memset(buffer, 0, 7);

	sprintf(buffer, "= %1.2f", pressure.data_as_float);

	BSP_LCD_DisplayStringAt(375, 230, buffer, LEFT_MODE);

	xSemaphoreGive(mutex_drawer);

}


void setADC_Tension(union EEPROM_data tension) {

	xSemaphoreTake(mutex_drawer, portMAX_DELAY);

	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_FillRect(375, 159, 70, 20);

	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

	char buffer[7];
	memset(buffer, 0, 7);

	sprintf(buffer, "= %1.2f", tension.data_as_float);

	BSP_LCD_DisplayStringAt(375, 159, buffer, LEFT_MODE);

	xSemaphoreGive(mutex_drawer);

}



void setDAC_Tension() {

	xSemaphoreTake(mutex_drawer, portMAX_DELAY);

	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_FillRect(330, 175, 50, 20);

	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

	char buffer[5];
	memset(buffer, 0, 5);

	if (dac_set_new_value > 0)
		sprintf(buffer, "%1.1f", dac_set_new_value);
	else sprintf(buffer, "0.0");

	BSP_LCD_DisplayStringAt(330, 175, buffer, LEFT_MODE);

	xSemaphoreGive(mutex_drawer);

}

void setDAC_Period() {

	xSemaphoreTake(mutex_drawer, portMAX_DELAY);

	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_FillRect(330, 112, 50, 20);

	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

	char buffer[5];
	memset(buffer, 0, 5);

	sprintf(buffer, "%d", dac_period);

	BSP_LCD_DisplayStringAt(330, 112, buffer, LEFT_MODE);

	xSemaphoreGive(mutex_drawer);

}

void setDAC_Status(enum button_index source) {

	xSemaphoreTake(mutex_drawer, portMAX_DELAY);

	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_FillRect(350, 50, 130, 20);

	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);

	if (source == DAC_SIN) {
		BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
		BSP_LCD_DisplayStringAt(350, 50, "SIN. WAVE", LEFT_MODE);
	}
	else if (source == DAC_QUAD) {
		BSP_LCD_SetTextColor(LCD_COLOR_RED);
		BSP_LCD_DisplayStringAt(350, 50, "QUA. WAVE", LEFT_MODE);
	}
	else if (source == DAC_TRI){
		BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
		BSP_LCD_DisplayStringAt(350, 50, "TRI. WAVE", LEFT_MODE);
	}
	else {
		BSP_LCD_SetTextColor(LCD_COLOR_RED);
		BSP_LCD_DisplayStringAt(350, 50, "NO WAVE", LEFT_MODE);
	}

	xSemaphoreGive(mutex_drawer);

}


void draw_EEPROM(void* board) {

	pboard_t *aBoard = (pboard_t*) board;

	aScreen = EEPROM_SELECTED;

	xSemaphoreTake(mutex_drawer, portMAX_DELAY);

	BSP_LCD_Clear(LCD_COLOR_WHITE);

	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);

	BSP_LCD_SetFont(&Font24);

	BSP_LCD_SetTextColor(LCD_COLOR_BLUE);

	BSP_LCD_SetTextColor(aBoard->board.module.background_color);
	BSP_LCD_DisplayStringAt(10, 50, aBoard->board.module.name, LEFT_MODE);

	BSP_LCD_SetFont(&Font20);

	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTRED);
	BSP_LCD_DisplayStringAt(210, 10, (uint8_t*) "STATUS :", LEFT_MODE);

	/* STATUS X = 370, Y = 10 */
	BSP_LCD_SetTextColor(LCD_COLOR_RED);
	BSP_LCD_DisplayStringAt(330, 10, (uint8_t*) "STOPPED", LEFT_MODE);

	/* VALUES BOX */

	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGRAY);
	BSP_LCD_DrawRect(210, 40, 268, 170);

	/* MAX, MIN and MEAN */

	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGREEN);
	BSP_LCD_DisplayStringAt(210, 220, (uint8_t*) "MAX.", LEFT_MODE);

	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGRAY);
	BSP_LCD_DisplayStringAt(345, 220, (uint8_t*) "MEAN", LEFT_MODE);

	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);
	BSP_LCD_DisplayStringAt(210, 245, (uint8_t*) "MIN.", LEFT_MODE);

	buttons[EEPROM_START].isEnabled = 1;
	buttons[EEPROM_STOP].isEnabled = 0;
	buttons[EEPROM_STATS].isEnabled = 0;
	buttons[EEPROM_STATE].isEnabled = 0;

	xSemaphoreGive(mutex_drawer);

	placeButton(buttons[EEPROM_START], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);
	placeButton(buttons[EEPROM_STOP], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);
	placeButton(buttons[EEPROM_STATS], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);
	placeButton(buttons[EEPROM_STATE], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);

	placeButton(button_home, LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_CONNECT_WIDTH, BUTTON_HEIGTH);

}


void draw_ACCEL(void* board) {

	pboard_t *aBoard = (pboard_t*) board;

	aScreen = ACCEL_SELECTED;

	counterAccelerometerWorking = 0;
	isAccelerometerWorking = 0;

	xSemaphoreTake(mutex_drawer, portMAX_DELAY);

	BSP_LCD_Clear(LCD_COLOR_WHITE);

	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);

	BSP_LCD_SetFont(&Font24);

	BSP_LCD_SetTextColor(LCD_COLOR_BLUE);

	BSP_LCD_SetTextColor(aBoard->board.module.background_color);
	BSP_LCD_DisplayStringAt(10, 50, aBoard->board.module.name, LEFT_MODE);

	BSP_LCD_SetFont(&Font20);

	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTRED);
	BSP_LCD_DisplayStringAt(210, 50, (uint8_t*) "STATUS :", LEFT_MODE);

	/* STATUS X = 370, Y = 10 */
	BSP_LCD_SetTextColor(LCD_COLOR_RED);
	BSP_LCD_DisplayStringAt(330, 50, (uint8_t*) "STOPPED", LEFT_MODE);

	/* MAX, MIN and MEAN */

	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGREEN);
	BSP_LCD_DisplayStringAt(210, 90, (uint8_t*) "X:", LEFT_MODE);

	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGRAY);
	BSP_LCD_DisplayStringAt(210, 120, (uint8_t*) "Y:", LEFT_MODE);

	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);
	BSP_LCD_DisplayStringAt(210, 150, (uint8_t*) "Z:", LEFT_MODE);

	buttons[ACCEL_START].isEnabled = 1;
	buttons[ACCEL_STOP].isEnabled = 0;

	xSemaphoreGive(mutex_drawer);

	placeButton(buttons[ACCEL_START], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);
	placeButton(buttons[ACCEL_STOP], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);

	placeButton(button_home, LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_CONNECT_WIDTH, BUTTON_HEIGTH);

}

void draw_DAC(void* board) {

	pboard_t *aBoard = (pboard_t*) board;
	dac_period = 250;
	dac_set_new_value = 1.1;

	dac_isDrawing = 0;

	aScreen = DAC_SELECTED;

	xSemaphoreTake(mutex_drawer, portMAX_DELAY);

	BSP_LCD_Clear(LCD_COLOR_WHITE);

	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);

	BSP_LCD_SetFont(&Font24);

	BSP_LCD_SetTextColor(LCD_COLOR_BLUE);

	BSP_LCD_SetTextColor(aBoard->board.module.background_color);
	BSP_LCD_DisplayStringAt(25, 50, aBoard->board.module.name, LEFT_MODE);

	BSP_LCD_SetFont(&Font20);

	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTRED);
	BSP_LCD_DisplayStringAt(230, 50, (uint8_t*) "STATUS :", LEFT_MODE);

	BSP_LCD_SetTextColor(LCD_COLOR_RED);
	BSP_LCD_DisplayStringAt(350, 50, (uint8_t*) "NO WAVE", LEFT_MODE);

	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGRAY);
	BSP_LCD_DrawRect(225, 75, 250 , 60);

	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGREEN);
	BSP_LCD_DisplayStringAt(230, 80, (uint8_t*) "PERIOD (ms) :", LEFT_MODE);

	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(330, 112, (uint8_t*) "250", LEFT_MODE);

	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGRAY);
	BSP_LCD_DrawRect(225, 140, 250 , 60);

	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTYELLOW);
	BSP_LCD_DisplayStringAt(230, 145, (uint8_t*) "NEW VALUE (V) :", LEFT_MODE);

	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(330, 175, (uint8_t*) "1.1", LEFT_MODE);


	buttons[DAC_SIN].isEnabled = 1;
	buttons[DAC_QUAD].isEnabled = 1;
	buttons[DAC_TRI].isEnabled = 1;
	buttons[DAC_SET].isEnabled = 1;
	buttons[DAC_SET].isEnabled = 1;
	buttons[DAC_PERIOD_DECREASE].isEnabled = 1;
	buttons[DAC_PERIOD_INCREASE].isEnabled = 1;
	buttons[DAC_TENSION_DECREASE].isEnabled = 1;
	buttons[DAC_TENSION_INCREASE].isEnabled = 1;
	buttons[DAC_STOP].isEnabled = 0;

	xSemaphoreGive(mutex_drawer);

	placeButton(buttons[DAC_SIN], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);
	placeButton(buttons[DAC_QUAD], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);
	placeButton(buttons[DAC_TRI], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);
	placeButton(buttons[DAC_SET], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);
	placeButton(buttons[DAC_PERIOD_DECREASE], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_INCREASE_W, BUTTON_INCREASE_H);
	placeButton(buttons[DAC_PERIOD_INCREASE], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_INCREASE_W, BUTTON_INCREASE_H);
	placeButton(buttons[DAC_TENSION_DECREASE], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_INCREASE_W, BUTTON_INCREASE_H);
	placeButton(buttons[DAC_TENSION_INCREASE], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_INCREASE_W, BUTTON_INCREASE_H);
	placeButton(buttons[DAC_STOP], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_CONNECT_WIDTH, BUTTON_HEIGTH);

	placeButton(button_home, LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_CONNECT_WIDTH, BUTTON_HEIGTH);

}

void draw_ADC (void* board) {

	adc_temp = 0; adc_press = 0; adc_pot = 0;
	pboard_t *aBoard = (pboard_t*) board;
	dac_period = 250;
	dac_set_new_value = 1.1;

	aScreen = ADC_SELECTED;

	xSemaphoreTake(mutex_drawer, portMAX_DELAY);

	BSP_LCD_Clear(LCD_COLOR_WHITE);

	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);

	BSP_LCD_SetFont(&Font24);

	BSP_LCD_SetTextColor(LCD_COLOR_BLUE);

	BSP_LCD_SetTextColor(aBoard->board.module.background_color);
	BSP_LCD_DisplayStringAt(25, 50, aBoard->board.module.name, LEFT_MODE);

	BSP_LCD_SetFont(&Font20);

	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGRAY);
	BSP_LCD_DrawRect(225, 50, 250 , 60);

	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGREEN);
	BSP_LCD_DisplayStringAt(230, 55, (uint8_t*) "Temperature (oC):", LEFT_MODE);

	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(375, 85, (uint8_t*) "= ", LEFT_MODE);

	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGRAY);
	BSP_LCD_DrawRect(225, 124, 250 , 60);

	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTYELLOW);
	BSP_LCD_DisplayStringAt(230, 129, (uint8_t*) "Tension (V) :", LEFT_MODE);

	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(375, 159, (uint8_t*) "= ", LEFT_MODE);

	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGRAY);
	BSP_LCD_DrawRect(225, 194, 250 , 60);

	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTMAGENTA);
	BSP_LCD_DisplayStringAt(230, 199, (uint8_t*) "Pressure (Pa?) :", LEFT_MODE);

	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(375, 230, (uint8_t*) "= ", LEFT_MODE);

	buttons[ADC_TEMP_READ].isEnabled = 1;
	buttons[ADC_POT_READ].isEnabled = 1;
	buttons[ADC_PRESS_READ].isEnabled = 1;
	buttons[ADC_SEND_BLUETOOTH].isEnabled = 1;

	xSemaphoreGive(mutex_drawer);

	placeButton(buttons[ADC_TEMP_READ], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);
	placeButton(buttons[ADC_POT_READ], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);
	placeButton(buttons[ADC_PRESS_READ], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);
	placeButton(buttons[ADC_SEND_BLUETOOTH], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);

	placeButton(button_home, LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_CONNECT_WIDTH, BUTTON_HEIGTH);

}


void refreshBoardState (TS_StateTypeDef ts_event,  pboard_t *board) {

	if (isTouchedBoard(ts_event, board)) {

		if (!board->isPressed)
			paintPushedBoard(board);

		osDelay(100);

		selected_board = board;

		board->draw_initial_screen((void*) board);
		//board->refresh_function((void*) board);

		board->isPressed = 1;
	}
	else {

		if (board->isPressed)
			unpaintPushedBoard(board);

		board->isPressed = 0;
	}

}

void paintPushedButton (button_t button, uint16_t width, uint16_t heigth) {

	placeButton(button, LCD_COLOR_LIGHTGRAY, LCD_COLOR_BLACK, width, heigth);
}

void unpaintPushedButton (button_t button, uint16_t width, uint16_t heigth) {

	placeButton(button, LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, width, heigth);
}

void setStatus(enum connection_state isConnected){

	xSemaphoreTake(mutex_drawer, portMAX_DELAY);

	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);

	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);

	BSP_LCD_FillRect(0, STATUS_Y, 320, 20);

	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

	BSP_LCD_DisplayStringAt(30, STATUS_Y, (uint8_t*) "Status: ", LEFT_MODE);

	if (isConnected == CONNECTED) {
		BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
		BSP_LCD_DisplayStringAt(140, STATUS_Y, (uint8_t*) " CONNECTED.  ", LEFT_MODE);
	}
	else if (isConnected == DISCONNECTED){
		BSP_LCD_SetTextColor(LCD_COLOR_RED);
		BSP_LCD_DisplayStringAt(140, STATUS_Y, (uint8_t*) "DISCONNECTED.", LEFT_MODE);
	}
	else if (isConnected == CONNECTING){
		BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
		BSP_LCD_DisplayStringAt(140, STATUS_Y, (uint8_t*) "CONNECTING...", LEFT_MODE);
	}

	xSemaphoreGive(mutex_drawer);
}

void setReceivedData() {

	xSemaphoreTake(mutex_drawer, portMAX_DELAY);

	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);

	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);

	BSP_LCD_FillRect(245, 95, BSP_LCD_GetXSize() - 245, 20);

	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

	BSP_LCD_DisplayStringAt(240, 95, (uint8_t*) "Received packet: ", LEFT_MODE);

	xSemaphoreGive(mutex_drawer);

}

void setTitle(char *title) {

	xSemaphoreTake(mutex_drawer, portMAX_DELAY);

	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);

	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);

	BSP_LCD_FillRect(0, 10, BSP_LCD_GetXSize(), 20);

	BSP_LCD_SetTextColor(LCD_COLOR_BLUE);

	BSP_LCD_DisplayStringAt(0, 5, (uint8_t*) title, CENTER_MODE);

	xSemaphoreGive(mutex_drawer);
}


void setSubTitle(char *title) {

	xSemaphoreTake(mutex_drawer, portMAX_DELAY);

	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);

	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);

	BSP_LCD_FillRect(0, 30, BSP_LCD_GetXSize(), 20);

	BSP_LCD_SetTextColor(LCD_COLOR_RED);

	BSP_LCD_DisplayStringAt(0, 30, (uint8_t*) title, CENTER_MODE);

	xSemaphoreGive(mutex_drawer);
}


void placeBoard(pboard_t *board, uint32_t color_back, uint32_t color_text) {

	xSemaphoreTake(mutex_drawer, portMAX_DELAY);

	char buffer[32] = "Operation #";

	buffer[10] = '0' + board->order + 1;

	BSP_LCD_SetTextColor(color_back);

	if (board->isEnabled) {
		BSP_LCD_FillRect(board->x, board->y, BOARD_WIDTH, BOARD_HEIGTH);
		BSP_LCD_SetTextColor(color_text);
		BSP_LCD_SetBackColor(color_back);
	}
	else {

		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_FillRect(board->x, board->y, BOARD_WIDTH, BOARD_HEIGTH);

		BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGRAY);
		BSP_LCD_DrawRect(board->x, board->y, BOARD_WIDTH, BOARD_HEIGTH);
		BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGRAY);
		BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	}

	BSP_LCD_DisplayStringAt(board->x + 10, board->y + 15, (uint8_t*) buffer, LEFT_MODE);

	BSP_LCD_DisplayStringAt(board->x + 10, board->y + 35, (uint8_t*) board->board.module.name, LEFT_MODE);

	xSemaphoreGive(mutex_drawer);
}

void placeButton(button_t button, uint32_t color_back, uint32_t color_text, uint16_t width, uint16_t heigth) {

	xSemaphoreTake(mutex_drawer, portMAX_DELAY);

	BSP_LCD_SetTextColor(color_back);

	if (button.isEnabled) {
		BSP_LCD_FillRect(button.x_pos, button.y_pos, width, heigth);
		BSP_LCD_SetTextColor(color_text);
		BSP_LCD_SetBackColor(color_back);

	}
	else {

		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_FillRect(button.x_pos, button.y_pos, width, heigth);

		BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGRAY);
		BSP_LCD_DrawRect(button.x_pos, button.y_pos, width, heigth);
		BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGRAY);
		BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	}

	BSP_LCD_DisplayStringAt(button.x_pos + width/12, button.y_pos + heigth/6, (uint8_t*) button.title, LEFT_MODE);

	xSemaphoreGive(mutex_drawer);
}

uint8_t isTouchedBoard(TS_StateTypeDef ts_event,  pboard_t *board) {

	if (!ts_event.touchDetected || !board->isEnabled)
		return 0;

	uint16_t x = ts_event.touchX[0], y = ts_event.touchY[0];

	if (x >= board->x && x <= board->x + BOARD_WIDTH &&
			y >= board->y && y <= board->y + BOARD_HEIGTH)
		return 1;

	return 0;

}

uint8_t isTouched(TS_StateTypeDef ts_event,  button_t *button, uint16_t width, uint16_t heigth) {

	if (!button->isEnabled || !ts_event.touchDetected)
		return 0;

	uint16_t x = ts_event.touchX[0], y = ts_event.touchY[0];

	if (x >= button->x_pos && x <= button->x_pos + width &&
			y >= button->y_pos && y <= button->y_pos + heigth)
		return 1;

	return 0;

}

void print_initial_screen(){

	aScreen = BOARD_SELECTION;

	BSP_LCD_Clear(LCD_COLOR_WHITE);

	setTitle("Projeto EA076 2016");

	setSubTitle("Gustavo C. e Anderson U.");

	setStatus(isConnected);

	uint32_t color_connect;
	uint8_t willBeEnabled = 0;
	if (isConnected) {
		color_connect = LCD_COLOR_LIGHTRED;
		willBeEnabled = 1;
		strcpy(buttons[BUTTON_CONNECT].title, "Disc.");
	}
	else {
		color_connect = LCD_COLOR_LIGHTGREEN;
		strcpy(buttons[BUTTON_CONNECT].title, "Conn.");
	}

	placeButton(buttons[BUTTON_CONNECT], color_connect, LCD_COLOR_BLACK, BUTTON_CONNECT_WIDTH, BUTTON_HEIGTH);

	for (uint8_t i = 0; i < 4; i++) {
		pboards[i].isEnabled = willBeEnabled;
		placeBoard(&pboards[i], pboards[i].board.module.background_color, LCD_COLOR_BLACK);
	}

}


void back_initial_screen (void* button) {

	((button_t*) button)->hasBeenAcknowledged = 1;

	aScreen = BOARD_SELECTION;

	BSP_LCD_Clear(LCD_COLOR_WHITE);

	print_initial_screen();
}

void EEPROM_Start_Handler (void* button) {

	uint8_t command = EEPROM_START_CMD, answer_bytes[3];
	send_UART(&command, 1);

	memset(answer_bytes, 0, 3);

	HAL_StatusTypeDef ans = HAL_UART_Receive(&huart6, answer_bytes, 2, 4000);

	if (ans == HAL_OK && !strcmp(answer_bytes, "OK")) {

		setEEPROM_status(STARTED);

		button_home.isEnabled = 0;
		placeButton(button_home, LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_CONNECT_WIDTH, BUTTON_HEIGTH);

		buttons[EEPROM_START].isEnabled = 0;
		placeButton(buttons[EEPROM_START], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);

		buttons[EEPROM_STOP].isEnabled = 1;
		placeButton(buttons[EEPROM_STOP], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);

		buttons[EEPROM_STATS].isEnabled = 0;
		placeButton(buttons[EEPROM_STATS], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);

		buttons[EEPROM_STATE].isEnabled = 0;
		placeButton(buttons[EEPROM_STATE], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);

	}
	else {

		isConnected = 0;
		print_initial_screen();
	}
}

void EEPROM_Stop_Handler (void* button) {

	uint8_t command = EEPROM_STOP_CMD, answer_bytes[3];
	send_UART(&command, 1);

	memset(answer_bytes, 0, 3);

	HAL_StatusTypeDef ans = HAL_UART_Receive(&huart6, answer_bytes, 2, 4000);

	if (ans == HAL_OK && !strcmp(answer_bytes, "OK")) {
		setEEPROM_status(STOPPED);

		button_home.isEnabled = 1;
		placeButton(button_home, LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_CONNECT_WIDTH, BUTTON_HEIGTH);

		buttons[EEPROM_START].isEnabled = 1;
		placeButton(buttons[EEPROM_START], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);

		buttons[EEPROM_STOP].isEnabled = 0;
		placeButton(buttons[EEPROM_STOP], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);

		buttons[EEPROM_STATS].isEnabled = 1;
		placeButton(buttons[EEPROM_STATS], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);

		buttons[EEPROM_STATE].isEnabled = 1;
		placeButton(buttons[EEPROM_STATE], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);
	}
	else {

		isConnected = 0;
		print_initial_screen();
	}

}

void EEPROM_Print_Handler (void* button) {

	uint8_t command = EEPROM_STATS_CMD, answer_bytes[5], mustDisconnect = 0;
	union EEPROM_data data_ee;

	send_UART(&command, 1);

	memset(answer_bytes, 0, 5);

	HAL_StatusTypeDef ans = HAL_UART_Receive(&huart6, answer_bytes, 4, 4000);

	if (ans == HAL_OK && !strcmp(answer_bytes, "SEND")) {

		ans = HAL_UART_Receive(&huart6, data_ee.data_as_bytes, 4, 4000);

		if (ans == HAL_OK)
			setEEPROM_max(data_ee);
		else mustDisconnect = 1;

		ans = HAL_UART_Receive(&huart6, data_ee.data_as_bytes, 4, 4000);

		if (ans == HAL_OK)
			setEEPROM_min(data_ee);
		else mustDisconnect = 1;

		ans = HAL_UART_Receive(&huart6, data_ee.data_as_bytes, 4, 4000);

		if (ans == HAL_OK)
			setEEPROM_mean(data_ee);
		else mustDisconnect = 1;

	}
	else mustDisconnect = 1;

	if (mustDisconnect) {

		isConnected = 0;
		print_initial_screen();
	}
}

void EEPROM_List_Handler (void* button) {

	uint8_t command = EEPROM_STATUS_CMD, answer_bytes[5], mustDisconnect = 0;
	union EEPROM_data data_ee;
	uint16_t count, values;

	clearEEPROMValues();

	send_UART(&command, 1);

	memset(answer_bytes, 0, 5);

	HAL_StatusTypeDef ans = HAL_UART_Receive(&huart6, answer_bytes, 4, 4000);

	if (ans == HAL_OK && !strcmp(answer_bytes, "SEND")) {

		// Receive number of saved values in EEPROM
		memset(answer_bytes, 0, 5);
		ans = HAL_UART_Receive(&huart6, answer_bytes, 2, 4000);

		if (ans != HAL_OK)
			mustDisconnect = 1;
		else {

			count = (answer_bytes[1] << 8) + answer_bytes[0];

			for (uint16_t i = 0; i < count; i++) {

				command = '>';
				send_UART(&command, 1);

				ans = HAL_UART_Receive(&huart6, data_ee.data_as_bytes, 4, 4000);


				if (ans == HAL_OK)
					addEEPROMValueAt(i*4, data_ee);
				else {
					mustDisconnect = 1;
					break;
				}

			}

		}

	}
	else mustDisconnect = 1;

	if (mustDisconnect){

		isConnected = 0;
		print_initial_screen();
	}

}


void ACCEL_Start_Handler (void* button) {

	uint8_t command = ACCEL_START_CMD, answer_bytes[3];
	send_UART(&command, 1);

	memset(answer_bytes, 0, 3);

	HAL_StatusTypeDef ans = HAL_UART_Receive(&huart6, answer_bytes, 2, 4000);

	if (ans == HAL_OK && !strcmp(answer_bytes, "OK")) {

		setACCEL_status(STARTED);

		isAccelerometerWorking = 1;

		button_home.isEnabled = 0;
		buttons[ACCEL_START].isEnabled = 0;
		buttons[ACCEL_STOP].isEnabled = 1;

		placeButton(button_home, LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_CONNECT_WIDTH, BUTTON_HEIGTH);
		placeButton(buttons[ACCEL_START], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);
		placeButton(buttons[ACCEL_STOP], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);

	}
	else {

		isConnected = 0;
		print_initial_screen();
	}
}

void ACCEL_Stop_Handler (void* button) {

	uint8_t command = ACCEL_STOP_CMD, answer_bytes[3];
	send_UART(&command, 1);

	memset(answer_bytes, 0, 3);

	HAL_StatusTypeDef ans = HAL_UART_Receive(&huart6, answer_bytes, 2, 4000);

	if (ans == HAL_OK && !strcmp(answer_bytes, "OK")) {

		setACCEL_status(STOPPED);

		isAccelerometerWorking = 0;

		button_home.isEnabled = 1;
		buttons[ACCEL_START].isEnabled = 1;
		buttons[ACCEL_STOP].isEnabled = 0;

		placeButton(button_home, LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_CONNECT_WIDTH, BUTTON_HEIGTH);
		placeButton(buttons[ACCEL_START], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);
		placeButton(buttons[ACCEL_STOP], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);

	}
	else {

		isConnected = 0;
		print_initial_screen();
	}

}

void DAC_Stop_handler (void* button) {

	uint8_t command = DAC_STOP_CMD;

	dac_isDrawing = 0;

	send_UART(&command, 1);

	buttons[DAC_SIN].isEnabled = 1;
	buttons[DAC_TRI].isEnabled = 1;
	buttons[DAC_QUAD].isEnabled = 1;
	buttons[DAC_SET].isEnabled = 1;
	buttons[DAC_STOP].isEnabled = 0;
	button_home.isEnabled = 1;

	placeButton(buttons[DAC_SIN], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);
	placeButton(buttons[DAC_QUAD], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);
	placeButton(buttons[DAC_TRI], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);
	placeButton(buttons[DAC_SET], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);
	placeButton(buttons[DAC_STOP], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_CONNECT_WIDTH, BUTTON_HEIGTH);
	placeButton(button_home, LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_CONNECT_WIDTH, BUTTON_HEIGTH);

	setDAC_Status(DAC_STOP);

}

void DAC_Decrease_Period_handler (void* button) {

	uint8_t command = DAC_PERIOD_DECREASE_CMD;

	((button_t*) button)->hasBeenAcknowledged = 1;

	if (dac_period > 25) {
		dac_period -= 25;
	}

	else {
		dac_period = 0;
		((button_t*) button)->isEnabled = 0;
	}

	if (!buttons[DAC_PERIOD_INCREASE].isEnabled) {
		buttons[DAC_PERIOD_INCREASE].isEnabled = 1;
		placeButton(buttons[DAC_PERIOD_INCREASE], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_INCREASE_W, BUTTON_INCREASE_H);
	}


	if (dac_isDrawing) {

		send_UART(&command, 1);

		uint8_t answer_bytes[5];
		memset(answer_bytes, 0, 5);
		HAL_StatusTypeDef ans = HAL_UART_Receive(&huart6, answer_bytes, 2, 4000);

		if (ans == HAL_OK && !strcmp(answer_bytes, "OK")) {

			setDAC_Period();
		}

	}
	else setDAC_Period();

}

void DAC_Increase_Period_handler (void* button) {

	uint8_t command = DAC_PERIOD_INCREASE_CMD;

	((button_t*) button)->hasBeenAcknowledged = 1;

	dac_period += 25;

	if (!buttons[DAC_PERIOD_DECREASE].isEnabled) {
		buttons[DAC_PERIOD_DECREASE].isEnabled = 1;
		placeButton(buttons[DAC_PERIOD_DECREASE], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_INCREASE_W, BUTTON_INCREASE_H);
	}


	if (dac_isDrawing) {
		send_UART(&command, 1);

		uint8_t answer_bytes[5];
		memset(answer_bytes, 0, 5);
		HAL_StatusTypeDef ans = HAL_UART_Receive(&huart6, answer_bytes, 2, 4000);

		if (ans == HAL_OK && !strcmp(answer_bytes, "OK")) {

			setDAC_Period();
		}
	}
	else setDAC_Period();



}

void DAC_Decrease_Tension_handler (void* button) {


	((button_t*) button)->hasBeenAcknowledged = 1;

	if (dac_set_new_value > 0.2) {
		dac_set_new_value -= 0.1;
	}

	else {
		dac_set_new_value = 0;
		((button_t*) button)->isEnabled = 0;
	}

	if (!buttons[DAC_TENSION_INCREASE].isEnabled) {
		buttons[DAC_TENSION_INCREASE].isEnabled = 1;
		placeButton(buttons[DAC_TENSION_INCREASE], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_INCREASE_W, BUTTON_INCREASE_H);
	}

	uint8_t command = DAC_TENSION_DECREASE_CMD;

	if (dac_isDrawing) {

		send_UART(&command, 1);

		uint8_t answer_bytes[5];
		memset(answer_bytes, 0, 5);
		HAL_StatusTypeDef ans = HAL_UART_Receive(&huart6, answer_bytes, 2, 4000);

		if (ans == HAL_OK && !strcmp(answer_bytes, "OK")) {

			setDAC_Tension();
		}

	}
	else setDAC_Tension();

}

void DAC_Increase_Tension_handler (void* button) {

	((button_t*) button)->hasBeenAcknowledged = 1;

	if (dac_set_new_value < 3.2) {
		dac_set_new_value += 0.1;
	}

	else {
		dac_set_new_value = 3.3;
		((button_t*) button)->isEnabled = 0;
	}

	if (!buttons[DAC_TENSION_DECREASE].isEnabled) {
		buttons[DAC_TENSION_DECREASE].isEnabled = 1;
		placeButton(buttons[DAC_TENSION_DECREASE], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_INCREASE_W, BUTTON_INCREASE_H);
	}

	uint8_t command = DAC_TENSION_INCREASE_CMD;

	if (dac_isDrawing) {

		send_UART(&command, 1);

		uint8_t answer_bytes[5];
		memset(answer_bytes, 0, 5);
		HAL_StatusTypeDef ans = HAL_UART_Receive(&huart6, answer_bytes, 2, 4000);

		if (ans == HAL_OK && !strcmp(answer_bytes, "OK")) {

			setDAC_Tension();
		}

	}
	else setDAC_Tension();
}

void DAC_Sin_handler (void* button) {

	uint8_t command = DAC_SIN_CMD, answer_bytes[5], mustDisconnect = 0;
	union EEPROM_data data_ee;
	union DAC_data data_dac;

	send_UART(&command, 1);

	memset(answer_bytes, 0, 5);
	HAL_StatusTypeDef ans = HAL_UART_Receive(&huart6, answer_bytes, 4, 4000);

	if (ans == HAL_OK && !strcmp(answer_bytes, "SEND")) {

		data_ee.data_as_float = dac_set_new_value;
		data_dac.data_as_uint32 = dac_period;

		command = 'P';
		send_UART(&command, 1);

		memset(answer_bytes, 0, 5);
		ans = HAL_UART_Receive(&huart6, answer_bytes, 2, 4000);

		if (ans != HAL_OK || strcmp(answer_bytes, "OK"))
			mustDisconnect = 1;
		else {

			for (int i = 0; i < 4; i++)
				send_UART(&data_dac.data_as_bytes[i], 1);

			command = 'A';
			send_UART(&command, 1);

			memset(answer_bytes, 0, 5);
			ans = HAL_UART_Receive(&huart6, answer_bytes, 2, 4000);

			if (ans != HAL_OK || strcmp(answer_bytes, "OK"))
				mustDisconnect = 1;
			else {

				for (int i = 0; i < 4; i++)
					send_UART(&data_ee.data_as_bytes[i], 1);

				buttons[DAC_SIN].isEnabled = 0;
				buttons[DAC_TRI].isEnabled = 0;
				buttons[DAC_QUAD].isEnabled = 0;
				buttons[DAC_SET].isEnabled = 0;
				buttons[DAC_STOP].isEnabled = 1;
				button_home.isEnabled = 0;

				placeButton(buttons[DAC_SIN], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);
				placeButton(buttons[DAC_QUAD], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);
				placeButton(buttons[DAC_TRI], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);
				placeButton(buttons[DAC_SET], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);
				placeButton(buttons[DAC_STOP], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_CONNECT_WIDTH, BUTTON_HEIGTH);
				placeButton(button_home, LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_CONNECT_WIDTH, BUTTON_HEIGTH);

				setDAC_Status(DAC_SIN);
				dac_isDrawing = 1;
			}
		}

	}
	else mustDisconnect = 1;

	if (mustDisconnect){

		dac_isDrawing = 0;
		isConnected = 0;
		print_initial_screen();
	}

}

void DAC_Quad_handler (void* button) {

	uint8_t command = DAC_QUAD_CMD, answer_bytes[5], mustDisconnect = 0;
	union EEPROM_data data_ee;
	union DAC_data data_dac;

	send_UART(&command, 1);

	memset(answer_bytes, 0, 5);
	HAL_StatusTypeDef ans = HAL_UART_Receive(&huart6, answer_bytes, 4, 4000);

	if (ans == HAL_OK && !strcmp(answer_bytes, "SEND")) {

		data_ee.data_as_float = dac_set_new_value;
		data_dac.data_as_uint32 = dac_period;

		command = 'P';
		send_UART(&command, 1);

		memset(answer_bytes, 0, 5);
		ans = HAL_UART_Receive(&huart6, answer_bytes, 2, 4000);

		if (ans != HAL_OK || strcmp(answer_bytes, "OK"))
			mustDisconnect = 1;
		else {

			for (int i = 0; i < 4; i++)
				send_UART(&data_dac.data_as_bytes[i], 1);

			command = 'A';
			send_UART(&command, 1);

			memset(answer_bytes, 0, 5);
			ans = HAL_UART_Receive(&huart6, answer_bytes, 2, 4000);

			if (ans != HAL_OK || strcmp(answer_bytes, "OK"))
				mustDisconnect = 1;
			else {

				for (int i = 0; i < 4; i++)
					send_UART(&data_ee.data_as_bytes[i], 1);

				setDAC_Status(DAC_QUAD);

				buttons[DAC_SIN].isEnabled = 0;
				buttons[DAC_TRI].isEnabled = 0;
				buttons[DAC_QUAD].isEnabled = 0;
				buttons[DAC_SET].isEnabled = 0;
				buttons[DAC_STOP].isEnabled = 1;
				button_home.isEnabled = 0;

				placeButton(buttons[DAC_SIN], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);
				placeButton(buttons[DAC_QUAD], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);
				placeButton(buttons[DAC_TRI], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);
				placeButton(buttons[DAC_SET], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);
				placeButton(buttons[DAC_STOP], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_CONNECT_WIDTH, BUTTON_HEIGTH);
				placeButton(button_home, LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_CONNECT_WIDTH, BUTTON_HEIGTH);

				dac_isDrawing = 1;
			}
		}

	}
	else mustDisconnect = 1;

	if (mustDisconnect){

		dac_isDrawing = 0;
		isConnected = 0;
		print_initial_screen();
	}

}

void DAC_Tri_handler (void* button) {

	uint8_t command = DAC_TRI_CMD, answer_bytes[5], mustDisconnect = 0;
	union EEPROM_data data_ee;
	union DAC_data data_dac;

	send_UART(&command, 1);

	memset(answer_bytes, 0, 5);
	HAL_StatusTypeDef ans = HAL_UART_Receive(&huart6, answer_bytes, 4, 4000);

	if (ans == HAL_OK && !strcmp(answer_bytes, "SEND")) {

		data_ee.data_as_float = dac_set_new_value;
		data_dac.data_as_uint32 = dac_period;

		command = 'P';
		send_UART(&command, 1);

		memset(answer_bytes, 0, 5);
		ans = HAL_UART_Receive(&huart6, answer_bytes, 2, 4000);

		if (ans != HAL_OK || strcmp(answer_bytes, "OK"))
			mustDisconnect = 1;
		else {

			for (int i = 0; i < 4; i++)
				send_UART(&data_dac.data_as_bytes[i], 1);

			command = 'A';
			send_UART(&command, 1);

			memset(answer_bytes, 0, 5);
			ans = HAL_UART_Receive(&huart6, answer_bytes, 2, 4000);

			if (ans != HAL_OK || strcmp(answer_bytes, "OK"))
				mustDisconnect = 1;
			else {

				for (int i = 0; i < 4; i++)
					send_UART(&data_ee.data_as_bytes[i], 1);

				setDAC_Status(DAC_TRI);

				buttons[DAC_SIN].isEnabled = 0;
				buttons[DAC_TRI].isEnabled = 0;
				buttons[DAC_QUAD].isEnabled = 0;
				buttons[DAC_SET].isEnabled = 0;
				buttons[DAC_STOP].isEnabled = 1;
				button_home.isEnabled = 0;

				placeButton(buttons[DAC_SIN], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);
				placeButton(buttons[DAC_QUAD], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);
				placeButton(buttons[DAC_TRI], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);
				placeButton(buttons[DAC_SET], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_WIDTH, BUTTON_HEIGTH);
				placeButton(buttons[DAC_STOP], LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_CONNECT_WIDTH, BUTTON_HEIGTH);
				placeButton(button_home, LCD_COLOR_LIGHTBLUE, LCD_COLOR_WHITE, BUTTON_CONNECT_WIDTH, BUTTON_HEIGTH);

				dac_isDrawing = 1;
			}
		}

	}
	else mustDisconnect = 1;

	if (mustDisconnect){

		dac_isDrawing = 0;
		isConnected = 0;
		print_initial_screen();
	}

}

void DAC_Set_handler (void* button) {

	uint8_t command = DAC_SET_CMD, answer_bytes[5], mustDisconnect = 0;
	union EEPROM_data data_ee;

	dac_isDrawing = 0;

	send_UART(&command, 1);

	memset(answer_bytes, 0, 5);
	HAL_StatusTypeDef ans = HAL_UART_Receive(&huart6, answer_bytes, 4, 4000);

	if (ans == HAL_OK && !strcmp(answer_bytes, "SEND")) {

		data_ee.data_as_float = dac_set_new_value;

		for (int i = 0; i < 4; i++)
			send_UART(&data_ee.data_as_bytes[i], 1);

	}
	else mustDisconnect = 1;

	if (mustDisconnect){

		isConnected = 0;
		print_initial_screen();
	}

	setDAC_Status(DAC_SET);

}

void ADC_Temp_handler (void* button) {

	uint8_t command = ADC_TEMP_CMD, answer_bytes[5], mustDisconnect = 0;
	union EEPROM_data data_ee;

	send_UART(&command, 1);

	memset(answer_bytes, 0, 5);

	HAL_StatusTypeDef ans = HAL_UART_Receive(&huart6, answer_bytes, 4, 4000);

	if (ans == HAL_OK && !strcmp(answer_bytes, "SEND")) {

		ans = HAL_UART_Receive(&huart6, data_ee.data_as_bytes, 4, 4000);

		if (ans == HAL_OK) {
			setADC_Temperature( data_ee);
			adc_temp = data_ee.data_as_float;
		}
		else mustDisconnect = 1;


	}
	else mustDisconnect = 1;

	if (mustDisconnect){

		isConnected = 0;
		print_initial_screen();
	}
}

void ADC_Potent_handler (void* button) {

	uint8_t command = ADC_POT_CMD, answer_bytes[5], mustDisconnect = 0;
	union EEPROM_data data_ee;

	send_UART(&command, 1);

	memset(answer_bytes, 0, 5);

	HAL_StatusTypeDef ans = HAL_UART_Receive(&huart6, answer_bytes, 4, 4000);

	if (ans == HAL_OK && !strcmp(answer_bytes, "SEND")) {

		ans = HAL_UART_Receive(&huart6, data_ee.data_as_bytes, 4, 4000);

		if (ans == HAL_OK) {
			setADC_Tension( data_ee);
			adc_pot = data_ee.data_as_float;
		}
		else mustDisconnect = 1;


	}
	else mustDisconnect = 1;

	if (mustDisconnect){

		isConnected = 0;
		print_initial_screen();
	}
}

void ADC_Pressure_handler (void* button) {

	union EEPROM_data teste;
	teste.data_as_float = adc_press;

	setADC_Pressure(teste);

}

void ADC_Bluetooth_handler (void* button) {

	uint8_t command = ADC_BLUE_CMD, answer_bytes[5], mustDisconnect = 0;
	union EEPROM_data data_ee;

	send_UART(&command, 1);

	memset(answer_bytes, 0, 5);

	HAL_StatusTypeDef ans = HAL_UART_Receive(&huart6, answer_bytes, 4, 4000);

	if (ans == HAL_OK && !strcmp(answer_bytes, "SEND")) {

		data_ee.data_as_float = adc_temp;

		for (int i = 0; i < 4; i++)
			send_UART(&data_ee.data_as_bytes[i], 1);

		data_ee.data_as_float = adc_pot;

		for (int i = 0; i < 4; i++)
			send_UART(&data_ee.data_as_bytes[i], 1);

		data_ee.data_as_float = adc_press;
		for (int i = 0; i < 4; i++)
			send_UART(&data_ee.data_as_bytes[i], 1);

	}
	else mustDisconnect = 1;

	if (mustDisconnect){

		isConnected = 0;
		print_initial_screen();
	}


}

void init_interface(void) {

	BSP_LCD_Init();

	BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);

	BSP_LCD_DisplayOn();

	BSP_LCD_SelectLayer(0);

	BSP_LCD_Clear(LCD_COLOR_WHITE);

	BSP_LCD_SetTransparency(0, 200);

	BSP_LCD_SetFont(&Font20);

	button_home.x_pos = 5;
	button_home.y_pos = 5;
	button_home.hasBeenAcknowledged = 0;
	button_home.isEnabled = 1;
	button_home.isPushed = 0;
	memcpy(button_home.title, "<Home", 6);
	button_home.buttonPressedHandler = back_initial_screen;

	buttons[EEPROM_START].x_pos = EEPROM_START_X;
	buttons[EEPROM_START].y_pos = EEPROM_START_Y;
	buttons[EEPROM_START].isPushed = 0;
	buttons[EEPROM_START].hasBeenAcknowledged = 0;
	buttons[EEPROM_START].isEnabled = 0;
	buttons[EEPROM_START].buttonPressedHandler = EEPROM_Start_Handler;
	memcpy(buttons[EEPROM_START].title, "Start Sampl.\0", 13);

	buttons[EEPROM_STOP].x_pos = EEPROM_STOP_X;
	buttons[EEPROM_STOP].y_pos = EEPROM_STOP_Y;
	buttons[EEPROM_STOP].isPushed = 0;
	buttons[EEPROM_STOP].hasBeenAcknowledged = 0;
	buttons[EEPROM_STOP].buttonPressedHandler = EEPROM_Stop_Handler;
	memcpy(buttons[EEPROM_STOP].title, "Stop Sampl.\0", 12);
	buttons[EEPROM_STOP].isEnabled = 0;

	buttons[EEPROM_STATS].x_pos = EEPROM_STATS_X;
	buttons[EEPROM_STATS].y_pos = EEPROM_STATS_Y;
	buttons[EEPROM_STATS].isPushed = 0;
	buttons[EEPROM_STATS].isEnabled = 0;
	buttons[EEPROM_STATS].hasBeenAcknowledged = 0;
	buttons[EEPROM_STATS].buttonPressedHandler = EEPROM_Print_Handler;
	memcpy(buttons[EEPROM_STATS].title, "Print Stats.\0", 13);

	buttons[EEPROM_STATE].x_pos = EEPROM_STATE_X;
	buttons[EEPROM_STATE].y_pos = EEPROM_STATE_Y;
	buttons[EEPROM_STATE].isPushed = 0;
	buttons[EEPROM_STATE].hasBeenAcknowledged = 0;
	buttons[EEPROM_STATE].isEnabled = 0;
	buttons[EEPROM_STATE].buttonPressedHandler = EEPROM_List_Handler;
	memcpy(buttons[EEPROM_STATE].title, "List Values.\0", 13);

	buttons[ACCEL_START].x_pos = ACCEL_START_X;
	buttons[ACCEL_START].y_pos = ACCEL_START_Y;
	buttons[ACCEL_START].isPushed = 0;
	buttons[ACCEL_START].hasBeenAcknowledged = 0;
	buttons[ACCEL_START].isEnabled = 0;
	buttons[ACCEL_START].buttonPressedHandler = ACCEL_Start_Handler;
	memcpy(buttons[ACCEL_START].title, "Start!\0", 6);

	buttons[ACCEL_STOP].x_pos = ACCEL_STOP_X;
	buttons[ACCEL_STOP].y_pos = ACCEL_STOP_Y;
	buttons[ACCEL_STOP].isPushed = 0;
	buttons[ACCEL_STOP].hasBeenAcknowledged = 0;
	buttons[ACCEL_STOP].buttonPressedHandler = ACCEL_Stop_Handler;
	memcpy(buttons[ACCEL_STOP].title, "Stop!\0", 6);
	buttons[ACCEL_STOP].isEnabled = 0;



	buttons[ADC_TEMP_READ].x_pos = ADC_TEMP_READ_X;
	buttons[ADC_TEMP_READ].y_pos = ADC_TEMP_READ_Y;
	buttons[ADC_TEMP_READ].isPushed = 0;
	buttons[ADC_TEMP_READ].hasBeenAcknowledged = 0;
	buttons[ADC_TEMP_READ].isEnabled = 0;
	buttons[ADC_TEMP_READ].buttonPressedHandler = ADC_Temp_handler;
	memcpy(buttons[ADC_TEMP_READ].title, "Get Temper.\0", 12);

	buttons[ADC_POT_READ].x_pos = ADC_POT_READ_X;
	buttons[ADC_POT_READ].y_pos = ADC_POT_READ_Y;
	buttons[ADC_POT_READ].isPushed = 0;
	buttons[ADC_POT_READ].hasBeenAcknowledged = 0;
	buttons[ADC_POT_READ].isEnabled = 0;
	buttons[ADC_POT_READ].buttonPressedHandler = ADC_Potent_handler;
	memcpy(buttons[ADC_POT_READ].title, "Get Potent.\0", 12);

	buttons[ADC_PRESS_READ].x_pos = ADC_PRESS_READ_X;
	buttons[ADC_PRESS_READ].y_pos = ADC_PRESS_READ_Y;
	buttons[ADC_PRESS_READ].isPushed = 0;
	buttons[ADC_PRESS_READ].hasBeenAcknowledged = 0;
	buttons[ADC_PRESS_READ].isEnabled = 0;
	buttons[ADC_PRESS_READ].buttonPressedHandler = ADC_Pressure_handler;
	memcpy(buttons[ADC_PRESS_READ].title, "Get Pressure\0", 13);

	buttons[ADC_SEND_BLUETOOTH].x_pos = ADC_SEND_BLUETOOTH_X;
	buttons[ADC_SEND_BLUETOOTH].y_pos = ADC_SEND_BLUETOOTH_Y;
	buttons[ADC_SEND_BLUETOOTH].isPushed = 0;
	buttons[ADC_SEND_BLUETOOTH].hasBeenAcknowledged = 0;
	buttons[ADC_SEND_BLUETOOTH].isEnabled = 0;
	buttons[ADC_SEND_BLUETOOTH].buttonPressedHandler = ADC_Bluetooth_handler;
	memcpy(buttons[ADC_SEND_BLUETOOTH].title, "Send Bluet.\0", 12);

	buttons[DAC_SIN].x_pos = DAC_SIN_X;
	buttons[DAC_SIN].y_pos = DAC_SIN_Y;
	buttons[DAC_SIN].isPushed = 0;
	buttons[DAC_SIN].hasBeenAcknowledged = 0;
	buttons[DAC_SIN].isEnabled = 0;
	buttons[DAC_SIN].buttonPressedHandler = DAC_Sin_handler;
	memcpy(buttons[DAC_SIN].title, "Sin() Wave\0", 11);

	buttons[DAC_QUAD].x_pos = DAC_QUAD_X;
	buttons[DAC_QUAD].y_pos = DAC_QUAD_Y;
	buttons[DAC_QUAD].isPushed = 0;
	buttons[DAC_QUAD].hasBeenAcknowledged = 0;
	buttons[DAC_QUAD].isEnabled = 0;
	buttons[DAC_QUAD].buttonPressedHandler = DAC_Quad_handler;
	memcpy(buttons[DAC_QUAD].title, "Step Wave\0", 10);

	buttons[DAC_TRI].x_pos = DAC_TRI_X;
	buttons[DAC_TRI].y_pos = DAC_TRI_Y;
	buttons[DAC_TRI].isPushed = 0;
	buttons[DAC_TRI].hasBeenAcknowledged = 0;
	buttons[DAC_TRI].isEnabled = 0;
	buttons[DAC_TRI].buttonPressedHandler = DAC_Tri_handler;
	memcpy(buttons[DAC_TRI].title, "Tri. Wave\0", 10);

	buttons[DAC_SET].x_pos = DAC_SET_X;
	buttons[DAC_SET].y_pos = DAC_SET_Y;
	buttons[DAC_SET].isPushed = 0;
	buttons[DAC_SET].hasBeenAcknowledged = 0;
	buttons[DAC_SET].isEnabled = 0;
	buttons[DAC_SET].buttonPressedHandler = DAC_Set_handler;
	memcpy(buttons[DAC_SET].title, "Set Value.\0", 10);

	buttons[DAC_PERIOD_DECREASE].x_pos = DAC_PERIOD_DECREASE_X;
	buttons[DAC_PERIOD_DECREASE].y_pos = DAC_PERIOD_DECREASE_Y;
	buttons[DAC_PERIOD_DECREASE].isPushed = 0;
	buttons[DAC_PERIOD_DECREASE].hasBeenAcknowledged = 0;
	buttons[DAC_PERIOD_DECREASE].isEnabled = 0;
	buttons[DAC_PERIOD_DECREASE].buttonPressedHandler = DAC_Decrease_Period_handler;
	memcpy(buttons[DAC_PERIOD_DECREASE].title, "<<\0", 3);

	buttons[DAC_PERIOD_INCREASE].x_pos = DAC_PERIOD_INCREASE_X;
	buttons[DAC_PERIOD_INCREASE].y_pos = DAC_PERIOD_INCREASE_Y;
	buttons[DAC_PERIOD_INCREASE].isPushed = 0;
	buttons[DAC_PERIOD_INCREASE].hasBeenAcknowledged = 0;
	buttons[DAC_PERIOD_INCREASE].isEnabled = 0;
	buttons[DAC_PERIOD_INCREASE].buttonPressedHandler = DAC_Increase_Period_handler;
	memcpy(buttons[DAC_PERIOD_INCREASE].title, ">>\0", 3);


	buttons[DAC_TENSION_DECREASE].x_pos = DAC_TENSION_DECREASE_X;
	buttons[DAC_TENSION_DECREASE].y_pos = DAC_TENSION_DECREASE_Y;
	buttons[DAC_TENSION_DECREASE].isPushed = 0;
	buttons[DAC_TENSION_DECREASE].hasBeenAcknowledged = 0;
	buttons[DAC_TENSION_DECREASE].isEnabled = 0;
	buttons[DAC_TENSION_DECREASE].buttonPressedHandler = DAC_Decrease_Tension_handler;
	memcpy(buttons[DAC_TENSION_DECREASE].title, "<<\0", 3);

	buttons[DAC_TENSION_INCREASE].x_pos = DAC_TENSION_INCREASE_X;
	buttons[DAC_TENSION_INCREASE].y_pos = DAC_TENSION_INCREASE_Y;
	buttons[DAC_TENSION_INCREASE].isPushed = 0;
	buttons[DAC_TENSION_INCREASE].hasBeenAcknowledged = 0;
	buttons[DAC_TENSION_INCREASE].isEnabled = 0;
	buttons[DAC_TENSION_INCREASE].buttonPressedHandler = DAC_Increase_Tension_handler;
	memcpy(buttons[DAC_TENSION_INCREASE].title, ">>\0", 3);

	buttons[DAC_STOP].x_pos = DAC_STOP_X;
	buttons[DAC_STOP].y_pos = DAC_STOP_Y;
	buttons[DAC_STOP].isPushed = 0;
	buttons[DAC_STOP].hasBeenAcknowledged = 0;
	buttons[DAC_STOP].isEnabled = 0;
	buttons[DAC_STOP].buttonPressedHandler = DAC_Stop_handler;
	memcpy(buttons[DAC_STOP].title, "STOP!\0", 6);


	buttons[BUTTON_CONNECT].x_pos = CONNECT_BUTTON_X;
	buttons[BUTTON_CONNECT].y_pos = CONNECT_BUTTON_Y;
	buttons[BUTTON_CONNECT].isPushed = 0;
	buttons[BUTTON_CONNECT].hasBeenAcknowledged = 0;
	buttons[BUTTON_CONNECT].buttonPressedHandler = buttonConnectPressed;
	buttons[BUTTON_CONNECT].isEnabled = 1;
	memcpy(buttons[BUTTON_CONNECT].title, "Conn.\0", 6);

	pboards_c = 0;
	for (int i = 0; i < NUMBER_OF_BOARDS; i++){
		pboards[i].board.module = NONE;

		pboards[i].isEnabled = 0;
		pboards[i].isPressed = 0;
		pboards[i].order = (uint8_t) i;
	}

	pboards[0].board.module = EEPROM;
	pboards[0].draw_initial_screen = draw_EEPROM;
	pboards[0].x = 25;
	pboards[0].y = 90;

	pboards[1].board.module = TEMP;
	pboards[1].draw_initial_screen = draw_ACCEL;
	pboards[1].x = 25;
	pboards[1].y = 180;

	pboards[2].board.module = ADC_O;
	pboards[2].draw_initial_screen = draw_ADC;
	pboards[2].x = 240;
	pboards[2].y = 90;

	pboards[3].board.module = DAC_O;
	pboards[3].draw_initial_screen = draw_DAC;
	pboards[3].x = 240;
	pboards[3].y = 180;

	BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());

	//setReceivedData();

	aScreen = BOARD_SELECTION;

	print_initial_screen();

	isReady = 0;
	//BSP_TS_ITConfig();
}
