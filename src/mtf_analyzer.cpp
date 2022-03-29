
#include <stdio.h>

#include <string.h> // basename

#include "config.h"
#include "common.h"
#include "show_progress.h"
#include "print_msg.h"
#include "context.h"

#include "mtf_scan.h"

#include "options.h"

//--------------------------------------------------------------------

bool mtf_explore(
    const void* addr,
    size_t size,
    FILE* f_out,
    bool skip_target,
    statistics* pstat
    )
{
    dprint("addr: %p, size: %lx\n", addr, size);
    mtf_scan scan(addr, size);

    show_progress pgs(30);
    print_msg msg(f_out);    
    size_t pos = 0;
    while (pos < size) {

        msg.append("{%05d} ", msg.item_no);
        msg.append("[0x%012lx] ", pos);
        msg.append_magic(addr, pos);

        uint32_t sz_hdr;
        auto off = scan.forward(pos, &sz_hdr);
        if (-1 == off) {
            //handle_error("bad next");
            break;
        }
        if (pstat) {
            pstat->account(addr, pos, off, sz_hdr);
        }
        pgs.notify(msg.item_no, pos, size, 30);

        msg.append_type(sz_hdr);
        if (!skip_target) {

            msg.append("-(%lx)", off);
            msg.align(16, '-');
            msg.append("-> ");

            pos += off;

            if (pos > size) {
                msg.append("overflow");
            } else if (pos == size) {
                msg.append("END");
#ifdef DEBUG_SHOW_UNIQUE_MAGICS_ONLY
                ++msg.skip_flush;
#endif // DEBUG_SHOW_UNIQUE_MAGICS_ONLY
            } else {
                msg.append("0x%lx ", pos);
                msg.append_magic(addr, pos);
            }
        }
        msg.flush();
        
        if (msg.next_item()) {
            break;
        }
    }

    if (size == pos) {
        msg.append("OK!");
    } else {
        msg.append("!!!ERROR!!!");
    }
    msg.flush();

    return (size == pos);
}

//--------------------------------------------------------------------
/*
void mtf_analyze_tags(
    const void* addr,
    size_t size,
    FILE* f_out
    )
{
    auto ptr = reinterpret_cast<const uint32_t*>(addr);
    size_t cnt = size / sizeof(uint32_t);
    for (size_t idx = 0; idx < cnt; ++idx, ++ptr) {
        if (might_be_magic(*ptr)) {
            fprintf(f_out, "[0x%lx] 0x%08x '%.4s\n", idx * sizeof(uint32_t), *ptr, reinterpret_cast<const char*>(ptr));
        }
    }
}
*/
//--------------------------------------------------------------------

bool mtf_analyzer(
    const options& opt
    )
{
    context ctx;
    if (!ctx.initialize(opt)) {
        return true;
    }

    auto pstat = ctx.get_statistics();    
    bool result(true);
    result = mtf_explore(
        offset_to_pointer(ctx.get_data(), opt.offset_start),
        (ctx.get_size() - opt.offset_start),
        ctx.get_fout(),
        opt.skip_target,
        pstat
    );

    if (pstat) {
        pstat->show(ctx.get_fstat());
    }

    return result;
}

//--------------------------------------------------------------------

int main(int argc, const char* argv[])
{
    options opt;
    opt.parse(argc, argv);

    if (!opt.fname_in) {
        const char* name = basename(argv[0]);
        options::print_usage(name);
        return -1;
    }

    bool result = mtf_analyzer(opt);
    return (result) ? 0 : -1;
}

//--------------------------------------------------------------------
