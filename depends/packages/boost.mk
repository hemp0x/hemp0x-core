package=boost
$(package)_version=1_71_0
$(package)_download_path=https://archives.boost.io/release/1.71.0/source/
$(package)_file_name=boost_$($(package)_version).tar.bz2
$(package)_sha256_hash=d73a8da01e8bf8c7eda40b4c84915071a8c8a0df4a6734537ddde4a8580524ee
$(package)_dependencies=native_b2

define $(package)_set_vars
  $(package)_config_opts=--layout=tagged --build-type=complete threading=multi link=static -sNO_COMPRESSION=1
  $(package)_config_opts_linux=target-os=linux threadapi=pthread runtime-link=shared
  $(package)_cxxflags=-std=c++17 -fvisibility=hidden -fpermissive -w -fPIC -D_GLIBCXX_USE_DEPRECATED=1
endef

define $(package)_preprocess_cmds
	find . -name "thread.cpp" -exec sed -i 's/token_compress_on/algorithm::token_compress_on/g' {} +
	find . -name "*.hpp" -exec sed -i 's/((Model\*)0)->[^;]*;/((void)0);/g' {} +
	find . -name "thread_data.hpp" -exec sed -i 's/#if PTHREAD_STACK_MIN > 0/#ifdef PTHREAD_STACK_MIN/g' {} +
endef

define $(package)_config_cmds
	./bootstrap.sh --without-icu --with-libraries=chrono,filesystem,program_options,system,thread
	echo "using gcc : : $(CXX) : <cxxflags>\"$($(package)_cxxflags)\" ;" > project-config.jam
endef

define $(package)_build_cmds
	b2 -d0 -j2 --prefix=$($(package)_staging_prefix_dir) $($(package)_config_opts) stage
endef

define $(package)_stage_cmds
	b2 -d0 -j2 --prefix=$($(package)_staging_prefix_dir) $($(package)_config_opts) install
endef
