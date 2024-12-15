#ifndef _CRAWLER_H
#define _CRAWLER_H

#define ALPHABET_SIZE 26
#define MAX_WORD_LEN  50
#define BUFFER_SIZE   300000
#define MAX_PAGES     1000

typedef struct TrieNode {
  int wordCount;
  char word[MAX_WORD_LEN];
  struct TrieNode *children[ALPHABET_SIZE];
} TrieNode;

typedef struct PageNode {
  char address[BUFFER_SIZE];
  int termCount;
  TrieNode* root;
} PageNode;

int indexPage(const char *url, TrieNode* root, int* termCount);
int addWordOccurrence(TrieNode* root, const char* word, const int wordLength);
void printTrieContents(TrieNode* node, char* buffer, int depth);
int freeTrieMemory(TrieNode* node);
int getText(const char *srcAddr, char *buffer, const int bufSize);
int contains(const TrieNode* root, const char* address);
PageNode* crawlPage();

#endif