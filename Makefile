
CXXFLAGS += -std=c++0x -fno-deduce-init-list -O3 -pg
# -fno-deduce-init-list not quite sure what GCC wants with this warning, but stuff works so I'll
# disable it.

OUTFILES = test_output1.txt test_output2.txt gmon.out

VARIANTS = std-strstr straightforward lipman
TARGETS =  $(foreach var,$(VARIANTS),searchwords_$(var))

all: $(TARGETS)

searchwords_%: main.o search_%.o
	$(CXX) $(CXXFLAGS) -o $@ $^

.PHONY: clean
clean: clean_output
	rm -rf $(TARGETS) *.o

.PHONY: clean_output
clean_output:
	rm -rf $(OUTFILES)

