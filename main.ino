#include <SPI.h>
#include <SD.h>
#include "headers/shell.h"

void setup()
{
    Serial.begin(9600);
    while(!Serial);

    if(!SD.begin(4))
    {
        Serial.println("Cannot access SD card!");
        return;
    }

    Serial.println("Succesfully accessed the SD card!");
    Serial.println("Welcome to Corneliu's SD card CLI!");
    Serial.flush();
}

void loop()
{
    handle_commands();
}