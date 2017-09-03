//
// Created by xaq on 9/2/17.
//

#include "IO_releated.h"

IOfp *base_IOfp = new IOfp;

void check_IO_file(int argc, char *argv[]){
    char mat_fn[MAX_FILENAME_LENGTH];
    char tally_fn[MAX_FILENAME_LENGTH];

    /* important: initial allocation */
    if(argc >= 3){
        if(strlen(argv[1]) < MAX_FILENAME_LENGTH && strlen(argv[2]) < MAX_FILENAME_LENGTH){
            strcpy(base_IOfp->inp_file_name, argv[1]);
            strcpy(base_IOfp->opt_file_name, argv[2]);
        }
        else{
            puts("filename too long.");
            exit(0);
        }

    }
    else if(argc == 2){
        if(strlen(argv[1]) < MAX_FILENAME_LENGTH)
            strcpy(base_IOfp->inp_file_name, argv[1]);
        else{
            puts("filename too long.");
            exit(0);
        }
        strcpy(base_IOfp->opt_file_name, base_IOfp->inp_file_name);
        strcat(base_IOfp->opt_file_name, ".out");
    }
    else if(argc == 1){
        strcpy(base_IOfp->inp_file_name, "inp");
        strcpy(base_IOfp->opt_file_name, "inp.out");
    }

    strcpy(mat_fn, base_IOfp->inp_file_name);
    strcat(mat_fn, ".mat");
    strcpy(tally_fn, base_IOfp->inp_file_name);
    strcat(tally_fn, ".tally");

    base_IOfp->inp_fp = fopen(base_IOfp->inp_file_name, "rb");
    if(!base_IOfp->inp_fp){
        printf("%s does not exist.\n", base_IOfp->inp_file_name);
        exit(0);
    }

    base_IOfp->opt_fp = fopen(base_IOfp->opt_file_name, "w");
    base_IOfp->mat_fp = fopen(mat_fn, "w");
    base_IOfp->tally_fp = fopen(tally_fn, "w");
}