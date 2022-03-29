
#include "common.h"

#include <stdlib.h>
#include <sys/time.h>

//--------------------------------------------------------------------

FILE* log_file               = nullptr;
const char* curr_filename    = nullptr;

//--------------------------------------------------------------------

void handle_error(const char* msg)
{
    perror(msg);
    if (log_file) {
        fprintf(log_file, "'%s': %s\n", curr_filename, msg);    
    }
    exit(EXIT_FAILURE);    
}

//--------------------------------------------------------------------

uint64_t getts_ms()
{
    timeval tp;
    gettimeofday(&tp, 0);
    long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    return ms;
}

//--------------------------------------------------------------------

const char* sprintf_timediff(uint64_t tdiff_sec)
{
    static char buf[100];
    char* ptr(buf);

    int64_t hh = (tdiff_sec / 3600);
    if (hh > 0) {
        ptr += sprintf(ptr, "%02ldh:", hh);
        tdiff_sec -= (hh * 3600);
    }
    
    int64_t mm = (tdiff_sec / 60);
    if (mm > 0) {
        ptr += sprintf(ptr, "%02ldm:", mm);
        tdiff_sec -= (mm * 60);
    }

    sprintf(ptr, "%02lds", tdiff_sec);
	return buf;
}

//--------------------------------------------------------------------

bool is_exist(const uint32_t* pu32, int cnt, uint32_t val)
{
    for (int n = 0; n < cnt; ++n) {
        if (pu32[n] == val) {
            return true;
        }
    }
    return false;
}

//--------------------------------------------------------------------
