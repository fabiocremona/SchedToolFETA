#GTEST_DIR = /Usrs/fabiocremona/Documents/gtest-1.6.0
CXXFLAGS = -Wall --std=c++11
LDFLAGS =
CPP = g++
LD = g++

#GTEST_HEADERS = $(STEST_DIR)/include/
#LDFLAGS += $(GTEST_DIR)/src/gtest_amain.o
#LDFLAGS += $(GTEST_DIR)/src/gtest-all.o

.SUFFIXES:
.SUFFIXES: .o .cpp
EXECS = scmr


SRCS = main.cpp Component.cpp Parser.cpp Function.cpp Task.cpp Extras.cpp TaskSet.cpp Feta.cpp RtwOpt.cpp Block.cpp

OBJS = ${SRCS:.cpp=.o}

all: $(EXECS)

.cpp.o:
	$(CPP) $(CXXFLAGS) -c $<

scmr: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

clean:
	rm -rf *.o
	rm -rf $(EXECS)

cleanedit:
	rm -rf *~
