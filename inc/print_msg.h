
#pragma once

#include <stdio.h>
#include <stdint.h>
#include "config.h"

//--------------------------------------------------------------------

struct print_msg
{
    char msg_buf[MSG_BUF_SIZE];
    size_t pos;
    FILE* f_out;
    int item_no;
#ifdef DEBUG_LIMIT_RESULTS_MAX
    int dbg_cnt_max;
#endif // DEBUG_LIMIT_RESULTS_MAX

#ifdef DEBUG_SHOW_UNIQUE_MAGICS_ONLY
    int skip_flush;
#endif // DEBUG_SHOW_UNIQUE_MAGICS_ONLY

    print_msg(FILE* f_out);

    bool next_item();

    void flush();

    size_t get_avail() const;

    void append(const char* fmt, ...);

    void append_magic(const void* addr, size_t pos);

    void append_type(uint32_t sz_hdr);

    void align(int n, char c);
};

//--------------------------------------------------------------------

