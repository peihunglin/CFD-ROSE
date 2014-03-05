include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

TESTCODE_INCLUDES = 

TESTCODES_REQUIRED_TO_PASS = \
				   simpleExample1.F \
				   simpleExample2.F \
				   simpleExample3.F \
				   simpleExample4.F


ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) 

PASSING_FORTRAN_TEST_OBJECTS = $(TESTCODES_REQUIRED_TO_PASS:.F=.o)

# if ROSE_USE_OPEN_FORTRAN_PARSER
if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT

FortranInliner:
	$(MAKE) -C ../src FortranInliner
	

$(PASSING_FORTRAN_TEST_OBJECTS): %.o: $(srcdir)/%.F FortranInliner 
	../src/FortranInliner -c $<


check-local:
	@echo "Test for FortranInliner"
	@$(MAKE) $(PASSING_FORTRAN_TEST_OBJECTS)
	@echo "======== make check rule for FortranInliner terminated normally ========"

else
check-local:
	@echo "\nROSE Fortran development tests using the LANL POP code NOT run.  To run these reconfigure with:"
	@echo "*********************************************************************************************"
	@echo "******************* make check not run here: configure --with-fortran ***********************"
	@echo "*********************************************************************************************"
endif

clean-local:	
	rm -f $(PASSING_FORTRAN_TEST_OBJECTS) *.c *.f *.dot

EXTRA_DIST = $(ALL_TESTCODES) 

