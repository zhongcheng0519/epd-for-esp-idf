/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "epd_dev.h"
#include "picture.h"

void app_main(void)
{
    /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
       muxed to GPIO on reset already, but some default to other
       functions and need to be switched to GPIO. Consult the
       Technical Reference for a list of pads and their default
       functions.)
    */
    uint8_t m;
    uint8_t k;
    gpio_init();
    printf("gpio init OK.\n");
    while (1)
    {
        // Full screen refresh
        printf("\nFirst Image\n");
        EPD_HW_Init();      // Electronic paper initialization
        EPD_ALL_image(gImage_1_BW, gImage_1_RED);       // Refresh the picture in full screen
        EPD_DeepSleep();    // Enter deep sleep
        driver_delay_xms(3000);

        printf("\nSecond Image\n");
        EPD_HW_Init();      // Electronic paper initialization
        EPD_ALL_image(gImage_2_BW, gImage_2_RED);       // Refresh the picture in full screen
        EPD_DeepSleep();    // Enter deep sleep
        driver_delay_xms(3000);

        printf("\nThird Image\n");
        EPD_HW_Init();      // Electronic paper initialization
        EPD_ALL_image(gImage_base_BW, gImage_base_RED);
        EPD_DeepSleep();    // Enter deep sleep
        driver_delay_xms(1000);

        // paritial refresh presentation
        printf("\nPartial refresh\n");
        for (k = 0; k < 2; k++)
        {
            m = k*2;
            printf("Show %d\n", m);
            EPD_HW_Init();
            EPD_Dis_Part(72, 156, Num[m], MONO, 56, 56);
            EPD_DeepSleep();
            driver_delay_xms(1000);

            printf("Show %d\n", m+1);
            EPD_HW_Init();
            EPD_Dis_Part(72, 100, Num[m+1], RED, 56, 56);
            EPD_DeepSleep();
            driver_delay_xms(1000);
        }

        printf("\nShow all right\n");
        EPD_HW_Init();          // Electronic paper initialization
        EPD_WhiteScreen_White();    // Show all white
        EPD_DeepSleep();    // Enter deep sleep
        while(1)
        {
            printf("Do nothing!\n");
            driver_delay_xms(1000);
        }
            
    }
}
