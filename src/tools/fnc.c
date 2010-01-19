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

    char* output_dir;
    char* output_type;
} Param;


int opterr = 0;

static void usage(char**);
static void illegal_option(char);
static void illegal_filetype(char*, char*, char*);
static void parameter_required(char);
static int has_input_files(int,int);
static int has_suffix(char*, char*);
static int check_input_files(char**, int, int, char*);
static Param check_args(int, char**);


int main(int argc, char** argv)
{

    int ret_status = -1;

    Param param = check_args(argc, argv);
    if(!param.check_status) {
        goto exit;
    }

    if(flag_help) {
        usage(argv);
        goto exit;
    }

    if(!has_input_files(optind, argc)) {
        fprintf(stderr, "No input files\n");
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
    fprintf(stderr, "Specified output type is '%s', expecting a '%s' file, but found '%s'\n", type, suffix, str);
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
            fprintf(stderr, "Can't read file '%s'\n", file);
            return 0;
        }
    }

    return 1;
}
