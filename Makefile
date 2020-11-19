# Project layout

DIRSRC=./src
DIRBUILD=./build
DIROBJ=$(DIRBUILD)/obj

OFILES=$(addprefix $(DIROBJ)/,$(patsubst src/%.cc,%.o,$(wildcard src/*/*.cc)))
TARGET=aithena

# Compiler settings

CXX=g++
CXXFLAGS=--std=c++17 -I$(DIRSRC)

# Rules

.PHONY: all
all: build

.PHONY: build
build: $(OFILES)
	@mkdir -p $(DIRBUILD)
	@$(CXX) $(CXXFLAGS) -o $(DIRBUILD)/$(TARGET) $(DIRSRC)/main.cc $(OFILES)

$(DIROBJ)/%.o: $(DIRSRC)/%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $^

%.cc:
	echo $@

.PHONY: clean
clean:
	@rm -rf $(DIRBUILD)
