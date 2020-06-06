THEFT_LIBS  := $(shell pkg-config libtheft --libs)
THEFT_FLAGS := $(shell pkg-config libtheft --cflags)

all: proptest

proptest: proptest.o
proptest: override CFLAGS += $(THEFT_FLAGS) -ggdb -std=c99
proptest: override LDLIBS += $(THEFT_LIBS)

clean:
	$(RM) proptest.o proptest

check: proptest
	@$(<D)/$<

.PHONY: check
