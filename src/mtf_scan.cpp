
#include <vector>
#include <algorithm>

#include "mtf_scan.h"
#include "config.h"
#include "common.h"

#include "mtf_magic.h"

//--------------------------------------------------------------------

struct possible_offset
{
    uint32_t magic1;
    uint32_t magic2;
    int64_t offset;
    int field_index;
    int heu_points;
    int sz_hdr;

    possible_offset(int64_t offset = -1, int offset_index = 0);

    bool is_to_end() const {
        return (0 == magic1) && (0 == magic2) && (offset >= 0);
    }

    bool operator<(const possible_offset& rhs) const {
        if (heu_points != rhs.heu_points) {
            return heu_points < rhs.heu_points;
        }
        return offset > rhs.offset; // less offset is better
    }
};

//--------------------------------------------------------------------

possible_offset::possible_offset(
    int64_t offset, /*= -1 */
    int offset_index /*= 0 */
    )
    : magic1(0)
    , magic2(0)
    , offset(offset)
    , field_index(field_index)
    , heu_points(0)
{
}

//--------------------------------------------------------------------

class possible_offsets : public std::vector<possible_offset>
{
    mtf_scan& scan;
    size_t from;
    size_t to;
    uint32_t sz_hdr;

    bool try_add_impl(int64_t offset, int field_index);

    void try_add(
        const char* prefix,
        size_t pos,
        size_t pos_next,
        uint64_t sz,
        uint32_t sz_add
    );

public:
    possible_offsets(
        mtf_scan& scan,
        size_t from,
        size_t to,
        uint32_t sz_hdr
    );

    void assign_heu_points(size_t pos);

    void lookup_offset16(
        uint32_t sz_add,
        uint32_t off_skip
    );

    void lookup_offset64(
        uint32_t sz_add,
        uint32_t off_skip
    );
};


//--------------------------------------------------------------------

possible_offsets::possible_offsets(
    mtf_scan& scan,
    size_t from,
    size_t to,
    uint32_t sz_hdr
    )
    : scan(scan)
    , from(from)
    , to(to)
    , sz_hdr(sz_hdr)
{
    if ((from >= to) || (from >= scan.size)) {
        handle_error("(16) bad region");
    }

    auto avail = (scan.size - from);
    if (avail < sz_hdr) {
        handle_error("(16) no data");
    }
}

//--------------------------------------------------------------------

void possible_offsets::assign_heu_points(size_t pos)
{
    for (auto& off : *this) {
        //printf("%lx {%x/%x}\n", off.offset, off.magic1, off.magic2);
        if (off.is_to_end()) {
            off.heu_points += HEU_PTS_EXACTLY_EOF;
            //printf("HEU_PTS_EXACTLY_EOF\n");
            continue;
        }

        int next_hdr_align = ((pos + off.offset) & 3);
        if (0 == next_hdr_align) {
            //
            // whole block size is aligned to dword boundaries
            //
            off.heu_points += HEU_PTS_NEXT_HDR_DWORD_ALIGNED;
            //printf("HEU_PTS_NEXT_HDR_DWORD_ALIGNED\n");

        } else if (2 != next_hdr_align) {
            off.heu_points += HEU_PTS_NEXT_HDR_MISALIGNED;
            //printf("HEU_PTS_NEXT_HDR_MISALIGNED\n");
        }

        uint32_t off_chksum = scan.checksum(pos + off.offset);
        //printf("off_chksum=%x\n", off_chksum);
        if (-1 == off_chksum) {
            off.heu_points += HEU_PTS_NO_HDR_CHKSUM;
            //printf("HEU_PTS_NO_HDR_CHKSUM\n");
            continue;
        }
        uint32_t sz_hdr(off_chksum + sizeof(uint16_t));
        off.sz_hdr = sz_hdr;

        if (8 == off.field_index) {
            //
            // typical index value
            //
            off.heu_points += HEU_PTS_HDR_SIZE_AT_TYPICAL_INDEX;
            //printf("HEU_PTS_HDR_SIZE_AT_TYPICAL_INDEX\n");
        }

        if (MTF_MAGIC_SPECIAL_VALUE == off.magic1) {
            if (well_known_magic_spec(off.magic2)) {
                off.heu_points += HEU_PTS_HDR_WELL_KNOW_MAGIC_SPEC;
                //printf("HEU_PTS_HDR_WELL_KNOW_MAGIC_SPEC\n");
            }
        } else if (well_known_magic(off.magic1)) {
            off.heu_points += HEU_PTS_HDR_WELL_KNOW_MAGIC;
            //printf("HEU_PTS_HDR_WELL_KNOW_MAGIC\n");
        }
    }

    std::sort(rbegin(), rend()); // in descending order
}

//--------------------------------------------------------------------

bool possible_offsets::try_add_impl(int64_t offset, int field_index)
{
    if (offset < 0 || field_index < 0) {
        return false;
    }

    possible_offset po(offset, field_index);
    auto pos_next = (from + offset);

    if (pos_next > scan.size) {
        return false;
    }

    if (pos_next == scan.size) {
        push_back(po);
        return true;
    }

    auto avail = (scan.size - pos_next);
    auto avail_u32 = avail / sizeof(uint32_t);
    if (avail_u32 < 1) {
        return true;
    }
    auto pu32 = reinterpret_cast<uint32_t*>(offset_to_pointer(scan.addr, from + offset));

    po.magic1 = pu32[0];
    if (!might_be_magic(po.magic1)) {
        if (MTF_MAGIC_SPECIAL_VALUE != po.magic1) {
            return false;
        }
        if (avail_u32 < 2) {
            return false;
        }
        po.magic2 = pu32[1];
        if (!might_be_magic(po.magic2)) {
            return false;
        }
    } else {
        uint32_t magic2 = pu32[1];
        if (is_up_al((magic2>>0) & 0xFF)) {
            po.heu_points += HEU_PTS_EACH_ASCII_UP_AFTER_MAGIC;
            //printf("HEU_PTS_EACH_ASCII_UP_AFTER_MAGIC(0)\n");
        }
        if (is_up_al((magic2>>8) & 0xFF)) {
            po.heu_points += HEU_PTS_EACH_ASCII_UP_AFTER_MAGIC;
            //printf("HEU_PTS_EACH_ASCII_UP_AFTER_MAGIC(1)\n");
        }
        if (is_up_al((magic2>>16) & 0xFF)) {
            po.heu_points += HEU_PTS_EACH_ASCII_UP_AFTER_MAGIC;
            //printf("HEU_PTS_EACH_ASCII_UP_AFTER_MAGIC(2)\n");
        }
        if (is_up_al((magic2>>24) & 0xFF)) {
            po.heu_points += HEU_PTS_EACH_ASCII_UP_AFTER_MAGIC;
            //printf("HEU_PTS_EACH_ASCII_UP_AFTER_MAGIC(3)\n");
        }
    }
    push_back(po);
    return true;
}

//--------------------------------------------------------------------

void possible_offsets::try_add(
    const char* prefix,
    size_t pos,
    size_t pos_next,
    uint64_t sz,
    uint32_t sz_add
    )
{
    if (sz_hdr > sz || pos_next > scan.size) {
        return;
    }

    auto offset = (pos_next - from);
    auto field_index = (pos - from);
    bool result = try_add_impl(offset, field_index);

    const char* mleft = "   ";
    const char* mright = "   ";
    if (result) {
        mleft = ">>>";
        mright = "<<<";
    }

    dprint(
        "%s %s "
        "%08lx [%02lx]: %04lx +%x -> "
        "%08lx %s\n",
        prefix, mleft, 
        pos, field_index, (sz - sz_add), sz_add,
        pos_next, mright
    );
}

//--------------------------------------------------------------------

void possible_offsets::lookup_offset16(
    uint32_t sz_add,
    uint32_t off_skip
    )
{
    size_t pos(from + off_skip);
    auto finish(std::min(scan.size, to));
    finish -= sizeof(uint16_t);

    while (pos < finish) {
        auto sz = *reinterpret_cast<const uint16_t*>(offset_to_pointer(scan.addr, pos));
        sz += sz_add;

        auto pos_next = (from + sz);
        try_add("(16)", pos, pos_next, sz, sz_add);

        if (pos_next & 3) {
            pos_next = round_up(pos_next, sizeof(uint32_t));
            try_add("(16)", pos, pos_next, sz, sz_add);
        }
        pos += 2;        
    }
}

//--------------------------------------------------------------------

void possible_offsets::lookup_offset64(
    uint32_t sz_add,
    uint32_t off_skip
    )
{
    size_t pos(from + off_skip);
    auto finish(std::min(scan.size, to));
    finish -= sizeof(int64_t);

    while (pos < finish) {
        auto sz = *reinterpret_cast<const uint64_t*>(offset_to_pointer(scan.addr, pos));
        sz += sz_add;
        
        auto pos_next = (from + sz);
        try_add("(64)", pos, pos_next, sz, sz_add);

        if (pos_next & 3) {
            pos_next = round_up(pos_next, sizeof(uint32_t));
            try_add("(64)", pos, pos_next, sz, sz_add);
        }
        pos += 2;        
    }
}

//--------------------------------------------------------------------

mtf_scan::mtf_scan(const void* addr, size_t size)
    : addr(addr)
    , size(size)
{}

//--------------------------------------------------------------------

uint32_t mtf_scan::checksum(size_t pos)
{
    auto pu16 = reinterpret_cast<const uint16_t*>(offset_to_pointer(addr, pos));

    auto bytes_avail = (size - pos);
    auto bytes_to_scan = std::min<size_t>(MAX_SCAN_CHKSUM_BYTES, bytes_avail);
    auto idx_max = bytes_to_scan / sizeof(uint16_t);
    auto idx_min = MIN_SCAN_CHKSUM_BYTES / sizeof(uint16_t);

    uint16_t chksum = 0;
    for (int idx = 0; idx < idx_max; idx++) {
        chksum ^= pu16[idx];
        if (idx >= idx_min) {
            if (!chksum) {
                return (idx * sizeof(uint16_t));
            }
        }
    }
    return -1;
}

//--------------------------------------------------------------------

int64_t mtf_scan::forward(size_t pos, uint32_t* psz_hdr)
{
    //
    // Searchint for the end of header (assumes it completes by 'checksum' field)
    //
    uint32_t off_chksum = checksum(pos);
    if (-1 == off_chksum) {
        handle_error("no checksum");
    }
    uint32_t sz_hdr(off_chksum + sizeof(uint16_t));
    if (psz_hdr) {
        *psz_hdr = sz_hdr;
    }

    auto pchksum = reinterpret_cast<const uint16_t*>(offset_to_pointer(addr, off_chksum));
    dprint("off_chksum: 0x%x {%04x}, sz_hdr: 0x%x\n", off_chksum, *pchksum, sz_hdr);

    uint32_t off_skip = 8;
    auto pmagic = reinterpret_cast<const uint32_t*>(offset_to_pointer(addr, pos));
    if (MTF_MAGIC_SPECIAL_VALUE == *pmagic) {
        off_skip += 4;
    }

    //
    // Scanning the header for potential offset candidates
    //
    possible_offsets po(*this, pos, (pos + off_chksum), sz_hdr);
    po.lookup_offset64(sz_hdr, off_skip);
    po.lookup_offset16(     0, off_skip);
    //printf("candidates count: %ld\n", po.size());

    //
    // Testing all candidates and assign heuristic points
    //
    po.assign_heu_points(pos);


#ifdef DEBUG
    for (auto& off : po) {
        printf("off=%lx, points=%d, <%x>, '%.4s'/'%.4s'\n", off.offset, off.heu_points, off.sz_hdr, (char*)&off.magic1, (char*)&off.magic2);
    }
#endif

    if (po.empty()) {
        handle_error("next not found");
//    } else if (offsets.size() > 1) {
//        handle_error("next not obvious");
    }

    auto off_next = po[0].offset;
    //print("off_next: 0x%lx\n", off_next);    
    return off_next;
}

//--------------------------------------------------------------------
