# utilities
MKDIR = mkdir
MKDIRFLAGS = -p

# compiler settings
CXX = g++
CXXFLAGS = -std=c++11
OUTPUT_DIR = build

# OpenCL dependency paths (linux only)
OPENCL_INCDIR = /usr/local/cuda/include
OPENCL_LIBDIR = /usr/local/cuda/lib64

# detect OS (mac/linux only)
UNAME_OS = $(shell uname -s)
ifeq ($(UNAME_OS),Darwin)
	LINKFLAGS += -framework OpenCL
else
	CXXFLAGS += -I$(OPENCL_INCDIR)
	LINKFLAGS += -L$(OPENCL_LIBDIR) -lOpenCL
endif

SOURCES = *.cpp

all: directories
	$(CXX) $(CXXFLAGS) -o $(OUTPUT_DIR)/genWitnessSolver $(SOURCES) $(LINKFLAGS)

directories:
	$(MKDIR) $(MKDIRFLAGS) $(OUTPUT_DIR)

clean:
	rm -rf build