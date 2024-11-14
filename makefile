COMMAND = gcc

# Define the source files and their respective folders by function
SRCS = main.c \
       ./telemetry/server_thr.c \
       ./telemetry/client_tcp_thr.c \
       ./telemetry/mqtt_thr.c \
       ./telemetry/file_upload.c \
	   ./operation/audio_input_thr.c \
	   ./operation/audio_save_thr.c \
	   ./operation/threshold_detect.c \
	   ./prediction/ml_thr.c \
	   ./db_helper/db_helper.c \
    #    ./network/network_func.c \
    #    ./utils/helper_func.c

# Application name and libraries
APP_NAME = implementation
LIBS = -lm -lpthread -lpaho-mqtt3c -lcjson -lasound -lsqlite3 -lcurl

# Output folder for compiled object files
OUT = ./out

# Define object files, adjusting paths to place them in the OUT folder
OBJS = $(patsubst %.c,$(OUT)/%.o,$(SRCS))

# Default make target
all : clean $(OUT) $(APP_NAME) run

# Link all object files to create the final executable
$(APP_NAME) : $(OBJS)
	$(COMMAND) -g -o $(APP_NAME) $(OBJS) $(LIBS)

# Compile each source file into the out folder
$(OUT)/%.o : %.c
	mkdir -p $(dir $@)
	$(COMMAND) -c $< -o $@

# Create the output folder if it doesn't exist
$(OUT):
	mkdir -p $(OUT)

# Run the application
run:
	./$(APP_NAME)

# Clean command to remove object files and the executable
clean:
	rm -f $(OUT)/*.o $(APP_NAME) ./captured_audio/*.wav
	find $(OUT) -type f -name '*.o' -delete
