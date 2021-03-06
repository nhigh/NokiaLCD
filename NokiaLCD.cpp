#include "Arduino.h"
#include "NokiaLCD.h"
#include "Commands.h"
#include "Color.h"

NokiaLCD::NokiaLCD(short CS, short SCK, short DIO, short RST) {
    _CS = CS;
    _SCK = SCK;
    _DIO = DIO;
    _RST = RST;
    
    _COMMAND = 0;
    _DATA = 1;

    pinMode(_CS, OUTPUT);
    pinMode(_SCK, OUTPUT);
    pinMode(_DIO, OUTPUT);
    pinMode(_RST, OUTPUT);
}

void NokiaLCD::setupLCD() {
    hardwareReset(); //Reset the LCD.
    
    sendSPI(SLEEPOUT, _COMMAND); //Exit sleep mode.
    
    sendSPI(INVON, _COMMAND); //Invert the display.
    
    sendSPI(COLMOD, _COMMAND); //Set the color mode to 12 bits per pixel. (4096 Color mode)
    sendSPI(0x03, _DATA);
    
    sendSPI(MADCTL, _COMMAND);
    sendSPI(0x60, 1);
    
    sendSPI(SETCON, _COMMAND); //Set the contrast to 0x30.
    sendSPI(0x30, _DATA);
    
    delay(20);
    
    sendSPI(DISPON, _COMMAND); //Turn on the display.
    
    delay(20);
}

void NokiaLCD::hardwareReset() {
    digitalWrite(_RST, LOW);
    delay(20);
    digitalWrite(_RST, HIGH);
    delay(20);
}

void NokiaLCD::setPixel(int x, int y, Color c) {
    sendSPI(CASET, _COMMAND);
    sendSPI(x, _DATA);
    sendSPI(x, _DATA);
    
    sendSPI(PASET, _COMMAND);
    sendSPI(y, _DATA);
    sendSPI(y, _DATA);
    
    sendSPI(RAMWR, _COMMAND);
    
    unsigned char b1 = ((c.getR() & 0xF) << 4) + (c.getG() & 0xF);
    unsigned char b2 = c.getB() << 4;
    
    sendSPI(b1, _DATA);
    sendSPI(b2, _DATA);
    sendSPI(NOP, _COMMAND);
}

void NokiaLCD::setRect(int x, int y, int w, int h, Color c) {
    sendSPI(CASET, _COMMAND);
    sendSPI(x, _DATA);
    sendSPI(x+w, _DATA);
    
    sendSPI(PASET, _COMMAND);
    sendSPI(y, _DATA);
    sendSPI(y+h, _DATA);
    
    sendSPI(RAMWR, _COMMAND);
    
    unsigned char b1 = ((c.getR() & 0xF) << 4) + (c.getG() & 0xF);
    unsigned char b2 = ((c.getB() & 0xF) << 4) + (c.getR() & 0xF);
    unsigned char b3 = ((c.getG() & 0xF) << 4) + (c.getB() & 0xF);
      
    for (int i = 0; i < (((w+1)*(h+1))/2)+1; i++) {
        sendSPI(b1, _DATA);
        sendSPI(b2, _DATA);
        sendSPI(b3, _DATA);
    }
}

void NokiaLCD::setRectOutline(int x, int y, int w, int h, int os, Color c) {
    unsigned char b1 = ((c.getR() & 0xF) << 4) + (c.getG() & 0xF);
    unsigned char b2 = ((c.getB() & 0xF) << 4) + (c.getR() & 0xF);
    unsigned char b3 = ((c.getG() & 0xF) << 4) + (c.getB() & 0xF);

    //Left side
    sendSPI(CASET, _COMMAND);
    sendSPI(x, _DATA);
    sendSPI(x+os-1, _DATA);
    
    sendSPI(PASET, _COMMAND);
    sendSPI(y, _DATA);
    sendSPI(y+h, _DATA);

    sendSPI(RAMWR, _COMMAND);

    for (int i = 0; i < (((os+1)*(h+1))/2)+1; i++) {
        sendSPI(b1, _DATA);
        sendSPI(b2, _DATA);
        sendSPI(b3, _DATA);
    }

    //Right side
    sendSPI(CASET, _COMMAND);
    sendSPI((x+w)-os+1, _DATA);
    sendSPI(x+w, _DATA);

    sendSPI(PASET, _COMMAND);
    sendSPI(y, _DATA);
    sendSPI(y+h, _DATA);

    sendSPI(RAMWR, _COMMAND);
    
    for (int i = 0; i < (((os+1)*(h+1))/2)+1; i++) {
        sendSPI(b1, _DATA);
        sendSPI(b2, _DATA);
        sendSPI(b3, _DATA);
    }

    //Top
    sendSPI(CASET, _COMMAND);
    sendSPI(x, _DATA);
    sendSPI(x+w, _DATA);

    sendSPI(PASET, _COMMAND);
    sendSPI(y, _DATA);
    sendSPI(y+os-1, _DATA);

    sendSPI(RAMWR, _COMMAND);

    for (int i = 0; i < (((os+1)*(h+1))/2)+1; i++) {
        sendSPI(b1, _DATA);
        sendSPI(b2, _DATA);
        sendSPI(b3, _DATA);
    }

    //Bottom
    sendSPI(CASET, _COMMAND);
    sendSPI(x, _DATA);
    sendSPI(x+w, _DATA);
    
    sendSPI(PASET, _COMMAND);
    sendSPI((y+h)-os+1, _DATA);
    sendSPI(y+h, _DATA);

    sendSPI(RAMWR, _COMMAND);

    for (int i = 0; i < (((os+1)*(h+1))/2)+1; i++) {
        sendSPI(b1, _DATA);
        sendSPI(b2, _DATA);
        sendSPI(b3, _DATA);
    }
}

void NokiaLCD::clearScreen(Color c) {
    sendSPI(CASET, _COMMAND);
    sendSPI(0, _DATA);
    sendSPI(131, _DATA);
    
    sendSPI(PASET, _COMMAND);
    sendSPI(0, _DATA);
    sendSPI(131, _DATA);
    
    sendSPI(RAMWR, _COMMAND);
    
    unsigned char b1 = ((c.getR() & 0xF) << 4) + (c.getG() & 0xF);
    unsigned char b2 = ((c.getB() & 0xF) << 4) + (c.getR() & 0xF);
    unsigned char b3 = ((c.getG() & 0xF) << 4) + (c.getB() & 0xF);
    
    for (int i = 0; i < (132*(132/2))+133; i++) {
        sendSPI(b1, _DATA);
        sendSPI(b2, _DATA);
        sendSPI(b3, _DATA);
    }
}

void NokiaLCD::sendSPI(unsigned char b, boolean isCommand) {
    digitalWrite(_CS, LOW);
    
    //Send 9 bits starting with data bit, then moving to MSB;
    digitalWrite(_SCK, LOW);
    digitalWrite(_DIO, isCommand);
    digitalWrite(_SCK, HIGH);

    for (int i = 0; i < 8; i++) {
        boolean nextBit = (b >> (7-i)) & 1;
        digitalWrite(_SCK, LOW);
        digitalWrite(_DIO, nextBit);
        digitalWrite(_SCK, HIGH);
    }
    digitalWrite(_CS, HIGH);
}
