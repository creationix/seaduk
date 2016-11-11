#include "dns.h"

duk_ret_t duv_new_dns(duk_context *ctx) {
  uv_getaddrinfo_t *ai = duk_push_fixed_buffer(ctx, sizeof(uv_getaddrinfo_t));
  uv_getnameinfo_t *ni = duk_push_fixed_buffer(ctx, sizeof(uv_getnameinfo_t));
  duv_setup_handle(ctx, (uv_handle_t*)ai, DUV_DNS);
  duv_setup_handle(ctx, (uv_handle_t*)ni, DUV_DNS);
  return 1;
}

duk_ret_t duv_lookup_sync(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"address", duk_is_string},
    {0,0}
  });

  struct addrinfo hints;
  hints.ai_family = PF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = 0;

  uv_getaddrinfo_t req;

  const char *address = duk_get_string(ctx, 0);

  int r = uv_getaddrinfo(duv_loop(ctx),&req, NULL, address, NULL, &hints);
  if(r !=0 ){
      printf("DNS failed : %s\n",gai_strerror(r));
      return -1;
  }

  char ip[17] = {'\0'};
  uv_ip4_name((struct sockaddr_in*) req.addrinfo->ai_addr, ip, 16);
  printf("DNS result : %s\n", ip);
  uv_freeaddrinfo(req.addrinfo);
  duk_push_string(ctx,ip);
  return -1;
}

duk_ret_t duv_resolve_sync(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"address", duk_is_string},
    {0,0}
  });

  struct addrinfo hints;
  hints.ai_family = PF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = 0;

  uv_getaddrinfo_t req;

  const char *address = duk_get_string(ctx, 0);

  int r = uv_getaddrinfo(duv_loop(ctx),&req, NULL, address, NULL, &hints);
  if(r !=0 ){
      printf("DNS failed : %s\n",gai_strerror(r));
      return -1;
  }

  char ip[17] = {'\0'};
  struct addrinfo *ai,*ai0 = req.addrinfo;
  for (ai = ai0; ai; ai = ai->ai_next) {
    uv_ip4_name((struct sockaddr_in*) ai->ai_addr, ip, 16);
    printf("DNS result : %s\n", ip);
  }
  uv_freeaddrinfo(req.addrinfo);
  duk_push_string(ctx,ip);
  return 1;
}

