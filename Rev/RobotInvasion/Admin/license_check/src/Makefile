CC = g++
CFLAGS = -static -fno-PIC -fpermissive -Wno-narrowing -fno-plt -z execstack -no-pie

TARGET = license_check
SRCS = main.c mtwister.c
HEADERS = mtwister.h

$(TARGET): $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)
	python3 encrypt_funcs.py
	strip $(TARGET)

clean:
	rm -f $(TARGET)
