
#pragma once

#include <stdio.h>

#include "statistics.h"

//--------------------------------------------------------------------

class options;

class context
{
    int fd_in;
    FILE* f_out;

    void* addr;
    size_t size;

    FILE* f_stat;
    statistics stat;

public:
    context();
    ~context();

    bool initialize(const options& opt);

    const void* get_data() const {
        return addr;
    }

    size_t get_size() const {
        return size;
    }

    statistics* get_statistics();

    FILE* get_fout() {
        return f_out;
    }

    FILE* get_fstat() {
        return f_stat;
    }
};

//--------------------------------------------------------------------
