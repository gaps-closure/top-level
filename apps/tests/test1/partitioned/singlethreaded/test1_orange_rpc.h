#ifndef ORANGE
#define ORANGE
#include "xdcomms.h"
#include "codec.h"

# define MUX_NEXTRPC    1
# define SEC_NEXTRPC    1
# define MUX_OKAY       2
# define SEC_OKAY       2
# define MUX_REQUESTA   1
# define SEC_REQUESTA   1
# define MUX_RESPONSEA  2
# define SEC_RESPONSEA  2

extern double get_a();

extern void _get_next_tag(gaps_tag* n_tag);
extern void _hal_init(char *inuri, char *outuri);
extern void _handle_requesta(gaps_tag* tag_requesta);
extern int  _slave_rpc_loop();

#endif /* ORANGE */