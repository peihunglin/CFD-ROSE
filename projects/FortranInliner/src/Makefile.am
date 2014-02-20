
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(ROSE_LIBS)

bin_PROGRAMS = FortranInliner


FortranInliner_SOURCES=\
	main.C 

#include_HEADERS = fortranInliner.h

#EXTRA_DIST = fortranInliner.h
