bin_PROGRAMS = lxi

lxi_SOURCES = lxi.c \
              options.c \
              include/options.h \
              include/error.h \
              include/screenshot.h \
              screenshot.c \
              plugins/screenshot_keysight.c \
              plugins/screenshot_rigol-1000z.c \
              plugins/screenshot_rigol-2000.c \
              plugins/screenshot_rigol-dg4000.c \
              plugins/screenshot_rigol-dm3000.c \
              plugins/screenshot_rigol-dp800.c \
              plugins/screenshot_rigol-dsa700.c \
              plugins/screenshot_rigol-dsa800.c \
              plugins/screenshot_rohde-schwarz-hmo1000.c \
              plugins/screenshot_siglent-sds1000x.c \
              plugins/screenshot_siglent-ssa3000x.c \
              plugins/screenshot_tektronix.c

lxi_CFLAGS =
lxi_LDADD = -llxi -lreadline

if ENABLE_BASH_COMPLETION
bashcompletiondir=@BASH_COMPLETION_DIR@
dist_bashcompletion_DATA=bash-completion/lxi
endif
