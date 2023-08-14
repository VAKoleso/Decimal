prj 			:= 	s21_decimal
prj_lib			:=	$(prj).a
prj_o			:=  $(prj).o
CFLAGS			:=	-std=c11 -Wall -Werror -Wextra
gcov 			:= 	--coverage -g
gcc_cc			:=	gcc #$(CFLAGS)
gcc_gcov 		:=	$(gcc_cc) $(gcov)
gcc_test		:=	gcc
СС				:=	$(gcc_cc)
build_dir		:=
# определяем ОС специфед -lcheck
UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
	LCHECK = $(shell pkg-config --cflags --libs check) -lm
	LEAKS_CMD = CK_FORK=no valgrind  --tool=memcheck --leak-check=full --track-origins=yes --leak-check=full --show-leak-kinds=all
endif
ifeq ($(UNAME), Darwin)
	LCHECK = -lcheck
	LEAKS_CMD = CK_FORK=no leaks --atExit -- 
endif


.PHONY:	$(prj_lib) $(prj_template) gcov_report clean test s21_math.a gcov_report $(gcc_gcov)
.IGNORE: $(prj_lib) cppcheck cpplint s21_test.out report/s21_test.out test s21_decimal.a

all_c_files 	:= $(wildcard *.c)
all_o_files		:= $(patsubst %.c, %.o, $(all_c_files))

#	lm - подключает библиотеку m (math.h)
debug_%:
	gcc -D $@ $(all_c_files) -lm

all: clean test $(prj_lib) 

clean: clean_wo_report
	rm -rf report/*

clean_wo_report:
	rm -rf *.o *.a *.out tests/*.info tests/*.out *.gc* tests/rm/*

$(prj_lib):
	$(СС) -c $(all_c_files)
	ar rc $(build_dir)$(prj).a $(all_o_files)
	ranlib $(build_dir)$(prj).a

cpplint:
	cp ../materials/linters/CPPLINT.cfg CPPLINT.cfg
	python3 ../materials/linters/cpplint.py --extensions=c *.c
	python3 ../materials/linters/cpplint.py --extensions=c tests/*.c
	rm -rf CPPLINT.cfg
cppcheck: 
	cppcheck --enable=all --suppress=missingIncludeSystem *.c

check: cpplint cppcheck

#export CK_MAX_MSG_SIZE=4096000000
test: clean $(prj_lib) cpplint #cppcheck 
	$(gcc_test) tests/s21_test.c $(prj_lib) $(LCHECK)
	./a.out
	make clean

test1: clean $(prj_lib) 
	$(gcc_test) other_tests/test1.c $(prj_lib) $(LCHECK)
	./a.out
	make clean

test2: clean $(prj_lib) 
	$(gcc_test) other_tests/test2.c $(prj_lib) $(LCHECK)
	./a.out
	make clean

gcov_report: СС:=$(gcc_gcov)

gcov_report: build_dir:=report/

gcov_report: clean $(prj_template) $(prj_lib) 
	$(gcc_gcov) tests/s21_test.c $(build_dir)$(prj).a -o $(build_dir)s21_test.out $(LCHECK)
	$(build_dir)s21_test.out
	lcov -t "s21_math" -o $(build_dir)s21_test.info -c -d .
	genhtml -o report $(build_dir)s21_test.info
	make clean_wo_report
