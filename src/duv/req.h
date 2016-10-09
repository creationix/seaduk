#ifndef REQ_H
#define REQ_H

#include "duv.h"

duk_ret_t duv_req_tostring(duk_context *ctx);
duk_ret_t duv_cancel(duk_context *ctx);

#endif
