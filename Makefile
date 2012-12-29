
TARGETS=hello

.PHONY: clean all

all: $(TARGETS)

%: %.cpp
	g++ $? -o $@ -lSDL

clean:
	rm -f $(TARGETS)