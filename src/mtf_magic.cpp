
#include "mtf_magic.h"
#include "common.h"

//--------------------------------------------------------------------

bool might_be_magic(uint32_t u32)
{
    return (is_up_alnum((u32>>0) & 0xFF) &&
            is_up_alnum((u32>>8) & 0xFF) &&
            is_up_alnum((u32>>16) & 0xFF) &&
            is_up_alnum((u32>>24) & 0xFF));
}

//--------------------------------------------------------------------

//
// standart Common Block Header size is 0x34
//
bool well_known_dblk(uint32_t u32)
{
    uint32_t ids[] = {
        //
        // Exists in spec
        //
        0x45504154, // 'TAPE'
        0x54455353, // 'SSET'
        0x424c4f56, // 'VOLB'
        0x42524944, // 'DIRB'
        0x454c4946, // 'FILE'
        0x42505345, // 'ESPB'
        0x54455345, // 'ESET'
        0x4d544f45, // 'EOTM'

        //
        // Unknown
        //
        0x43434332, // '2CCC'
        0x484c4332, // '2CLH'
        0x43574332, // '2CWC'
        0x43434532, // '2ECC'
        0x53434532, // '2ECS'
        0x484c4532, // '2ELH'
        0x44554532, // '2EUD'
        0x43574532, // '2EWC'
        0x43474441, // 'ADGC'
        0x44474441, // 'ADGD'
        0x47474441, // 'ADGG'
        0x50474441, // 'ADGP'
        0x55575551, // 'ADGU'
        0x46444d56, // 'VMDF'
        0x4d474d56, // 'VMGM'
        0x534c5153, // 'SQLS'
        0x34535153, // 'SQS4'
    };
    return is_exist(ids, countof(ids), u32);
}

//--------------------------------------------------------------------
//
// Standart Stream Header size is 0x16
//
bool well_known_strm(uint32_t u32)
{
    uint32_t ids[] = {
        //
        // Exists in spec
        //
        0x44415053, // 'SPAD'
        0x44444654, // 'TFDD'
        0x4d555343, // 'CSUM'
        0x4c43414e, // 'NACL'
        0x4145544e, // 'NTEA'
        0x5551544e, // 'NTQU'
        0x494f544e, // 'NTOI'
        0x5052544e, // 'NTRP'
        0x4e415453, // 'STAN'
        0x54414441, // 'ADAT'
        0x4B4e494c, // 'LINK'
        0x4d414e50, // 'PNAM'

        //
        // Unknown
        //
        0x4449464d, // 'MFID'
        0x5043544f, // 'OTCP'
        0x32504d53, // 'SMP2'
        0x444d4d56, // 'VMMD'
        0x53504548, // 'HEPS'
        0x54414c50, // 'PLAT'
        0x44424353, // 'SCBD'
        0x54574353, // 'SCWT'
        0x5051544e, // 'NTQP'
        0x4c494645, // 'EFIL'
        0x54454d45, // 'EMET'
        0x44574353, // 'SCWD'
        0x54494450, // 'PDIT'
        0x52494445, // 'EDIR'
        0x4f4f4458, // 'XDOO'
        0x53494450, // 'PDIS'
        0x44415043, // 'CPAD'
    };
    return is_exist(ids, countof(ids), u32);
}

//--------------------------------------------------------------------

bool well_known_magic(uint32_t u32)
{
    return well_known_dblk(u32) || well_known_strm(u32);
}

//--------------------------------------------------------------------
//
// Unknown type header size is 0x24.
//
bool well_known_magic_spec(uint32_t u32)
{
    uint32_t ids[] = {
        //
        // First 'DWORD' of header is also 0x24 (MTF_MAGIC_SPECIAL_VALUE)
        //
        0x444d564d, // 'MVMD'
        0x4d555343, // 'CSUM'
        0x44415041, // 'APAD'
        0x444d5656, // 'VVMD'
        0x44415043, // 'CPAD'

        0x44415053, // 'SPAD'
    };
    return is_exist(ids, countof(ids), u32);

}

//--------------------------------------------------------------------
