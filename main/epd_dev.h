#ifndef _EPD_DEV_H_
#define _EPD_DEV_H_

#include "sdkconfig.h"
#include "driver/gpio.h"

/* Can use project configuration menu (idf.py menuconfig) to choose the GPIO to e-paper,
   or you can edit the following line and set a number here.
*/
#define EPD_BUSY   CONFIG_EPD_BUSY
#define EPD_RES    CONFIG_EPD_RES
#define EPD_DC     CONFIG_EPD_DC
#define EPD_CS     CONFIG_EPD_CS
#define EPD_SCK    CONFIG_EPD_SCK
#define EPD_SDI    CONFIG_EPD_SDI


// 200*200 resolution

#define MAX_LINE_BYTES 25     // =200/8
#define MAX_COLUMN_BYTES 200

#define ALLSCREEN_GRAGHBYTES   5000   // =200/8*200

#define MONO 1           // black and white
#define RED  2           // black, white and red

#define EPD_W21_MOSI_0	gpio_set_level(EPD_SDI, 0)
#define EPD_W21_MOSI_1	gpio_set_level(EPD_SDI, 1)

#define EPD_W21_CLK_0	gpio_set_level(EPD_SCK, 0)
#define EPD_W21_CLK_1	gpio_set_level(EPD_SCK, 1)

#define EPD_W21_CS_0	gpio_set_level(EPD_CS, 0)
#define EPD_W21_CS_1	gpio_set_level(EPD_CS, 1)

#define EPD_W21_DC_0	gpio_set_level(EPD_DC, 0)
#define EPD_W21_DC_1	gpio_set_level(EPD_DC, 1)

#define EPD_W21_RST_0	gpio_set_level(EPD_RES, 0)
#define EPD_W21_RST_1	gpio_set_level(EPD_RES, 1)

#define EPD_W21_BUSY_LEVEL 0
#define isEPD_W21_BUSY gpio_get_level(EPD_BUSY)

void gpio_init();

void driver_delay_xms(unsigned long xms);			
void Epaper_READBUSY(void);
void Epaper_Spi_WriteByte(unsigned char TxData);
void Epaper_Write_Command(unsigned char cmd);
void Epaper_Write_Data(unsigned char data);

void EPD_HW_Init(void); //Electronic paper initialization
void EPD_Update(void);

void EPD_WhiteScreen_Red(void);
void EPD_WhiteScreen_Black(void);
void EPD_WhiteScreen_White(void);
void EPD_DeepSleep(void);
//Display 

void EPD_ALL_image(const unsigned char *datas1,const unsigned char *datas2);
void EPD_Dis_Part(unsigned int x_start,unsigned int y_start,const unsigned char * datas,const unsigned char color_mode,unsigned int PART_COLUMN,unsigned int PART_LINE);
void EPD_Dis_Part_mult(unsigned int x_startA,unsigned int y_startA,const unsigned char * datasA1,const unsigned char * datasA2,
                         unsigned int x_startB,unsigned int y_startB,const unsigned char * datasB1,const unsigned char * datasB2,
                         unsigned int PART_COLUMN,unsigned int PART_LINE);


#endif