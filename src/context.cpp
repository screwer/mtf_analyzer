

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string>
#include <unistd.h>

#include "config.h"
#include "common.h"
#include "context.h"
#include "options.h"

//--------------------------------------------------------------------

context::context()
    : fd_in(-1)
    , f_out(stdout)
    , addr(nullptr)
    , f_stat(nullptr)
    , size(0)
{
}

//--------------------------------------------------------------------

context::~context()
{
    if (addr) {
        munmap(addr, size);
    }
    if (-1 != fd_in) {
        close(fd_in);
    }

    if (stdout != f_out) {
        fclose(f_out);
    }
    if (log_file) {
        fclose(log_file);
    }    
}

//--------------------------------------------------------------------

bool context::initialize(const options& opt)
{
    fd_in = open(opt.fname_in, O_RDONLY);
    if (fd_in == -1) {
        handle_error("open");
    }

    struct stat sb;
    if (fstat(fd_in, &sb) == -1) {
        handle_error("fstat");
    }

    if (0 == sb.st_size) {
        return false;
    }

    addr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd_in, 0);
    if (MAP_FAILED == addr) {
        handle_error("mmap");
    }

    if (opt.fname_out) {
        f_out = fopen(opt.fname_out, "w");
    }

    if (opt.fname_log) {
        log_file = fopen(opt.fname_log, "w+");
    }

    if (opt.show_statistics) {
        std::string fname;
        if (opt.fname_out) {
			fname = opt.fname_out;
            std::string::size_type pos = fname.rfind('.');
            fname = (std::string::npos != pos)
                ? fname.substr(0, pos) + ".stat" + fname.substr(pos)
                : fname + ".stat";
        } else {
			fname = "statistics.txt";
		}

		//printf("try open file for stat: '%s'\n", fname.c_str());
        f_stat = fopen(fname.c_str(), "w");
    }

    size = sb.st_size;
    curr_filename = opt.fname_in;
    return true;
}

//--------------------------------------------------------------------

statistics* context::get_statistics()
{
    return (f_stat) ? &stat : nullptr;
}

//--------------------------------------------------------------------




//--------------------------------------------------------------------
