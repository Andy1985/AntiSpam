CPP = g++
AR = ar
RANLIB = ranlib

FLAGS = -Wall #-Werror
FLAGS += -g
INCS = -I./ 

LIBS = -L./ -lscws -lhiredis

LIBFENCI_SRC = ./fenci
LIBFENCI = $(LIBFENCI_SRC)/libfenci.a
INCS += -I$(LIBFENCI_SRC)

LIBMIME_SRC = ./mime
LIBMIME = $(LIBMIME_SRC)/libmime.a
INCS += -I$(LIBMIME_SRC)

LIBREDIS_SRC = ./redis
LIBREDIS = $(LIBREDIS_SRC)/libredis.a
INCS += -I$(LIBREDIS_SRC)

LIBCOMM_SRC = ./comm
LIBCOMM = $(LIBCOMM_SRC)/libcomm.a
INCS += -I$(LIBCOMM_SRC)

#bayes
LIBGSL_SRC=./bayes
LIBGSL = $(LIBGSL_SRC)/libgsl.a
INCS += -I./bayes -I./bayes/gsl

OBJS = CAntiSpamMail.o

TARGET = test

all: $(TARGET) feed lexer

$(TARGET): test.cpp $(OBJS) $(LIBMIME) $(LIBFENCI) $(LIBREDIS) $(LIBCOMM) $(LIBGSL)
	$(CPP) -o $(TARGET) test.cpp $(LIBS) $(INCS) $(OBJS) $(LIBMIME) $(LIBFENCI) $(LIBREDIS) $(LIBCOMM) $(LIBGSL)

feed: feed.cpp $(LIBMIME) $(LIBFENCI) $(LIBREDIS) $(LIBCOMM) $(LIBGSL)
	$(CPP) -o feed feed.cpp $(LIBS) $(LIBMIME) $(LIBFENCI) $(LIBREDIS) $(LIBCOMM) $(LIBGSL) $(INCS)

lexer: lexer.cpp $(LIBMIME) $(LIBFENCI) $(LIBREDIS) $(LIBCOMM) $(LIBGSL)
	$(CPP) -o lexer lexer.cpp $(LIBS) $(LIBMIME) $(LIBFENCI) $(LIBREDIS) $(LIBCOMM) $(LIBGSL) $(INCS)

%.o: %.cpp
	$(CPP) -o $@ -c $< $(FLAGS) $(INCS)

$(LIBFENCI):
	make -C $(LIBFENCI_SRC)

$(LIBMIME):
	make -C $(LIBMIME_SRC)

$(LIBREDIS):
	make -C $(LIBREDIS_SRC)

$(LIBCOMM):
	make -C $(LIBCOMM_SRC)

$(LIBGSL):
	make -C $(LIBGSL_SRC)

.PHONY: clean
clean:
	make -C $(LIBMIME_SRC) clean
	make -C $(LIBFENCI_SRC) clean
	make -C $(LIBREDIS_SRC) clean
	make -C $(LIBCOMM_SRC) clean
	make -C $(LIBGSL_SRC) clean
	rm -f $(TARGET)
	rm -f $(OBJS)
	rm -f test
	rm -f feed
	rm -f lexer
