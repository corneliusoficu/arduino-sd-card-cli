#include "headers/shell.h"

char **arguments;
String input_text;

int start_millis, timeout_value = 1000;

void handle_commands()
{
    Serial.print( ">" );
    input_text = read_line();
    arguments = get_command_arguments(input_text);

    
}

String read_line()
{
    String input_line;

    while( !Serial.available() );
    
    input_line = Serial.readString();
    return input_line;

}

char** get_command_arguments(String input)
{

    state char_state = READ_SPACE;
    char *argument = NULL,  **arguments = NULL, *chr, *start_chr;
    uint32_t nr_words = 0, len_word = 0, len_input = input.length();

    argument  = (char*)  malloc( sizeof(char)  * len_input  );
    arguments = (char**) malloc( sizeof(char*) * MAXIMUM_ARGUMENTS_COUNT );

    if( argument == NULL )
    {
        Serial.println("Error alocating memory for input buffer!");
        exit( EXIT_FAILURE );
    }

    if ( arguments == NULL )
    {
        Serial.println("Error allocating memory for command arguments buffer!");
        exit( EXIT_FAILURE );
    }

    input.toCharArray( argument, len_input );

    chr = argument;
    
    while( *chr != '\0' )
    {
        switch(char_state)
        {
            case READ_CHAR:

                if ( *chr == ' ' )
                {
                    arguments[nr_words] = (char*) malloc( sizeof(char) * len_word );
                    if ( !arguments[nr_words] )
                    {
                        Serial.println("Error allocating memory in the arguments array");
                        exit( EXIT_FAILURE );
                    }

                    memcpy(arguments[nr_words], start_chr, len_word);
                    arguments[nr_words][len_word] = '\0';
                    nr_words++;
                        
                    char_state = READ_SPACE;

                    Serial.println(arguments[nr_words - 1]);

                }
                else
                {
                    len_word++;
                }
                break;
            case READ_QUOTE:
                
                break;
            case READ_SPACE:
                
                if ( *chr == '"' )
                {

                    state = READ_QUOTE;
                }
                else if ( *chr != ' ' )
                {
                    start_chr = chr;
                    len_word = 1;
                    char_state = READ_CHAR;
                }

                break;
            default:

                break;
        }
        chr++;
    }
    

}

