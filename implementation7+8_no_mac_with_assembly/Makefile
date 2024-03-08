THIS_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

# Build directory
OBJDIR = obj
CFLAGS += -Ofast -I./$(OBJDIR) -Wall -msse2 -msse -march=native -maes -msse4 

# Files and folders
SRCS = main.c fenc.c fenc_ref.c butterknife.c butterknife_ref.c helpers.c
OBJS = $(addprefix $(OBJDIR)/,$(patsubst %.c,%.o,$(SRCS)))

# Targets
all: clean fenc 

fenc: $(OBJS)
	g++ $(CPPFLAGS) -o $@ $^

$(OBJDIR)/%.o: %.c | objdir
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.cpp | objdir
	g++ $(CPPFLAGS) -c $< -o $@

# Targeted eviction

clean:
	rm -rf safe $(OBJDIR)

objdir:
	@mkdir -p $(OBJDIR)

run:
	./fenc

.PHONY: all clean
