
#pragma once

//--------------------------------------------------------------------

#include <memory>

class statistics
{
    class impl;
    std::unique_ptr<impl> pimpl;

public:
    statistics();
    ~statistics();

    void account(
        const void* addr,
        uint64_t    pos,
        uint64_t    sz_item,
        uint32_t    sz_hdr
    );

    void show(FILE* f) const;
};

//--------------------------------------------------------------------