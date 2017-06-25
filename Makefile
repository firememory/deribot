#include quickfix/Makefile

CXX=g++-4.9
CPPFLAGS=-W -Wall -fexceptions -finline-functions -std=c++11 -Iquickfix/include -Wno-ignored-qualifiers -Wno-unused-parameter -Wno-missing-field-initializers
LDFLAGS=-L./quickfix/src/C++/.libs/ -lquickfix -lcrypto
DEPS = recorder.h tools.h
SRCS = recorder.cpp tools.cpp
OBJS = $(subst .cpp,.o,$(SRCS))

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CPPFLAGS)

recorder: $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)

all: recorder

clean:
	rm -f recorder *.o
