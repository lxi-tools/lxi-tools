EXTRA_DIST = lxi-gui.pro \
             lxi-gui.pro.user \
             lxi-gui.qrc \
             lxi-tools_256x256.png \
             main.cpp \
             mainwindow.cpp \
             mainwindow.h \
             mainwindow.ui \
             photo-camera.png

Makefile.qmake: lxi-gui.pro
	$(QMAKE) -makefile -o Makefile.qmake lxi-gui.pro "INCDIR=$(STAGE)/$(includedir)" "LIBDIR=$(STAGE)/$(libdir)"
                
all-local: Makefile.qmake
	$(MAKE) -f Makefile.qmake $(AM_MAKEFLAGS) all

install-exec-local:
	cp lxi-gui $(DESTDIR)/$(bindir)

check-local: Makefile.qmake
	$(MAKE) -f Makefile.qmake $(AM_MAKEFLAGS) test

clean-local: Makefile.qmake
	$(MAKE) -f Makefile.qmake $(AM_MAKEFLAGS) clean
	rm -f Makefile.qmake lxi-gui
