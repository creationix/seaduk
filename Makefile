LIBUV= deps/libuv

# Make sure to `make distclean` before building when changing CC.
# Default build is debug mode.
CC= cc -g
# Uncomment the following to make a small binary
#CC= cc -Os
# Uncomment the following to make a static musl binary on linux
# CC= musl-gcc -Os -static
# export CC

BINS=\
        target/main.o\

LIBS=\
	target/env.o\
	target/path.o\
	target/miniz.o\
	target/libduv.a\
	target/duktape.o

DUV_LIBS=\
	target/duv_loop.o\
	target/duv_handle.o\
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

all: 	target/libseaduk.so target/libduv.so target/nucleus target/libuv.a

lib: 	target/libseaduk.so target/libduv.so target/libduv.a

target/libseaduk.so: ${LIBS}
	${CC} $^ -shared -fPIC -L/usr/local/lib -luv -pthread -o $@

target/libduv.so: ${DUV_LIBS}
	${CC} $^ -shared -fPIC -L/usr/local/lib -luv -pthread -lseaduk -Ltarget -o $@

target/nucleus: ${BINS} ${LIBS}
	${CC} $^ -lm -L/usr/local/lib -luv -pthread -lseaduk -Ltarget -o $@

install: install-bin install-lib install-header

install-bin: target/nucleus
	install $< /usr/local/bin/

install-lib: target/libseaduk.so target/libduv.so
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
	${CC} -std=c99 -Wall -Wextra -pedantic -Werror -c $< -o $@

target/path.o: src/path.c src/path.h
	${CC} -std=c99 -Wall -Wextra -pedantic -Werror -c $< -o $@

target/main.o: src/main.c src/*.h
	${CC} -std=c99 -Wall -Wextra -pedantic -Werror -c $< -o $@

target/duktape.o: deps/duktape-releases/src/duktape.c deps/duktape-releases/src/duktape.h
	${CC} -std=c99 -Wall -Wextra -pedantic -c $< -o $@

target/miniz.o: deps/miniz.c
	${CC} -std=gnu99 -c $< -o $@

target/libuv.a: ${LIBUV}/.libs/libuv.a
	cp $< $@

target/libduv.a: ${DUV_LIBS}
	${AR} cr $@ ${DUV_LIBS}

target/duv_%.o: src/duv/%.c src/duv/%.h
	${CC} -std=c99 -D_POSIX_C_SOURCE=200112 -Wall -Wextra -pedantic -Werror -c $< -o $@

init-duktape:
	git submodule init deps/duktape-releases
	git submodule update deps/duktape-releases

init-libuv:
	git submodule init deps/libuv
	git submodule update deps/libuv

${LIBUV}/.libs/libuv.a: ${LIBUV}/Makefile
	${MAKE} -C ${LIBUV}

${LIBUV}/Makefile: ${LIBUV}/configure
	cd ${LIBUV}; ./configure; cd -

${LIBUV}/configure: ${LIBUV}/autogen.sh
	cd ${LIBUV}; ./autogen.sh; cd -

clean:
	rm -rf target/*

distclean: clean
	cd ${LIBUV}; git clean -xdf; cd -
