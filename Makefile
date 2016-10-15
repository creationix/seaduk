#
# Set LIBUV to deps/libuv (for the submodule version), run make init-libuv to checkout the submodule
# Set LIBUV to system to use the version deployed in the system
# Set LIBUV to pkgconfig to use pkg-config for the setup
#
#UVSRC=pkg-config
UVSRC=git

CC=musl-gcc -static
export CC

#
# Define buildtype : shared or static
#
BUILDTYPE=static
#BUILDTYPE=shared

# Make sure to `make distclean` before building when changing CC.
# Default build is debug mode.
CFLAGS=-g
# Uncomment the following to make a small binary
#CFLAGS=-Os
# Uncomment the following to make a static musl binary on linux, set BUILTYPE to static
#CFLAGS=-Os -static
#CC=musl-gcc
#BUILDTYPE=static
#export CC

LDFLAGS+=-L/usr/local/lib -Ltarget -luv

BINS=\
        target/main.o\

LIBS=\
	target/env.o\
	target/path.o\
	target/miniz.o\
	target/libduv.a\
	target/mbed.a\
	target/http-parser.o\
	target/duktape.o

MBED_LIBS=\
	target/mbed_md5.o \
  target/mbed_sha1.o\
	target/mbed_sha256.o\
	target/mbed_sha512.o

DUV_LIBS=\
	target/duv_loop.o\
	target/duv_handle.o\
	target/duv_req.o\
	target/duv_timer.o\
	target/duv_prepare.o\
	target/duv_check.o\
	target/duv_idle.o\
	target/duv_async.o\
	target/duv_poll.o\
	target/duv_signal.o\
	target/duv_process.o\
	target/duv_stream.o\
	target/duv_tcp.o\
	target/duv_pipe.o\
	target/duv_tty.o\
	target/duv_udp.o\
	target/duv_fs_event.o\
	target/duv_fs_poll.o\
	target/duv_fs.o\
	target/duv_dns.o\
	target/duv_misc.o\
	target/duv_duv.o\
	target/duv_utils.o\
	target/duv_callbacks.o\
	target/duv_dschema.o

DUV_HEADER=\
	deps/duktape-releases/src/duktape.h\
	deps/duktape-releases/src/duk_config.h\
	src/duv/duv.h


LIBUV=deps/libuv
MBEDTLS=deps/mbedtls

ifeq ($(BUILDTYPE), shared)
   CFLAGS+=-fPIC
   LDFLAGS+=-lseaduk
   UVTARGET=target/libuv.so
else
   UVTARGET=target/libuv.a
endif
ifeq ($(UVSRC), pkg-config)
   CFLAGS+=$(shell pkg-config --cflags libuv)
   LDFLAGS+=$(shell pkg-config --libs libuv)
endif
ifeq ($(UVSRC), git)
   CFLAGS+=-Ideps/libuv/include
endif
ifeq ($(UVSRC), system)
   CFLAGS+=-I/usr/local/include
endif
CFLAGS+=-I${MBEDTLS}/include

all:		all-${BUILDTYPE}
install:	install-${BUILDTYPE}

all-static: 	${UVTARGET} target/libduv.a target/nucleus

all-shared: 	${UVTARGET} lib-shared target/nucleus

lib-static: 	target/libduv.a

lib-shared: 	target/libseaduk.so target/libduv.so

target/libseaduk.so: ${LIBS}
	${CC} $^ ${CFLAGS} -shared -pthread -o $@

target/libduv.so: ${DUV_LIBS}
	${CC} $^ ${LDFLAGS} ${CFLAGS} -shared -L/usr/local/lib -luv -pthread -o $@

target/nucleus: ${BINS} ${LIBS}
	${CC} $^ ${LDFLAGS} ${CFLAGS} -lm -L/usr/local/lib -luv -pthread -o $@

install-static: install-bin install-lib-static install-header
install-shared : install-bin install-lib-shared install-header

target/nucleus-tiny: target/nucleus
	cp $< $@
	strip $@
	upx $@

install-bin: target/nucleus-tiny
	install $< /usr/local/bin/

install-lib-static: target/libduv.a
	install $^ /usr/local/lib/

install-lib-shared: target/libseaduk.so target/libduv.so
	install $^ /usr/local/lib/

install-header: ${DUV_HEADER}
	mkdir -p /usr/local/include/duv
	install $^ /usr/local/include/duv/

test: test-dir test-zip test-app test-app-tiny test-path

test-path:
	$(CC) src/test-path.c
	./a.out
	rm -f a.out

test-dir: target/nucleus
	$< test-app -- 1 2 3

test-zip: target/nucleus target/test-app.zip
	$^ -- 4 5 6

test-app: target/app
	$< 7 8 9

test-app-tiny: target/app-tiny
	$< 10 11 12

target/app: target/nucleus test-app/* test-app/deps/*
	$< test-app -o $@

target/app-tiny: target/nucleus test-app/* test-app/deps/*
	$< test-app -l -o $@

target/test-app.zip: target/nucleus test-app/* test-app/deps/*
	$< test-app -z -o $@

target/env.o: src/env.c src/env.h
	${CC} -std=c99 ${CFLAGS} -Wall -Wextra -pedantic -Werror -c $< -o $@

target/path.o: src/path.c src/path.h
	${CC} -std=c99 ${CFLAGS} -Wall -Wextra -pedantic -Werror -c $< -o $@

target/main.o: src/main.c src/*.h
	${CC} -std=c99 ${CFLAGS} -Wall -Wextra -pedantic -Werror -c $< -o $@

target/duktape.o: deps/duktape-releases/src/duktape.c deps/duktape-releases/src/duktape.h
	${CC} -std=c99 ${CFLAGS} -Wall -Wextra -pedantic -c $< -o $@

target/miniz.o: deps/miniz.c
	${CC} -std=gnu99 ${CFLAGS} -c $< -o $@

target/http-parser.o: deps/http-parser/http_parser.c
	${CC} -std=c99 ${CFLAGS} -Wall -Wextra -pedantic -c $< -o $@

target/libuv.a: ${LIBUV}/.libs/libuv.a
	cp $< $@

target/libuv.so: ${LIBUV}/.libs/libuv.so
	@if [ `uname -s` == Linux ]; then cp ${LIBUV}/.libs/libuv.so target; fi
	@if [ `uname -s` == Darwin ]; then cp ${LIBUV}/.libs/libuv.dylib target; fi

target/libduv.a: ${DUV_LIBS}
	${AR} cr $@ ${DUV_LIBS}

target/mbed.a: ${MBED_LIBS}
	${AR} cr $@ ${MBED_LIBS}

target/duv_%.o: src/duv/%.c src/duv/%.h
	${CC} -std=c99 ${CFLAGS} -D_POSIX_C_SOURCE=200112 -Wall -Wextra -pedantic -Werror -c $< -I./deps/libuv/include -o $@

target/mbed_%.o: deps/mbedtls/library/%.c deps/mbedtls/include/mbedtls/%.h
	${CC} -std=c99 ${CFLAGS} -D_POSIX_C_SOURCE=200112 -Wall -Wextra -pedantic -Werror -c $< -I./deps/mbedtls/include -o $@

init-duktape:
	git submodule init deps/duktape-releases
	git submodule update deps/duktape-releases

init-libuv:
	git submodule init deps/libuv
	git submodule update deps/libuv

${LIBUV}/.libs/libuv.a: ${LIBUV}/Makefile
	${MAKE} -C ${LIBUV}

${LIBUV}/.libs/libuv.so: ${LIBUV}/Makefile
	${MAKE} -C ${LIBUV}

${LIBUV}/Makefile: ${LIBUV}/configure
	cd ${LIBUV}; ./configure; cd -

${LIBUV}/configure: ${LIBUV}/autogen.sh
	cd ${LIBUV}; ./autogen.sh; cd -

clean:
	rm -rf target/*

distclean: clean
	cd ${LIBUV}; git clean -xdf; cd -
