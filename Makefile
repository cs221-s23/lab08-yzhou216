PROG = lab08
OBJS = lab08.o
TMP = $(PROG) $(OBJS) *.html *.tmp

%.o: %.c
	gcc -c -g -o $@ $<

$(PROG): $(OBJS)
	gcc -g -o $@ $^

clean:
	rm -rf $(TMP)
