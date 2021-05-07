/********************************************************************************
 * Author   : Abhinav Thakur                                                    *
 * Email    : compilepeace@gmail.com                                            *
 * Filename : parse_opts.cpp                                                    *
 *                                                                              *
 * Description: Responsible for parsing command line options/flags provided by  *
 *              the end user. Supports both short and long option               *
 *                                                                              *
 * NOTE: Look for placeholder "____" for editing this file                      *
 *                                                                              * 
 ********************************************************************************/


#include <getopt.h>

#include "peace_helper.h"

/* MACRO definitions */
#define DEBUG_INFO  __FILE__,__FUNCTION__,__LINE__
#define MY_LOG_OPT_CHAR  0x200
#define MAX_LEN_FILENAME 0xff


/* Parse cmd line flags to get information that decides further program flow */
void parse_opts (int argc, char **argv) {   

    static struct option long_options[] = {
        {"log",             optional_argument,  NULL,   MY_LOG_OPT_CHAR},
    /*  {"something",       required_argument,  NULL,   's'}, add options here ____ */      
        {"usage",           no_argument,        NULL,   'u'},
        {0, 0, 0, 0}
    };
    int optchar, openLogFlags, failFlag = 0;
    mode_t openLogMode;
    char *logFileName;


    /* ____ add validations for command line options here */
    if (argc < 2)
        usage_error (argv[0]);

    /* add command-line options here */
    while ( (optchar = getopt_long (argc, argv, "+:l::u" /* ____ add optstring here */, long_options, nullptr)) != -1) {
        /*  '+' char in  optstring above is for portability reasons
            ':' char after '+' is to supress getopt() default error messages
        
            Optional argument (2 colons after option character)
            optstring - "a::" means -a <optional_argument> 
        */

        /* There are 2 errors associated with getopt(), i.e.
            1. Unrecognized Option
            2. Missing Argument
            see getopt return values '?' and ':' for handling part.
        */
        switch (optchar) {

            /* ____ add options */
            case MY_LOG_OPT_CHAR:
                        /* --log [filename] */

                        if (optarg) {
                            debugMsg ( DEBUG_INFO, "Creating a logfile: %s\n", optarg);
                            logFileName = strndup (optarg, MAX_LEN_FILENAME-1);
                            if (logFileName == NULL) {
                                errUsrMsg (DEBUG_INFO, "strdup(logFileName)... perhaps no memory left");
                                failFlag = 1;
                            }          
                        }
                            
                        else {
                            debugMsg ( DEBUG_INFO, "Creating a logfile: %s.log\n", argv[0]);
                            logFileName = (char *) malloc (MAX_LEN_FILENAME);
                            if (logFileName == NULL) {
                                errSysMsg (DEBUG_INFO, "malloc()'ing for logfile name: %s.log", argv[0]);
                                failFlag = 1;
                            }
                            else    
                                snprintf (logFileName, MAX_LEN_FILENAME-1, "%s.log", basename (argv[0]));
                        }

                        openLogFlags = O_CREAT|O_WRONLY|O_APPEND|O_TRUNC;
                        openLogMode  = S_IRUSR|S_IWUSR|S_IRGRP;

                        if (failFlag)   
                            logfd = open ("error_debugging.log", openLogFlags, openLogMode);
                        else            
                            logfd = open (logFileName, openLogFlags, openLogMode);

                        if (logfd == -1)
                            errSysExit (DEBUG_INFO, "while opening logfile: %s", logFileName);

                        free (logFileName);
                        break;

            case 'u':   /* --usage */
                        usage_error (argv[0]);
                        break; 

            case '?':   /* 
                            'Unrecognized Option' character encountered & stored in optopt. 
                            opterr = 1 (by default), if we set opterr = 0, it will surpress
                            error messages printed by getopt(). This is used to display 
                            customized error messages.

                            Another way to supress getopt() error messages is to put a ':'
                            character at the start of 'optstring' parameter.
                        */
                        debugMsg (DEBUG_INFO, "Unrecognized Option: %c\n", optopt);
                        usage_error (argv[0]);

            case ':':   /* 
                            'Missing Argument' for option 
                        */
                        debugMsg (DEBUG_INFO, "Missing Argument for option: %c\n", optopt);
                        usage_error (argv[0]);
            default :
                        usage_error (argv[0]);
        }
    }

    /* ____ confirm if you take last argument as some filename
    // argv[optind] is NULL if end of argument list is reached
    if (optind < argc && argv[optind] != NULL)
        filename = &argv[optind][0];
    else 
        usage_error (argv[0]);
    */

   return;
}



void usage_error (const char *program_name) {

    fprintf (stderr,

                    "\n" BOLDRED
                    "[-]" BOLDCYAN
                    " Usage: " BOLDGREEN "%s " BOLDWHITE "[l|u] \n\t" RESET /* ____ add options here */
                    BOLDBLUE "NA" RESET " | " BOLDBLUE "--log=[filename]            " RESET ":" DIM YELLOW " log in [filename] (if filename not supplied, create a <project_name>.log)\n\t" RESET
                    BOLDBLUE "-u" RESET " | " BOLDBLUE "--usage                     " RESET ":" DIM YELLOW " print help \n\t" RESET
                    "\n" RED 
                    "NOTE" RESET ": --log option does not support short flag (NA stands for Not Applicable)."
                                 " An '=' sign is mandatory to specify filename (undocumented by getopt manual) \n\n",

            program_name);
    
    exit (0xd3ad);
}
