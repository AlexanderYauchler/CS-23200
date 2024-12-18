#ifndef _CRAWLER_H
#define _CRAWLER_H

#define ALPHABET_SIZE 26
#define MAX_WORD_LEN  50
#define BUFFER_SIZE   300000
#define MAX_PAGES     1000
#define MAX_SIZE      1000

typedef struct TrieNode {
  int wordCount;
  char word[MAX_WORD_LEN];
  struct TrieNode *children[ALPHABET_SIZE];
} TrieNode;

typedef struct PageNode {
  char address[BUFFER_SIZE];
  int termCount;
  double score;
  
  struct PageNode* next;
  TrieNode* terms;
} PageNode;

TrieNode* indexPage(const char* url);
int addWordOccurrence(TrieNode* root, const char* word, const int wordLength, const int depth);
void printTrieContents(const TrieNode* node, char* buffer, int depth);
int freeTrieMemory(TrieNode* node);
int getText(const char *srcAddr, char *buffer, const int bufSize);
PageNode* crawlPage();
TrieNode* createTrieNode();

#endif