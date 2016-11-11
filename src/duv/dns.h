#ifndef DNS_H
#define DNS_H

#include "duv.h"

duk_ret_t duv_resolve_sync(duk_context *ctx);
duk_ret_t duv_lookup_sync(duk_context *ctx);
duk_ret_t duv_new_dns(duk_context *ctx);

#endif
