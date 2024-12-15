#include <stdio.h>
#include <stdlib.h>

#include "crawler.h"

TrieNode *createTrieNode() {
  TrieNode *newNode = (TrieNode *)malloc(sizeof(TrieNode));

  if (newNode == NULL) { return NULL; }

  newNode->wordCount = 0;
  for (int i = 0; i < ALPHABET_SIZE; i++) {
    newNode->children[i] = NULL;
  }

  return newNode;
}

int indexPage(const char *url, TrieNode* root, int* termCount) {
  char buffer[300000];
  int bytesRead = getText(url, buffer, sizeof(buffer));

  if (bytesRead == 0) {
    (void) fprintf(stderr, "ERROR while retrieving the text from the URL: %s\n", url);
    return 1;
  } 

  char word[MAX_WORD_LEN];
  int wordLength = 0;

  for (int i = 0; i < bytesRead; i++) {
    if (isalpha(buffer[i])) {
      word[wordLength++] = tolower(buffer[i]);
      if(wordLength >= MAX_WORD_LEN - 1) break;
    } else if (wordLength > 0) {
      word[wordLength] = '\0';
      addWordOccurrence(root, word, wordLength);
      wordLength = 0;
    }
  }

  if (wordLength > 0) {
    word[wordLength] = '\0';
    addWordOccurrence(root, word, wordLength);
  }

  return 0;
}

int addWordOccurrence(TrieNode *root, const char *word, const int wordLength) {
  TrieNode *currentNode = root;

  for (int i = 0; i < wordLength; i++) {
    int index = word[i] - 'a';

    if (!currentNode->children[index]) {
      currentNode->children[index] = createTrieNode();

      if (!currentNode->children[index]) {
        fprintf(stderr, "ERROR while allocating memory for trie node.\n");
        return -1;
      }
    }
    currentNode = currentNode->children[index];
  }

  currentNode->wordCount += 1;
  return 0;
}

int freeTrieMemory(TrieNode *node) {
  for (int i = 0; i < ALPHABET_SIZE; i++) {
    if (node->children[i]) {
      freeTrieMemory(node->children[i]);
    }
  }
  free(node);
  return 0;
}

int getText(const char *srcAddr, char *buffer, const int bufSize) {
  FILE *pipe;
  int bytesRead;

  snprintf(buffer, bufSize, "curl -s \"%s\" | python3 getText.py", srcAddr);

  pipe = popen(buffer, "r");
  if (pipe == NULL) {
    fprintf(stderr, "ERROR: could not open the pipe for command %s\n",
            buffer);
    return 0;
  }

  bytesRead = fread(buffer, sizeof(char), bufSize - 1, pipe);
  buffer[bytesRead] = '\0';

  pclose(pipe);

  return bytesRead;
}