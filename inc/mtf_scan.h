
#pragma once

#include <cstddef>
#include <stdint.h>

//--------------------------------------------------------------------

class mtf_scan
{
    const void* addr;
    size_t size;
    friend struct possible_offsets;

public:

    mtf_scan(const void* addr, size_t size);

    uint32_t checksum(size_t pos);

    int64_t forward(size_t pos, uint32_t* psz_hdr);
};

//--------------------------------------------------------------------
