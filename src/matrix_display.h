#ifndef MATRIX_DISPLAY_H
#define MATRIX_DISPLAY_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include "conf_gpio.h"

#include <Adafruit_GFX.h>
#include <Adafruit_DotStarMatrix.h>
#include <Adafruit_DotStar.h>

#define APA102MATRIX_WIDTH 72
#define APA102MATRIX_HEIGHT 6

class MatrixDisplay
{
private:
    Adafruit_DotStarMatrix matrix;

public:
    MatrixDisplay() : matrix(
                          APA102MATRIX_WIDTH, APA102MATRIX_HEIGHT,
                          GPIO_PASTEUR_DATA, GPIO_PASTEUR_CLK,
                          DS_MATRIX_TOP + DS_MATRIX_RIGHT + DS_MATRIX_ROWS + DS_MATRIX_ZIGZAG,
                          DOTSTAR_BRG)
    {

        matrix.begin();
        matrix.setBrightness(10);
        matrix.setTextWrap(false);
        matrix.setTextColor(matrix.Color(0, 0, 255));
    }

    void set_text(const String text)
    {
        matrix.fillScreen(0);
        matrix.setCursor(0, 0);
        matrix.print(text);
        matrix.show();
    }
};

#endif