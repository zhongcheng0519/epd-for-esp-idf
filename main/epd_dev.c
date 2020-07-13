#include "epd_dev.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

void gpio_init()
{
    gpio_pad_select_gpio(EPD_BUSY);
    gpio_pad_select_gpio(EPD_RES);
    gpio_pad_select_gpio(EPD_DC);
    gpio_pad_select_gpio(EPD_CS);
    gpio_pad_select_gpio(EPD_SCK);
    gpio_pad_select_gpio(EPD_SDI);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(EPD_BUSY,GPIO_MODE_INPUT);
    gpio_set_direction(EPD_RES, GPIO_MODE_OUTPUT);
    gpio_set_direction(EPD_DC,  GPIO_MODE_OUTPUT);
    gpio_set_direction(EPD_CS,  GPIO_MODE_OUTPUT);
    gpio_set_direction(EPD_SCK, GPIO_MODE_OUTPUT);
    gpio_set_direction(EPD_SDI, GPIO_MODE_OUTPUT);
}

void driver_delay_xms(unsigned long xms)
{
    printf("driver_delay_xms for %ld ms\n", xms);
    vTaskDelay(xms / portTICK_PERIOD_MS);
}

void Epaper_Spi_WriteByte(unsigned char TxData)
{
    unsigned char TempData;
    unsigned char scnt;
    TempData = TxData;

    EPD_W21_CLK_0;
    for (scnt = 0; scnt < 8; scnt++)
    {
        if (TempData & 0x80)
            EPD_W21_MOSI_1;
        else
            EPD_W21_MOSI_0;

        EPD_W21_CLK_1;
        EPD_W21_CLK_0;
        TempData <<= 1;
    }
}

void Epaper_READBUSY(void)
{
    while (1)
    { //=1 BUSY
        if (isEPD_W21_BUSY == 0)
            break;
    }
}

void Epaper_Write_Command(unsigned char cmd)
{
    EPD_W21_CS_1;
    EPD_W21_CS_0;
    EPD_W21_DC_0; // D/C#   0:command  1:data

    Epaper_Spi_WriteByte(cmd);
    EPD_W21_CS_1;
}

void Epaper_Write_Data(unsigned char data)
{
    EPD_W21_CS_1;
    EPD_W21_CS_0;
    EPD_W21_DC_1; // D/C#   0:command  1:data

    Epaper_Spi_WriteByte(data);
    EPD_W21_CS_1;
}

/////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

void EPD_HW_Init(void)
{
    printf("EPD_HW_Init\n");
    EPD_W21_RST_0;
    driver_delay_xms(100);
    EPD_W21_RST_1; //hard reset
    driver_delay_xms(100);

    Epaper_READBUSY();
    Epaper_Write_Command(0x12); //SWRESET
    Epaper_READBUSY();

    Epaper_Write_Command(0x01); //Driver output control
    Epaper_Write_Data(0xC7);
    Epaper_Write_Data(0x00);
    Epaper_Write_Data(0x00);

    Epaper_Write_Command(0x11); //data entry mode
    Epaper_Write_Data(0x01);

    Epaper_Write_Command(0x44); //set Ram-X address start/end position
    Epaper_Write_Data(0x00);
    Epaper_Write_Data(0x18); //0x18-->(24+1)*8=200

    Epaper_Write_Command(0x45); //set Ram-Y address start/end position
    Epaper_Write_Data(0xC7);    //0xC7-->(199+1)=200
    Epaper_Write_Data(0x00);
    Epaper_Write_Data(0x00);
    Epaper_Write_Data(0x00);

    Epaper_Write_Command(0x18);
    Epaper_Write_Data(0x80);

    Epaper_Write_Command(0x4E); // set RAM x address count to 0;
    Epaper_Write_Data(0x00);
    Epaper_Write_Command(0x4F); // set RAM y address count to 0X199;
    Epaper_Write_Data(0xC7);
    Epaper_Write_Data(0x00);
    Epaper_READBUSY();
}

//////////////////////////////All screen update////////////////////////////////////////////
void EPD_ALL_image(const unsigned char *datas1, const unsigned char *datas2)
{
    unsigned int i;
    printf("EPD_ALL_image\n");
    Epaper_Write_Command(0x24); //write RAM for black(0)/white (1)
    for (i = 0; i < ALLSCREEN_GRAGHBYTES; i++)
    {
        Epaper_Write_Data(*datas1);
        datas1++;
    }
    Epaper_Write_Command(0x26); //write RAM for red(1)/white (0)
    for (i = 0; i < ALLSCREEN_GRAGHBYTES; i++)
    {
        Epaper_Write_Data(~*datas2);
        datas2++;
    }
    EPD_Update();
}
/////////////////////////////////////////////////////////////////////////////////////////
void EPD_Update(void)
{
    Epaper_Write_Command(0x22);
    Epaper_Write_Data(0xF7);
    Epaper_Write_Command(0x20);
    Epaper_READBUSY();
}

void EPD_DeepSleep(void)
{
    printf("EPD_DeepSleep\n");
    Epaper_Write_Command(0x10); //enter deep sleep
    Epaper_Write_Data(0x01);
    driver_delay_xms(100);
}

/////////////////////////////////Single display////////////////////////////////////////////////
void EPD_WhiteScreen_Red(void)

{
    unsigned int k;
    Epaper_Write_Command(0x24); //write RAM for black(0)/white (1)
    for (k = 0; k < ALLSCREEN_GRAGHBYTES; k++)
    {
        Epaper_Write_Data(0xff);
    }

    Epaper_Write_Command(0x26); //write RAM for red(1)/white (0)
    for (k = 0; k < ALLSCREEN_GRAGHBYTES; k++)
    {
        Epaper_Write_Data(0xff);
    }

    EPD_Update();
}

void EPD_WhiteScreen_Black(void)
{
    unsigned int k;
    Epaper_Write_Command(0x24); //write RAM for black(0)/white (1)
    for (k = 0; k < ALLSCREEN_GRAGHBYTES; k++)
    {
        Epaper_Write_Data(0x00);
    }

    Epaper_Write_Command(0x26); //write RAM for red(1)/white (0)
    for (k = 0; k < ALLSCREEN_GRAGHBYTES; k++)
    {
        Epaper_Write_Data(0x00);
    }

    EPD_Update();
}

void EPD_WhiteScreen_White(void)

{
    unsigned int k;
    Epaper_Write_Command(0x24); //write RAM for black(0)/white (1)
    for (k = 0; k < ALLSCREEN_GRAGHBYTES; k++)
    {
        Epaper_Write_Data(0xff);
    }

    Epaper_Write_Command(0x26); //write RAM for red(1)/white (0)
    for (k = 0; k < ALLSCREEN_GRAGHBYTES; k++)
    {
        Epaper_Write_Data(0x00);
    }

    EPD_Update();
}

///////////////////////////Part update//////////////////////////////////////////////
void EPD_Dis_Part(unsigned int x_start, unsigned int y_start, const unsigned char *datas, const unsigned char color_mode, unsigned int PART_COLUMN, unsigned int PART_LINE)
{
    unsigned int i;
    unsigned int x_end, y_start1, y_start2, y_end1, y_end2;

    x_start = (x_start + 8) / 8;
    x_end = x_start + PART_LINE / 8 - 1;

    y_start1 = 0;
    y_start2 = y_start;
    if (y_start >= 256)
    {
        y_start1 = y_start2 / 256;
        y_start2 = y_start2 % 256;
    }
    y_end1 = 0;
    y_end2 = y_start + PART_COLUMN - 1;
    if (y_end2 >= 256)
    {
        y_end1 = y_end2 / 256;
        y_end2 = y_end2 % 256;
    }

    Epaper_Write_Command(0x44);  // set RAM x address start/end, in page 35
    Epaper_Write_Data(x_start);  // RAM x address start
    Epaper_Write_Data(x_end);    // RAM x address end
    Epaper_Write_Command(0x45);  // set RAM y address start/end, in page 35
    Epaper_Write_Data(y_start2); // RAM y address start
    Epaper_Write_Data(y_start1); // RAM y address start
    Epaper_Write_Data(y_end2);   // RAM y address end
    Epaper_Write_Data(y_end1);   // RAM y address end

    Epaper_Write_Command(0x4E); // set RAM x address count to 0;
    Epaper_Write_Data(x_start);
    Epaper_Write_Command(0x4F); // set RAM y address count to 0X127;
    Epaper_Write_Data(y_start2);
    Epaper_Write_Data(y_start1);

    if (color_mode == MONO)
    {
        Epaper_Write_Command(0x24); //write RAM for black(0)/white (1)
        for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++)
        {
            Epaper_Write_Data(*datas);
            datas++;
        }

        EPD_Update();
    }

    if (color_mode == RED)
    {
        Epaper_Write_Command(0x26); //write RAM for black(0)/white (1)
        for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++)
        {
            Epaper_Write_Data(~*datas);
            datas++;
        }

        EPD_Update();
    }
}

void EPD_Dis_Part_mult(unsigned int x_startA, unsigned int y_startA, const unsigned char *datasA1, const unsigned char *datasA2,
                       unsigned int x_startB, unsigned int y_startB, const unsigned char *datasB1, const unsigned char *datasB2,
                       unsigned int PART_COLUMN, unsigned int PART_LINE)
{
    unsigned int i;
    unsigned int x_endA, y_startA1, y_startA2, y_endA1, y_endA2;
    unsigned int x_endB, y_startB1, y_startB2, y_endB1, y_endB2;

    //Data A////////////////////////////
    x_startA = (x_startA + 8) / 8;
    x_endA = x_startA + PART_LINE / 8 - 1;

    y_startA1 = 0;
    y_startA2 = y_startA;
    if (y_startA2 >= 256)
    {
        y_startA1 = y_startA2 / 256;
        y_startA2 = y_startA2 % 256;
    }
    y_endA1 = 0;
    y_endA2 = y_startA + PART_COLUMN - 1;
    if (y_endA2 >= 256)
    {
        y_endA1 = y_endA2 / 256;
        y_endA2 = y_endA2 % 256;
    }

    Epaper_Write_Command(0x44);   // set RAM x address start/end, in page 35
    Epaper_Write_Data(x_startA);  // RAM x address start at 00h;
    Epaper_Write_Data(x_endA);    // RAM x address end at 0fh(15+1)*8->128
    Epaper_Write_Command(0x45);   // set RAM y address start/end, in page 35
    Epaper_Write_Data(y_startA2); // RAM y address start at 0127h;
    Epaper_Write_Data(y_startA1); // RAM y address start at 0127h;
    Epaper_Write_Data(y_endA2);   // RAM y address end at 00h;
    Epaper_Write_Data(y_endA1);

    Epaper_Write_Command(0x4E); // set RAM x address count to 0;
    Epaper_Write_Data(x_startA);
    Epaper_Write_Command(0x4F); // set RAM y address count to 0X127;
    Epaper_Write_Data(y_startA2);
    Epaper_Write_Data(y_startA1);

    Epaper_Write_Command(0x24); //Write Black and White image to RAM
    for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++)
    {
        Epaper_Write_Data(*datasA1);
        datasA1++;
    }

    Epaper_Write_Command(0x26); //Write Black and White image to RAM
    for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++)
    {
        Epaper_Write_Data(~*datasA2);
        datasA2++;
    }

    //Data B/////////////////////////////////////
    x_startB = (x_startB + 8) / 8;      // change to bytes
    x_endB = x_startB + PART_LINE / 8 - 1;

    y_startB1 = 0;
    y_startB2 = y_startB;
    if (y_startB2 >= 256)
    {
        y_startB1 = y_startB2 / 256;
        y_startB2 = y_startB2 % 256;
    }
    y_endB1 = 0;
    y_endB2 = y_startB + PART_COLUMN - 1;
    if (y_endB2 >= 256)
    {
        y_endB1 = y_endB2 / 256;
        y_endB2 = y_endB2 % 256;
    }

    Epaper_Write_Command(0x44);   // set RAM x address start/end, in page 35
    Epaper_Write_Data(x_startB);  // RAM x address start at 00h;
    Epaper_Write_Data(x_endB);    // RAM x address end at 0fh(15+1)*8->128
    Epaper_Write_Command(0x45);   // set RAM y address start/end, in page 35
    Epaper_Write_Data(y_startB2); // RAM y address start at 0127h;
    Epaper_Write_Data(y_startB1); // RAM y address start at 0127h;
    Epaper_Write_Data(y_endB2);   // RAM y address end at 00h;
    Epaper_Write_Data(y_endB1);

    Epaper_Write_Command(0x4E); // set RAM x address count to 0;
    Epaper_Write_Data(x_startB);
    Epaper_Write_Command(0x4F); // set RAM y address count to 0X127;
    Epaper_Write_Data(y_startB2);
    Epaper_Write_Data(y_startB1);

    Epaper_Write_Command(0x24); //Write Black and White image to RAM
    for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++)
    {
        Epaper_Write_Data(*datasB1);
        datasB1++;
    }

    Epaper_Write_Command(0x26); //Write Black and White image to RAM
    for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++)
    {
        Epaper_Write_Data(~*datasB2);
        datasB2++;
    }

    EPD_Update();
}
//////////////////////////////////////////////////////////////////////////////////////
