#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define OPTSTRING "+ho:t:v"
#define MAX_STRLEN 255

static void usage(char** s)
{
    printf("Usage: %s [-hv] [-o outputdir] [-t format]  \n", basename(s[0]));
    printf("  -h    show this help message and exit\n");
    printf("  -o    Specify the output directory. Default is the\n");
    printf("        current working directory\n");
    printf("  -t    Specify the output format. Valid values are:\n");
    printf("        'beam', 'ast', 'tree', 'lex', 'erl'. Default\n");
    printf("        output type is 'beam'\n");
    printf("  -v    Turn on verbose mode for compiler\n");
}

int main(int argc, char** argv)
{
    int flag_help= 0;
    int flag_verbose = 0;
    char* output_dir = NULL;
    char* output_type = NULL;

    int ret_status = 0;

    while(1) {
        int c = getopt(argc, argv, OPTSTRING);

        switch(c) {
            case 'h': flag_help = 1; break;
            case 'v': flag_verbose = 1; break;
            case 'o': output_dir = strndup(optarg, MAX_STRLEN); break;
            case 't': output_type = strndup(optarg, MAX_STRLEN); break;
        }

        if(c == -1) {
            break;
        }

    }

    if(flag_help) {
        usage(argv);
        goto exit;
    }

        if(flag_verbose) {
            printf("verbose is on\n");
        }

        if(output_dir) {
            printf("output_dir = %s\n", output_dir);
        }

        if(output_type) {
            printf("output_type = %s\n", output_type);
        }



exit:
    if(output_dir) { 
        free(output_dir);
    }

    if(output_type) { 
        free(output_type);
    }

    return ret_status;
}
