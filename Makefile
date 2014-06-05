CXXFLAGS+=-c -Wall -Werror -O2 -g -I.
CFLAGS+=-I. -O2
LDFLAGS+=-L./ -g -O2

TEST=test

all:  $(TEST)

$(TEST):	test.o serial_io.o FDBlocking.o mezz_tester.o 
	$(CXX) $(LDFLAGS) -o $@ $^

clean:
	@rm $(TEST) > /dev/null 2>&1
	@rm *.o > /dev/null 2>&1
	@rm *~ > /dev/null 2>&1
