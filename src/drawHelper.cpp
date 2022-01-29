#include "drawHelper.hpp"

void fill(color* render_data, uint8_t pos, uint8_t len, color col) {
    for(uint8_t i = pos; i < pos + len; i++) render_data[i] = col;
}

uint8_t printChar(color* render_data, const char var, uint8_t position, color on, color off) {
    uint8_t idx = 0;
    if(var == ' ') idx = 1;
    if(var >= 'a' && var <= 'z') idx = var - 'a' + 12;
    if(var >= 'A' && var <= 'Z') idx = var - 'A' + 12;
    if(var >= '0' && var <= '9') idx = var - '0' + 2;
    uint16_t chars[] = {
        /*
            0b0'xxxxxxx'gfedcba'0:
             aa 
            f  b
            f  b
             gg 
            e  c
            e  c
             dd 

            0b0'nmlkjih'gfedcba'1:
             ab 
            l  c
            k  d
             mn 
            j  e
            i  f
             hg 

            0b1'nmlkjih'gfedcba'0:
             aa    hh 
            f  b  m  i
            f  b  m  i
             gg    nn 
            e  c  l  j
            e  c  l  j
             dd    kk 
        */
        0b0100110101001011, // invalid char
        0b0000000000000000, // off
        0b0000000001111110, // 0
        0b0000000000001100, // 1
        0b0000000010110110, // 2
        0b0000000010011110, // 3
        0b0000000011001100, // 4
        0b0000000011011010, // 5
        0b0000000011111010, // 6
        0b0000000000001110, // 7
        0b0000000011111110, // 8
        0b0000000011011110, // 9
        0b0000000011101110, // a
        0b0000000011111000, // b
        0b0000000001110010, // c
        0b0000000010111100, // d
        0b0000000011110010, // e
        0b0000000011100010, // f
        0b0101111111100111, // g
        0b0000000011101000, // h
        0b0000000001101001, // i
        0b0000000000011100, // j
        0b0000000000000000, // k // TODO
        0b0000000001110000, // l
        0b0000000000000000, // m // TODO // 2
        0b0000000010101000, // n
        0b0000000010111000, // o
        0b0000000011100110, // p
        0b0000000011001110, // q
        0b0000000000000000, // r // TODO
        0b0000000000000000, // s // TODO // 3
        0b0000000011110000, // t
        0b0000000001111100, // u
        0b0000000000111000, // v
        0b0000000000000000, // w // TODO // 2
        0b0000000000000000, // x // TODO
        0b0000000011011100, // y
        0b0000000000000000, // z // TODO // 3
    };
    uint16_t chr = chars[idx];
    if(chr & 1) { // 14 seg
        for(uint8_t i = 0; i < 14; i++) render_data[i + position] = ((chr >> (i + 1)) & 1) ? on : off;
        return 14;
    } else if ((chr >> 15) & 1) { // double space 7 seg
        for(uint8_t i = 0; i < 28; i++) render_data[i + position] = ((chr >> ((i & 0xFFFE) / 2 + 1)) & 1) ? on : off;
        return 28;
    } else { // normal 7 seg
        for(uint8_t i = 0; i < 14; i++) render_data[i + position] = ((chr >> ((i & 0xFFFE) / 2 + 1)) & 1) ? on : off;
        return 14;
    }
}

color fadeToBlack(color c, uint16_t numerator, uint16_t denominator) {
    return {
        .r=(uint8_t) min(255, ((uint16_t) c.r * numerator) / denominator),
        .g=(uint8_t) min(255, ((uint16_t) c.g * numerator) / denominator),
        .b=(uint8_t) min(255, ((uint16_t) c.b * numerator) / denominator),
    };
}

void doCharPrint(uint8_t &textI, const uint8_t textMaxLenght, const char* text, color* rd, uint8_t &ci, const color on, const color off) {
    if(textI < textMaxLenght && text[textI] != '\0') ci += printChar(rd, text[textI++], ci, on, off);
}

void checkColon(uint8_t &textI, const uint8_t textMaxLenght, const char* text, color* rd, uint8_t &ci, const color on, const color off, bool &doColon) {
    if(textI < textMaxLenght && text[textI] != '\0') {
        if(text[textI] == ':') {
            doColon = true;
            textI++;
        } else ci += printChar(rd, text[textI++], ci, on, off);
    }
}

uint8_t drawCoustomText(color* renderdata, const char* text, uint8_t textMaxLenght, uint8_t pos, uint8_t length, color on, color off) {
    color rd[14*7];
    uint8_t textI = 0;
    uint8_t ci = 0;
    bool col0 = false;
    bool col1 = false;
    if(ci == 14 * 0) doCharPrint(textI, textMaxLenght, text, rd, ci, on, off);
    if(ci == 14 * 1) doCharPrint(textI, textMaxLenght, text, rd, ci, on, off);
    if(ci == 14 * 2) checkColon(textI, textMaxLenght, text, rd, ci, on, off, col0);
    if(ci == 14 * 2) doCharPrint(textI, textMaxLenght, text, rd, ci, on, off);
    if(ci == 14 * 3) doCharPrint(textI, textMaxLenght, text, rd, ci, on, off);
    if(ci == 14 * 4) checkColon(textI, textMaxLenght, text, rd, ci, on, off, col1);
    if(ci == 14 * 4) doCharPrint(textI, textMaxLenght, text, rd, ci, on, off);
    if(ci == 14 * 5) doCharPrint(textI, textMaxLenght, text, rd, ci, on, off);
    if(col0) fill(rd, 14*6 + 0, 2, on); else fill(rd, 14*6 + 0, 2, off);
    if(col1) fill(rd, 14*6 + 2, 2, on); else fill(rd, 14*6 + 2, 2, off);
    for(uint8_t i = 0, j = pos; i < min(length, (uint8_t) (14*7)); i++, j++) renderdata[j] = rd[i];
    return ci;
}
