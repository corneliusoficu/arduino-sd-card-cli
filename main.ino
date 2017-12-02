#include "headers/shell.h"

void setup()
{
    Serial.begin(9600);
    while(!Serial);

    if(!SD.begin(4))
    {
        Serial.println(F("Cannot access SD card!"));
        return;
    }

    Serial.println(F("Welcome to Corneliu's SD card CLI!"));

    if( !set_current_directory("/") )
    {
        Serial.print  (F("Failed to open the root directory of SD Card"));
        Serial.flush();
        exit( EXIT_FAILURE );
    }

    Serial.println(F("Succesfully accessed the SD card!"));
    
    Serial.flush();
}

void loop()
{
    handle_commands();
}