CXX=g++
CPPFLAGS=-W -Wall -fexceptions -finline-functions -lquickfix -std=c++11
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
