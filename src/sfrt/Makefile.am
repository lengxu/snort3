
noinst_LIBRARIES = libsfrt.a

x_includedir = $(pkgincludedir)/sfrt

x_include_HEADERS = \
sfrt.h \
sfrt_trie.h \
sfrt_dir.h \
sfrt_flat.h \
sfrt_flat_dir.h

libsfrt_a_SOURCES = \
sfrt.cc \
sfrt_dir.cc \
sfrt_flat.cc \
sfrt_flat_dir.cc

if ENABLE_UNIT_TESTS
libsfrt_a_SOURCES += sfrt_test.cc
endif

