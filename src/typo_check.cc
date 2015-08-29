#include <iostream>
#include <string>

#include "typo.h"

using std::cout;
using std::endl;
using std::string;

static void usage()
{
	cout << endl;
	cout << "usage:" << endl;
	cout << "\ttypochecker typos checkfile1" << endl;
	cout << endl;
}

int main(int argc, char **argv)
{
	if (argc != 3) {
		usage();
		return -1;
	}

	cout << "Checking typos, please wait a moment" << endl;

	init();

	init_typos(string(argv[1]));

	print_typo_info();

	search_typo_per_file(string(argv[2]));

	return 0;
}
