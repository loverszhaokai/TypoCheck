#include <iostream>
#include <list>
#include <string>
#include <vector>

using std::cout;
using std::endl;
using std::list;
using std::string;
using std::vector;

struct TypoInfo {

	TypoInfo()
	{
		ch = ' ';
		rw = "";
		children = list<TypoInfo>();
	}

	TypoInfo(char _ch, const string &_rw)
	{
		ch = _ch;
		rw = _rw;
		children = list<TypoInfo>();
	}

	char ch;
	string rw; // right word

	list<TypoInfo> children;
};

char cidx[256]; // char to index, a~z and A-Z to 0~25, others to 255

TypoInfo typo_info[26];

void init()
{
	for (int i = 0; i < sizeof(cidx) / sizeof(char); i++) {
		if (i >= 'a' && i <= 'z')
			cidx[i] = i - 'a';
		else if (i >= 'A' && i <= 'Z')
			cidx[i] = i - 'A';
		else
			cidx[i] = 255;
	}

	for (int i = 0; i < sizeof(typo_info) / sizeof(TypoInfo); i++) {
		typo_info[i].ch = 'a' + i;
	}
}

int insert_typo(const string &ww, const string &rw)
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
			it = tip->children.insert(it, TypoInfo(ww[index], ""));
		tip = &(*it);

		index++;
	}
	tip->rw = rw;


	return 0;
}

void init_typos()
{
	insert_typo("abcced", "abced");
	insert_typo("kaii", "kai");
	insert_typo("alaxander", "alexander");
}

vector<TypoInfo*> words;

void print_typo(TypoInfo *tip)
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
	for (int i = 0; i < sizeof(typo_info) / sizeof(TypoInfo); i++) {
		print_typo(typo_info + i);
		words.clear();
	}
}

int main()
{
	cout << "Checking typos, please wait a moment" << endl;

	init();

	init_typos();

	print_typo_info();

	return 0;
}
