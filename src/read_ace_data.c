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

extern map *base_nucs;
static char data_path[64];
static char cwd[64];

int
_read_ace(const char *ace_path,
          int file_type,
          int start_addr,
          nuclide_t *nuc);

void
read_ace_data()
{
    char ace_path[64];
    char temp[16];
    char buf[90];
    int file_type, start_addr;

    map_entry *nuc_entry;
    nuclide_t *nuc;
    FILE *xsdir_fp;

    printf("Reading XSDIR/ACE library...");
    xsdir_fp = fopen("xsdir", "r");
    if(!xsdir_fp) {
        puts("Library index file \"xsdir\" does not exist!");
        release_resource();
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

    while(true) {
        fscanf(xsdir_fp, "%s", temp);
        if(strcmp(temp, "directory") == 0) break;
        fgets(buf, 90, xsdir_fp);
        if(feof(xsdir_fp)) {
            puts("keyword 'directory' is not found in xsdir file");
            fclose(xsdir_fp);
            release_resource();
            exit(0);
        }
    }

    while(!feof(xsdir_fp)) {
        fscanf(xsdir_fp, "%s", temp);
        nuc_entry = map_find(base_nucs, (uint64_t) temp);
        if(nuc_entry) {
            nuc = (nuclide_t *) nuc_entry->v.val;
            fscanf(xsdir_fp, "%lf %s %*d %d %d %d %*d %*d %*lf",
                   &nuc->atom_wgt, ace_path, &file_type, &start_addr, &nuc->XSS_sz);
            switch(_read_ace(ace_path, file_type, start_addr, nuc)) {
                case FILE_NOT_EXIST:printf("file %s does not exist in directory %s.\n", ace_path, data_path);
                    fclose(xsdir_fp);
                    release_resource();
                    exit(0);
                case FILE_TYPE_ERR:printf("wrong ACE file type in xsdir, nuclide: %s.\n", nuc->id);
                    fclose(xsdir_fp);
                    release_resource();
                    exit(0);
                default:break;
            }
        }
        fgets(buf, 90, xsdir_fp);
    }

    fclose(xsdir_fp);
    puts("Finished.");

#if defined(OS_LINUX) || defined(OS_MAC)
    chdir(cwd);
#elif defined(OS_WIN32)
    SetCurrentDiretory(cwd);
#endif
}

int
_read_ace(const char *ace_path,
          int file_type,
          int start_addr,
          nuclide_t *nuc)
{
    FILE *ace_fp;
    char *buf;
    int i;

    buf = (char *) malloc(1UL << 22);    /* 4M bytes buffer */
    ace_fp = fopen(ace_path, "rb");

    if(!ace_fp) return FILE_NOT_EXIST;

    /* decimal type ACE file */
    if(file_type == 1) {
        for(i = 1; i < start_addr; i++)
            fgets(buf, CHAR_PER_LINE, ace_fp);

        fgets(buf, CHAR_PER_LINE, ace_fp);
        sscanf(buf, "%*s %lf %lf", &nuc->atom_wgt, &nuc->tmp);

        if(ISNUMBER(*nuc->id))    /* CE ACE data */
            fseek(ace_fp, 80 + 72 * 4 + 5, SEEK_CUR);
        else {                     /* SAB ACE data */
            fread(buf, sizeof(char), 80 + 72 * 4 + 5, ace_fp);
            nuc->zaid = (int) strtol(buf + 80 + 1, NULL, 10);
        }

        /* read NXS array */
        for(i = 1; i <= 16; i++)
            fscanf(ace_fp, "%d", nuc->NXS + i);

        /* read JXS array */
        for(i = 1; i <= 32; i++)
            fscanf(ace_fp, "%d", nuc->JXS + i);

        /* get ZAID if CE ACE data */
        if(ISNUMBER(*nuc->id))
            nuc->zaid = nuc->NXS[2];

        /* start read XSS array */
        getc(ace_fp);
        nuc->XSS = (double *) malloc((nuc->XSS_sz + 1) * sizeof(double));
        for(i = 1; i <= nuc->XSS_sz; i++)
            fscanf(ace_fp, "%lf", &nuc->XSS[i]);
        fgets(buf, CHAR_PER_LINE, ace_fp);
    } else if(file_type == 2) {
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

        if(ISNUMBER(*nuc->id)) {
            for(i = 1; i <= 16; i++) {
                fread(&IZ, sizeof(int), 1, ace_fp);
                fread(&temp, sizeof(double), 1, ace_fp);
            }
        } else {
            fread(&nuc->zaid, sizeof(int), 1, ace_fp);
            fread(&temp, sizeof(double), 1, ace_fp);
            for(i = 1; i <= 15; i++) {
                fread(&IZ, sizeof(int), 1, ace_fp);
                fread(&temp, sizeof(double), 1, ace_fp);
            }
        }

        fread(nuc->NXS + 1, sizeof(int), 16, ace_fp);
        fread(nuc->JXS + 1, sizeof(int), 32, ace_fp);

        if(ISNUMBER(*nuc->id))
            nuc->zaid = nuc->NXS[2];

        fseek(ace_fp, start_addr * 4096, SEEK_SET);
        nuc->XSS = (double *) malloc((nuc->XSS_sz + 1) * sizeof(double));
        fread(nuc->XSS + 1, sizeof(double), nuc->XSS_sz, ace_fp);
    } else {
        fclose(ace_fp);
        return FILE_TYPE_ERR;
    }

    fclose(ace_fp);
    free(buf);

    return 0;
}