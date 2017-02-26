CXX=g++-4.9
CPPFLAGS=-W -Wall -fexceptions -finline-functions -L./quickfix/src/C++/.libs/ -lquickfix -std=c++11 -Iquickfix/include -Wno-ignored-qualifiers -Wno-unused-parameter -Wno-missing-field-initializers
DEPS = recorder.h
SRCS = recorder.cpp sha256.cpp base64.cpp
OBJS = $(subst .cpp,.o,$(SRCS))

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CPPFLAGS)

recorder: $(OBJS)
	$(CXX) -o $@ $(OBJS) $(CPPFLAGS)

all: recorder

clean:
	rm -f recorder *.o
