#ifndef _WEBAPP_H_
#define _WEBAPP_H_

#include <stdlib.h>
#include <string.h>

#include "fio.h"
#include "fio_cli.h"
#include "http.h"
#include "multipart_helper.h"

void initialize_http_service(int argc, char const *argv[]);
void housekeep_http_service(void);

#endif /* _WEBAPP_H_ */
