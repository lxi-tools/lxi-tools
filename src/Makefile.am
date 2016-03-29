bin_PROGRAMS = lxi rigol_1000z_screenshot

lxi_SOURCES = lxi.c \
              options.c \
              include/options.h

lxi_CFLAGS = 
lxi_LDADD = -llxi -lreadline

rigol_1000z_screenshot_SOURCES=rigol_1000z_screenshot.c

bashcompletiondir=$(sysconfdir)/bash_completion.d
dist_bashcompletion_DATA=bash-completion/lxi
