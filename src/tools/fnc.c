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
static void illegal_filetype(char*, char*, char*);
static void parameter_required(char);
static int has_input_files(int,int);
static int has_suffix(char*, char*);
static int check_input_files(char**, int, int, char*);


int main(int argc, char** argv)
{
    int flag_help= 0;
    int flag_verbose = 0;
    char* output_dir = ".";
    char* output_type = "beam";
    int free_outputdir = 0;
    int free_outputtype = 0;

    int ret_status = -1;

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
                          goto exit;
                      }
            case ':' : {
                           parameter_required(optopt);
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

    if(!check_input_files(argv, argc, optind, output_type)) {
        goto exit;
    }

    ret_status = 0;

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
    printf("        'beam', 'ast', 'tree', 'lex', 'erl' 'erl2ast'.\n");
    printf("        Default output type is 'beam'\n");
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

static int has_suffix(char* str, char* suffix)
{
    int suffix_len = strnlen(suffix, MAX_STRLEN);
    int str_len = strnlen(str, MAX_STRLEN);

    char* str_cmp_offset = str + (str_len - suffix_len);

    if(strncmp(str_cmp_offset, suffix, MAX_STRLEN) == 0) {
        return 1;
    }

    return 0;
}

static void illegal_filetype(char* str, char* suffix, char* type)
{ 
    printf("Specified output type is '%s', expecting a '%s' file, but found '%s'\n", type, suffix, str);
}

static int check_input_files(char** files, int count, int index, char* type)
{
    int i;
    for(i = index; i<count; i++) { 
        char* file = files[i];

        if(strncmp(type, "erl2ast", MAX_STRLEN) == 0) {
            if(!has_suffix(file, ".erl")) {
                illegal_filetype(file, ".erl", type);
                return 0;
            }
        } else {
            if(!has_suffix(file, ".fn")) {
                illegal_filetype(file, ".fn", type);
                return 0;
            } 
        }

        if(access(file, R_OK) != 0) {
            printf("Can't read file '%s'\n", file);
            return 0;
        }
    }

    return 1;
}
