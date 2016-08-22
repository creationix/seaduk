#ifndef UDP_H
#define UDP_H

#include "duv.h"

extern const char* duv_protocol_to_string(int family);
duk_ret_t duv_new_udp(duk_context *ctx);
duk_ret_t duv_udp_bind(duk_context *ctx);
duk_ret_t duv_udp_send(duk_context *ctx);
duk_ret_t duv_udp_getsockname(duk_context *ctx);
duk_ret_t duv_udp_recv_start(duk_context *ctx);
duk_ret_t duv_udp_recv_stop(duk_context *ctx);
duk_ret_t duv_udp_broadcast(duk_context *ctx);
duk_ret_t duv_udp_set_ttl(duk_context *ctx);

#endif
