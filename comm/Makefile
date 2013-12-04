CPP = g++
AR = ar

FLAGS = -Wall -Werror
FLAGS += -g
INCS = -I./ 

LIBS = -L./

OBJS =  CDataParse.o Common.o
TARGET = libcomm.a 

all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) cru $(TARGET) $(OBJS)

%.o: %.cpp
	$(CPP) -o $@ -c $< $(FLAGS) $(INCS)

test: $(TARGET) test.o
	$(CPP) -o test test.o $(TARGET) $(LIBS)

.PHONY: clean
clean:
	rm -f $(TARGET)
	rm -f $(OBJS)
	rm -f test test.o
