#include "headers/commands.h"

char   output_buffer[50];
String current_dir;

int freeRam() 
{
    extern int __heap_start,*__brkval;
    int v;
    return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int) __brkval);  
}

bool set_current_directory(String new_directory)
{
    File new_dir = SD.open(new_directory);

    if(new_dir)
    {
        current_dir = new_directory;
        new_dir.close();
        return true;
    }
    else
    {
        new_dir.close();
        return false;
    }  
}

bool are_arguments_valid(char **args, uint8_t nr)
{
    uint8_t nr_args = 0;
    char *arg = args[nr_args];

    while( arg != NULL )
    {
        arg = args[++nr_args];
    }

    return nr == nr_args;
}

uint8_t get_len_commands_array()
{
    return sizeof(available_commands_str) / sizeof(char*);
}

status mkdir(char **args)
{
    if ( !are_arguments_valid(args, 2) )
    {
        Serial.println(F("Wrong number of arguments, format: mkdir name!"));
        return STATUS_OK;
    }

    char *filename = args[1];

    if( !SD.mkdir(filename) )
    {
        Serial.println(F("Directory creation failed!"));
    }
    else
    {
        Serial.println(F("Successfully created the directory!"));
    }

    return STATUS_OK;
}

status ls(char **args)
{
    sprintf(output_buffer, "%15s %15s %15s", "Name", "Size", "Date created");
    Serial.println(output_buffer);

    File current_directory = SD.open(current_dir.c_str());
    while(true)
    {
        File entry = current_directory.openNextFile();

        if ( !entry )
        {
            break;
        }

        if(!entry.isDirectory())
        {
            sprintf(output_buffer, "%15s %15lu", entry.name(), entry.size());
            
        }
        else
        {
            sprintf(output_buffer, "%15s %15s", entry.name(), "Directory"); 
        }

        entry.close();

        Serial.println(output_buffer);
        Serial.flush();
    }

    return STATUS_OK;
}

status cd(char **args)
{

    if( !are_arguments_valid(args, 2) )
    {
        Serial.println(F("Wrong number of arguments, format cd {folder name}. Type cd . to see the current folder name!"));
        return STATUS_OK;
    }

    char *folder = args[1];

    if ( strcmp(folder, ".") == 0 )
    {
        Serial.println(current_dir);
        return STATUS_OK;
    }

    char new_path[100];

    if(folder[0] == '/')
    {
        strcpy(new_path, folder);
    }
    else
    {
        size_t len_folder_name = strlen(folder);
        if ( len_folder_name > 1 )
        {
            if(folder[0] == '.' && folder[1] == '/')
            {
                current_dir.toCharArray(new_path, current_dir.length() + 1);
                strncat(new_path, folder + 2, len_folder_name - 2);
            }
        }
    }

    if(set_current_directory(String(new_path)))
    {
        Serial.println(current_dir);
    }
    else
    {
        Serial.println(F("Cannot change path to specified directory!"));
    }

    return STATUS_OK;

}