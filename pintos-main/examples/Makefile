SRCDIR = ..

# Test programs to compile, and a list of sources for each.
# To add a new test, put its name on the PROGS list
# and then add a name_SRC line that lists its source files.
PROGS = cat cmp cp echo halt hex-dump rm \
	lineup recursor lab1test lab2test lab2test_new lab4test1 lab4test2 \
	printf recursor_ng noop

# The example files should start to work as intended in the following order: 
# Should work once the main-stack is correctly setup (Lab 1)
noop_SRC = noop.c

# Should work once the write() syscall is implemented (Lab 2)
lab1test_SRC = lab1test.c

# Should work once the basic system call handler is implemented (Lab 2)
lab2test_SRC = lab2test.c
lab2test_new_SRC = lab2test_new.c
printf_SRC = printf.c
cat_SRC = cat.c
cmp_SRC = cmp.c
cp_SRC = cp.c
echo_SRC = echo.c
halt_SRC = halt.c
hex-dump_SRC = hex-dump.c
lineup_SRC = lineup.c
rm_SRC = rm.c

# Should work once exec() is implemented (Lab 4)
lab4test1_SRC = lab4test1.c
lab4test2_SRC = lab4test2.c

# Should work once wait() is implemented (Lab 5)
recursor_SRC = recursor.c
recursor_ng_SRC = recursor_ng.c

include $(SRCDIR)/Make.config
include $(SRCDIR)/Makefile.userprog
