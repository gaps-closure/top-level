#ifndef _PURPLE_RPC_
#define _PURPLE_RPC_

#include "codec.h"
#include <pthread.h>
#include <assert.h>
#include "log.h"

#define INURI "ipc:///tmp/secdesksubpurple"
#define OUTURI "ipc:///tmp/secdeskpubpurple"

#define MY_IPC_ADDR_DEFAULT_IN  "ipc:///tmp/halpub1"
#define MY_IPC_ADDR_DEFAULT_OUT "ipc:///tmp/halsub1"


#define ADU_SIZE_MAX_C  2000
#define MY_DATA_TYP_MAX 200
#define RX_FILTER_LEN   12


/* Closure tag structure */
typedef struct _tag {
  uint32_t    mux;      /* APP ID */
  uint32_t    sec;      /* Security tag */
  uint32_t    typ;      /* data type */
} gaps_tag;

/* CLOSURE packet */
typedef struct _sdh_ha_v1 {
  gaps_tag  tag;
  uint32_t  data_len;
  uint8_t   data[ADU_SIZE_MAX_C];
} sdh_ha_v1;

typedef void (*codec_func_ptr)(void *, void *, size_t *);
typedef struct _codec_map {
  int valid;
  codec_func_ptr encode;
  codec_func_ptr decode;
} codec_map;

#define APP_BASE 0

extern int _slave_rpc_loop();

#pragma cle def TAG_NEXTRPC {"level": "purple", \
    "cdf": [{"remotelevel": "purple", "direction": "egress", \
            "guardhint": {"operation": "allow", "gapstag": [1,1,1]}}]}

#pragma cle def TAG_OKAY {"level": "purple", \
    "cdf": [{"remotelevel": "orange", "direction": "egress", \
            "guardhint": {"operation": "allow", "gapstag": [2,2,2]}}]}

#pragma cle def TAG_REQUEST_RECOGNIZE {"level": "purple", \
    "cdf": [{"remotelevel": "purple", "direction": "egress", \
            "guardhint": {"operation": "allow", "gapstag": [1,1,3]}}]}

#pragma cle def TAG_RESPONSE_RECOGNIZE {"level": "purple", \
    "cdf": [{"remotelevel": "orange", "direction": "egress", \
            "guardhint": {"operation": "allow", "gapstag": [2,2,4]}}]}

#pragma cle def TAG_REQUEST_START_RECOGNIZER {"level": "purple", \
    "cdf": [{"remotelevel": "purple", "direction": "egress", \
            "guardhint": {"operation": "allow", "gapstag": [1,1,5]}}]}

#pragma cle def TAG_RESPONSE_START_RECOGNIZER {"level": "purple", \
    "cdf": [{"remotelevel": "orange", "direction": "egress", \
            "guardhint": {"operation": "allow", "gapstag": [2,2,6]}}]}

#pragma cle def TAG_REQUEST_STOP_RECOGNIZER {"level": "purple", \
    "cdf": [{"remotelevel": "purple", "direction": "egress", \
            "guardhint": {"operation": "allow", "gapstag": [1,1,7]}}]}

#pragma cle def TAG_RESPONSE_STOP_RECOGNIZER {"level": "purple", \
    "cdf": [{"remotelevel": "orange", "direction": "egress", \
            "guardhint": {"operation": "allow", "gapstag": [2,2,8]}}]}

#define MUX_NEXTRPC APP_BASE + 1
#define SEC_NEXTRPC APP_BASE + 1
#define MUX_OKAY APP_BASE + 2
#define SEC_OKAY APP_BASE + 2

#define MUX_REQUEST_RECOGNIZE APP_BASE + 1
#define SEC_REQUEST_RECOGNIZE APP_BASE + 1
#define MUX_RESPONSE_RECOGNIZE APP_BASE + 2
#define SEC_RESPONSE_RECOGNIZE APP_BASE + 2
#define MUX_REQUEST_START_RECOGNIZER APP_BASE + 1
#define SEC_REQUEST_START_RECOGNIZER APP_BASE + 1
#define MUX_RESPONSE_START_RECOGNIZER APP_BASE + 2
#define SEC_RESPONSE_START_RECOGNIZER APP_BASE + 2
#define MUX_REQUEST_STOP_RECOGNIZER APP_BASE + 1
#define SEC_REQUEST_STOP_RECOGNIZER APP_BASE + 1
#define MUX_RESPONSE_STOP_RECOGNIZER APP_BASE + 2
#define SEC_RESPONSE_STOP_RECOGNIZER APP_BASE + 2

extern int recognize(double embedding[]);
extern int start_recognizer();
extern int stop_recognizer();

#endif /* _PURPLE_RPC_ */