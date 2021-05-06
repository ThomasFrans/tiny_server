MKFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
MKFILE_DIR := $(dir $(MKFILE_PATH))

all: build

build:
	g++ -o $(MKFILE_DIR)bin/server -I$(MKFILE_DIR)vendor/json/include -I$(MKFILE_DIR)vendor/spdlog/include -I$(MKFILE_DIR)vendor/cereal/include -I$(MKFILE_DIR)src/include $(MKFILE_DIR)src/*.cpp -DDEBUG `pkg-config --cflags --libs gtkmm-3.0`

release:
	g++ -o $(MKFILE_DIR)bin/server -I$(MKFILE_DIR)vendor/json/include -I$(MKFILE_DIR)vendor/spdlog/include -I$(MKFILE_DIR)vendor/cereal/include -I$(MKFILE_DIR)src/include $(MKFILE_DIR)src/*.cpp `pkg-config --cflags --libs gtkmm-3.0`

pch:
	g++ -I$(MKFILE_DIR)vendor/json/include -I$(MKFILE_DIR)vendor/spdlog/include -I$(MKFILE_DIR)vendor/cereal/include -I$(MKFILE_DIR)src/include -x c++-header $(MKFILE_DIR)src/include/Tiny.h `pkg-config --cflags gtkmm-3.0`