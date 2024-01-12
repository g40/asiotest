# version:1.0.1.8
# version:1.0.1.6
#
# make 2>&1 | tee build.txt
# nm -a C <app>
# to get coverage info:
#
# make clean 
# make COVERAGE=1 
# ./$(TARGET) 
# make coverage
# make clean 
#
# profiling data
#
# make PROFILE=1 
# ./$(TARGET) 
# make profile

# for Linux/OSX/BSD etc
OSTYPE:=$(shell uname -s)
# x86_64|aarch64 for example
OSARCH:=$(shell uname -m)
# get the local hash
HASH:=$(shell git rev-parse --short HEAD)

# explicit naming of compilers
LINK ?= $(CROSS)arLINKFLAGS ?= rcs
AS ?= $(CROSS)gc
CC ?= $(CROSS)gcc
CXX ?= $(CROSS)g++
# MacOSX. Need Windows version
ifeq ($(OSTYPE), Darwin)
	CXXFLAGS += -D_IS_OSX
	CFLAGS += -D_IS_OSX
	CC=clang
	CXX=clang++
endif

#
COMPILE_PTHREADS:=-pthread
LINK_PTHREADS:=-lpthread

# debug is set by default
# multi-threaded by default
DEBUG ?= 1
ifeq ($(DEBUG), 1)
DEFFLAGS := -D_DEBUG
OPTFLAGS := -g -O0
CFLAGS :=  $(COMPILE_PTHREADS) -Wall 
CXXFLAGS := $(COMPILE_PTHREADS) -Wall -std=c++11  
OBJDIR := deb
else
DEFFLAGS := -DNDEBUG
OPTFLAGS := -O2
CFLAGS := $(COMPILE_PTHREADS) -Wall 
CXXFLAGS := $(COMPILE_PTHREADS) -Wall -std=c++11
OBJDIR := rel
endif

# compilation flags for C++ code
# can be overridden in makefile.local
CXXFLAGS+=-Wno-unused-function
CXXFLAGS+=-Wno-unused-variable
CXXFLAGS+=-Wno-unused-private-field


# coverage analysis
ifeq ($(COVERAGE), 1)
COVFLAGS:=--coverage
else
COVFLAGS:=
endif

# profiling analysis
ifeq ($(PROFILE), 1)
PROFFLAGS:=-pg
else
PROFFLAGS:=
endif

# autogenerate dependencies
DEPFLAGS := -MD -MP

# append all local content
include makefile.local

# set up the destination for install (exe|static|shared)
ifeq ($(PROJECT_TYPE), exe)
DESTDIR :=/usr/local/bin
MODE :=-m 755
else
MODE :=
DESTDIR :=/usr/local/lib
endif

# now we have vars from .local
ASMFLAGS += $(INCLUDES) 
CFLAGS += $(INCLUDES) -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable
CXXFLAGS += $(INCLUDES) -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable
# add in any extra -D options
ASMFLAGS += $(DEFFLAGS)
CFLAGS += $(DEFFLAGS)
CXXFLAGS += $(DEFFLAGS)


# extract file names from types for .o
C_OBJS := $(C_SOURCES:%.c=$(OBJDIR)/%.o)
CC_OBJS := $(CC_SOURCES:%.cc=$(OBJDIR)/%.o)
CPP_OBJS := $(CPP_SOURCES:%.cpp=$(OBJDIR)/%.o)
ASM_OBJS := $(ASM_SOURCES:%.s=$(OBJDIR)/%.o)
OBJS :=  $(CPP_OBJS) $(C_OBJS) $(ASM_OBJS)
DEPS = $(OBJS:$(OBJDIR)/%.o=$(OBJDIR)/%.d)

# .PHONY means lhs is not a file target
# i.e. 'all' is not a filename
.PHONY: all dirs libs clean clean_src clean_libs install coverage

# the goal ...
all: dirs libs $(TARGET) 

# ensure we've got object directories
dirs:
	@mkdir -p $(OBJDIR)

# gives us a rule for our libraries 
libs: $(PROJECT_LIBS) 

# make the dependencies
$(PROJECT_LIBS):
	$(MAKE) -C $(dir $@)


# link into source directory
$(TARGET): $(OBJS)
	@echo 'Linking ...' $@
	@if [ "$(PROJECT_TYPE)" = "static" ]; then \
		$(LINK) $(LINKFLAGS) lib$@ $(OBJS); \
	else \
		$(CXX)  $(OPTFLAGS) $(PROFFLAGS) $(COVFLAGS) $(OBJS) $(LIBS) $(LINK_PTHREADS) -o $@; \
	fi
	@([ $$? -eq 0 ] && echo "$(OSTYPE) $(CXX) => success! ") || echo "failure!"
	
# To obtain object files in OBJDIR
$(OBJDIR)/%.o: %.cpp
	@# make object/dep directory if absent
	@mkdir -p $(@D)
	$(CXX) -c $(CXXFLAGS) $(OPTFLAGS) $(PROFFLAGS) $(COVFLAGS) $(DEPFLAGS) -o $@ $<

# To obtain object files in OBJDIR
$(OBJDIR)/%.o: %.cc
	@# make object/dep directory if absent
	@mkdir -p $(@D)
	$(CXX) -c $(CXXFLAGS) $(OPTFLAGS) $(PROFFLAGS) $(COVFLAGS) $(DEPFLAGS) -o $@ $<

$(OBJDIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) $(OPTFLAGS) $(PROFFLAGS) $(COVFLAGS) $(DEPFLAGS) -o $@ $<

# as
$(OBJDIR)/%.o: %.s
	@mkdir -p $(@D)
	$(CC) -c $(ASMFLAGS) $(OPTFLAGS) $(PROFFLAGS) $(COVFLAGS) $(DEPFLAGS) -o $@ $<

# To remove all generated files. $(PROJECT_LIBS) is optional thus this test ..
ifneq ($(PROJECT_LIBS),)
clean: clean_libs clean_src 
else
clean: clean_src 
endif

# clean libraries
clean_libs:
	$(MAKE) -C $(dir $(PROJECT_LIBS)) clean

# To remove generated files
clean_src:
	rm -f $(TARGET) $(OBJS) $(DEPS)
	rm -f $(OBJDIR)/*.info $(OBJDIR)/*.gcda $(OBJDIR)/*.gcno $(OBJDIR)/*.html $(OBJDIR)/*.png $(OBJDIR)/*.css

# install 
install:
	# uses GNU install
	@if [ "$(PROJECT_TYPE)" = "static" ] && [ -e $(INCFILES) ]; then \
		install $(MODE) lib$(TARGET) $(DESTDIR); \
		install $(MODE) $(INCFILES) $(INCDIR); \
	else \
		install -d $(DESTDIR); \
		install $(MODE) $(TARGET) $(DESTDIR); \
	fi
	@echo "Installed $(TARGET) to $(DESTDIR)"

# build GCC coverage analysis stuff *after* target has been run
coverage:
	lcov --capture --directory $(OBJDIR) --output-file $(OBJDIR)/coverage.info
	genhtml $(OBJDIR)/coverage.info --output-directory $(OBJDIR)

# build GCC coverage analysis stuff *after* target has been run
profile:
	gprof $(TARGET) gmon.out > profile.txt

# now all .d files are dependencies
# do not move this as it will interfere with dependency graph!
-include $(DEPS)

# end of makefile

