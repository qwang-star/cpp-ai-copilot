CXX ?= g++
CXXFLAGS ?= -std=c++20 -Wall -Wextra -pedantic -O2 -finput-charset=UTF-8 -fexec-charset=UTF-8 -Iinclude
INCLUDES := -Iinclude

SRC := src/application.cpp src/config.cpp src/http.cpp src/router.cpp src/logger.cpp
SERVER_SRC := src/main.cpp src/simple_server.cpp $(SRC)
TEST_SRC := tests/test_core.cpp $(SRC)

ifeq ($(OS),Windows_NT)
  SERVER_LIBS := -lws2_32
  EXE := .exe
else
  SERVER_LIBS :=
  EXE :=
endif

.PHONY: all test run clean

all: build/cpp-ai-copilot$(EXE)

build:
	@if not exist build mkdir build

build/cpp-ai-copilot$(EXE): build $(SERVER_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SERVER_SRC) -o $@ $(SERVER_LIBS)

build/test_core$(EXE): build $(TEST_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(TEST_SRC) -o $@

test: build/test_core$(EXE)
	./build/test_core$(EXE)

run: build/cpp-ai-copilot$(EXE)
	./build/cpp-ai-copilot$(EXE) config/app.env

clean:
	@if exist build rmdir /s /q build
