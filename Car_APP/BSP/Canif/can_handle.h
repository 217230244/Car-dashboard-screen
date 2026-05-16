#ifndef __CAN_HANDLE_H__
#define __CAN_HANDLE_H__
#include "main.h"

typedef struct
{
		uint32_t ui_value;
		uint8_t ui_freshflag;
} ui_Typedef;

#define CAN_ID_UI    0x0B4
#define CAN_MSG_HEAD 0xAF
#define CAN_ID_CALL_OTA 0x0B3

typedef enum
{
		E_UI_TYPE_RPM = 0x1,
		E_UI_TYPE_WATER_TEMP,	
		E_UI_TYPE_SAFTY_BELT,	
		E_UI_TYPE_LIGHT,		
		E_UI_TYPE_TURN_LIGHT,	
		E_UI_TYPE_TURN_DATE,	
		E_UI_TYPE_TURN_TIME,		
} E_Ui_Type;

uint8_t get_ota_flag(void);
uint8_t ui_get_rpm_value(uint32_t *rpm_val);
uint8_t ui_get_water_temp_value(uint32_t *water_temp_val);
uint8_t ui_get_safty_belt_value(uint32_t *safty_belt_val);
uint8_t ui_get_light_value(uint32_t *light_val);
uint8_t ui_get_turn_light_value(uint32_t *turn_light_val);
uint8_t ui_get_date_value(uint32_t *date_val);
uint8_t ui_get_time_value(uint32_t *time_val);

void Can_Handle(void);

#endif

