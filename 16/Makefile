
# Assumes: target name == source name without extension
OUTPUT=../out/16/
TARGET=motion
FLAGS=-lSDL -lSDL_image -lSDL_ttf

OUTPUT_IMAGES=$(patsubst ./%.png, $(OUTPUT)%.png , $(shell find -type f -name '*.png') )
OUTPUT_FONTS=$(patsubst ./%.ttf, $(OUTPUT)%.ttf , $(shell find -type f -name '*.ttf') )

.PHONY: clean all compile $(OUTPUT)

# Everything
all: $(OUTPUT)$(TARGET) $(OUTPUT_IMAGES) $(OUTPUT_FONTS) $(OUTPUT)

# Compile and copy executable
$(OUTPUT)$(TARGET): $(TARGET).cpp
	mkdir -p $(OUTPUT)
	g++ $? -o $@ $(FLAGS)

# Copy images
$(OUTPUT)%.png: %.png
	mkdir -p $(OUTPUT)
	cp $< $@

# Copy fonts
$(OUTPUT)%.ttf: %.ttf
	mkdir -p $(OUTPUT)
	cp $< $@

# Removes out directory
clean:
	rm -rf $(OUTPUT)