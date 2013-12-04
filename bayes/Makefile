CPP = g++
CFLAG = -Wall -g
CFLAG += -DHIDE_INLINE_STATIC
INCS = -I./ -I./gsl
AR = ar

OBJS = libgsl_cdf_chisq.o libgsl_cdf_gamma.o libgsl_spec_gamma.o \
	libgsl_err_error.o libgsl_sys_infnan.o libgsl_sys_fdiv.o \
	libgsl_spec_exp.o libgsl_spec_trig.o libgsl_spec_log.o \
	libgsl_spec_psi.o libgsl_spec_zeta.o libgsl_spec_elementary.o \
	libgsl_err_stream.o libgsl_spec_expint.o libgsl_spec_erfc.o \
	libgsl_sys_coerce.o libgsl_sys_pow_int.o libgsl_sys_minmax.o

TARGET = libgsl.a

all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) cru $(TARGET) $(OBJS)

test: test.cpp $(OBJS)
	$(CPP) $(INCS) $(CFLAG) -o test test.cpp $(OBJS) -lm 
 
libgsl_cdf_chisq.o: ./gsl/cdf/chisq.c
	$(CPP) $(CFLAG) $(INCS) -o $@ -c ./gsl/cdf/chisq.c 
libgsl_cdf_gamma.o: ./gsl/cdf/gamma.c
	$(CPP) $(CFLAG) $(INCS) -o $@ -c ./gsl/cdf/gamma.c
libgsl_spec_gamma.o: ./gsl/specfunc/gamma_inc.c
	$(CPP) $(CFLAG) $(INCS) -o $@ -c ./gsl/specfunc/gamma_inc.c
libgsl_err_error.o: ./gsl/err/error.c
	$(CPP) $(CFLAG) $(INCS) -o $@ -c ./gsl/err/error.c
libgsl_sys_infnan.o: ./gsl/sys/infnan.c
	$(CPP) $(CFLAG) $(INCS) -o $@ -c ./gsl/sys/infnan.c
libgsl_sys_fdiv.o: ./gsl/sys/fdiv.c
	$(CPP) $(CFLAG) $(INCS) -o $@ -c ./gsl/sys/fdiv.c
libgsl_spec_exp.o: ./gsl/specfunc/exp.c
	$(CPP) $(CFLAG) $(INCS) -o $@ -c ./gsl/specfunc/exp.c
libgsl_spec_trig.o: ./gsl/specfunc/trig.c
	$(CPP) $(CFLAG) $(INCS) -o $@ -c ./gsl/specfunc/trig.c
libgsl_spec_log.o: ./gsl/specfunc/log.c
	$(CPP) $(CFLAG) $(INCS) -o $@ -c ./gsl/specfunc/log.c
libgsl_spec_psi.o: ./gsl/specfunc/psi.c
	$(CPP) $(CFLAG) $(INCS) -o $@ -c ./gsl/specfunc/psi.c
libgsl_spec_zeta.o: ./gsl/specfunc/zeta.c
	$(CPP) $(CFLAG) $(INCS) -o $@ -c ./gsl/specfunc/zeta.c
libgsl_spec_elementary.o: ./gsl/specfunc/elementary.c
	$(CPP) $(CFLAG) $(INCS) -o $@ -c ./gsl/specfunc/elementary.c
libgsl_err_stream.o: ./gsl/err/stream.c
	$(CPP) $(CFLAG) $(INCS) -o $@ -c ./gsl/err/stream.c
libgsl_spec_expint.o: ./gsl/specfunc/expint.c
	$(CPP) $(CFLAG) $(INCS) -o $@ -c ./gsl/specfunc/expint.c
libgsl_spec_erfc.o: ./gsl/specfunc/erfc.c
	$(CPP) $(CFLAG) $(INCS) -o $@ -c ./gsl/specfunc/erfc.c
libgsl_sys_coerce.o: ./gsl/sys/coerce.c
	$(CPP) $(CFLAG) $(INCS) -o $@ -c ./gsl/sys/coerce.c
libgsl_sys_pow_int.o: ./gsl/sys/pow_int.c
	$(CPP) $(CFLAG) $(INCS) -o $@ -c ./gsl/sys/pow_int.c
libgsl_sys_minmax.o: ./gsl/sys/minmax.c
	$(CPP) $(CFLAG) $(INCS) -o $@ -c ./gsl/sys/minmax.c

.PHONY: clean
clean:
	rm -f test
	rm -f $(OBJS)
	rm -f $(TARGET)
