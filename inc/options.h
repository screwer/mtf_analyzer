
#pragma once

#include <stdint.h>

//--------------------------------------------------------------------

struct options
{
    const char* fname_in;
    const char* fname_out;
    const char* fname_log;
    const char* fname_stat;

    //bool analyze_tags;
    bool show_statistics;
    bool skip_target;

    int64_t offset_start;

    options();

    static void print_usage(const char* name);

    void parse(int argc, const char* argv[]);
};

//--------------------------------------------------------------------

