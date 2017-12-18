//
// Created by xaq on 11/9/17.
//

#include "IO_releated.h"
#include "map.h"
#include "nuclide.h"

#if defined(OS_LINUX) || defined(OS_MAC)
    #include <unistd.h>
#elif defined(OS_WIN32)
    #include <windows.h>
#endif

#define MAX_LINES         51781
#define CHAR_PER_LINE     81

#define FILE_NOT_EXIST    1
#define FILE_TYPE_ERR     2

//extern acedata_t base_acedata;

extern map *base_nucs;
static char data_path[64];
static char cwd[64];

int _read_ace(const char *ace_path, int file_type, int start_addr, nuclide_t *nuc);

void read_ace_data(){
    char ace_path[64];
    char temp[16];
    char buf[90];
    int file_type, start_addr;

    map_entry *nuc_entry;
    nuclide_t *nuc;
    FILE *xsdir_fp;

    xsdir_fp = fopen("xsdir", "r");
    if(!xsdir_fp){
        puts("Library index file \"xsdir\" does not exist!");
        exit(0);
    }

    fscanf(xsdir_fp, "%*[^=]=%s", data_path);

#if defined(OS_LINUX) || defined(OS_MAC)
    getcwd(cwd, 64);
    chdir(data_path);
#elif defined(OS_WIN32)
    GetCurrentDirectory(64, cwd);
    SetCurrentDirectory(data_path);
#endif

    while(true){
        fscanf(xsdir_fp, "%s", temp);
        if(strcmp(temp, "directory") == 0) break;
        fgets(buf, 90, xsdir_fp);
        if(feof(xsdir_fp)){
            puts("keyword 'directory' is not found in xsdir file");
            exit(0);
        }
    }

    while(!feof(xsdir_fp)){
        fscanf(xsdir_fp, "%s", temp);
        nuc_entry = map_find(base_nucs, (uint64_t)temp);
        if(nuc_entry){
            nuc = (nuclide_t *)nuc_entry->v.val;
            fscanf(xsdir_fp, "%lf %s %*d %d %d %d %*d %*d %lf",
                   &nuc->atom_wgt, ace_path, &file_type, &start_addr, &nuc->XSS_sz, &nuc->tmp);
            switch(_read_ace(ace_path, file_type, start_addr, nuc)) {
                case FILE_NOT_EXIST:
                    printf("file %s does not exist in directory %s.\n", ace_path, data_path);
                    exit(0);
                case FILE_TYPE_ERR:
                    printf("wrong ACE file type in xsdir, nuclide: %s.\n", nuc->id);
                    exit(0);
                default:
                    break;
            }
        }
        fgets(buf, 90, xsdir_fp);
    }

    fclose(xsdir_fp);

#if defined(OS_LINUX) || defined(OS_MAC)
    chdir(cwd);
#elif defined(OS_WIN32)
    SetCurrentDiretory(cwd);
#endif
}

int _read_ace(const char *ace_path, int file_type, int start_addr, nuclide_t *nuc){
    FILE *ace_fp;
    char *buf;

    buf = new char[1UL << 22];    /* 4M bytes buffer */
    ace_fp = fopen(ace_path, "rb");

    if(!ace_fp) return FILE_NOT_EXIST;

    /* decimal type ACE file */
    if(file_type == 1){
        for(int i = 1; i < start_addr; i++)
            fgets(buf, CHAR_PER_LINE, ace_fp);

        if(ISNUMBER(*nuc->id))    /* CE ACE data */
            fseek(ace_fp, 45 + 80 + 72 * 4 + 6, SEEK_CUR);
        else{                     /* SAB ACE data */
            fread(buf, sizeof(char), 45 + 80 + 72 * 4 + 6, ace_fp);
            nuc->zaid = (int)strtol(buf + 45 + 80 + 2, nullptr, 10);
        }

        /* read NXS array */
        for(int i = 1; i <= 16; i++)
            fscanf(ace_fp, "%d", nuc->NXS + i);

        /* read JXS array */
        for(int i = 1; i <= 32; i++)
            fscanf(ace_fp, "%d", nuc->JXS + i);

        /* get ZAID if CE ACE data */
        if(ISNUMBER(*nuc->id))
            nuc->zaid = nuc->NXS[2];

        /* start read XSS array */
        getc(ace_fp);
        nuc->XSS = (double *) malloc((nuc->XSS_sz + 1) * sizeof(double));
        for(int i = 1; i <= nuc->XSS_sz; i++)
            fscanf(ace_fp, "%lf", &nuc->XSS[i]);
        fgets(buf, CHAR_PER_LINE, ace_fp);
//        int tot_lines = nuc->XSS_sz / 4 + 1;
//        int k = tot_lines / MAX_LINES;
//        char *start, *end;
//        int j = 1;
//        do{
//            int xss_to_read;
//            k-- == 0 ? xss_to_read = ((tot_lines % MAX_LINES) - 1) * 4 + nuc->XSS_sz % 4
//                     : xss_to_read = MAX_LINES * 4;
//            fread(buf, sizeof(char), 1 << 22, ace_fp);
//            start = buf;
//            for(int i = 1; i <= xss_to_read; i++){
//                nuc->XSS[j++] = strtod(start, &end);
//                start = end;
//            }
//        } while(k > -1);
    }
    else if(file_type == 2){
        char HZ[10], HD[10], HK[70], HM[10];
        int IZ;
        double temp;
        fseek(ace_fp, (start_addr - 1) * 4096, SEEK_SET);

        fread(HZ, sizeof(char), 10, ace_fp);
        fread(&nuc->atom_wgt, sizeof(double), 1, ace_fp);
        fread(&nuc->tmp, sizeof(double), 1, ace_fp);
        fread(HD, 10, 1, ace_fp);
        fread(HK, 70, 1, ace_fp);
        fread(HM, 10, 1, ace_fp);

        if(ISNUMBER(*nuc->id)){
            for(int i = 1; i <= 16; i++){
                fread(&IZ, sizeof(int), 1, ace_fp);
                fread(&temp, sizeof(double), 1, ace_fp);
            }
        }
        else{
            fread(&nuc->zaid, sizeof(int), 1, ace_fp);
            fread(&temp, sizeof(double), 1, ace_fp);
            for(int i = 1; i <= 15; i++){
                fread(&IZ, sizeof(int), 1, ace_fp);
                fread(&temp, sizeof(double), 1, ace_fp);
            }
        }

        fread(nuc->NXS + 1, sizeof(int), 16, ace_fp);
        fread(nuc->JXS + 1, sizeof(int), 32, ace_fp);

        if(ISNUMBER(*nuc->id))
            nuc->zaid = nuc->NXS[2];

        fseek(ace_fp, start_addr * 4096, SEEK_SET);
        nuc->XSS = new double[nuc->XSS_sz + 1];
        fread(nuc->XSS + 1, sizeof(double), nuc->XSS_sz, ace_fp);
    }
    else return FILE_TYPE_ERR;

    fclose(ace_fp);
    delete[]buf;

    return 0;
}