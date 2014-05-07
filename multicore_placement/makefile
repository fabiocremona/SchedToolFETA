#CXXFLAGS = -Wall --std=c++11
CXXFLAGS = -Wall --std=c++11 -fopenmp
LDFLAGS =
#CPP = g++
CPP = g++-mp-4.9
#LD = g++
LD = g++-mp-4.9

#GTEST_HEADERS = $(STEST_DIR)/include/
#LDFLAGS += $(GTEST_DIR)/src/gtest_amain.o
#LDFLAGS += $(GTEST_DIR)/src/gtest-all.o

.SUFFIXES:
.SUFFIXES: .o .cpp
EXECS = mcmr


SRCS = main.cpp Component.cpp Parser.cpp Function.cpp Task.cpp Extras.cpp TaskSet.cpp \
		Feta.cpp RtwOpt.cpp Block.cpp Processor.cpp Allocation.cpp sa_solver.cpp

OBJS = ${SRCS:.cpp=.o}

all: $(EXECS)

.cpp.o:
	$(CPP) $(CXXFLAGS) -c $<

mcmr: $(OBJS)
	$(LD) $(LDFLAGS) -fopenmp -o $@ $^

clean:
	rm -rf *.o
	rm -rf $(EXECS)

cleanedit:
	rm -rf *~
