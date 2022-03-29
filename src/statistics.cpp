
#include <set>
#include <assert.h>


#include "config.h"
#include "common.h"
#include "statistics.h"

#include "mtf_magic.h"

//--------------------------------------------------------------------

class statistics::impl
{
public:
    struct item_info
    {
        uint32_t magic1;
        uint32_t magic2;

        //
        // To DONT create a map with very tiny elements.
        //
        mutable uint64_t cnt;
        mutable uint64_t size;

        uint32_t sz_hdr;

        item_info()
            : magic1(0)
            , magic2(0)
            , cnt(0)
            , size(0)
            , sz_hdr()
        {
		}

        void merge(const item_info& rhs) const
        {
            assert(magic1 == rhs.magic1);
            assert(magic2 == rhs.magic2);

            cnt += rhs.cnt;
            size += rhs.size;
        }

        bool operator<(const item_info& rhs) const
        {
            if (MTF_MAGIC_SPECIAL_VALUE == magic1) {
                if (MTF_MAGIC_SPECIAL_VALUE == rhs.magic1) {
                    return magic2 < rhs.magic2;
                }
                return false;
            }

            if (MTF_MAGIC_SPECIAL_VALUE == rhs.magic1) {
                return true;
            }

            return magic1 < rhs.magic1;
        }

        void show(FILE* f) const;
    };

	impl()
	{
        ts_start = getts_ms();
	}

    void get_total(
        uint64_t& cnt,
        uint64_t& size
    ) const;

    typedef std::set<item_info> items_type;
    items_type items;
	uint64_t ts_start;
};

//--------------------------------------------------------------------

void statistics::impl::item_info::show(FILE* f) const
{
	union
	{
		char str[1];
		struct __attribute__((__packed__)) {
			char is_known;
			char space;
			char is_special;
			uint32_t magic;
			char zero;
		};
	} name;

	name.is_known = name.space = name.is_special = ' ';

	
    if (MTF_MAGIC_SPECIAL_VALUE == magic1) {
		if (!well_known_magic_spec(magic2)) {
			name.is_known = '?';
		}
		name.is_special = '$';
        name.magic = magic2;
    } else {
		if (!well_known_magic(magic1)) {
			name.is_known = '?';
		}
        name.magic = magic1;
    }
	name.zero = 0;

    fprintf(f, "%s<%x> count=%ld, size=%ld MB\n", name.str, sz_hdr, cnt, size / (1024*1024));
}

//--------------------------------------------------------------------

void statistics::impl::get_total(
    uint64_t& cnt,
    uint64_t& size
) const
{
    cnt = size = 0;
    for (const auto& i : items) {
        cnt += i.cnt;
        size += i.size;
    }
}

//--------------------------------------------------------------------

statistics::statistics()
{
    pimpl.reset(new impl());
}

//--------------------------------------------------------------------

statistics::~statistics()
{
}

//--------------------------------------------------------------------

void statistics::account(
    const void* addr,
    uint64_t    pos,
    uint64_t    sz_item,
    uint32_t    sz_hdr
)
{
    auto pu32 = reinterpret_cast<const uint32_t*>(offset_to_pointer(addr, pos));

    statistics::impl::item_info key;
    key.magic1  = pu32[0];
    if (MTF_MAGIC_SPECIAL_VALUE == key.magic1) {
        key.magic2 = pu32[1];
    }
    key.size    = (sz_item - sz_hdr);
    key.cnt     = 1;
    key.sz_hdr  = sz_hdr;

    statistics::impl::items_type::iterator it = pimpl->items.find(key);
    if (pimpl->items.end() != it) {
        it->merge(key);
    } else {
        pimpl->items.insert(key);
    }
}

//--------------------------------------------------------------------

void statistics::show(FILE* f) const
{
    uint64_t cnt, size;
    pimpl->get_total(cnt, size);
    fprintf(f, "Total count=%ld, size=%ld MB\n", cnt, size / (1024*1024));

    for (const auto& i : pimpl->items) {
        i.show(f);
    }

	uint64_t elapsed = (getts_ms() - pimpl->ts_start) / 1000;
	fprintf(f, "\nElapsed: %s\n", sprintf_timediff(elapsed));

    fflush(f);
}

//--------------------------------------------------------------------
