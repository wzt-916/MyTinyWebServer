SRCS := $(wildcard *.cpp *.h)
UTILS := utils/utils.h utils/utils.cpp
TIMER := timer/lst_timer.h timer/lst_timer.cpp

server: $(SRCS) $(UTILS) $(TIMER)
	g++ $^ -o $@

debug:
	echo $(SRCS)

clean:
	rm -rf server