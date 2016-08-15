#include "udp.h"

duk_ret_t duv_new_udp(duk_context *ctx) {
  uv_udp_t *udp = duk_push_fixed_buffer(ctx, sizeof(uv_udp_t));
  duv_check(ctx, uv_udp_init(duv_loop(ctx), udp));
  duv_setup_handle(ctx, (uv_handle_t*)udp, DUV_UDP);
  return 1;
}

duk_ret_t duv_udp_broadcast(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"enable", duk_is_boolean},
    {0,0}
  });
  uv_udp_t *udp = duv_require_this_handle(ctx, DUV_UDP_MASK);
   
  duv_check(ctx, uv_udp_set_broadcast(udp, duk_get_int(ctx, 1)));
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
  duk_push_pointer(ctx, address);
  duk_put_prop_string(ctx, -2, "\xff""addr");
}

// connect() only sets the address that the socket will send packets to if you call send(); 
//duk_ret_t duv_udp_connect(duk_context *ctx) {
//  dschema_check(ctx, (const duv_schema_entry[]) {
//    {"host", duk_is_string},
//    {"port", duk_is_number},
//    {0,0}
//  });
//  uv_udp_t *udp = duv_require_this_handle(ctx, DUV_UDP_MASK);
//  if(!udp) {}
//  const char *host = duk_get_string(ctx, 1);
//  int port = duk_get_number(ctx, 2);
//  //    flags = 0;
//  struct sockaddr_storage addr;
//  if (uv_ip4_addr(host, port, (struct sockaddr_in*)&addr) &&
//      uv_ip6_addr(host, port, (struct sockaddr_in6*)&addr)) {
//    duk_error(ctx, DUK_ERR_TYPE_ERROR, "Invalid IP address or port");
//  }
//  // TODO : implement flags, store addr6
//  duv_push_sockaddr(ctx, &addr, sizeof(addr));
//  //DUK_PUSH_PROP_POINTER("\xff""\xff""data",TP);
//
//  return 0;
//}

duk_ret_t duv_udp_bind(duk_context *ctx) {
  // UV_UDP_REUSEADDR = 4
  int port, flags = 4;
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"host", duk_is_string},
    {"port", duk_is_number},
    {0,0}
  });
  uv_udp_t *udp = duv_require_this_handle(ctx, DUV_UDP_MASK);
  const char *host = duk_get_string(ctx, 1);
  port = duk_get_number(ctx, 2);
  struct sockaddr_storage addr;
  if (uv_ip4_addr(host, port, (struct sockaddr_in*)&addr) &&
      uv_ip6_addr(host, port, (struct sockaddr_in6*)&addr)) {
    duk_error(ctx, DUK_ERR_TYPE_ERROR, "Invalid IP address or port");
    return 0;
  }
  // TODO : implement flags
  duv_check(ctx, uv_udp_bind(udp, (struct sockaddr*)&addr, flags));
  return 0;
}

duk_ret_t duv_udp_getsocket(duk_context *ctx) {
  uv_udp_t *udp = duv_require_this_handle(ctx, DUV_UDP_MASK);
  struct sockaddr_storage address;
  int addrlen = sizeof(address);
  duv_check(ctx, uv_udp_getsockname(udp, (struct sockaddr*)&address, &addrlen));
  duv_push_sockaddr(ctx, &address, addrlen);
  return 1;
}

duk_ret_t duv_udp_getsockname(duk_context *ctx) {
  uv_udp_t *udp = duv_require_this_handle(ctx, DUV_UDP_MASK);
  struct sockaddr_storage address;
  int addrlen = sizeof(address);
  duv_check(ctx, uv_udp_getsockname(udp, (struct sockaddr*)&address, &addrlen));
  duv_push_sockaddr(ctx, &address, addrlen);
  return 1;
}

duk_ret_t duv_udp_recv_stop(duk_context *ctx) {
  uv_udp_t *udp = duv_require_this_handle(ctx, DUV_UDP_MASK);
  duv_check(ctx, uv_udp_recv_stop(udp));
  return 0;
}


duk_ret_t duv_udp_recv_start(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"callback", dschema_is_continuation},
    {0,0}
  });
  uv_udp_t *udp = duv_require_this_handle(ctx, DUV_UDP_MASK);
  duk_put_prop_string(ctx, 0, "\xffon-read");
  duv_check(ctx, uv_udp_recv_start(udp, duv_on_alloc, duv_on_udp_recv_start));
  return 0;
}

duk_ret_t duv_udp_send(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"host", duk_is_string},
    {"port", duk_is_number},
    {"data", dschema_is_data},
    {"callback", dschema_is_continuation},
    {0,0}
  });
  uv_udp_t *udp = duv_require_this_handle(ctx, DUV_UDP_MASK);
  const char *host = duk_get_string(ctx, 1);
  int port = duk_get_number(ctx, 2);
  //    flags = 0;
  struct sockaddr_storage addr;
  if (uv_ip4_addr(host, port, (struct sockaddr_in*)&addr) &&
      uv_ip6_addr(host, port, (struct sockaddr_in6*)&addr)) {
    duk_error(ctx, DUK_ERR_TYPE_ERROR, "Invalid IP address or port");
    return 0;
  }
  // TODO : implement flags
  duv_push_sockaddr(ctx, &addr, sizeof(addr));
  uv_udp_send_t *req = duk_push_fixed_buffer(ctx, sizeof(*req));
  uv_buf_t buf;
  duv_get_data(ctx, 3, &buf);
  duv_check(ctx, uv_udp_send(req, udp, &buf, 1, (const struct sockaddr*)&addr, duv_on_udp_send));
  duv_setup_request(ctx, (uv_req_t*)req, 4);
  return 0;
}

//duk_ret_t duv_udp_connect(duk_context *ctx) {
//  dschema_check(ctx, (const duv_schema_entry[]) {
//    {"host", duk_is_string},
//    {"port", duk_is_number},
//    {"callback", dschema_is_continuation},
//    {0,0}
//  });
//  uv_udp_t *udp = duv_require_this_handle(ctx, DUV_UDP_MASK);
//  uv_connect_t *req = duk_push_fixed_buffer(ctx, sizeof(*req));
//  const char *host = duk_get_string(ctx, 1);
//  int port = duk_get_number(ctx, 2);
//  struct sockaddr_storage addr;
//  if (uv_ip4_addr(host, port, (struct sockaddr_in*)&addr) &&
//      uv_ip6_addr(host, port, (struct sockaddr_in6*)&addr)) {
//    duk_error(ctx, DUK_ERR_TYPE_ERROR, "Invalid IP address or port");
//  }
//  duv_check(ctx, uv_udp_connect(req, udp, (struct sockaddr*)&addr, duv_on_connect));
//  duv_setup_request(ctx, (uv_req_t*)req, 3);
//  return 0;
//}
