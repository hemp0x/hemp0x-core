package=bdb
$(package)_version=4.8.30
$(package)_download_path=https://download.oracle.com/berkeley-db
$(package)_file_name=db-$($(package)_version).NC.tar.gz
$(package)_sha256_hash=12edc0df75bf9abd7f82f821795bcee50f42cb2e5f76a6a281b85732798364ef

define $(package)_set_vars
  $(package)_config_opts=--enable-cxx --disable-shared --with-pic --with-mutex=POSIX/pthreads --disable-atomicsupport
endef

define $(package)_preprocess_cmds
	sed -i.old 's/__atomic_compare_exchange/__atomic_compare_exchange_db/' dbinc/atomic.h
endef

define $(package)_config_cmds
	mkdir -p build_unix && cd build_unix && ../dist/configure $($(package)_config_opts)
endef

define $(package)_build_cmds
	cd build_unix && $(MAKE) libdb.a libdb_cxx.a
endef

define $(package)_stage_cmds
	cd build_unix && $(MAKE) DESTDIR=$($(package)_staging_dir) install_lib install_include
endef

# FIX: Added chmod to fix 'Permission denied' and restore mv logic
define $(package)_postprocess_cmds
	mkdir -p $($(package)_staging_prefix_dir)/include/bdb4.8 $($(package)_staging_prefix_dir)/lib
	cp -a $($(package)_staging_dir)/usr/local/BerkeleyDB.4.8/include/db*.h $($(package)_staging_prefix_dir)/include/
	cp -a $($(package)_staging_dir)/usr/local/BerkeleyDB.4.8/lib/libdb*.a     $($(package)_staging_prefix_dir)/lib/
	cd $($(package)_staging_prefix_dir)/lib && rm -f libdb_cxx.a && ln -sf libdb_cxx-4.8.a libdb_cxx.a
	ln -sf ../db_cxx.h $($(package)_staging_prefix_dir)/include/bdb4.8/db_cxx.h
	ln -sf ../db.h     $($(package)_staging_prefix_dir)/include/bdb4.8/db.h
	cd $($(package)_staging_prefix_dir)/include && \
	chmod u+w db.h db_cxx.h 2>/dev/null || true && \
	if [ -f db.h ] && [ ! -f db_orig.h ]; then mv db.h db_orig.h; fi && \
	if [ -f db_cxx.h ] && [ ! -f db_cxx_orig.h ]; then mv db_cxx.h db_cxx_orig.h; fi && \
	printf "#ifndef HEMP0X_BDB48_DB_WRAPPER_H\n#define HEMP0X_BDB48_DB_WRAPPER_H\n#include <sys/types.h>\n#include <stdint.h>\n#include <stddef.h>\n#include \"db_orig.h\"\n#endif\n" > db.h && \
	printf "#ifndef HEMP0X_BDB48_DB_CXX_WRAPPER_H\n#define HEMP0X_BDB48_DB_CXX_WRAPPER_H\n#include <sys/types.h>\n#include <stdint.h>\n#include <stddef.h>\n#include <string.h>\n#include \"db_cxx_orig.h\"\n#endif\n" > db_cxx.h
endef
