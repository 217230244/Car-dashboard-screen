#include "main.h"
#include "canif.h"
#include "can.h"
#include "can_handle.h"
#include "usart.h"
#include <stdio.h>

ui_Typedef ui_rpm;
ui_Typedef ui_water_temp;
ui_Typedef ui_safty_belt;
ui_Typedef ui_light;
ui_Typedef ui_turn_light;
ui_Typedef ui_date;
ui_Typedef ui_time;

static uint8_t tp_ota_flag = 0;
 
uint8_t get_ota_flag(void)
{
    return tp_ota_flag;
}

uint8_t ui_get_rpm_value(uint32_t *rpm_val) //指针
{
		if(ui_rpm.ui_freshflag == 1)
		{
				ui_rpm.ui_freshflag = 0;
				*rpm_val = ui_rpm.ui_value;
				return 0;
		}
		else
		{
				return 1;		
		}
}

uint8_t ui_get_water_temp_value(uint32_t *water_temp_val)//水温
{
		if(ui_water_temp.ui_freshflag == 1)
		{
				ui_water_temp.ui_freshflag = 0;
				*water_temp_val = ui_water_temp.ui_value;
				return 0;
		}
		else
		{
				return 1;		
		}
}

uint8_t ui_get_safty_belt_value(uint32_t *safty_belt_val)//安全带
{
		if(ui_safty_belt.ui_freshflag == 1)
		{
				ui_safty_belt.ui_freshflag = 0;
				*safty_belt_val = ui_safty_belt.ui_value;
				return 0;
		}
		else
		{
				return 1;		
		}
}

uint8_t ui_get_light_value(uint32_t *light_val)//灯
{
		if(ui_light.ui_freshflag == 1)
		{
				ui_light.ui_freshflag = 0;
				*light_val = ui_light.ui_value;
				return 0;
		}
		else
		{
				return 1;		
		}
}

uint8_t ui_get_turn_light_value(uint32_t *turn_light_val)//转向灯
{
		if(ui_turn_light.ui_freshflag == 1)
		{
				ui_turn_light.ui_freshflag = 0;
				*turn_light_val = ui_turn_light.ui_value;
				return 0;
		}
		else
		{
				return 1;		
		}
}

uint8_t ui_get_date_value(uint32_t *date_val)//日期
{
		if(ui_date.ui_freshflag == 1)
		{
				ui_date.ui_freshflag = 0;
				*date_val = ui_date.ui_value;
				return 0;
		}
		else
		{
				return 1;		
		}
}

uint8_t ui_get_time_value(uint32_t *time_val)//时间
{
		if(ui_time.ui_freshflag == 1)
		{
				ui_time.ui_freshflag = 0;
				*time_val = ui_time.ui_value;
				return 0;
		}
		else
		{
				return 1;		
		}

}

void Can_Handle(void)
{
		uint32_t recv_len;
		uint32_t recv_id;
		uint8_t rx_data[8] = {0};
	
		recv_len = Can_Recv_Msg(&recv_id, rx_data);
		if(recv_len > 0)
		{
				if(recv_id == CAN_ID_UI)
				{
						if(rx_data[0] == CAN_MSG_HEAD)
						{
								if(rx_data[1] == E_UI_TYPE_RPM)
								{
										ui_rpm.ui_value = rx_data[2];
										ui_rpm.ui_freshflag = 1;
								}
								else if(rx_data[1] == E_UI_TYPE_WATER_TEMP)
								{
										ui_water_temp.ui_value = rx_data[2];
										ui_water_temp.ui_freshflag = 1;
								}	
								else if(rx_data[1] == E_UI_TYPE_SAFTY_BELT)
								{
										ui_safty_belt.ui_value = rx_data[2];
										ui_safty_belt.ui_freshflag = 1;
								}
								else if(rx_data[1] == E_UI_TYPE_LIGHT)
								{
										ui_light.ui_value = rx_data[2];
										ui_light.ui_freshflag = 1;
								}	
								else if(rx_data[1] == E_UI_TYPE_TURN_LIGHT)
								{
										ui_turn_light.ui_value = rx_data[2];
										ui_turn_light.ui_freshflag = 1;
								}	
								else if(rx_data[1] == E_UI_TYPE_TURN_DATE)
								{
										ui_date.ui_value = ((uint32_t)rx_data[2] << 24) | ((uint32_t)rx_data[3] << 16) | ((uint32_t)rx_data[4] << 8) | ((uint32_t)rx_data[5] << 0);
										ui_date.ui_freshflag = 1;
								}	
								else if(rx_data[1] == E_UI_TYPE_TURN_TIME)
								{
										ui_time.ui_value = ((uint32_t)rx_data[2] << 24) | ((uint32_t)rx_data[3] << 16) | ((uint32_t)rx_data[4] << 8) ;
										ui_time.ui_freshflag = 1;
								}									
						}
				
				}else if (recv_id == CAN_ID_CALL_OTA)        {
            if ((rx_data[0] == 0xBE) &&
                (rx_data[1] == 0xAD) &&
                (rx_data[2] == 0xBE) &&
                (rx_data[3] == 0xEF))
            {
                if (rx_data[4] == 0x01)
                {
                    tp_ota_flag = 1;
                    printf("[tp server] Get ota flag\r\n");
                }
            }
        }
		}

}