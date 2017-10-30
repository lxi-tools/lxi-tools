bin_PROGRAMS = lxi

lxi_SOURCES = lxi.c \
              options.c \
              include/options.h \
              include/screenshot.h \
              screenshot.c \
              plugins/screenshot_rigol.c

lxi_CFLAGS =
lxi_LDADD = -llxi -lreadline

if ENABLE_BASH_COMPLETION
bashcompletiondir=@BASH_COMPLETION_DIR@
dist_bashcompletion_DATA=bash-completion/lxi
endif
