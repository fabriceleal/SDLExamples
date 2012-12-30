
# Assumes: target name == source name without extension
OUTPUT=../out/11/
TARGET=sounds
FLAGS=-lSDL -lSDL_image -lSDL_ttf -lSDL_mixer

OUTPUT_IMAGES=$(patsubst ./%.png, $(OUTPUT)%.png , $(shell find -type f -name '*.png') )
OUTPUT_FONTS=$(patsubst ./%.ttf, $(OUTPUT)%.ttf , $(shell find -type f -name '*.ttf') )
OUTPUT_MUSIC=$(patsubst ./%.wav, $(OUTPUT)%.wav , $(shell find -type f -name '*.wav') )

.PHONY: clean all compile $(OUTPUT)

# Everything
all: $(OUTPUT)$(TARGET) $(OUTPUT_IMAGES) $(OUTPUT_FONTS) $(OUTPUT_MUSIC) $(OUTPUT)

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

# Copy sounds
$(OUTPUT)%.wav: %.wav
	mkdir -p $(OUTPUT)
	cp $< $@

# Removes out directory
clean:
	rm -rf $(OUTPUT)
