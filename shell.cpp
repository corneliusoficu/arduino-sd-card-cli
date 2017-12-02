#include "headers/shell.h"

char **arguments;
char  *input_text;

void handle_commands()
{
    Serial.print( F(">") );
    input_text = read_line();
    arguments = get_command_arguments(input_text);
    execute_command( arguments );
    free(arguments);
    free(input_text);
    Serial.print(F("Free Ram = "));Serial.println(freeRam());
    Serial.flush();
    
}

char *read_line()
{
    String input_line;
    uint16_t input_len;
    char *cmd_input;

    while( !Serial.available() );
    
    input_line = Serial.readString();
    input_len  = input_line.length();
    
    cmd_input = (char*) malloc( sizeof(char)  * input_len  );

    if ( cmd_input == NULL )
    {
        Serial.println(F("Error alocating memory for input buffer!"));
        exit( EXIT_FAILURE );
    }

    input_line.toCharArray( cmd_input, input_len );

    return cmd_input;

}

void add_argument_in_arguments_list(char**    arguments, 
                                    uint32_t& nr_words, 
                                    char*     argument, 
                                    uint32_t  len_word, 
                                    char*     start_chr, 
                                    state&    char_state)
{
    arguments[nr_words] = (char*) malloc( sizeof(char) * (len_word + 1) );
    if ( !arguments[nr_words] )
    {
        Serial.println(F("Error allocating memory in the arguments array"));
        exit( EXIT_FAILURE );
    }

    memcpy(arguments[nr_words], start_chr, len_word + 1);
    arguments[nr_words][len_word] = '\0';

    nr_words++;
    arguments[nr_words] = NULL;
    char_state = READ_SPACE;
}

char** get_command_arguments(char *argument)
{

    state char_state = READ_SPACE;
    char **arguments = NULL, *chr, *start_chr;
    uint32_t nr_words = 0, len_word = 0, len_input = strlen( argument );

    arguments = (char**) malloc( sizeof(char*) * MAXIMUM_ARGUMENTS_COUNT );

    if ( arguments == NULL )
    {
        Serial.println(F("Error allocating memory for command arguments buffer!"));
        exit( EXIT_FAILURE );
    }

    arguments[0] = NULL;

    chr = argument;
    
    while( *chr != '\0' )
    {
        switch(char_state)
        {
            case READ_CHAR:

                if ( *chr == ' ' )
                {
                    add_argument_in_arguments_list(arguments, nr_words, argument, len_word, start_chr, char_state);
                }
                else
                {
                    len_word++;
                }
                break;

            case READ_QUOTE:

                if ( *chr == '"' )
                {
                    add_argument_in_arguments_list(arguments, nr_words, argument, len_word - 1, start_chr + 1, char_state);
                }
                else
                {
                    len_word++;
                }
                break;

            case READ_SPACE:
                
                if ( *chr == '"' )
                {
                    start_chr = chr;
                    len_word = 1;
                    char_state = READ_QUOTE;
                }
                else if ( *chr != ' ' )
                {
                    start_chr = chr;
                    len_word = 1;
                    char_state = READ_CHAR;
                }

                break;
        }
        chr++;
    }

    if ( len_word > 0 )
    {
        add_argument_in_arguments_list(arguments, nr_words, argument, len_word, start_chr, char_state);
    }

    return arguments;
    

}

status execute_command(char **args)
{
    char *command_name;
    uint8_t len_commands_array;

    if ( args[0] == NULL )
    {
        Serial.println(F("You need to introduce a valid command!"));
        return STATUS_OK;
    }

    len_commands_array = get_len_commands_array();
    
    for( int index = 0; index < len_commands_array; index++ )
    {
        if ( strcmp(args[0], available_commands_str[index] ) == 0 )
        {
            return (*commands_ptr[index])(args);
        }
    }
    Serial.println();

    Serial.println(F("No command with that name found!"));
    return STATUS_OK;
}

