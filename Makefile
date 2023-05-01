PROG = lab08
OBJS = lab08.o async.o
TMP = $(PROG) $(OBJS) *.tmp

%.o: %.c
	gcc -c -g -o $@ $<

$(PROG): $(OBJS)
	gcc -g -o $@ $^

clean:
	rm -rf $(TMP)
