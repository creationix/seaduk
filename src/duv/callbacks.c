#include "callbacks.h"
#include "utils.h"

void duv_on_close(uv_handle_t *handle) {
  duv_call_callback(handle, "\xffon-close", 0, (const char*[]){
    "\xffon-close", NULL
  });
}

void duv_on_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
  (void)(handle);
  buf->base = malloc(suggested_size);
  buf->len = suggested_size;
}

void duv_on_timeout(uv_timer_t *timer) {
  duv_call_callback((uv_handle_t*)timer, "\xffon-timeout", 0, NULL);
}

void duv_on_prepare(uv_prepare_t *prepare) {
  duv_call_callback((uv_handle_t*)prepare, "\xffon-prepare", 0, NULL);
}

void duv_on_check(uv_check_t *check) {
  duv_call_callback((uv_handle_t*)check, "\xffon-check", 0, NULL);
}

void duv_on_idle(uv_idle_t *idle) {
  duv_call_callback((uv_handle_t*)idle, "\xffon-idle", 0, NULL);
}

void duv_on_async(uv_async_t *async) {
  duv_call_callback((uv_handle_t*)async, "\xffon-async", 0, NULL);
}

void duv_on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
  duk_context *ctx = stream->data;
  if (nread >= 0) {
    char* out;
    duk_push_null(ctx);
    out = duk_push_fixed_buffer(ctx, nread);
    memcpy(out, buf->base, nread);
  }
  free(buf->base);
  if (nread == 0) return;
  if (nread == UV_EOF) {
    duk_push_null(ctx); // no error
    duk_push_undefined(ctx); // undefined value to signify EOF
  }
  else if (nread < 0) {
    duv_push_status(ctx, nread);
    duk_push_undefined(ctx);
  }
  duv_call_callback((uv_handle_t*)stream, "\xffon-read", 2, NULL);
}

void duv_on_write(uv_write_t *write, int status) {
  uv_stream_t *stream = write->handle;
  duk_context *ctx = stream->data;
  duv_push_status(ctx, status);
  duv_call_callback((uv_handle_t*)stream, "\xffon-write", 1, (const char*[]){
    "\xffon-write", "\xffreq-write", NULL
  });
}

void duv_on_connect(uv_connect_t *connect, int status) {
  uv_stream_t *stream = connect->handle;
  duk_context *ctx = stream->data;
  duv_push_status(ctx, status);
  duv_call_callback((uv_handle_t*)stream, "\xffon-connect", 1, (const char*[]){
    "\xffon-connect", "\xffreq-connect", NULL
  });
}

void duv_on_shutdown(uv_shutdown_t *shutdown, int status) {
  uv_stream_t *stream = shutdown->handle;
  duk_context *ctx = stream->data;
  duv_push_status(ctx, status);
  duv_call_callback((uv_handle_t*)stream, "\xffon-shutdown", 1, (const char*[]){
    "\xffon-shutdown", "\xffreq-shutdown", NULL
  });
}

void duv_on_connection(uv_stream_t *server, int status) {
  duk_context *ctx = server->data;
  if (status) {
    duk_push_error_object(ctx, DUK_ERR_ERROR, "%s: %s", uv_err_name(status), uv_strerror(status));
  }
  else {
    duk_push_null(ctx);
  }
  duv_call_callback((uv_handle_t*)server, "\xffon-connection", 1, NULL);
}
