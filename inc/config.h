
#pragma once

//--------------------------------------------------------------------
//
// Heuristic points
//
#define HEU_PTS_EXACTLY_EOF                 400
#define HEU_PTS_NO_HDR_CHKSUM               -1000
#define HEU_PTS_NEXT_HDR_DWORD_ALIGNED      100
#define HEU_PTS_NEXT_HDR_MISALIGNED         -500
#define HEU_PTS_HDR_SIZE_AT_TYPICAL_INDEX   300
#define HEU_PTS_HDR_WELL_KNOW_MAGIC         300
#define HEU_PTS_HDR_WELL_KNOW_MAGIC_SPEC    200
#define HEU_PTS_EACH_ASCII_UP_AFTER_MAGIC   -50

//--------------------------------------------------------------------

//#define DEBUG

//#define DEBUG_LIMIT_RESULTS_MAX 1000000
//#define DEBUG_SHOW_UNIQUE_MAGICS_ONLY

//--------------------------------------------------------------------

#define PGS_SAVEPOINT_INTERVAL_MS   100
#define PGS_DISPLAY_INTERVAL_MS     200
#define PGS_SHOW_OVERALL_SPEED

//--------------------------------------------------------------------

#define MIN_SCAN_CHKSUM_BYTES   0x14
#define MAX_SCAN_CHKSUM_BYTES   0x40

//--------------------------------------------------------------------

#define MSG_BUF_SIZE    0x1000

//--------------------------------------------------------------------


