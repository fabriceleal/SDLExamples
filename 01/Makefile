
# Assumes: target name == source name without extension
OUTPUT=../out/01/
TARGET=hello

IMAGES=$(shell find -type f -name '*.bmp')
OUTPUT_IMAGES=$(patsubst ./%.bmp, $(OUTPUT)%.bmp , $(IMAGES))

.PHONY: clean all

# Everything
all: $(OUTPUT)$(TARGET) $(OUTPUT_IMAGES)

# Compile and copy executable
$(OUTPUT)$(TARGET): $(TARGET).cpp
	mkdir -p $(OUTPUT)
	g++ $? -o $@ -lSDL

# Copy images
$(OUTPUT)%.bmp: $(IMAGES)
	mkdir -p $(OUTPUT)
	cp $< $@

# Clean executable, images
clean:
	rm -f $(OUTPUT)$(TARGET) $(OUTPUT_IMAGES)
