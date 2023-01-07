#!/usr/bin/env python3
# ===================================================================================
# Project:   USB to I2C Bridge - Conway's Game of Life for CH551, CH552, CH554
# Version:   v1.0
# Year:      2022
# Author:    Stefan Wagner
# Github:    https://github.com/wagiminator
# License:   http://creativecommons.org/licenses/by-sa/3.0/
# ===================================================================================
#
# Description:
# ------------
# The Game of Life is a cellular automaton, and was invented by Cambridge 
# mathematician John Conway.
# This game became widely known when it was mentioned in an article published by 
# Scientific American in 1970. It consists of a grid of cells which, based on a 
# few mathematical rules, can live, die or multiply. Depending on the initial 
# conditions, the cells form various patterns throughout the course of the game.
#
# Rules:
# - For a space that is populated:
#   - Each cell with one or no neighbors dies, as if by solitude.
#   - Each cell with four or more neighbors dies, as if by overpopulation.
#   - Each cell with two or three neighbors survives.
# - For a space that is empty or unpopulated:
#   - Each cell with three neighbors becomes populated.
#
# Dependencies:
# -------------
# - pyserial

import sys
import time
from serial import Serial
from serial.tools.list_ports import comports


STEPS = 750     # number of steps to simulate
DELAY = 0       # delay between steps in seconds


# ===================================================================================
# Main Function
# ===================================================================================

def _main():
    try:
        print('Connecting to device ...')
        oled = Bridge()
    except Exception as ex:
        sys.stderr.write('ERROR: ' + str(ex) + '!\n')
        sys.exit(1)

    try:
        print('Starting Conway\'s Game of Life ...')
        for k in range(STEPS):
            oled.senddata(page1)
            time.sleep(DELAY)
            bringtolife()
    except Exception as ex:
        sys.stderr.write('ERROR: ' + str(ex) + '!\n')
        oled.close()
        sys.exit(1)

    print('DONE.')
    oled.close()
    sys.exit(0)


# ===================================================================================
# Bridge Class
# ===================================================================================

class Bridge(Serial):
    def __init__(self):
        super().__init__(baudrate = 57600, timeout = 1, write_timeout = 1)
        self.identify()

    # Identify port of programmer
    def identify(self):
        vid = '16C0'
        pid = '27DD'
        for p in comports():
            if vid and pid in p.hwid:
                self.port = p.device
                try:
                    self.open()
                except:
                    continue
                self.setup()
                return
        raise Exception('Device not found')

    def sendstream(self, stream):
        self.rts = True
        self.write(bytes(stream))
        self.flush()
        self.rts = False
        time.sleep(0.001)

    def sendcommand(self, cmd):
        self.sendstream([OLED_ADDR, OLED_CMD_MODE] + cmd)

    def senddata(self, data):
        self.sendstream([OLED_ADDR, OLED_DAT_MODE] + data)

    def setup(self):
        self.sendcommand(OLED_INIT_CMD)
        

# ===================================================================================
# Conway's Game of Life
# ===================================================================================
        
def getpixel(xpos, ypos):
    if xpos > 127: xpos -= 128
    if ypos >  63: ypos -=  64
    if xpos <   0: xpos += 128
    if ypos <   0: ypos +=  64
    return ((page1[(ypos >> 3) * 128 + xpos] >> (ypos & 7)) & 1)
        
def setpixel(xpos, ypos):
    page2[(ypos >> 3) * 128 + xpos] |= (1 << (ypos & 7))
        
def swappages():
    for i in range(1024):
        page1[i] = page2[i];
        page2[i] = 0;
            
def bringtolife():
    for x in range(128):
        for y in range(64):
            neighbors = getpixel(x-1, y-1) \
                      + getpixel(x  , y-1) \
                      + getpixel(x+1, y-1) \
                      + getpixel(x-1, y  ) \
                      + getpixel(x+1, y  ) \
                      + getpixel(x-1, y+1) \
                      + getpixel(x  , y+1) \
                      + getpixel(x+1, y+1)
            if (getpixel(x,y) == 1) and (neighbors >= 2) and (neighbors <= 3):
                setpixel(x,y)
            if (getpixel(x,y) == 0) and (neighbors == 3):
                setpixel(x,y)
    swappages()
        

# ===================================================================================
# OLED Constants
# ===================================================================================

OLED_ADDR     = 0x78    # OLED write address
OLED_CMD_MODE = 0x00    # set command mode
OLED_DAT_MODE = 0x40    # set data mode

# OLED initialisation sequence
OLED_INIT_CMD = [
  0xA8, 0x3F,           # set multiplex ratio  
  0x8D, 0x14,           # set DC-DC enable  
  0x20, 0x00,           # set horizontal memory addressing mode
  0xC8, 0xA1,           # flip screen
  0xDA, 0x12,           # set com pins
  0xAF                  # display on
]

# ===================================================================================
# Pages
# ===================================================================================

page1 = [
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xC0, 0xC0, 0x80,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xF0, 0xFE, 0xFE,
0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xF0, 0x80, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x80, 0xC0, 0xC0, 0xC0, 0x80, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x20, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08,
0x00, 0x10, 0x00, 0x20, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x0E, 0x1F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x1F, 0x0E, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xF0, 0xEC,
0xCE, 0xE2, 0xF0, 0x78, 0x38, 0x1C, 0x14, 0x14, 0x1C, 0x18,
0x38, 0x70, 0xE0, 0xE0, 0xC0, 0xC0, 0xE0, 0xE0, 0x60, 0x60,
0xE0, 0xE0, 0xE0, 0xC0, 0x83, 0x87, 0xE6, 0xF0, 0x80, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF8,
0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0x3F, 0x0F, 0x07, 0x07, 0x03, 0x03, 0x03, 0x03, 0x03,
0x07, 0x07, 0x0F, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xF8, 0xF0, 0xF0, 0xF0, 0xF0,
0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0xF8, 0xEB, 0xFA, 0xFF, 0xFF, 0xFF,
0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0xF8,
0xF1, 0xFF, 0xFF, 0x71, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x01, 0xC7, 0xFF, 0xFF, 0xFF, 0x3F, 0xF9, 0x80, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x07, 0x07, 0x0F, 0x0F, 0x0F, 0x0F, 0x3F, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC,
0xF0, 0xE0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xE0,
0xF0, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0x3F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x07, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0xF0, 0xBE, 0x7F, 0xFF, 0xFF, 0xFF, 0x63, 0x07, 0x0F, 0x0E,
0x04, 0x00, 0x00, 0x08, 0x1E, 0x3F, 0x7F, 0xFC, 0xFC, 0xF8,
0xF8, 0xF0, 0x71, 0x10, 0x0E, 0x00, 0x00, 0x02, 0x07, 0x01,
0x1F, 0x3F, 0xFF, 0xF5, 0xEE, 0xEF, 0xFF, 0xF8, 0x80, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0xF8, 0xFE, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0xFF, 0xFF, 0x7F, 0x0F, 0x03,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x0F, 0x7F,
0xFF, 0xFF, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE,
0xF8, 0x70, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA0,
0x80, 0xC0, 0xE0, 0x32, 0x38, 0x38, 0x3E, 0xFF, 0xFF, 0xF3,
0xF8, 0x07, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x01, 0x0F, 0x3F, 0x87, 0x9F, 0xC0, 0xFE, 0x3C, 0x38,
0x70, 0x30, 0x64, 0xC0, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00,
0x00, 0x00, 0x80, 0xC0, 0xC0, 0x80, 0x00, 0x80, 0x80, 0x80,
0xC0, 0x80, 0x00, 0x00, 0x80, 0xC1, 0xC3, 0x83, 0x01, 0x81,
0xC0, 0x80, 0xC0, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x80,
0xC0, 0xC0, 0x80, 0x00, 0x00, 0x80, 0xC0, 0xC0, 0x80, 0x00,
0x80, 0xC0, 0x00, 0x01, 0x81, 0x83, 0x03, 0xC1, 0x80, 0xC0,
0xC0, 0x00, 0x80, 0xC0, 0xC0, 0x80, 0x00, 0x80, 0x80, 0xC0,
0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x63, 0xF5, 0xFF, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x8F, 0xE7, 0x7F, 0x0F, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F,
0x1F, 0x18, 0x18, 0x0F, 0x07, 0xFF, 0xFF, 0x18, 0x18, 0x1F,
0x0F, 0x07, 0x1F, 0x1A, 0x1A, 0x1B, 0x03, 0x0F, 0x1F, 0x00,
0x00, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0xC9, 0xDB, 0x1A, 0x1E,
0x1E, 0x0C, 0x0F, 0x1F, 0x18, 0x18, 0x1F, 0x07, 0x07, 0x1F,
0x18, 0x18, 0x1F, 0x1F, 0x00, 0x1F, 0x1F, 0x00, 0x00, 0x0F,
0x1F, 0x18, 0x18, 0x18, 0x06, 0x0F, 0x1A, 0x1A, 0x1B, 0x0B,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x80, 0x40, 0x20, 0x11, 0x13, 0x08, 0x08, 0x08,
0x18, 0x18, 0x18, 0x38, 0xF8, 0xF8, 0xF0, 0xF0, 0xE0, 0xE0,
0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xE0, 0xE0, 0xF0,
0xF0, 0xF8, 0x78, 0x38, 0x18, 0x18, 0x08, 0x08, 0x08, 0x0F,
0x13, 0x10, 0x20, 0x40, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x3F, 0x3F, 0x03, 0x03, 0x3F, 0x3E, 0x30,
0x3B, 0x2B, 0x2B, 0x3F, 0x3E, 0x00, 0x3F, 0x03, 0x03, 0x03,
0x3E, 0x3F, 0x23, 0x23, 0x3F, 0x3F, 0x07, 0x3F, 0x38, 0x3E,
0x07, 0x3E, 0x38, 0x1F, 0x03, 0x38, 0x3B, 0x2B, 0x2F, 0x3E,
0x00, 0x3F, 0x3F, 0x03, 0x03, 0x01, 0x3E, 0x3F, 0x2B, 0x2F,
0x2E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F,
0x1F, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
0x3E, 0x3E, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x1F, 0x1F, 0x0F,
0x04, 0x00, 0x04, 0x1F, 0x1F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
0x3E, 0x3E, 0x3E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
0x1E, 0x1E, 0x1F, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00]

page2 = [0] * 1024


# ===================================================================================

if __name__ == "__main__":
    _main()
