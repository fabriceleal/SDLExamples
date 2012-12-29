
# Assumes: target name == source name without extension
OUTPUT=../out/05/
TARGET=colorkeying
FLAGS=-lSDL -lSDL_image

OUTPUT_IMAGES=$(patsubst ./%.png, $(OUTPUT)%.png , $(shell find -type f -name '*.png') )

.PHONY: clean all compile $(OUTPUT)

# Everything
all: $(OUTPUT)$(TARGET) $(OUTPUT_IMAGES) $(OUTPUT)

# Compile and copy executable
$(OUTPUT)$(TARGET): $(TARGET).cpp
	mkdir -p $(OUTPUT)
	g++ $? -o $@ $(FLAGS)

# Copy images
$(OUTPUT)%.png: %.png
	mkdir -p $(OUTPUT)
	cp $< $@

# Removes out directory
clean:
	rm -rf $(OUTPUT)