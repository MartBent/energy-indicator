/**
 *  @filename   :   epd2in9b.h
 *  @brief      :   Header file for Dual-color e-paper library epd2in9b.cpp
 *  @author     :   Yehui from Waveshare
 *  
 *  Copyright (C) Waveshare     July 31 2017
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef EPD2IN9B_H
#define EPD2IN9B_H

#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "freertos/task.h"

// Display resolution
#define EPD_WIDTH       128
#define EPD_HEIGHT      296

#define EPD_FREQ 2000000
#define EPAPER_QUE_SIZE_DEFAULT 10

#define BUSY_PIN        26 // D1
#define RST_PIN         27 // D2
#define DC_PIN          25 // D3
#define CS_PIN          15 // D4
#define MOSI_PIN 23
#define SCK_PIN 18

// EPD2IN9B commands
#define PANEL_SETTING                               0x00
#define POWER_SETTING                               0x01
#define POWER_OFF                                   0x02
#define POWER_OFF_SEQUENCE_SETTING                  0x03
#define POWER_ON                                    0x04
#define POWER_ON_MEASURE                            0x05
#define BOOSTER_SOFT_START                          0x06
#define DEEP_SLEEP                                  0x07
#define DATA_START_TRANSMISSION_1                   0x10
#define DATA_STOP                                   0x11
#define DISPLAY_REFRESH                             0x12
#define DATA_START_TRANSMISSION_2                   0x13
#define PLL_CONTROL                                 0x30
#define TEMPERATURE_SENSOR_COMMAND                  0x40
#define TEMPERATURE_SENSOR_CALIBRATION              0x41
#define TEMPERATURE_SENSOR_WRITE                    0x42
#define TEMPERATURE_SENSOR_READ                     0x43
#define VCOM_AND_DATA_INTERVAL_SETTING              0x50
#define LOW_POWER_DETECTION                         0x51
#define TCON_SETTING                                0x60
#define TCON_RESOLUTION                             0x61
#define GET_STATUS                                  0x71
#define AUTO_MEASURE_VCOM                           0x80
#define VCOM_VALUE                                  0x81
#define VCM_DC_SETTING_REGISTER                     0x82
#define PARTIAL_WINDOW                              0x90
#define PARTIAL_IN                                  0x91
#define PARTIAL_OUT                                 0x92
#define PROGRAM_MODE                                0xA0
#define ACTIVE_PROGRAM                              0xA1
#define READ_OTP_DATA                               0xA2
#define POWER_SAVING                                0xE3

static spi_device_handle_t epd_spi_handle = NULL;

int  IfInit();
void DigitalWrite(uint8_t pin, uint32_t value); 
uint32_t DigitalRead(uint8_t pin);
void DelayMs(unsigned int delaytime);
void SpiTransfer(unsigned char data);

int Init();
void SendCommand(unsigned char command);
void SendData(unsigned char data);
void WaitUntilIdle(void);
void Reset(void);
void SetPartialWindow(const unsigned char* buffer_black, const unsigned char* buffer_red, int x, int y, int w, int l);
void SetPartialWindowBlack(const unsigned char* buffer_black, int x, int y, int w, int l);
void SetPartialWindowRed(const unsigned char* buffer_red, int x, int y, int w, int l);
void DisplayFrame(const unsigned char* frame_buffer_black, const unsigned char* frame_buffer_red);
void DisplayFrameRam(void);
void ClearFrame(void);
void Sleep(void);  

#endif /* EPD2IN9B_H */

/* END OF FILE */
