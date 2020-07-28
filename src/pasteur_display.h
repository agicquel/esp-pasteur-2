#ifndef PASTEUR_DISPLAY_H
#define PASTEUR_DISPLAY_H

#include <SPI.h>
#define BUFFER_SIZE 256

class PasteurDisplay
{
private:
    char buffer[BUFFER_SIZE];

public:
    PasteurDisplay()
    {
        SPI.begin();
        SPI.setClockDivider(SPI_CLOCK_DIV128);
    }

    void set_text(const String text)
    {
        text.toCharArray(buffer, BUFFER_SIZE);
        for (char c : text)
        {
            SPI.transfer(c);
            delay(50);
        }
        SPI.transfer('\r');
    }
};

#endif