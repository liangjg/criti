//
//  main.c
//  criti
//
//  Created by 叶鑫 on 2017/8/24.
//  Copyright © 2017年 xaq. All rights reserved.
//

#include "IO_releated.h"

unsigned warnings = 0;

int main(int argc, char *argv[]){
    /* check command line arguments */
    check_IO_file(argc, argv);

    /* output heading */
    output_heading();

    /* read input file */
//    read_input_blocks();

    /* run calculation */
//    run_calculation();

    /* output ending */
//    output_ending();

    return 0;
}
