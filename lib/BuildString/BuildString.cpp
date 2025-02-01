#include "BuildString.h"

uint8_t countDigits(int num) {
   return((num<0?2:1)+(int8_t)log10(abs(num))) ;
}

String buildString(char* literal, uint32_t number) {
    uint8_t numberSize = countDigits(number);
    uint8_t literalSize = strlen(literal);
    char buf[numberSize+literalSize];
    char buffer [numberSize];
    char* append = itoa(number, buffer, 10);
    strcpy(buf, literal);
    strcpy(buf + literalSize, append);
    String out = buf;
    return out;
}

String buildString(char* literal) {
    uint8_t literalSize = strlen(literal);
    char buf[literalSize];
    strcpy(buf, literal);
    String out = buf;
    return out;
}

String buildString(uint32_t number) {
    uint8_t numberSize = countDigits(number);
    char buf[numberSize];
    char buffer [numberSize];
    char* append = itoa(number, buffer, 10);
    strcpy(buf, append);
    String out = buf;
    return out;
}

String buildString(float number) {
    uint8_t numberSize = countDigits(number) + 3;
    char buf[numberSize];
    char buffer [numberSize];
    dtostrf(number, numberSize, 2, buffer);
    strcpy(buf, buffer);
    String out = buf;
    return out;
}

String buildString(char* literal, float number, char* suffix) {
    uint8_t literalSize = strlen(literal);
    uint8_t numberSize = countDigits(number) + 3;
    uint8_t suffixSize = strlen(suffix);
    char buf[numberSize+literalSize+suffixSize];
    char buffer [numberSize];
    char* append = dtostrf(number, numberSize, 2, buffer);
    strcpy(buf, literal);
    strcpy(buf + literalSize, buffer);
    strcpy(buf + literalSize + numberSize, suffix);
    String out = buf;
    return out;
}
