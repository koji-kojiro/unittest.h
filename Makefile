PROG=./bin/utest
SRCS=unittest.h ./src/utest.bash.tpl

.phony: all clean install

all: $(PROG)

$(PROG): $(SRCS)
	@python build.py
	@chmod +x $(PROG)

clean:
	@rm -fv $(PROG)
