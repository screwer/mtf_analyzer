
#include "show_progress.h"
#include "config.h"
#include "common.h"

//--------------------------------------------------------------------

show_progress::show_progress(int sz_savepoints)
    : savepoint_idx(0)
    , savepoints_cnt(0)
    , ts_first(0)
    , ts_last_display(0)
    , sz_last_display(0)
{
    savepoints.resize(sz_savepoints);
}

//--------------------------------------------------------------------

void show_progress::update_savepoints(uint64_t tstamp, size_t pos)
{
    savepoint* sp_current = &savepoints[savepoint_idx];
    if (0 != savepoints_cnt) {
        if (tstamp < (sp_current->ts + PGS_SAVEPOINT_INTERVAL_MS)) {
            return;
        }
        savepoint_idx = (savepoint_idx + 1)  % savepoints.size();
        sp_current = &savepoints[savepoint_idx];
    }

    sp_current->ts = tstamp;
    sp_current->pos = pos;
    ++savepoints_cnt;            
}

//--------------------------------------------------------------------

int64_t show_progress::get_speed() const
{
    int cnt = (savepoints_cnt < savepoints.size())
        ? savepoints_cnt
        : savepoints.size();

    if (cnt < 1) {
        return 0;
    }

    auto start_idx = (savepoint_idx + savepoints.size() - cnt + 1) % savepoints.size();
    const savepoint* sp_current = &savepoints[savepoint_idx];
    const savepoint* sp_start = &savepoints[start_idx];

    size_t size = (sp_current->pos - sp_start->pos);
    uint64_t ts = (sp_current->ts - sp_start->ts);

    //printf("cnt=%d, idx=%d:%d, sz=%ld, ts=%ld ", cnt, start_idx, savepoint_idx, size, ts);

    if (0 == ts) {
        return 0;
    }
    return (size * 1000) / ts; // 1000 - convert ms in s.
}

//--------------------------------------------------------------------

void show_progress::notify(int idx, size_t pos, size_t size, int bar_len_max)
{        
    auto tstamp = getts_ms();
    update_savepoints(tstamp, pos);

    if (!ts_first) {
        ts_first = tstamp;
	}

    if ((tstamp - ts_last_display) < PGS_DISPLAY_INTERVAL_MS) {
        return;
    }
    ts_last_display = tstamp;

    int64_t speed = 0;
#ifdef PGS_SHOW_OVERALL_SPEED
    if (ts_first != tstamp) {
        speed = (pos * 1000) / (tstamp - ts_first); // 1000 - convert ms in s.
    }
#else
	speed = get_speed();
#endif // PGS_SHOW_OVERALL_SPEED

	const char* rem_time = "";
    if (speed > 0) {
		int64_t tdiff_sec = (size - pos) / speed;
		rem_time = sprintf_timediff(tdiff_sec);
	}
    
    int pgs = (pos * 100) / size;
    int bar_len = (pos * bar_len_max + 1) / size;
        
    char buf[200];
    char* ptr = buf;

    ptr += sprintf(ptr, "{%d} %d%% [", idx, pgs);
    for (int i = 0; i < bar_len; i++) {
        *ptr++ = '#';
    }
    for (int i = 0; i < (bar_len_max - bar_len); i++) {
        *ptr++ = '-';
    }

    ptr += sprintf(ptr, "] %ld Kb/sec. %s", (speed / 1024), rem_time);
    auto sz = (ptr - buf);
    if (sz < sz_last_display) {
        auto ptr_end = ptr + (sz_last_display - sz);
        while (ptr <= ptr_end) {
            *ptr++ = ' ';
        }
        *ptr = 0;
    }
    sz_last_display = sz;

    printf("%s\r", buf);
}

//--------------------------------------------------------------------
