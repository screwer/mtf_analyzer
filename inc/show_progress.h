
#pragma once

//--------------------------------------------------------------------

#include <stdint.h>
#include <vector>

//--------------------------------------------------------------------

class show_progress
{
    struct savepoint {
        size_t pos;
        uint64_t ts;        
    };
    std::vector<savepoint> savepoints;
    int savepoint_idx;
    int savepoints_cnt;
    uint64_t ts_first;
    uint64_t ts_last_display;
    int sz_last_display;

public:
    show_progress(int sz_savepoints);

    void update_savepoints(uint64_t tstamp, size_t pos);

    int64_t get_speed() const;

    void notify(int idx, size_t pos, size_t size, int bar_len_max);
};

//--------------------------------------------------------------------
