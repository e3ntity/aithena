CXX=g++
CFLAGS=

DIRSRC=./src
DIRBUILD=./build
TARGET=aithena

.PHONY: all clean

all:
	@mkdir -p $(DIRBUILD)
	@$(CXX) $(CFLAGS) $(DIRSRC)/*.cc -o $(DIRBUILD)/$(TARGET)

run: all
	@$(DIRBUILD)/$(TARGET)

clean:
	@rm -rf $(DIRBUILD)
