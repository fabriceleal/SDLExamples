
TARGETS=hello hello_full

.PHONY: clean all

all: $(TARGETS)

%: %.cpp
	g++ $? -o $@ `pkg-config --libs allegro-5.1 allegro_image-5.1 allegro_primitives-5.1` 

clean:
	rm -f $(TARGETS)