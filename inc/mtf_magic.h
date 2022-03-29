
#pragma once

#include <stdint.h>

//--------------------------------------------------------------------

#define MTF_MAGIC_SPECIAL_VALUE 0x24

//--------------------------------------------------------------------

bool might_be_magic(uint32_t u32);

//--------------------------------------------------------------------

bool well_known_dblk(uint32_t u32);
bool well_known_strm(uint32_t u32);
bool well_known_magic(uint32_t u32);
bool well_known_magic_spec(uint32_t u32);

//--------------------------------------------------------------------