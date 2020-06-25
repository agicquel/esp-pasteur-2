#ifndef INFO_DISPLAY_H
#define INFO_DISPLAY_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Wire.h>
#include "conf_gpio.h"

#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 32

class InfoDisplay
{
private:
    Adafruit_SSD1306 SSD1306_display;

public:
    InfoDisplay() : SSD1306_display(SSD1306_WIDTH, SSD1306_HEIGHT, &Wire, -1)
    {
        Wire.begin(GPIO_SSD1306_SDA, GPIO_SSD1306_SCL);
        SSD1306_display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
        SSD1306_display.clearDisplay();
        SSD1306_display.display();
    }

    void set_text(const String menu_title, const String line1, const String line2)
    {
        SSD1306_display.clearDisplay();
        SSD1306_display.setTextColor(SSD1306_WHITE);
        SSD1306_display.setTextSize(1);
        SSD1306_display.setFont(&FreeSansBold9pt7b);
        SSD1306_display.setCursor(0, 12);
        SSD1306_display.println(menu_title);
        SSD1306_display.setFont();
        SSD1306_display.drawLine(0, 14, SSD1306_WIDTH, 14, SSD1306_WHITE);
        SSD1306_display.setCursor(0, 16);
        SSD1306_display.println(line1);
        SSD1306_display.setCursor(0, 24);
        SSD1306_display.println(line2);
        SSD1306_display.display();
    }

    int generate_menu(const String title, const String menu[], int menu_size)
    {
        int selected = 0;
        menu_size--;

        while (1)
        {
            if (digitalRead(GPIO_SW3) == HIGH)
            {
                return selected;
            }
            else if (digitalRead(GPIO_SW1) == HIGH)
            {
                selected++;
            }
            else if (digitalRead(GPIO_SW2) == HIGH)
            {
                selected--;
            }

            if (selected < 0)
                selected = 0;
            if (selected > menu_size)
                selected = menu_size;

            if (selected < menu_size)
            {
                set_text(title, "-> " + menu[selected], menu[selected + 1]);
            }
            else
            {
                set_text(title, menu[selected - 1], "-> " + menu[selected]);
            }
            delay(100);
        }
    }
};

#endif