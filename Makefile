DESTDIR=
PREFIX=/var/spool/apache/viewvc
WEBPATH=/var/spool/apache/htdocs/viewsvn


all:

clean:

distclean:


install:
	./viewvc-install --prefix=$(PREFIX) --destdir=$(DESTDIR)
	install -d ${DESTDIR}${WEBPATH}
	rsync -a ${DESTDIR}${PREFIX}/bin/mod_python/ ${DESTDIR}${WEBPATH}
	install -T distrotech.conf ${DESTDIR}${PREFIX}/viewvc.conf
