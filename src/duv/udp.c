#include "udp.h"

duk_ret_t duv_new_udp(duk_context *ctx) {
  uv_udp_t *udp = duk_push_fixed_buffer(ctx, sizeof(uv_udp_t));
  duv_check(ctx, uv_udp_init(duv_loop(ctx), udp));
  duv_setup_handle(ctx, (uv_handle_t*)udp, DUV_UDP);
  return 1;
}

duk_ret_t duv_udp_bind(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"host", duk_is_string},
    {"port", duk_is_number},
    {0,0}
  });
  uv_udp_t *udp = duv_require_this_handle(ctx, DUV_UDP_MASK);
  const char *host = duk_get_string(ctx, 1);
  int port = duk_get_number(ctx, 2),
      flags = 0;
  struct sockaddr_storage addr;
  if (uv_ip4_addr(host, port, (struct sockaddr_in*)&addr) &&
      uv_ip6_addr(host, port, (struct sockaddr_in6*)&addr)) {
    duk_error(ctx, DUK_ERR_TYPE_ERROR, "Invalid IP address or port");
  }
  duv_check(ctx, uv_udp_bind(udp,
    (struct sockaddr*)&addr,
    flags
  ));
  return 0;
}

static void duv_push_sockaddr(duk_context *ctx, struct sockaddr_storage* address, int addrlen) {
  char ip[INET6_ADDRSTRLEN];
  int port = 0;
  if (address->ss_family == AF_INET) {
    struct sockaddr_in* addrin = (struct sockaddr_in*)address;
    uv_inet_ntop(AF_INET, &(addrin->sin_addr), ip, addrlen);
    port = ntohs(addrin->sin_port);
  } else if (address->ss_family == AF_INET6) {
    struct sockaddr_in6* addrin6 = (struct sockaddr_in6*)address;
    uv_inet_ntop(AF_INET6, &(addrin6->sin6_addr), ip, addrlen);
    port = ntohs(addrin6->sin6_port);
  }

  duk_push_object(ctx);
  duk_push_string(ctx, duv_protocol_to_string(address->ss_family));
  duk_put_prop_string(ctx, -2, "family");
  duk_push_number(ctx, port);
  duk_put_prop_string(ctx, -2, "port");
  duk_push_string(ctx, ip);
  duk_put_prop_string(ctx, -2, "ip");
}

duk_ret_t duv_udp_getsockname(duk_context *ctx) {
  uv_udp_t *udp = duv_require_this_handle(ctx, DUV_UDP_MASK);
  struct sockaddr_storage address;
  int addrlen = sizeof(address);
  duv_check(ctx, uv_udp_getsockname(udp, (struct sockaddr*)&address, &addrlen));
  duv_push_sockaddr(ctx, &address, addrlen);
  return 1;
}
