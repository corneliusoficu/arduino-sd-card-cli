#include "headers/commands.h"

char *current_dir = NULL;

int freeRam() 
{
    extern int __heap_start,*__brkval;
    int v;
    return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int) __brkval);  
}

bool set_current_directory(const char *new_directory)
{
    if( current_dir == NULL )
    {
        current_dir = (char*) malloc(sizeof(char) * 50);
    }

    File new_dir = SD.open(new_directory);

    if(new_dir)
    {
        strcpy(current_dir, new_directory);
        new_dir.close();
        return true;
    }
    else
    {
        new_dir.close();
        return false;
    }  
}

String get_complete_file_path(char *filename)
{
    String file_path;
    if(filename[0] == '/')
    {
        file_path = String(filename);
    }
    else if ( strcmp(current_dir, "/") == 0 )
    {
        file_path = String(current_dir) + filename;
    }
    else
    {
        file_path = String(current_dir) + "/" + filename;
    }
    return file_path;
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

    String new_folder = get_complete_file_path(args[1]);
    
    if( !SD.mkdir(new_folder) )
    {
        Serial.println(F("Directory creation failed!"));
    }
    else
    {
        Serial.println(F("Successfully created the directory!"));
    }

    return STATUS_OK;
}

status rmdir(char **args)
{
    if(!are_arguments_valid(args, 2))
    {
        Serial.println(F("Wrong number of arguments, format: rmdir {folder}"));
        return STATUS_OK;
    }

    String file_path = get_complete_file_path(args[1]);
    
    if(!SD.exists(file_path))
    {
        Serial.println("That file does not exist!");
        return STATUS_OK;
    }
    else
    {
        if(SD.rmdir(file_path))
        {
            Serial.println("Successfully deleted the folder!");
        }
        else
        {
            Serial.println("Cannot delete de folder!");
        }
    }
    return STATUS_OK;
}

status ls(char **args)
{
    char output_buffer[50];
    sprintf(output_buffer, "%15s %15s %15s", "Name", "Size", "Date created");
    Serial.println(output_buffer);

    File current_directory = SD.open(current_dir);
    current_directory.rewindDirectory();
    File entry;
    
    while(true)
    {
        entry = current_directory.openNextFile();

        if ( !entry )
        {
            current_directory.rewindDirectory();
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

    current_directory.close();

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

    new_path[0] = '\0';

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
                if ( strcmp(current_dir, "/") != 0 )
                {
                    strcat(current_dir, "/");
                }
                strcpy (new_path, current_dir);
                strncat(new_path, folder + 2, len_folder_name - 2);
            }
            if(len_folder_name >= 2 && folder[0] == '.' && folder[1] == '.')
            {

            }
            else
            {
                String new_path_str = get_complete_file_path(folder);
                strcpy(new_path, new_path_str.c_str());
            }
        }
    }

    if( new_path[0] == '\0' )
    {
        Serial.println(F("Cannot change path to specified directory!"));
    }
    else if(set_current_directory(new_path))
    {
        Serial.println(current_dir);
    }
    else
    {
        Serial.println(F("Cannot change path to specified directory!"));
    }

    return STATUS_OK;
}

status cat(char **args)
{
    if ( !are_arguments_valid(args, 2) )
    {
        Serial.println(F("Wrong number of arguments, format: cat {file_name}"));
        return STATUS_OK;
    }

    String file_path = get_complete_file_path(args[1]);

    if ( !SD.exists(file_path) )
    {
        Serial.println(F("That file does not exist!"));
        return STATUS_OK;
    }

    uint16_t len_file_path = file_path.length();
    String extension = file_path.substring(len_file_path - 3);
    if ( len_file_path > 3 && ( extension == "BMP" || extension == "bmp" ))
    {
        PrintBMPASCII(file_path.c_str());
        return STATUS_OK;
    }

    File read_file = SD.open(file_path.c_str());

    while(read_file.available())
    {
        Serial.write(read_file.read());
    }
    Serial.println();
    read_file.close();

    return STATUS_OK;
}

status echo(char **args)
{
    if( !are_arguments_valid(args, 4) )
    {
        Serial.println(F("Wrong number of arguments, format: echo \"something to write into a file\" > file.txt"));
        return STATUS_OK;
    }

    char *content =       args[1];
    char *redirect_char = args[2];
    char *file_name     = args[3];

    if(strcmp(redirect_char, ">") != 0)
    {
        Serial.println(F("Wrong arguments, format: echo \"something to write into a file\" > file.txt"));
        return STATUS_OK;
    }

    String file_path = get_complete_file_path(file_name);

    File output_file = SD.open(file_path.c_str(), FILE_WRITE);
    if ( !output_file )
    {
        Serial.println(F("Error opening the file!"));
        output_file.close();
        return STATUS_OK;
    }

    output_file.println(content);

    output_file.close();

    return STATUS_OK;
}

void printDirectory(File dir, char *file_path, char *searchedFile)
{
    dir.seek(0);
    uint8_t len_file_path = strlen(file_path);
    if (file_path[len_file_path - 1] != '/' )
    {
        file_path[len_file_path] = '/';
        file_path[len_file_path + 1] = '\0';
        len_file_path += 1;
    }

    while (true)
    {
        File entry = dir.openNextFile();

        if (!entry)
        {
            dir.rewindDirectory();
            return;
        }

        char *filename = entry.name();
        
        memcpy(file_path + len_file_path, filename, strlen(filename) + 1);
        file_path[ len_file_path + strlen(filename) + 1 ] = '\0';

        if(strcmp(filename, searchedFile) == 0)
        {
            Serial.println(file_path);
        }
        if (entry.isDirectory())
        {
            printDirectory(entry, file_path, searchedFile);
        }

        entry.close();
    }
}

status find(char **args)
{
    if(!are_arguments_valid(args, 2))
    {
        Serial.println(F("Wrong number of arguments, format: find {file name}"));
        return STATUS_OK;
    }

    File dir_name = SD.open(current_dir);

    if(!dir_name)
    {
        Serial.println(F("Cannot open the current directory!"));
        return STATUS_OK;
    }

    char file_path_buffer[100];
    strcpy(file_path_buffer, current_dir);

    dir_name.rewindDirectory();
    
    printDirectory(dir_name, file_path_buffer, args[1]);

    dir_name.close();
}

status cp(char **args)
{
    if(!are_arguments_valid(args, 3))
    {
        Serial.println(F("Wrong number of arguments, format: cp {path/file} {folder}"));
        return STATUS_OK;
    }

    char *first_file    = args[1];
    char *second_folder = args[2];

    File file1, file2;

    if(first_file[0] == '/')
    {
        file1 = SD.open(first_file, FILE_READ);
    }
    else
    {
        String complete_path = get_complete_file_path(first_file);
        file1 = SD.open(complete_path.c_str(), FILE_READ);
    }

    if(!file1)
    {
        Serial.println(F("Could not open the source file!"));
        return STATUS_OK;
    }

    String complete_path_second_file = get_complete_file_path(second_folder);

    Serial.println(complete_path_second_file);
    
    if(!SD.exists(complete_path_second_file.c_str()))
    {
        Serial.println(F("Destination folder does not exist!"));
        return STATUS_OK;
    }
    if(strcmp(second_folder, "/") != 0)
    {
        complete_path_second_file = complete_path_second_file + "/" + file1.name();
    }
    else
    {
        complete_path_second_file = complete_path_second_file + file1.name();
    }

    Serial.println(complete_path_second_file);

    file2 = SD.open(complete_path_second_file.c_str(), FILE_WRITE);
    
    if(!file2)
    {
        Serial.println(F("Could not create destination file"));
        return STATUS_OK;
    }

    size_t n;  
    uint8_t buf[64];

    while ((n = file1.read(buf, sizeof(buf))) > 0) {
        file2.write(buf, n);
    }

    file1.close();
    file2.close();

    Serial.println(F("Finished copying the file!"));

    return STATUS_OK;
}

bool is_one_in_divident(char *divident, uint8_t len)
{
    for(int ix = 0; ix < len; ix++)
    {
        if ( divident[ix] == '1' )
        {
            return true;
        }
    }
    return false;
}

uint8_t find_first_one_index(char *input, uint8_t len)
{
    for(uint8_t index = 0; index < len; index++)
    {
        if(input[index] == '1')
        {
            return index;
        }
    }
    return len - 1;
}

char *calculate_crc(char *input, char *polynomial, char type, char *crc)
{
    uint8_t len_polynomial = strlen(polynomial);
    uint8_t len_input      = strlen(input);
    
    char initial_padding[len_polynomial];

    for(int index = 0; index < len_polynomial - 1; index++)
    {
        if(type == 'e')
        {
            initial_padding[index] = '0';
        }
        else if(type == 'd')
        {
            initial_padding[index] = crc[index];
        }
        
    }
    initial_padding[len_polynomial - 1] = '\0';

    char input_padded[len_input + len_polynomial];

    strcpy(input_padded, input);
    strcat(input_padded, initial_padding);

    input_padded[(len_input + len_polynomial) - 1] = '\0';

    while(is_one_in_divident(input_padded, len_input))
    {
        int cur_shift = find_first_one_index(input_padded, len_input + len_polynomial - 1);
        for(uint8_t index = 0; index < len_polynomial; index++)
        {
            if ( polynomial[index] == input_padded[cur_shift + index] )
            {
                input_padded[cur_shift + index] = '0';
            }
            else
            {
                input_padded[cur_shift + index] = '1';
            }
        }
    }

    char *crc_output = (char*) malloc( sizeof(char) * len_polynomial );
    memcpy(crc_output, input_padded + len_input, len_polynomial);
    return crc_output;
}

char *get_bits_from_file(File file)
{
    char *bits = (char*) malloc( sizeof(char) * 256 );

    int position = 0;

    while(file.available())
    {
        char byt = file.read();
        for(int i = 0; i < 8; i++)
        {
            uint8_t new_bit = ((byt >> i) & 1);
            if ( new_bit == 1)
            {
                bits[position + (7 - i)] = '1';
            }
            else if ( new_bit == 0 )
            {
                bits[position + (7 - i)] = '0';
            }
        }
        position += 8;
        bits[position] = '\0';
    }
    
    return bits;
    
}

status crc(char **args)
{
    if(!(are_arguments_valid(args, 4) || are_arguments_valid(args, 5)))
    {
        Serial.println(F("Wrong number of arguments, format: crc {-e}{-d} {file} [crc] {polynomial}"));
        return STATUS_OK;
    }

    char *argument   = args[1];
    char *file_name  = args[2];
    char *crc;
    char *polynomial;

    if ( strcmp(argument,"-e") == 0 )
    {
        polynomial = args[3];
    }
    else if ( strcmp(argument,"-d") == 0 )
    {
        crc        = args[3];
        polynomial = args[4];
    }

    String file_path;
    if(file_name[0] == '/')
    {
        file_path = file_name;
    }
    else
    {
        file_path = get_complete_file_path(file_name);
    }

    File crc_file = SD.open(file_path.c_str(), FILE_READ);

    if(!crc_file)
    {
        Serial.println(F("Cannot open the file provided!"));
        return STATUS_OK;
    }

    uint8_t len_polynomial = strlen(polynomial);

    for(uint8_t index = 0; index < len_polynomial; index++)
    {
        if(!(polynomial[index] == '1' || polynomial[index] == '0'))
        {
            Serial.println(F("Wrong polynomial format, you must input a valid binary number!"));
            return STATUS_OK;
        }
    }

    char *input_bits = get_bits_from_file(crc_file);

    if ( strcmp(argument,"-e") == 0 )
    {
        char *resulted_crc = calculate_crc(input_bits, polynomial, 'e', NULL);
        Serial.print(F("Resulted CRC from the given file: "));
        Serial.println(resulted_crc);
        free(resulted_crc);
    }
    else if ( strcmp(argument,"-d") == 0 )
    {
        char *reversed_crc = calculate_crc(input_bits, polynomial, 'd', crc);
        Serial.print(F("Resulted value from the given file using the crc: "));
        Serial.println(reversed_crc);
        uint8_t nr_ones = 0;
        for(int index = 0; index < len_polynomial - 1; index++)
        {
            nr_ones += (reversed_crc[index] - 48);
        }
        if(nr_ones == 0)
        {
            Serial.println(F("The file provided is correct using the provided CRC and polynomial!"));
        }
        else
        {
            Serial.println(F("The file provided is incorrect using the provided CRC and polynomial!"));
        }
        free(reversed_crc);
    }

    free(input_bits);

    crc_file.close();
}