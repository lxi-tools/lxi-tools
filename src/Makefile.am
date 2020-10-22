bin_PROGRAMS = lxi
noinst_LIBRARIES = libapp.a

lxi_SOURCES = main.c
libapp_a_SOURCES = options.c \
                   discover.c \
                   scpi.c \
                   screenshot.c \
                   benchmark.c \
				   run.c \
				   lxilua.c \
                   include/error.h \
                   include/options.h \
                   include/discover.h \
                   include/scpi.h \
                   include/screenshot.h \
                   include/benchmark.h \
                   include/run.h \
                   include/lxilua.h \
                   plugins/screenshot_keysight-dmm.c \
                   plugins/screenshot_keysight-ivx.c \
                   plugins/screenshot_rigol-1000z.c \
                   plugins/screenshot_rigol-2000.c \
                   plugins/screenshot_rigol-dg4000.c \
                   plugins/screenshot_rigol-dl3000.c \
                   plugins/screenshot_rigol-dm3068.c \
                   plugins/screenshot_rigol-dp800.c \
                   plugins/screenshot_rigol-dsa.c \
                   plugins/screenshot_rohde-schwarz-hmo-rtb.c \
                   plugins/screenshot_siglent-sdm3000.c \
                   plugins/screenshot_siglent-sdg.c \
                   plugins/screenshot_siglent-sds.c \
                   plugins/screenshot_siglent-ssa3000x.c \
                   plugins/screenshot_tektronix.c

libapp_a_CFLAGS = @lua_CFLAGS@

lxi_LDADD = -llxi -lreadline libapp.a @lua_LIBS@

if ENABLE_BASH_COMPLETION
bashcompletiondir=@BASH_COMPLETION_DIR@
dist_bashcompletion_DATA=bash-completion/lxi \
                         bash-completion/lxi.snap
endif
