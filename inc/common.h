
#pragma once

#include <cstddef>
#include <stdint.h>
#include <stdio.h>

//--------------------------------------------------------------------

extern FILE* log_file;
extern const char* curr_filename;

void handle_error(const char* msg);

//--------------------------------------------------------------------

#define offset_to_pointer(p, o) (((char*)(p)) + (o))
#define ptr_diff(p1, p2)        (((char*)(p1)) - ((char*)(p2)))

#define _mask_pw2(n)        ((n) - 1)
#define _mask_pw2i(n)       (~_mask_pw2(n))

#define round_up(x, n)      (((x) + (n) - 1) & _mask_pw2i(n))
#define round_down(x, n)    ( (x)            & _mask_pw2i(n))

//--------------------------------------------------------------------

template <typename T, std::size_t N>
constexpr std::size_t countof(T const (&)[N]) noexcept
{
    return N;
}

//--------------------------------------------------------------------

#ifdef DEBUG
    #define dprint(...) printf(__VA_ARGS__)
#else
    #define dprint(...)
#endif // DEBUG

//--------------------------------------------------------------------

uint64_t getts_ms();
const char* sprintf_timediff(uint64_t tdiff_sec);

//--------------------------------------------------------------------

inline bool is_up_al(unsigned char u)
{
    return ('A' <= u ) && (u <= 'Z');
}

inline bool is_up_alnum(unsigned char u)
{
    return (('0' <= u ) && (u <= '9')) || is_up_al(u);
}

//--------------------------------------------------------------------

bool is_exist(const uint32_t* pu32, int cnt, uint32_t val);

//--------------------------------------------------------------------
