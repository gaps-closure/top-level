/* Entire file to be autogenerated by CAPO */

#ifndef _ORANGE_RPC_
#define _ORANGE_RPC_
#include "xdcomms.h"
#include "codec.h"
#include <pthread.h>

#define APP_BASE 0
#define MUX_NEXTRPC    APP_BASE + 1
#define SEC_NEXTRPC    APP_BASE + 1
#define MUX_OKAY       APP_BASE + 2
#define SEC_OKAY       APP_BASE + 2
#define MUX_REQUESTA   APP_BASE + 1
#define SEC_REQUESTA   APP_BASE + 1
#define MUX_RESPONSEA  APP_BASE + 2
#define SEC_RESPONSEA  APP_BASE + 2

#define INURI  "ipc:///tmp/test1suborange"
#define OUTURI "ipc:///tmp/test1puborange"

/* When receiving from the other side, only level changes 
(our level is the remote level of our peer). Preserve
the rest of CLE json for data provenance. gapstag is [mux,sec,typ]. */

#pragma cle def TAG_RESPONSEA {"level":"orange",\
  "cdf": [\
    {"remotelevel":"purple", \
     "direction": "egress", \
     "guardhint": { "operation": "allow", \
                    "gapstag": [2,2,4] }} \
  ] }

#pragma cle def TAG_REQUESTA {"level":"orange",\
  "cdf": [\
    {"remotelevel":"orange", \
     "direction": "egress", \
     "guardhint": { "operation": "allow", \
                    "gapstag": [1,1,3] }} \
  ] }

#pragma cle def TAG_OKAY {"level":"orange",\
  "cdf": [\
    {"remotelevel":"purple", \
     "direction": "egress", \
     "guardhint": { "operation": "allow", \
                    "gapstag": [2,2,2] }} \
  ] }

#pragma cle def TAG_NEXTRPC {"level":"orange",\
  "cdf": [\
    {"remotelevel":"orange", \
     "direction": "egress", \
     "guardhint": { "operation": "allow", \
                    "gapstag": [1,1,1] }} \
  ] }

extern int  _slave_rpc_loop();

/* Signatures of functions from original program called in RPCs */
/* Add signature here if not already in another header, else include that header */
/* May need to include other headers for types references in signature */
extern double get_a();

#endif /* _ORANGE_RPC_ */
