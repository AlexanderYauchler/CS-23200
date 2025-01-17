/* File: indexPage.c */
/* Author: Britton Wolfe */
/* Date: September 3rd, 2010 */

/* This program indexes a web page, printing out the counts of words on that page */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "crawler.h"

int main(int argc, char **argv)
{
  // argv[1] will be the URL to index, if argc > 1
  if (argc < 2)
  {
    (void)fprintf(stderr, "Invalid argument count!");
    return -1;
  }

  indexPage(argv[1]);

  return 0;
}

TrieNode *createTrieNode() {
  TrieNode* pNode = malloc(sizeof(TrieNode));
  if (pNode == NULL) { return NULL; }

  pNode->wordCount = 0;
  for (int i = 0; i < ALPHABET_SIZE; i++) {
    pNode->children[i] = NULL;
  }

  return pNode;
}

int indexPage(const char *url) {
  char buffer[BUFFER_SIZE];
  int bytesRead = getText(url, buffer, sizeof(buffer));

  if (bytesRead == 0) {
    fprintf(stderr, "ERROR while retrieving the text from the URL: %s\n", url);
    return 1;
  }

  TrieNode *root = createTrieNode();
  if (!root) {
    fprintf(stderr, "ERROR while trying to create trie node\n");
    return 2;
  }

  char word[100];
  int wordLength = 0;

  for (int i = 0; i < bytesRead; i++) {
    if (isalpha(buffer[i])) {
      word[wordLength++] = tolower(buffer[i]);
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

void printTrieContents(TrieNode *node, char *buffer, int depth) {
  if (node->wordCount > 0) {
    buffer[depth] = '\0';
    printf("%s: %d\n", buffer, node->wordCount);
  }

  for (int i = 0; i < ALPHABET_SIZE; i++) {
    if (node->children[i]) {
      buffer[depth] = 'a' + i;
      printTrieContents(node->children[i], buffer, depth + 1);
    }
  }
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