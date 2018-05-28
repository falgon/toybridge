.PHONY: all

all: bridge

FLAGS := -Wall -Wextra -pedantic -std=c++14 -O3
INCLUDE_PATH := -I ./includes -I ./includes/SrookCppLibraries
GXX := g++
OUTS := ./src/main.o

bridge: $(OUTS)
	$(RM) -r dst
	mkdir dst
	mv ./src/main.o ./dst

$(OUTS): %.o: %.cpp
	$(GXX) $(FLAGS) $(INCLUDE_PATH) $< -o $@

run:
	sudo ./dst/main.o

clean:
	$(RM) -r dst
