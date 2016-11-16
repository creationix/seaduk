#ifndef DNS_H
#define DNS_H

#include "duv.h"

duk_ret_t duv_resolve_sync(duk_context *ctx);
duk_ret_t duv_resolve4_sync(duk_context *ctx);
duk_ret_t duv_resolve6_sync(duk_context *ctx);
duk_ret_t duv_lookup_sync(duk_context *ctx);
duk_ret_t duv_new_dns(duk_context *ctx);

void duv_pushaddrinfo(duk_context *ctx, struct addrinfo* res);
duk_ret_t duv_getaddrinfo(duk_context *ctx);
duk_ret_t duv_getnameinfo(duk_context *ctx);

#endif
