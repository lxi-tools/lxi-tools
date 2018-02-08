EXTRA_DIST = lxi-gui.pro \
             lxi-gui.pro.user \
             lxi-gui.qrc \
             lxi-tools_256x256.png \
             photo-camera.png \
             main.cpp \
             mainwindow.cpp \
             mainwindow.h \
             mainwindow.ui \
             workerthread.cpp \
             workerthread.h

QT_SELECT ?= 5

Makefile.qmake: lxi-gui.pro
	$(QTCHOOSER) -run-tool=qmake -makefile -o Makefile.qmake lxi-gui.pro \
		INCDIR="$(SNAPCRAFT_STAGE)/$(includedir)" \
		LIBDIR="$(SNAPCRAFT_STAGE)/$(libdir)" \
		SNAPCRAFT="$(SNAPCRAFT)" \
		SNAPCRAFT_STAGE="$(SNAPCRAFT_STAGE)" \
		$(QMAKE_ARGUMENTS)

all-local: Makefile.qmake
	$(MAKE) -f Makefile.qmake $(AM_MAKEFLAGS) all

install-exec-local:
	cp lxi-gui $(DESTDIR)/$(bindir)

check-local: Makefile.qmake
	$(MAKE) -f Makefile.qmake $(AM_MAKEFLAGS) test

clean-local: Makefile.qmake
	$(MAKE) -f Makefile.qmake $(AM_MAKEFLAGS) clean
	rm -f Makefile.qmake lxi-gui
