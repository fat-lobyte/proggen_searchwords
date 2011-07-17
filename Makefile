
CXXFLAGS += -std=c++0x -fno-deduce-init-list
# -fno-deduce-init-list not quite sure what GCC wants with this warning, but stuff works so I'll
# disable it.

OUTFILES = test_output1.txt test_output2.txt

VARIANTS = std-strstr straightforward blockwise
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

