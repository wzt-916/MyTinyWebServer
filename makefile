SRCS := $(wildcard *.cpp *.h)

server: $(SRCS)
	g++ $^ -o $@

debug:
	echo $(SRCS)

clean:
	rm -rf server