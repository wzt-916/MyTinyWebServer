SRCS := $(wildcard *.cpp *.h)
UTILS := utils/utils.h utils/utils.cpp
TIMER := timer/lst_timer.h timer/lst_timer.cpp
THREADPOOL := threadpool/threadpool.h
HTTP := http/http_conn.h http/http_conn.cpp
LOCK := lock/locker.h lock/locker.cpp

ALL := $(SRCS) $(UTILS) $(TIMER) $(THREADPOOL) $(HTTP) $(LOCK)

server: $(ALL)
	g++ $^ -o $@ -lpthread -std=c++17

debug:
	echo $(SRCS)

clean:
	rm -rf server