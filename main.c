/* 
-x: This option specifies that the output should be displayed in hexadecimal format.

-o: This option specifies that the output should be displayed in octal format.

-n: This option, which requires an integer argument, specifies how many elements (bytes) should be printed per line. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>

#define _GNU_SOURCE // to provide getopt work style as GNU instead of BSD.


#define DEFAULT_LINE_CHAR       16 // option -n's default argument.

bool disp_text(FILE *f);
bool disp_hex(FILE *f, int n_arg); // function definition of hex display.
bool disp_octal(FILE *f, int n_arg); // function definition of octal display.
int check_number(const char *str); // function to determine option -n's argument even it includes whitespace characters.


int main(int argc, char *argv[])
{
    int result;
    int t_flag, o_flag, x_flag, n_flag, err_flag;
    int n_arg;
    FILE *f;
    
    t_flag = o_flag = x_flag = n_flag = err_flag = 0; // all of flags set to the 0.
    n_arg = DEFAULT_LINE_CHAR;
    opterr = 0; // default error mesages ignored.


    while ((result = getopt(argc, argv, "toxn:")) != -1) { //"toxn:" meaning => there is 4 option as -t, -o, -x, -n
                                                                        //      but -n requires an argument because of
                                                                        //      using : after n.
                                                                        
        switch (result) {
            case 't':
                t_flag = 1;
                break;
            case 'o':
                o_flag = 1;
                break;
            case 'x':
                x_flag = 1;
                break;
            case 'n':
                n_flag = 1;
                if ((n_arg = check_number(optarg)) < 0) { // option -n's argument must be greater than 0.
                    fprintf(stderr, "-n argument is invalid!..\n");
                    err_flag = 1;
                }
                break;
            case '?': // getopt function returns '?' character when option is not valid.
                if (optopt == 'n') // optopt keeps the which character caused error.
                    fprintf(stderr, "-%c option given without argument!..\n", optopt);
                    // if it's n, that means there is a problem on argument.
                
                else // if it's not n, that menas there is a problem about given option.
                    fprintf(stderr, "invalid option: -%c\n", optopt);
                
                err_flag = 1; //err_flag set to 1 to check error situation at out of this loop. 


            break;
        }
    }


    if (err_flag) // if err_flag equal to 1, exit.
        exit(EXIT_FAILURE);


    if (t_flag + o_flag + x_flag > 1) { // only one of this options must be used.
        fprintf(stderr, "only one of -[tox] option may be specified!..\n");
        exit(EXIT_FAILURE);
    }


   if (t_flag + o_flag + x_flag == 0) // if no options were used, make -t option default used.
        t_flag = 1;


    if (t_flag && n_flag) { // -t and -n option can't be used together.
        fprintf(stderr, "-n option cannot be used with -t option!..\n");
        exit(EXIT_FAILURE);
    }


    if (argc - optind == 0) {  
        fprintf(stderr, "file must be specified!..\n");
        exit(EXIT_FAILURE);

    /* argc holds the number of command-line arguments, including the program name.
    optind is the index of the first non-option argument (usually the filename) after getopt processes all options.
    If argc - optind == 0, it means no filename was provided, which results in an error. */
    }

    if (argc - optind > 1) {
        fprintf(stderr, "too many files specified!..\n");
        exit(EXIT_FAILURE);

    /* argc holds the number of command-line arguments, including the program name.
    optind is the index of the first non-option argument (usually the filename) after getopt processes all options.
    If argc - optind == 0, it means no filename was provided, which results in an error. */
    }


    if ((f = fopen(argv[optind], t_flag ? "r" : "rb")) == NULL) { 
        fprintf(stderr, "cannot open file: %s\n", argv[optind]);
        exit(EXIT_FAILURE);
         
        /*  fopen(filename, open mode). if t_flag is equal to 1 that means program open file with text mode and read directly.
        if t_flag equal to 0, that means -o or -x option used this situation program must read raw text. so file opens 
        as binary mode rb. */
    }

    if (t_flag) // all of disp functions return _Bool (thanks to stdbool.h, bool) type.
        result = disp_text(f); 
    else if (x_flag)
        result = disp_hex(f, n_arg);
    else if (o_flag)
        result = disp_octal(f, n_arg);


    if (!result) { // if result false, exit.
        fprintf(stderr, "cannot read file: %s\n", argv[optind]);
        exit(EXIT_FAILURE);
    }


    fclose(f); //close file.


    return 0;
}


bool disp_text(FILE *f) // function if option -t choosen.
{
    int ch;


    while ((ch = fgetc(f)) != EOF) // read the characters till the end of file (eof) -fgetc returns EOF when it's end-
        putchar(ch); // print character one by one to standart output.


    return feof(f); // if pointer f points end of the file, returns 1. if not returns 0.
}


bool disp_hex(FILE *f, int n_arg) // function if option -x choosen.
{
    size_t i;
    int ch;


    for (i = 0;(ch = fgetc(f)) != EOF; ++i) { // read the binaries till the end of file (eof) -fgetc returns EOF when it's end-
        if (i % n_arg == 0) { // n_arg is integer argument to specifies how many elements (bytes) should be printed per line. when i equals to n_arg, that means program continue binaries to the new line.
            if (i != 0)   
                putchar('\n');

        printf("%08zX ", i); // prints a size_t value in uppercase hexadecimal, padded with leading zeros to ensure it is 8 characters wide. for example 16 becomes 00000010.
        }
        printf("%02X ", ch); // prints a value in uppercase hexadecimal, padded with leading zeros to ensure it is 2 characters wide. for example 5 becomes 05.


    }
    putchar('\n');


    return feof(f); // if pointer f points end of the file, returns 1. if not returns 0.
}


bool disp_octal(FILE *f, int n_arg) // function if option -o choosen.
{
    size_t i;
    int ch;


    for (i = 0;(ch = fgetc(f)) != EOF; ++i) { // read the binaries till the end of file (eof) -fgetc returns EOF when it's end-
        if (i % n_arg == 0) // n_arg is integer argument to specifies how many elements (bytes) should be printed per line. when i equals to n_arg, that means program continue binaries to the new line.
            printf("%08zo ", i); // prints a size_t value in octal format, padded with leading zeros to ensure it is 8 characters wide. for example 16 becomes 00000020 (in octal). 
    
        printf("%03o ", ch); // prints a value in octal format, padded with leading zeros to ensure it is 3 characters wide. for example 5 becomes 005 (in octal).

        if (i % n_arg == n_arg - 1)
            putchar('\n');
    }
    putchar('\n');


    return feof(f); // if pointer f points end of the file, returns 1. if not returns 0.
}


int check_number(const char *str) // function to determine option -n's argument even it includes whitespace characters.
{
    const char *temp;
    int result;


    while (isspace(*str))
        ++str;
    
    temp = str;


    while (isdigit(*str))
        ++str;
    
    if (*str != '\0')
        return -1;
    
    result = atoi(temp);
    if (!result)
        return -1;


    return result;
}

