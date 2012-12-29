
# Assumes: target name == source name without extension
OUTPUT=../out/01/
TARGET=hello
IMGS='*.bmp'
FLAGS=-lSDL

OUTPUT_IMAGES=$(patsubst ./%.bmp, $(OUTPUT)%.bmp , $(shell find -type f -name $(IMGS)) )

.PHONY: clean all compile $(OUTPUT)

# Everything
all: $(OUTPUT)$(TARGET) $(OUTPUT_IMAGES) $(OUTPUT)

# Compile and copy executable
$(OUTPUT)$(TARGET): $(TARGET).cpp
	mkdir -p $(OUTPUT)
	g++ $? -o $@ $(FLAGS)

# Copy images
$(OUTPUT)%.bmp: %.bmp
	mkdir -p $(OUTPUT)
	cp $< $@

# Removes out directory
clean:
	rm -rf $(OUTPUT)