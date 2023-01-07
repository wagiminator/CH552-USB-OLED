// ===================================================================================
// SSD1306 128x64 Pixels OLED Terminal Functions
// ===================================================================================

#pragma once

void OLED_init(void);           // OLED init function
void OLED_clearScreen(void);    // OLED clear screen
void OLED_write(char c);        // OLED write a character or handle control characters
void OLED_print(char* str);     // OLED print string
void OLED_println(char* str);   // OLED print string with newline
