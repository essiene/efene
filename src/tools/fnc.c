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

typedef struct {
    int flag_help;
    int flag_verbose;

    int free_outputdir;
    int free_outputtype;

    int check_status;

    int index;

    char* output_dir;
    char* output_type;
} Param;


int opterr = 0;

static void usage(char**);
static void illegal_option(char);
static void illegal_filetype(char*, char*, char*);
static void parameter_required(char);
static int has_input_files(Param,int);
static int has_suffix(char*, char*);
static int check_input_files(char**, int, Param);
static Param check_args(int, char**);


int main(int argc, char** argv)
{

    int ret_status = -1;

    Param param = check_args(argc, argv);
    if(!param.check_status) {
        goto exit;
    }


    if(param.flag_help) {
        usage(argv);
        goto exit;
    }

    if(!has_input_files(param, argc)) {
        fprintf(stderr, "No input files\n");
        goto exit;
    }

    if(!check_input_files(argv, argc, param)) {
        goto exit;
    }

    ret_status = 0;

exit:
    if(param.free_outputdir) { 
        free(param.output_dir);
    }

    if(param.free_outputtype) { 
        free(param.output_type);
    }

    return ret_status;
}


/* private */

static void usage(char** s)
{
    fprintf(stdout, "Usage: %s [-hv] [-o outputdir] [-t format]  infiles\n", basename(s[0]));
    fprintf(stdout, "  -h    show this help message and exit\n");
    fprintf(stdout, "  -o    Specify the output directory. Default is the\n");
    fprintf(stdout, "        current working directory\n");
    fprintf(stdout, "  -t    Specify the output format. Valid values are:\n");
    fprintf(stdout, "        'beam', 'ast', 'tree', 'lex', 'erl' 'erl2ast'.\n");
    fprintf(stdout, "        Default output type is 'beam'\n");
    fprintf(stdout, "  -v    Turn on verbose mode for compiler\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "All input files should have an extension of .fn\n");
}

static void illegal_option(char c)
{
    fprintf(stderr, "Illegal option: -%c\n", c);
    fprintf(stderr, "  use -h for usage instructions\n");
}

static void parameter_required(char c)
{
    fprintf(stderr, "Option '-%c' requires an argument\n", c);
    fprintf(stderr, "  use -h for usage instructions\n");
}

static int has_input_files(Param param, int count)
{
    if(param.index < count) {
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
    fprintf(stderr, "Specified output type is '%s', expecting a '%s' file, but found '%s'\n", type, suffix, str);
}

static int check_input_files(char** files, int count, Param param)
{
    int i;
    for(i = param.index; i<count; i++) { 
        char* file = files[i];

        if(strncmp(param.output_type, "erl2ast", MAX_STRLEN) == 0) {
            if(!has_suffix(file, ".erl")) {
                illegal_filetype(file, ".erl", param.output_type);
                return 0;
            }
        } else {
            if(!has_suffix(file, ".fn")) {
                illegal_filetype(file, ".fn", param.output_type);
                return 0;
            } 
        }

        if(access(file, R_OK) != 0) {
            fprintf(stderr, "Can't read file '%s'\n", file);
            return 0;
        }
    }

    return 1;
}

Param check_args(int argc, char** argv)
{
    Param param;

    param.flag_help= 0;
    param.flag_verbose = 0;
    param.free_outputdir = 0;
    param.free_outputtype = 0;
    param.output_dir = ".";
    param.output_type = "beam";
    param.check_status = 0;

    while(1) {
        int c = getopt(argc, argv, OPTSTRING);

        switch(c) {
            case 'h': param.flag_help = 1; break;
            case 'v': param.flag_verbose = 1; break;
            case 'o': {
                          param.output_dir = strndup(optarg, MAX_STRLEN);
                          param.free_outputdir = 1;
                          break;
                      }
            case 't': {
                          param.output_type = strndup(optarg, MAX_STRLEN); 
                          param.free_outputtype = 1;
                          break;
                      }
            /* getopt puts a '?' in global char 'optopt'
               when it detects an illegal option char */
            case '?': {
                          illegal_option(optopt);
                          return param;
                      }
            case ':' : {
                           parameter_required(optopt);
                           return param;
                       }
        }

        if(c == -1) {
            break;
        }

    }

    param.index = optind;
    param.check_status = 1;

    return param;
}
