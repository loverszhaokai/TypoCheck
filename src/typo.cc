#include "typo.h"

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

static TypoInfo typo_info; // This the root

void init()
{
	for (int i = 0; i < sizeof(cidx) / sizeof(char); i++) {
		if (i >= 'a' && i <= 'z')
			cidx[i] = i - 'a';
		else if (i >= 'A' && i <= 'Z')
			cidx[i] = i - 'A';
		else
			cidx[i] = -1;
	}
}

static int insert_typo(const string &ww, const string &rw)
{
	int index = 0;
	TypoInfo *tip;
	bool found;
	list<TypoInfo>::iterator it;

	tip = &typo_info;

	while (index < ww.size()) {
		found = false;
		for (it = tip->children.begin(); it != tip->children.end(); it++) {
			if (it->ch == ww[index]) {
				found = true;
				break;
			} else if (it->ch > ww[index]) {
				// The children is from A to Z and a to z
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

void init_typos(const string &typos_file)
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
	TypoInfo* tip = &typo_info;
	vector<TypoInfo*> all_tip;
	vector<TypoInfo*>::iterator ait;
	list<TypoInfo>::iterator it;

	while (index < line.size()) {

		if (cidx[line[index]] == -1) {
			// current char is not a~z or A~Z, so it is a delimiter

			if (found && tip->rw != "") {
				cout << "find typo:'" << tip->ww << "' maybe it should be '"
					<< tip->rw << "'" << endl;
			}

			tip = &typo_info;
			index++;
			continue;
		}

		found = false;
		for (it = tip->children.begin(); it != tip->children.end(); it++) {
			if (it->ch == line[index]) {
				found = true;


				break;
			} else if (it->ch > line[index]) {
				// The children is from A to Z and a to z
				break;
			}
		}

		if (found)
			tip = &(*it);
		else
			tip = &typo_info;

		index++;
	}

	// The last word is a typo
	if (found && tip->rw != "") {
		cout << "find typo:'" << tip->ww << "' maybe it should be '"
			<< tip->rw << "'" << endl;
	}

	return 0;
}

int search_typo_per_file(const string &file)
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

void print_typo_info()
{
	print_typo(&typo_info);
}

