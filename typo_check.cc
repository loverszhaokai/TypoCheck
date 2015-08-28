#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <vector>

using std::cout;
using std::endl;
using std::fstream;
using std::list;
using std::string;
using std::vector;

struct TypoInfo {

	TypoInfo()
	{
		ch = ' ';
		ww = "";
		rw = "";
		children = list<TypoInfo>();
	}

	TypoInfo(char _ch, const string &_ww, const string &_rw)
	{
		ch = _ch;
		ww = _rw;
		rw = _rw;
		children = list<TypoInfo>();
	}

	char ch;
	string ww; // wrong word
	string rw; // right word

	list<TypoInfo> children;
};

static char cidx[256]; // char to index, a~z and A-Z to 0~25, others to 255

static TypoInfo typo_info[26];

static void usage()
{
	cout << endl;
	cout << "usage:" << endl;
	cout << "\ttypochecker typos checkfile1" << endl;
	cout << endl;
}

static void init()
{
	for (int i = 0; i < sizeof(cidx) / sizeof(char); i++) {
		if (i >= 'a' && i <= 'z')
			cidx[i] = i - 'a';
		else if (i >= 'A' && i <= 'Z')
			cidx[i] = i - 'A';
		else
			cidx[i] = -1;
	}

	for (int i = 0; i < sizeof(typo_info) / sizeof(TypoInfo); i++) {
		typo_info[i].ch = 'a' + i;
	}
}

static int insert_typo(const string &ww, const string &rw)
{
	int index = 0;
	TypoInfo *tip;
	bool found;
	list<TypoInfo>::iterator it;

	tip = &typo_info[cidx[ww[index]]];
	index++;

	while (index < ww.size()) {
		found = false;
		for (it = tip->children.begin(); it != tip->children.end(); it++) {
			if (it->ch == ww[index]) {
				found = true;
				break;
			} else if (it->ch > ww[index]) {
				break;
			}
		}
		if (!found)
			it = tip->children.insert(it, TypoInfo(ww[index], "", ""));
		tip = &(*it);

		index++;
	}
	tip->ww = ww;
	tip->rw = rw;

	return 0;
}

static void init_typos(const string &typos_file)
{
	fstream fs;

	fs.open(typos_file.c_str(), fstream::in);

	if (!fs.is_open()) {
		cout << "failed to read typos file[" << typos_file << "]" << endl;
		return;
	}

	string line;
	size_t index;

	while (getline(fs, line)) {
		if (line.size() != 0 && line[0] == '#')
			continue;
		index = line.find("||");
		insert_typo(line.substr(0, index), line.substr(index + 2));
	}

	fs.close();
}

static int search_typo_per_line(const string &line)
{
	int index = 0;
	bool found;
	TypoInfo* tip;
	vector<TypoInfo*> all_tip;
	vector<TypoInfo*>::iterator ait;
	list<TypoInfo>::iterator it;

	while (index < line.size()) {

		if (cidx[line[index]] == -1) {
			// current char is not a~z or A~Z
			all_tip.clear();
			index++;
			continue;
		}

		ait = all_tip.begin();
		while (ait != all_tip.end()) {
			tip = *ait;
			found = false;

			for (it = tip->children.begin(); it != tip->children.end(); it++) {
				if (it->ch == line[index]) {
					found = true;

					if (it->rw != "") {
						cout << "find typo:'" << it->ww << "' maybe it should be '"
							<< it->rw << "'" << endl;
					}

					break;
				}
			}

			if (found) {
				*ait = &(*it);
				ait++;
			} else {
				all_tip.erase(ait);
			}
		}
		all_tip.push_back(&typo_info[cidx[line[index]]]);

		index++;
	}
}

static int search_typo_per_file(const string &file)
{
	fstream fs;

	fs.open(file.c_str(), fstream::in);

	if (!fs.is_open()) {
		cout << "failed to read typos file[" << file << "]" << endl;
		return -1;
	}

	string line;
	size_t index;

	while (getline(fs, line)) {
		search_typo_per_line(line);
	}

	fs.close();
	return 0;
}

static vector<TypoInfo*> words;

static void print_typo(TypoInfo *tip)
{
	words.push_back(tip);

	if (tip->rw != "") {
		cout << "wrong word:'";
		for (int i = 0; i < words.size(); i++) {
			cout << words[i]->ch;
		}
		cout << "' right word:'" << tip->rw << "'" << endl;
	}

	list<TypoInfo>::iterator it;
	for (it = tip->children.begin(); it != tip->children.end(); it++) {
		print_typo(&(*it));
		words.pop_back();
	}
}

static void print_typo_info()
{
	for (int i = 0; i < sizeof(typo_info) / sizeof(TypoInfo); i++) {
		print_typo(typo_info + i);
		words.clear();
	}
}

int main(int argc, char **argv)
{
	cout << "Checking typos, please wait a moment" << endl;

	if (argc != 3) {
		usage();
		return -1;
	}

	init();

	init_typos(string(argv[1]));

	print_typo_info();

	search_typo_per_file(string(argv[2]));

	return 0;
}
