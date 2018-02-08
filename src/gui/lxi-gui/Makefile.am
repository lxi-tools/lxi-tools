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
	$(QTCHOOSER) -run-tool=qmake -qt=$(QT_SELECT) -makefile -o Makefile.qmake lxi-gui.pro \
		QT_INCDIR="$(SNAPCRAFT_STAGE)/$(includedir)" \
		QT_LIBDIR="$(SNAPCRAFT_STAGE)/$(libdir)" \
		SNAPCRAFT="$(SNAPCRAFT)" \
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
