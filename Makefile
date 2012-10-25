
CXXFLAGS += -std=c++0x

ifneq ($(DEBUG),)
CXXFLAGS += -g
else
CXXFLAGS += -O3 -DNDBEBUG -msse2 -fno-rtti -fno-exceptions
endif


ifneq ($(DO_PROFILING),)
CXXFLAGS += -pg
endif


DATAFILES = txt/
OUTFILES = test_output1.txt test_output2.txt gmon.out profile.txt

VARIANTS = std-strstr straightforward lipman fatlobyte
TARGETS =  $(foreach var,$(VARIANTS),searchwords_$(var))

all: $(TARGETS)

searchwords_%: main.o search_%.o
	$(CXX) $(CXXFLAGS) -o $@ $^

ifneq ($(DO_PROFILING),)
.PHONY: analyze_%
analyze_%: searchwords_% txt/
	./$<
	gprof ./$< gmon.out > profile.txt
endif

txt/:
	unzip texte.zip

.PHONY: clean
clean: clean_output
	rm -rf $(TARGETS) *.o

.PHONY: clean-all
clean-all: clean clean_data

.PHONY: clean_output
clean_output:
	rm -rf $(OUTFILES)

.PHONY: clean_data
clean_data:
	rm -rf $(DATAFILES)
