
#include <string.h>
#include <cstdlib>

#include "options.h"
#include "common.h"

//--------------------------------------------------------------------

options::options()
    : fname_in(nullptr)
    , fname_out(nullptr)
//    , analyze_tags(false)
    , show_statistics(false)
    , skip_target(false)
    , offset_start(0)
{
}

//--------------------------------------------------------------------

void options::print_usage(const char* name)
{
    printf(
        "USAGE: %s [-s] [-i] <input file> [-o <output file>]\n"
        "   -s      show statistics\n"
        "   -st     skip show next block (in each line)\n"
        "   -off    offset to start parsing\n"
        "\n", name
    );
}

//--------------------------------------------------------------------

void options::parse(
    int argc,
    const char* argv[]
)
{
    bool next_fname_in = true;
    bool next_fname_out = false;

    for (int i = 1; i < argc; i++) {
        auto sarg = argv[i];
        if ('-' == sarg[0]) {
            next_fname_in = false;

            if (!strcmp(sarg, "-i")) {
                next_fname_in = true;
            } else if (!strcmp(sarg, "-o")) {
                next_fname_out = true;
            //} else if (!strcmp(sarg, "-t")) {
            //    analyze_tags = true;
            } else if (!strcmp(sarg, "-s")) {
                show_statistics = true;
            } else if (!strcmp(sarg, "-st")) {
                skip_target = true;
            } else if (!strcmp(sarg, "-off")) {
                sarg = argv[++i];
                offset_start = strtoll(sarg, NULL, 0);
                printf("sarg=%s, off=%lx\n", sarg, offset_start);
            } else {
                handle_error("unknown option");
            }
            continue;
        }

        if (next_fname_in) {
            if (fname_in) {
                handle_error("fname_in already set");
            }
            fname_in = sarg;
            next_fname_in = false;
        }

        if (next_fname_out) {
            if (fname_out) {
                handle_error("fname_out already set");
            }
            fname_out = sarg;
            next_fname_out = false;
        }        
    }
}

//--------------------------------------------------------------------
