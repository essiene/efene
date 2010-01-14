#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/*This definition of OPTSTRING for getopt
  will stop option parsing after the first
  non option argument it encounters, and
  will set 'optopt' to ':' when it encounters
  an option that needs an argument, without
  the argument being specified */
#define OPTSTRING "+:ho:t:v"
#define MAX_STRLEN 255

int opterr = 0;

static void usage(char**);
static void illegal_option(char);
static void parameter_required(char);
static int has_input_files(int,int);


int main(int argc, char** argv)
{
    int flag_help= 0;
    int flag_verbose = 0;
    char* output_dir = ".";
    char* output_type = "beam";
    int free_outputdir = 0;
    int free_outputtype = 0;

    int ret_status = 0;

    while(1) {
        int c = getopt(argc, argv, OPTSTRING);

        switch(c) {
            case 'h': flag_help = 1; break;
            case 'v': flag_verbose = 1; break;
            case 'o': {
                          output_dir = strndup(optarg, MAX_STRLEN);
                          free_outputdir = 1;
                          break;
                      }
            case 't': {
                          output_type = strndup(optarg, MAX_STRLEN); 
                          free_outputtype = 1;
                          break;
                      }
            /* getopt puts a '?' in global char 'optopt'
               when it detects an illegal option char */
            case '?': {
                          illegal_option(optopt);
                          ret_status = -1;
                          goto exit;
                      }
            case ':' : {
                           parameter_required(optopt);
                           ret_status = -1;
                           goto exit;
                       }
        }

        if(c == -1) {
            break;
        }

    }

    if(flag_help) {
        usage(argv);
        goto exit;
    }

    if(!has_input_files(optind, argc)) {
        printf("No input files\n");
        goto exit;
    }

exit:
    if(free_outputdir) { 
        free(output_dir);
    }

    if(free_outputtype) { 
        free(output_type);
    }

    return ret_status;
}


/* private */

static void usage(char** s)
{
    printf("Usage: %s [-hv] [-o outputdir] [-t format]  infiles\n", basename(s[0]));
    printf("  -h    show this help message and exit\n");
    printf("  -o    Specify the output directory. Default is the\n");
    printf("        current working directory\n");
    printf("  -t    Specify the output format. Valid values are:\n");
    printf("        'beam', 'ast', 'tree', 'lex', 'erl'. Default\n");
    printf("        output type is 'beam'\n");
    printf("  -v    Turn on verbose mode for compiler\n");
    printf("\n");
    printf("All input files should have an extension of .fn\n");
}

static void illegal_option(char c)
{
    printf("Illegal option: -%c\n", c);
    printf("  use -h for usage instructions\n");
}

static void parameter_required(char c)
{
    printf("Option '-%c' requires an argument\n", c);
    printf("  use -h for usage instructions\n");
}

static int has_input_files(int index, int count)
{
    if(index < count) {
        return 1;
    }

    return 0;
}
