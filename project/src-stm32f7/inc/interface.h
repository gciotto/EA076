/*
 * interface.h
 *
 *  Created on: May 5, 2016
 *      Author: gciotto
 */

#ifndef INTERFACE_H_
#define INTERFACE_H_

#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"
#include "stm32746g_discovery.h"
#include "cmsis_os.h"

#define NUMBER_OF_BUTTONS 	23
#define NUMBER_OF_BOARDS	4

#define STATUS_Y			60

#define BOARD_WIDTH			190
#define BOARD_HEIGTH		70

#define BUTTON_INCREASE_W	35
#define BUTTON_INCREASE_H	25

#define BUTTON_CONNECT_WIDTH	90
#define BUTTON_WIDTH		BOARD_WIDTH
#define BUTTON_HEIGTH		30

/* EEPROM Buttons  */
#define EEPROM_START_X		10
#define EEPROM_START_Y		90

#define EEPROM_STOP_X		10
#define EEPROM_STOP_Y		135

#define EEPROM_STATS_X		10
#define EEPROM_STATS_Y		180

#define EEPROM_STATE_X		10
#define EEPROM_STATE_Y		225

/* ACCEL. Buttons  */
#define ACCEL_START_X		10
#define ACCEL_START_Y		90

#define ACCEL_STOP_X		10
#define ACCEL_STOP_Y		135


/* ADC Buttons */
#define ADC_TEMP_READ_X			25
#define ADC_TEMP_READ_Y			90
#define ADC_POT_READ_X			25
#define ADC_POT_READ_Y			135
#define ADC_PRESS_READ_X		25
#define ADC_PRESS_READ_Y		180
#define ADC_SEND_BLUETOOTH_X	25
#define ADC_SEND_BLUETOOTH_Y	225

/* DAC Buttons */

#define DAC_SIN_X			25
#define DAC_SIN_Y			90
#define DAC_TRI_X			25
#define DAC_TRI_Y			135
#define DAC_QUAD_X			25
#define DAC_QUAD_Y			180
#define DAC_SET_X			25
#define DAC_SET_Y			225
#define DAC_PERIOD_DECREASE_X 390
#define DAC_PERIOD_DECREASE_Y 105
#define DAC_PERIOD_INCREASE_X 430
#define DAC_PERIOD_INCREASE_Y 105
#define DAC_TENSION_DECREASE_X 390
#define DAC_TENSION_DECREASE_Y 170
#define DAC_TENSION_INCREASE_X 430
#define DAC_TENSION_INCREASE_Y 170
#define DAC_STOP_X 380
#define DAC_STOP_Y 225

#define CONNECT_BUTTON_X	340
#define CONNECT_BUTTON_Y	STATUS_Y - 8

enum command {
	CONNECT_CMD = 'C',
	DISCONNECT_CMD,
	EEPROM_START_CMD = 0x10,
	EEPROM_STOP_CMD,
	EEPROM_STATS_CMD,
	EEPROM_STATUS_CMD,

	ADC_TEMP_CMD = 0x50,
	ADC_POT_CMD,
	ADC_PRESS_CMD,
	ADC_BLUE_CMD,
	DAC_SIN_CMD = 0x60,
	DAC_TRI_CMD,
	DAC_QUAD_CMD,
	DAC_SET_CMD,
	DAC_STOP_CMD,
	DAC_PERIOD_INCREASE_CMD,
	DAC_PERIOD_DECREASE_CMD,
	DAC_TENSION_INCREASE_CMD,
	DAC_TENSION_DECREASE_CMD,
	ACCEL_START_CMD = 0x80,
	ACCEL_STOP_CMD
};

enum connection_state {
	DISCONNECTED,
	CONNECTED,
	CONNECTING
};

enum button_index {
	EEPROM_START = 0,
	EEPROM_STOP,
	EEPROM_STATS,
	EEPROM_STATE,
	ACCEL_START,
	ACCEL_STOP,
	ADC_TEMP_READ,
	ADC_POT_READ,
	ADC_PRESS_READ,
	ADC_SEND_BLUETOOTH,
	DAC_SIN,
	DAC_TRI,
	DAC_QUAD,
	DAC_SET,
	DAC_STOP,
	DAC_PERIOD_INCREASE,
	DAC_PERIOD_DECREASE,
	DAC_TENSION_INCREASE,
	DAC_TENSION_DECREASE,
	BUTTON_CONNECT
};


enum screen_s {
	BOARD_SELECTION,
	EEPROM_SELECTED,
	ACCEL_SELECTED,
	DAC_SELECTED,
	ADC_SELECTED
};

enum EEPROM_status {
	STARTED,
	STOPPED
};

union EEPROM_data {
	uint8_t data_as_bytes[4];
	float data_as_float;
};

union DAC_data {
	uint8_t data_as_bytes[4];
	uint32_t data_as_uint32;
};

enum screen_s aScreen;

typedef void (*button_handler_t) (void *source);

typedef struct {

	uint16_t x_pos, y_pos;
	uint8_t isPushed, hasBeenAcknowledged, isEnabled;
	char title[12];
	button_handler_t buttonPressedHandler;

} button_t;

typedef struct {

	uint8_t type;
	char name[12];

	uint32_t background_color, pushed_background_color;

} module_t;

typedef struct {

	uint8_t id;

	module_t module;

} board_t;

typedef void (*board_refresh_screen_t) (void* board);

typedef struct {

	board_t board;
	uint16_t x, y;

	uint8_t isEnabled, isPressed, order;

	board_refresh_screen_t draw_initial_screen;

} pboard_t;


LTDC_HandleTypeDef hltdc;
UART_HandleTypeDef huart6;

button_t buttons[NUMBER_OF_BUTTONS], button_home;

uint8_t pboards_c;
pboard_t pboards [NUMBER_OF_BOARDS];

uint8_t isConnected, isReady, isReadyToSend, hasReceivedByte, dac_isDrawing, isAccelerometerWorking, counterAccelerometerWorking;
uint16_t dac_period;
float dac_set_new_value, adc_temp, adc_pot, adc_press;

pboard_t *selected_board;

SemaphoreHandle_t mutex_handler, mutex_drawer;

void refresh_EEPROM(void* board);
void draw_EEPROM(void* board);
void refreshBoardState (TS_StateTypeDef ts_event,  pboard_t *board);
uint8_t isTouchedBoard(TS_StateTypeDef ts_event,  pboard_t *board);
void init_interface(void);
void setTitle(char *title);
void placeButton(button_t button, uint32_t color_back, uint32_t color_text, uint16_t width, uint16_t heigth);
void paintPushedButton (button_t button, uint16_t width, uint16_t heigth);
void unpaintPushedButton (button_t button, uint16_t width, uint16_t heigth);
uint8_t isTouched(TS_StateTypeDef ts_event,  button_t *button, uint16_t width, uint16_t heigth);
void refreshButtonState (TS_StateTypeDef ts_event,  button_t *button, uint16_t width, uint16_t heigth);
void refreshConnectState(TS_StateTypeDef ts_event);
void buttonConnectPressed();
void setStatus(enum connection_state isConnected);
void executeCommand(enum command command);
void print_new_cycle();
void print_new_ramp();
void setSubTitle(char *title);
void setACCEL_Z(union EEPROM_data z);
void setACCEL_Y(union EEPROM_data y);
void setACCEL_X(union EEPROM_data x);

#endif /* INTERFACE_H_ */
