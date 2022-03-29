
#include <stdarg.h>

#include "config.h"
#include "common.h"
#include "print_msg.h"
#include "mtf_magic.h"

//--------------------------------------------------------------------

print_msg::print_msg(FILE* f_out)
    : pos(0),
    f_out(f_out),
    item_no(0)
{
#ifdef DEBUG_LIMIT_RESULTS_MAX
    dbg_cnt_max = DEBUG_LIMIT_RESULTS_MAX;
#endif // DEBUG_LIMIT_RESULTS_MAX

#ifdef DEBUG_SHOW_UNIQUE_MAGICS_ONLY
    skip_flush = 0;
#endif // DEBUG_SHOW_UNIQUE_MAGICS_ONLY
}

//--------------------------------------------------------------------

bool print_msg::next_item()
{
    ++item_no;
#ifdef DEBUG_LIMIT_RESULTS_MAX
    return (item_no >= dbg_cnt_max);
#else
    return false;
#endif  // DEBUG_LIMIT_RESULTS_MAX
}

//--------------------------------------------------------------------

void print_msg::flush()
{
#ifdef DEBUG_SHOW_UNIQUE_MAGICS_ONLY
    if (2 == skip_flush) {
        skip_flush = 0;
        pos = 0;
        return;
    }
    skip_flush = 0;
#endif // DEBUG_SHOW_UNIQUE_MAGICS_ONLY

    fprintf(f_out, "%s\n", msg_buf);
    pos = 0;
}

//--------------------------------------------------------------------

size_t print_msg::get_avail() const
{
    return (sizeof(msg_buf) - pos - 1);
}

//--------------------------------------------------------------------

void print_msg::append(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int result = vsnprintf(&msg_buf[pos], get_avail(), fmt, args);
    if (result < 0) {
        handle_error(fmt);
    }
    pos += result;       
    va_end(args);
}

//--------------------------------------------------------------------

void print_msg::append_magic(const void* addr, size_t pos)
{
    auto pmagic = reinterpret_cast<const uint32_t*>(offset_to_pointer(addr, pos));

    if (MTF_MAGIC_SPECIAL_VALUE == *pmagic) {
        append("$");
        ++pmagic;
    }
    append("%08x '%.4s'", *pmagic, reinterpret_cast<const char*>(pmagic));

#ifdef DEBUG_SHOW_UNIQUE_MAGICS_ONLY
    if (well_known_magic(*pmagic) || well_known_magic_spec(*pmagic)) {
        ++skip_flush;
    }
#endif // DEBUG_SHOW_UNIQUE_MAGICS_ONLY
}

//--------------------------------------------------------------------

void print_msg::append_type(uint32_t sz_hdr)
{
    const char* name = "????";
    switch (sz_hdr) {
    case 0x34:
        name = "DBLK";
        break;
        
    case 0x16:
        name = "STRM";
        break;
    }
    append("<%x> %s ", sz_hdr, name);
}

//--------------------------------------------------------------------

void print_msg::align(int n, char c)
{
    int pos_next = round_up(pos, n);
    if (pos_next >= sizeof(msg_buf)) {
        handle_error("msg_buf overflow");
    }

    for (;pos < pos_next; ++pos) {
        msg_buf[pos] = c;
    }
    msg_buf[pos] = 0;
}

//--------------------------------------------------------------------
