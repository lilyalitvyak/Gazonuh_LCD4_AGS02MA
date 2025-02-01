#include <Arduino.h>

String buildString(char* literal, uint32_t number);
String buildString(char* literal);
String buildString(uint32_t number);
String buildString(float number);
String buildString(char* literal, float number, char* suffix);