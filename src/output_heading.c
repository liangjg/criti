//
// Created by xaq on 9/2/17.
//

#include "IO_releated.h"

#ifdef USE_MPI
#include "parallel.h"

extern parallel_t base_parallel;
#endif

struct timeval start_time;
struct timeval finish_time;
extern IOfp_t base_IOfp;
extern int base_num_threads;

void
output_heading()
{
    gettimeofday(&start_time, NULL);
    char start_wall_clock_str[64];
    time_t start_wall_clock = time(NULL);
    strftime(start_wall_clock_str, sizeof(start_wall_clock_str), "%Y/%m/%d %X %A", localtime(&start_wall_clock));

    ////////////////////////// Write RMC LOGO///////////////////////
    puts("Copyrights Reserved by Reactor Engineering Analysis Laboratory(REAL).\n");
    puts("      RRRRRRRR       MM         MM        CCCCCCCC   ");
    puts("      RRRRRRRRR      MMM       MMM      CCCCCCCCC    ");
    puts("      RRR    RRR     MMMM     MMMM     CCC           ");
    puts("      RRR    RR      MMMMM   MMMMM    CCC            ");
    puts("      RRRRRRRR       MM MMM MMM MM    CCC            ");
    puts("      RRR   RRR      MM  MMMMM  MM    CCC            ");
    puts("      RRR    RRR     MM   MMM   MM     CCCCCCCCCC    ");
    puts("      RRR     RRR   MMM         MMM      CCCCCCCCC   ");
#ifdef CODE_VERSION
    printf("\nCode version    : %s\n", CODE_VERSION);
#else
    printf("\nCode version    : Unknown version\n");
#endif
    printf("Build time      : %s %s\n", __TIME__, __DATE__);
#ifdef GIT_SHA1
    printf("Git commit      : %s\n", GIT_SHA1);
#else
    printf("Git commit      : Unknown GIT_SHA1\n");
#endif
#ifdef USE_PTHREAD
    printf("Pthread parallel: ON, %d pthreads\n", base_num_threads);
#else
    printf("Pthread parallel: OFF\n");
#endif
#ifdef USE_MPI
    printf("MPI parallel    : ON, %d processes\n\n", base_parallel.tot_procs);
#else
    printf("MPI parallel    : OFF\n\n");
#endif

    fputs("Copyrights Reserved by Reactor Engineering Analysis Laboratory(REAL).\n\n", base_IOfp.opt_fp);
    fputs("      RRRRRRRR       MM         MM        CCCCCCCC   \n", base_IOfp.opt_fp);
    fputs("      RRRRRRRRR      MMM       MMM      CCCCCCCCC    \n", base_IOfp.opt_fp);
    fputs("      RRR    RRR     MMMM     MMMM     CCC           \n", base_IOfp.opt_fp);
    fputs("      RRR    RR      MMMMM   MMMMM    CCC            \n", base_IOfp.opt_fp);
    fputs("      RRRRRRRR       MM MMM MMM MM    CCC            \n", base_IOfp.opt_fp);
    fputs("      RRR   RRR      MM  MMMMM  MM    CCC            \n", base_IOfp.opt_fp);
    fputs("      RRR    RRR     MM   MMM   MM     CCCCCCCCCC    \n", base_IOfp.opt_fp);
    fputs("      RRR     RRR   MMM         MMM      CCCCCCCCC   \n", base_IOfp.opt_fp);
#ifdef CODE_VERSION
    fprintf(base_IOfp.opt_fp, "\nCode version    : %s\n", CODE_VERSION);
#else
    fprintf(base_IOfp.opt_fp, "\nCode version    : Unknown version\n");
#endif
    fprintf(base_IOfp.opt_fp, "Build time      : %s %s\n", __TIME__, __DATE__);
#ifdef GIT_SHA1
    fprintf(base_IOfp.opt_fp, "Git commit      : %s\n", GIT_SHA1);
#else
    fprintf(base_IOfp.opt_fp, "Git commit      : Unknown GIT_SHA1\n");
#endif
#ifdef USE_PTHREAD
    fprintf(base_IOfp.opt_fp, "Pthread parallel: ON, %d pthreads\n", base_num_threads);
#else
    fprintf(base_IOfp.opt_fp, "Pthread parallel: OFF\n");
#endif
#ifdef USE_MPI
    fprintf(base_IOfp.opt_fp, "MPI parallel    : ON, %d processes\n\n", base_parallel.tot_procs);
#else
    fprintf(base_IOfp.opt_fp, "MPI parallel    : OFF\n\n");
#endif

    printf("RMC Calculation Start.\nInput File = %s     %s \n\n", base_IOfp.inp_file_name, start_wall_clock_str);
    fprintf(base_IOfp.opt_fp, "RMC Calculation Start.\nInput File = %s     %s \n\n", base_IOfp.inp_file_name, start_wall_clock_str);

    fputs("------------------ Input File Start --------------------\n", base_IOfp.opt_fp);
    char buf[200];
    FILE *inp = fopen(base_IOfp.inp_file_name, "rb");
    while(!feof(inp)) {
        if(fgets(buf, 200, inp))
            fputs(buf, base_IOfp.opt_fp);
    }
    fputs("\n---------------------- Input File End------------------------\n", base_IOfp.opt_fp);
    fclose(inp);
}
