#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

TrieNode* indexPage(const char* url) {
  char buffer[BUFFER_SIZE];
  getText(url, buffer, 300000);
  printf("%s\n", url);

  TrieNode* root = malloc(sizeof(TrieNode));
  if (root == NULL) return NULL;
  root->wordCount = 0;
  for (int i = 0; i < 26; i++) { root->children[i] = NULL;  }

  for (int i = 0; buffer[i] != '\0'; i++) { buffer[i] = isalpha(buffer[i]) ? tolower(buffer[i]) : ' '; }

  int counter = 0;
  char word[50];

  while(sscanf(buffer+counter, "%s", word) == 1) {
    printf("\t%s\n", word);
    counter+=strlen(word);
    while (isspace(buffer[counter])) { counter++; }
    addWordOccurrence(root, word, strlen(word), 0); // adds word to trie
  }
  return root;
}

int addWordOccurrence(TrieNode* root, const char* word, const int wordLength, const int depth ) {
  if (wordLength == depth) {
    root->wordCount++;
  }
  else {
    if (root->children[word[depth] - 'a'] == NULL) {
      root->children[word[depth] - 'a'] = (TrieNode*)malloc(sizeof(TrieNode));
      if (!root->children[word[depth] - 'a']) return -1;

      for (int i = 0; i < 26; i++) { root->children[word[depth] - 'a'] ->children[i] = NULL; }

      if (root->children[word[depth] - 'a']  == NULL) { return -1; }

      char temp[2] = { tolower(word[depth]), '\0' };
      strncpy(root->children[word[depth] - 'a']->word, temp, MAX_WORD_LEN); // uhh?
      root->children[word[depth] - 'a'] ->wordCount = 0;
    }
    addWordOccurrence(root->children[word[depth] - 'a'] , word, wordLength, depth + 1);
  }
  return 1;
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

void printTrieContents(const TrieNode* root, char* buffer, int depth) {
  if (root == NULL) { return; }

  if (depth > 0) { buffer[depth-1] = root->word[0]; }

  if(root->wordCount > 0){
    buffer[depth] = '\0';
    printf("%s: %d\n", buffer, root->wordCount);
  }

    for(int i = 0; i < 26; i++){ 
        if(root->children[i] != NULL){ 
            printTrieContents(root->children[i], buffer, depth+1);
        } 
    }
}