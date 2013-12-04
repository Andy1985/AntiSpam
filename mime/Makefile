CPP = g++
AR = ar

FLAGS = -Wall #-Werror
FLAGS += -g
INCS = -I./ 

LIBS = -L./ 

OBJS  = CharsetUtils.o MimeActivation.o MimeUtility.o MimeObject.o \
           MimeContainer.o MimeEntity.o MimeParser.o MimeMessage.o

TARGET = libmime.a 

all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) cru $(TARGET) $(OBJS)

%.o: %.cpp
	$(CPP) -o $@ -c $< $(FLAGS) $(INCS)

.PHONY: clean
clean:
	rm -f $(TARGET)
	rm -f $(OBJS)
