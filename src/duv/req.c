#include "req.h"

duk_ret_t duv_req_tostring(duk_context *ctx) {
  duk_push_this(ctx);
  duk_get_prop_string(ctx, -1, "\xff""req-type");
  if (duk_is_undefined(ctx, -1)) return 0;
  const char* type = duv_req_type_to_string(duk_get_int(ctx, -1));
  duk_get_prop_string(ctx, -2, "\xffuv-data");
  void* data = duk_get_buffer(ctx, -1, 0);
  duk_pop_3(ctx);
  duk_push_sprintf(ctx, "[%s %p]", type, data);
  return 1;
}

duk_ret_t duv_cancel(duk_context *ctx) {
  duk_push_this(ctx);
  duk_get_prop_string(ctx, -1, "\xff""req-type");
  if (duk_is_undefined(ctx, -1)) return 0;
  duk_get_prop_string(ctx, -2, "\xffuv-data");
  uv_req_t* req = duk_get_buffer(ctx, -1, 0);
  duv_check(ctx, uv_cancel(req));
  duk_pop_2(ctx);
  return 0;
}
