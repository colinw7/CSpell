all:
	cd src; make
	cd build; make
	cd test; make

clean:
	cd src; make clean
	cd build; make clean
	cd test; make clean
