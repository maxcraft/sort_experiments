.PHONY: all clean

SRC := $(wildcard *.cpp)
OBJ := $(SRC:.cpp=.o)
BIN := sort_test
DEP = $(OBJ:.o=.d)

CXXFLAGS += -Wall -O2

CXXFLAGS += -MMD

all: $(BIN)

sort_test: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	$(RM) $(BIN) $(OBJ) $(DEP)

-include $(DEP)
