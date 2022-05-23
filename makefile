.POSIX:
.PHONY: default debug release clean install uninstall help

SHELL = /bin/sh

# Build variables
EXEC      = treedude
CC        = gcc
CPPFLAGS  = -MMD -MP
CFLAGS    = -ansi -Wall -Wextra -Wpedantic
CFLAGSDBG = -Og -g
CFLAGSREL = -O3
LDFLAGS   =
LDLIBS    = -lm -lcurses
SRCDIR    = src
OBJDIR    = obj
SRCS      = $(shell find $(SRCDIR) -type f -name *.c)
OBJS      = $(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Install variables
PREFIX  = /usr/local
BINDIR  = $(DESTDIR)$(PREFIX)/bin
BIN     = $(BINDIR)/$(EXEC)
MANDIR  = $(DESTDIR)$(PREFIX)/share/man/man6
MAN     = $(MANDIR)/$(EXEC).6
PERMREG = 644
PERMEXE = 755

default: release

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(EXEC) $(LDLIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $@

help:
	@echo
	@echo "             Build for release"
	@echo "  debug      Build for debug"
	@echo "  release    Build for release"
	@echo "  clean      Clean built files"
	@echo "  install    Install $(EXEC)"
	@echo "  uninstall  Uninstall $(EXEC)"
	@echo "  help       Display help"
	@echo

debug: CFLAGS += $(CFLAGSDBG)
debug: $(EXEC)

release: CFLAGS += $(CFLAGSREL)
release: $(EXEC)

clean:
	@rm -rf $(OBJDIR) $(EXEC)

$(BINDIR) $(MANDIR):
	mkdir -p $@

install: $(BINDIR) | $(MANDIR)
	cp -f $(EXEC) $(BIN) && chmod $(PERMEXE) $(BIN)
	cp -f $(EXEC).6 $(MAN) && chmod $(PERMREG) $(MAN)

uninstall:
	-rm -f $(BIN)
	-rm -f $(MAN)
	@echo "NOTE: The directory \$$XDG_DATA_HOME/treedude (~/.local/share/treedude) may still exist"

-include $(OBJS:.o=.d)
