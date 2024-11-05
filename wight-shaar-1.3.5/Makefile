#Executable for Linux/UNIX/BSD
linux:
	make -C src

#Executable for Windows, compiled in Windows using MinGW or equivalent
windows:
	make -C src -f Makefile.win

#Executable for Windows, compiled in Linux using MinGW
lwin:
	make -C src -f Makefile.lwin

#Clear object files
clean:
	make -C src clean

#Clear object files and executables
distclean:
	make -C src distclean
	make -C src -f Makefile.lwin distclean