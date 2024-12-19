#ifndef _WEB_SEACH
#define _WEB_SEARCH

#include "crawler.h"

typedef struct Scores {
    double scores[3];
    PageNode* nodes[3];
} Scores;

void topPages(PageNode* root);
void query(char* userInput, struct PageNode* root, int pageCount);
void printScores(int level, char* string, double tfidf);
double idfScore(PageNode* root, const char* string, int page);
double tfScore(PageNode* root, const char* string);
void printAddresses(const PageNode* pNode);
void destroyList(PageNode* pNode);
int contains(const PageNode* pNode, const char *addr);
void insertBack(PageNode* pNode, const char *addr, TrieNode* words, int* count);
void topPages(PageNode* root);
void traverse(const TrieNode* root, char* string, const char* target, int level, int* n);
int getLink(const char* srcAddr, char* link, const int maxLinkLength);

#endif
