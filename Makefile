all: typochecker

typochecker: src/typo_check.cc src/typo.cc src/typo.h
	g++ src/typo_check.cc src/typo.cc -o typochecker

clean:
	rm -f typochecker
