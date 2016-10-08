#include "utils.h"
#include <inttypes.h>

const char* duv_protocol_to_string(int family) {
  #ifdef AF_UNIX
    if (family == AF_UNIX) return "UNIX";
  #endif
  #ifdef AF_INET
    if (family == AF_INET) return "INET";
  #endif
  #ifdef AF_INET6
    if (family == AF_INET6) return "INET6";
  #endif
  #ifdef AF_IPX
    if (family == AF_IPX) return "IPX";
  #endif
  #ifdef AF_NETLINK
    if (family == AF_NETLINK) return "NETLINK";
  #endif
  #ifdef AF_X25
    if (family == AF_X25) return "X25";
  #endif
  #ifdef AF_AX25
    if (family == AF_AX25) return "AX25";
  #endif
  #ifdef AF_ATMPVC
    if (family == AF_ATMPVC) return "ATMPVC";
  #endif
  #ifdef AF_APPLETALK
    if (family == AF_APPLETALK) return "APPLETALK";
  #endif
  #ifdef AF_PACKET
    if (family == AF_PACKET) return "PACKET";
  #endif
  return NULL;
}

const char* duv_type_to_string(duv_type_t type) {
  switch (type) {
    case DUV_TIMER: return "uv_timer_t";
    case DUV_PREPARE: return "uv_prepare_t";
    case DUV_CHECK: return "uv_check_t";
    case DUV_IDLE: return "uv_idle_t";
    case DUV_ASYNC: return "uv_async_t";
    case DUV_POLL: return "uv_poll_t";
    case DUV_SIGNAL: return "uv_signal_t";
    case DUV_PROCESS: return "uv_process_t";
    case DUV_TCP: return "uv_tcp_t";
    case DUV_PIPE: return "uv_pipe_t";
    case DUV_TTY: return "uv_tty_t";
    case DUV_UDP: return "uv_udp_t";
    case DUV_FS_EVENT: return "uv_fs_event_t";
    case DUV_FS_POLL: return "uv_fs_poll_t";
  }
  return "unknown";
}
const char* duv_mask_to_string(duv_type_mask_t mask) {
  switch (mask) {
    case DUV_HANDLE_MASK: return "uv_handle_t";
    case DUV_TIMER_MASK: return "uv_timer_t";
    case DUV_PREPARE_MASK: return "uv_prepare_t";
    case DUV_CHECK_MASK: return "uv_check_t";
    case DUV_IDLE_MASK: return "uv_idle_t";
    case DUV_ASYNC_MASK: return "uv_async_t";
    case DUV_POLL_MASK: return "uv_poll_t";
    case DUV_SIGNAL_MASK: return "uv_signal_t";
    case DUV_PROCESS_MASK: return "uv_process_t";
    case DUV_STREAM_MASK: return "uv_stream_t";
    case DUV_TCP_MASK: return "uv_tcp_t";
    case DUV_PIPE_MASK: return "uv_pipe_t";
    case DUV_TTY_MASK: return "uv_tty_t";
    case DUV_UDP_MASK: return "uv_udp_t";
    case DUV_FS_EVENT_MASK: return "uv_fs_event_t";
    case DUV_FS_POLL_MASK: return "uv_fs_poll_t";
  }
  return "unknown";
}

uv_loop_t* duv_loop(duk_context *ctx) {
  duk_memory_functions funcs;
  duk_get_memory_functions(ctx, &funcs);
  return funcs.udata;
}

void duv_push_status(duk_context *ctx, int status) {
  if (status < 0) {
    duk_push_error_object(ctx, DUK_ERR_ERROR, "%s: %s", uv_err_name(status), uv_strerror(status));
  }
  else {
    duk_push_null(ctx);
  }
}

void duv_error(duk_context *ctx, int status) {
  duk_error(ctx, DUK_ERR_ERROR, "%s: %s", uv_err_name(status), uv_strerror(status));
}

void duv_check(duk_context *ctx, int status) {
  if (status < 0) duv_error(ctx, status);
}

#define KEYLEN sizeof(void*)*2+1
static char key[KEYLEN];

// Assumes buffer is at top of stack.
// replaces with this on top of stack.
void duv_setup_handle(duk_context *ctx, uv_handle_t *handle, duv_type_t type) {
  // Insert this before buffer
  duk_push_this(ctx);

  // Set buffer as uv-data internal property.
  duk_insert(ctx, -2);
  duk_put_prop_string(ctx, -2, "\xff""uv-data");

  // Set uv-type from provided parameter.
  duk_push_int(ctx, type);
  duk_put_prop_string(ctx, -2, "\xff""uv-type");

  // Store this object in the heap stack keyed by the handle's pointer address.
  // This will prevent it from being garbage collected and allow us to find
  // it with nothing more than the handle's address.
  duv_store_handle(ctx, handle);

  // Store the context in the handle so it can use duktape APIs.
  handle->data = ctx;
}

void duv_setup_request(duk_context *ctx, uv_req_t* req, int callback, void* extra) {
  // Create a new container object for the request
  duk_push_object(ctx);
  // TODO: should we have a shared prototype for uv_cancel_t and toString?
  //       Currently these objects aren't exposed to the user.

  // Set buffer as uv-data internal property.
  duk_insert(ctx, -2);
  duk_put_prop_string(ctx, -2, "\xff""uv-data");

  // Store a reference to the lua callback
  duk_dup(ctx, callback);
  duk_put_prop_string(ctx, -2, "\xff""uv-callback");

  if (extra) {
    duk_push_pointer(ctx, extra);
    duk_put_prop_string(ctx, -2, "\xff""uv-extra");
  }

  // Store this object in the heap stack keyed by the request's pointer address.
  // This will prevent it from being garbage collected and allow us to find
  // it with nothing more than the request's address.
  duv_store_handle(ctx, req);

  // Store the context in the handle so it can use duktape APIs.
  req->data = ctx;
}

void duv_store_handle(duk_context *ctx, void *handle) {
  duk_push_heap_stash(ctx);
  duk_dup(ctx, -2);
  snprintf(key, KEYLEN, "%"PRIXPTR, (uintptr_t)handle);
  duk_put_prop_string(ctx, -2, key);
  duk_pop(ctx);
}

void duv_remove_handle(duk_context *ctx, void *handle) {
  duk_push_heap_stash(ctx);
  snprintf(key, KEYLEN, "%"PRIXPTR, (uintptr_t)handle);
  duk_del_prop_string(ctx, -1, key);
  duk_pop(ctx);
}

void duv_push_handle(duk_context *ctx, void *handle) {
  duk_push_heap_stash(ctx);
  snprintf(key, KEYLEN, "%"PRIXPTR, (uintptr_t)handle);
  duk_get_prop_string(ctx, -1, key);
  duk_remove(ctx, -2);
}

void* duv_get_handle(duk_context *ctx, int index) {
  duk_get_prop_string(ctx, index, "\xff""uv-data");
  void* handle = duk_get_buffer(ctx, -1, 0);
  duk_pop(ctx);
  return handle;
}

duk_bool_t duv_is_handle_of(duk_context *ctx, int index, duv_type_mask_t mask) {
  if (!duk_is_object(ctx, index)) return 0;
  duk_get_prop_string(ctx, index, "\xff""uv-type");
  int type = duk_get_int(ctx, -1);
  duk_bool_t is = (1 << type) & mask;
  duk_pop(ctx);
  return is;
}

void* duv_require_this_handle(duk_context *ctx, duv_type_mask_t mask) {
  duk_push_this(ctx);
  if (!duv_is_handle_of(ctx, -1, mask)) {
    duk_pop(ctx);
    duk_error(ctx, DUK_ERR_TYPE_ERROR, "this must be %s", duv_mask_to_string(mask));
  }
  void *handle = duv_get_handle(ctx, -1);
  duk_insert(ctx, 0);
  return handle;
}

void duv_emit(uv_handle_t* handle, const char* key, int nargs, int cleanup) {
  duk_context *ctx = handle->data;
  duv_push_handle(ctx, handle);
  // stack: args... this
  duk_get_prop_string(ctx, -1, key);
  // stack: args... this fn
  if (cleanup) duk_del_prop_string(ctx, -2, key);
  // stack: args... this fn
  if (!duk_is_function(ctx, -1)) {
    duk_pop_n(ctx, 2 + nargs);
    return;
  }
  duk_insert(ctx, -(nargs + 2));
  // stack: fn args... this
  duk_insert(ctx, -(nargs + 1));
  // stack: fn this args...
  duk_call_method(ctx, nargs);
  // stack: result
  duk_pop(ctx);
}

// Assumes nargs are the top of the stack.  Rest comes from request
// Return value is not left on the stack.
void duv_resolve(uv_req_t* req, int nargs) {
  duk_context *ctx = req->data;
  duv_push_handle(ctx, req);
  // stack: args... obj
  duk_get_prop_string(ctx, -1, "\xff""uv-callback");
  // stack: args... obj callback
  duk_del_prop_string(ctx, -2, "\xff""uv-callback");
  // stack: args... obj callback

  if (!duk_is_function(ctx, -1)) {
    // stack: args... obj callback
    duk_pop_n(ctx, 2 + nargs);
    return;
  }
  duk_remove(ctx, -2);
  // stack: args... callback
  duk_insert(ctx, -(nargs + 1));
  // stack: callback args...
  duk_call(ctx, nargs);
  // stack: result
  duk_pop(ctx);

  // Remove the request from the GC roots
  duv_remove_handle(ctx, req);
}

void duv_get_data(duk_context *ctx, int index, uv_buf_t *buf) {
  if (duk_is_string(ctx, index)) {
    buf->base = (char*) duk_get_lstring(ctx, index, &buf->len);
  }
  else {
    buf->base = duk_get_buffer(ctx, index, &buf->len);
  }
}
