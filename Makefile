
CXXFLAGS += -std=c++0x
OUTFILES = test_output1.txt test_output2.txt

VARIANTS = std-strstr straightforward
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

