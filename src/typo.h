#ifndef _SRC_TYPO_H_
#define _SRC_TYPO_H_

#include <string>

// Initialize typo_info
void init();

// Initialize typo_info, add typos to typo_info
void init_typos(const std::string &typos_file);

// Search typos in file
int search_typo_per_file(const std::string &file);

// Print information of typo_info
void print_typo_info();

#endif
