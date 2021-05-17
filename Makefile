# Project layout

DIRSRC=./src
DIRTEST=./test
DIRBUILD=./build
DIROBJ=$(DIRBUILD)/obj

OFILES=$(addprefix $(DIROBJ)/,$(patsubst aithena/%.cc,%.o,$(wildcard aithena/*/*.cc)))
TARGET=aithena

# Compiler settings

CXX=g++
CXXFLAGS=--std=c++17 -I$(DIRSRC) -Wall

# Rules

.PHONY: all
all: build

.PHONY: build
build: $(OFILES)
	@mkdir -p $(DIRBUILD)
	@$(CXX) $(CXXFLAGS) -o $(DIRBUILD)/$(TARGET) $(DIRSRC)/main.cc $(OFILES)

.PHONY: test
test: clean $(OFILES)
	@mkdir -p $(DIRBUILD)
	@$(CXX) $(CXXFLAGS) -Wl,--unresolved-symbols=ignore-in-object-files -g \
		-o $(DIRBUILD)/test $(DIRTEST)/*.cc $(OFILES) -lgtest_main -lgtest \
		-lpthread

$(DIROBJ)/%.o: $(DIRSRC)/%.cc
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c -o $@ $^

.PHONY: clean
clean:
	@rm -rf $(DIRBUILD)
