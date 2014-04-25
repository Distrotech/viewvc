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
	install -T conf/distrotech.conf ${DESTDIR}${PREFIX}/viewvc.conf
	install -T conf/mimetypes.conf.dist ${DESTDIR}${PREFIX}/mimetypes.conf
	install -T conf/cvsgraph.conf.dist ${DESTDIR}${PREFIX}/cvsgraph.conf
