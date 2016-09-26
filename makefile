CC= gcc
CFLAGS=
INCLUDES= classics.str strategy.h
XSOURCES= interface.c mainwindow.c setupwindow.c tracewindow.c xmain.c

prison: $(INCLUDES) prison.c
	$(LINK.c) -o $@ prison.c

tinypri: $(INCLUDES) tinypri.c
	$(LINK.c) -o $@ tinypri.c


xprison: $(INCLUDES) tinypri.c $(XSOURCES)
	$(LINK.c) -D XPRISON -o $@ tinypri.c \
		-I/usr/X11/include \
		-I/usr/openwin/include \
		-L/usr/X11/lib \
		-L/usr/openwin/lib -lX11 -lxview -lolgx -lm

clean:	
	\rm -f prison tinypri xprison \
		*.bat *.cmd *.ps *.o *.dat *.tra *.zip *.tar *.tar.gz *~ *%


pack:
	mkdir -p bid/prison
	cp prison.c tinypri.c getopt.* \
		strategy.h  classics.str  renounce.str lift.str \
		pls.str misc.str slaves.str \
		readme changes makefile \
		translat.nam translat bid/prison
	cd bid ;\
	tar cvf ../prison.tar prison ;\
	gzip -9 ../prison.tar	;\
	cd ..	;\
	\rm -r -f bid


xpack:
	mkdir -p bid/xprison
	cp prison.c tinypri.c getopt.* \
		strategy.h  classics.str  renounce.str lift.str \
		pls.str misc.str slaves.str \
		readme changes makefile \
		sample1 sample2 sample3 sample4 \
		$(XSOURCES) \
		translat.nam translat bid/xprison
	cd bid ;\
	tar cvf ../xprison.tar xprison ;\
	gzip -9 ../xprison.tar	;\
	cd ..	;\
	\rm -r -f bid


