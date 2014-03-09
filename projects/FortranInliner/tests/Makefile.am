include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

TESTCODE_INCLUDES = 

TESTCODES_REQUIRED_TO_PASS = \
				   simpleExample1.F \
				   simpleExample2.F \
				   simpleExample3.F \
				   simpleExample4.F \
				   simpleExample5.F \
				   passArg1.F \
				   passArg2.F \
				   passArg3.F \
				   passArg4.F \
				   redimension1.F \
				   redimension2.F


ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) 

PASSING_FORTRAN_TEST_OBJECTS = $(TESTCODES_REQUIRED_TO_PASS:.F=.o)

# if ROSE_USE_OPEN_FORTRAN_PARSER
if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT

fortranInliner:
	$(MAKE) -C ../src fortranInliner
	

$(PASSING_FORTRAN_TEST_OBJECTS): %.o: $(srcdir)/%.F fortranInliner 
	../src/fortranInliner -c $<


check-local:
	@echo "Test for fortranInliner"
	@$(MAKE) $(PASSING_FORTRAN_TEST_OBJECTS)
	@echo "======== make check rule for fortranInliner terminated normally ========"

else
check-local:
	@echo "\nROSE fortran development tests using the LANL POP code NOT run.  To run these reconfigure with:"
	@echo "*********************************************************************************************"
	@echo "******************* make check not run here: configure --with-fortran ***********************"
	@echo "*********************************************************************************************"
endif

clean-local:	
	rm -f $(PASSING_FORTRAN_TEST_OBJECTS) *.c *.f *.dot rose_*

EXTRA_DIST = $(ALL_TESTCODES) 

