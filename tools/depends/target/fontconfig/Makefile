include ../../Makefile.include
DEPS= ../../Makefile.include fix-aarch64_atomics.patch lconv.patch Makefile

# lib name, version
LIBNAME=fontconfig
VERSION=2.12.4
SOURCE=$(LIBNAME)-$(VERSION)
ARCHIVE=$(SOURCE).tar.bz2

# configuration settings
ifeq (darwin, $(findstring darwin, $(HOST)))
  CONFIGURE_EXTRAS=ac_cv_func_mkostemp=no
endif

CONFIGURE=./configure --prefix=$(PREFIX) \
  --with-freetype-config=$(PREFIX)/bin/freetype-config \
  --disable-libxml2 --disable-docs --with-arch=$(PLATFORM) --disable-shared \
  $(CONFIGURE_EXTRAS)

LIBDYLIB=$(PLATFORM)/src/.libs/lib$(LIBNAME).a

CLEAN_FILES=$(ARCHIVE) $(PLATFORM)

all: .installed-$(PLATFORM)

$(TARBALLS_LOCATION)/$(ARCHIVE):
	cd $(TARBALLS_LOCATION); $(RETRIEVE_TOOL) $(RETRIEVE_TOOL_FLAGS) $(BASE_URL)/$(ARCHIVE)

$(PLATFORM): $(TARBALLS_LOCATION)/$(ARCHIVE) $(DEPS)
	rm -rf $(PLATFORM)/*; mkdir -p $(PLATFORM)
	cd $(PLATFORM); $(ARCHIVE_TOOL) $(ARCHIVE_TOOL_FLAGS) $(TARBALLS_LOCATION)/$(ARCHIVE)
	cd $(PLATFORM); patch -p1 < ../lconv.patch 
	cd $(PLATFORM); patch -p1 < ../fix-aarch64_atomics.patch
	cd $(PLATFORM); $(AUTORECONF) -vif
	cd $(PLATFORM); $(CONFIGURE)

$(LIBDYLIB): $(PLATFORM)
	$(MAKE) -C $(PLATFORM)

.installed-$(PLATFORM): $(LIBDYLIB)
	$(MAKE) -C $(PLATFORM) install
	touch $@

clean:
	$(MAKE) -C $(PLATFORM) clean
	rm -f .installed-$(PLATFORM)

distclean::
	rm -rf $(PLATFORM) .installed-$(PLATFORM)

