#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define OPTSTRING "+ho:t:v"
#define MAX_STRLEN 255

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
            printf("help is on\n");
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
