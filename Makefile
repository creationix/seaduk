LIBUV= deps/libuv

# Make sure to `make distclean` before building when changing CC.
# Default build is debug mode.
CC= cc -g
# Uncomment the following to make a small binary
# CC= cc -Os
# Uncomment the following to make a static musl binary on linux
# CC= musl-gcc -Os -static

LIBS=\
  target/main.o\
	target/miniz.o\
	target/duktape.o\
	target/duv.a\
	target/libuv.a\
	target/env.o\
	rust_path/target/release/libc_path.a

target/nucleus: ${LIBS}
	${CC} $^ -lm -lpthread -ldl -o $@

rust_path/target/release/libc_path.a: rust_path/src/lib.rs rust_path/src/helpers.rs
	cd rust_path && cargo build --release

install: target/nucleus
	install $< /usr/local/bin/

test: test-dir test-zip test-app test-app-tiny

test-dir: target/nucleus
	$< test-app -- 1 2 3

test-zip: target/nucleus target/test-app.zip
	$^ -- 4 5 6

test-app: target/app
	$< 7 8 9

test-app-tiny: target/app-tiny
	$< 10 11 12

target/app: target/nucleus target/test-app.zip
	cat $^ > $@
	chmod +x $@

target/app-tiny: prefix target/test-app.zip
	cat $^ > $@
	chmod +x $@

prefix: target/nucleus
	echo "#!$(shell pwd)/target/nucleus --" > prefix

target/test-app.zip: test-app/* test-app/deps/*
	rm -f app.zip
	cd test-app; zip -9 -r ../$@ .; cd -

target/env.o: src/env.c src/env.h
	${CC} -std=gnu99 -Wall -Wextra -pedantic -Werror -c $< -o $@

target/main.o: src/main.c src/*.h
	${CC} -std=gnu99 -Wall -Wextra -pedantic -Werror -c $< -o $@

target/duv.a: src/duv/duv.a
	cp $< $@

src/duv/duv.a: src/duv/*.c src/duv/*.h
	${MAKE} -C src/duv

target/duktape.o: deps/duktape-releases/src/duktape.c deps/duktape-releases/src/duktape.h
	${CC} -std=c99 -Wall -Wextra -pedantic -c $< -o $@

target/miniz.o: deps/miniz.c
	${CC} -std=gnu99 -c $< -o $@

target/libuv.a: ${LIBUV}/.libs/libuv.a
	cp $< $@

${LIBUV}/.libs/libuv.a: ${LIBUV}/Makefile
	${MAKE} -C ${LIBUV}

${LIBUV}/Makefile: ${LIBUV}/configure
	cd ${LIBUV}; ./configure; cd -

${LIBUV}/configure: ${LIBUV}/autogen.sh
	cd ${LIBUV}; ./autogen.sh; cd -

clean:
	rm -rf target/* rust_path/target
	${MAKE} -C src/duv clean

distclean: clean
	cd ${LIBUV}; git clean -xdf; cd -
