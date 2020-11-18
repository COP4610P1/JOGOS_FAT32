CC = gcc
CFLAGS = -std=c99 
LDFLAGS =
OBJFILES = project3.o main.o
TARGET = project3

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)
clean:
	rm -f $(OBJFILES) $(TARGET) *~
