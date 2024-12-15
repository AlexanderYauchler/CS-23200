#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "webSearch.h"

int main(int argc, char** argv) {
    if (argc != 4) {
        (void) fprintf(stderr, "Invalid Command Arguments!\nUsage: webSearch <fileName> <maxPages> <seed>\n");
        return -1;
    }

    int maxPages = atoi(argv[2]);
    srand(atoi(argv[3]));

    FILE* fp = fopen(argv[1], "r");
    if (!fp) {
        (void) fprintf(stderr, "Error opening file: %s\n", argv[1]);
        return -1;
    }

    PageNode* root = malloc(sizeof(PageNode));
    if (!root) {
        fprintf(stderr, "Failed to allocate memory!\n");
        fclose(fp);
        return -1;
    }

    memset(root->address, 0, MAX_SIZE);

    root->next = NULL;
    root->terms = NULL;
    root->termCount = 0;
    root->score = 0;

    printf("Indexing...\n");

    char url[MAX_SIZE], buffer[MAX_SIZE];
    int limit = 0, hops = 0, pages = 0, totalWords = 0;

    while (fgets(url, MAX_SIZE, fp) != NULL && pages < maxPages) {
        sscanf(url, "%s %d", buffer, &limit);
        buffer[strlen(buffer)] = '\0';

        while (hops <= limit && pages < maxPages) {
            char dest[MAX_SIZE];
            if (!contains(root, buffer)) {
                TrieNode* trie = indexPage(buffer);
                insertBack(root, buffer, trie, &totalWords);
                pages++;
            }
            if (!getLink(buffer, dest, MAX_SIZE)) break;
            hops++;
        }
    }

    /* ============
     * USER QUERIES
     * ============ */

    char userInput[MAX_SIZE];
    printf("Enter a web query: ");

    while (fgets(userInput, MAX_SIZE, stdin) != NULL) {
        if (userInput[0] == '\n') break;
        query(userInput, root, pages);
        printf("\nEnter a web query: ");
    }

    printf("Exiting the program\n");
    destroyList(root);
    fclose(fp);
    return 0;
}

void printScores(int level, char* string, double tfidf) {
    printf("%d. %s: (score: %.4f)\n", level, string, tfidf);
}

double tfScore(PageNode* root, const char* string) {
    if(root->terms != NULL){
        int c = 0;
        char buffer[MAX_SIZE];
        traverse(root->terms, buffer, string, 0, &c);
        return (double) c / root->termCount;
    }
    return 0;
}

double idfScore(PageNode* root, const char* string, int page) {
    if(root->terms != NULL){
        PageNode* current = root;
        int pages = 0;

        while(current != NULL){
            int c = 0;
            char buffer[MAX_SIZE];
            traverse(current->terms, buffer, string, 0, &c);
            if(c > 0){ pages++; }
            current = current->next;
        }
        if(pages > 0) { return log((double) page / (1 + pages)); }
    }
    return 0;
}
void topPages(PageNode* root) {
    Scores* topScores = malloc(sizeof(Scores));

    topScores->nodes[0] = topScores->nodes[1] = topScores->nodes[2] = NULL;
    topScores->scores[0] = topScores->scores[1] = topScores->scores[2] = 0.0;

    PageNode* cursor = root->next;
    while (cursor != NULL) {
        if (cursor->score > topScores->scores[0]) {
            if (topScores->scores[0] != 0.0) {
                topScores->scores[1] = topScores->scores[0];
                topScores->nodes[1] = topScores->nodes[0];
            }
            topScores->scores[0] = cursor->score;
            topScores->nodes[0] = cursor;
        } else if (cursor->score > topScores->scores[1]) {
            topScores->scores[1] = cursor->score;
            topScores->nodes[1] = cursor;
        } else if (cursor->score > topScores->scores[2]) {
            topScores->scores[2] = cursor->score;
            topScores->nodes[2] = cursor;
        }
        cursor = cursor->next;
    }

    printf("Top web pages:\n");
    for (int i = 0; i < 3; i++) {
        if (topScores->nodes[i] != NULL) { 
            printScores(i + 1, topScores->nodes[i]->address, topScores->scores[i]);
        }
    }

    free(topScores);
}

void query(char* userInput, PageNode* root, int pageCount) {
    int len = strlen(userInput);
    for (int i = 0; i < len; i++) {
        if (!isalpha(userInput[i]) || !islower(userInput[i])) {
            if (userInput[i] != '\n' && userInput[i] != ' ') {
                printf("Please enter a query containing only lowercase letters.\n");
                return;
            }
        }
    }
    userInput[len - 1] = '\0';

    printf("Query is \"%s\".\nIDF scores are:\n", userInput);
    char* current = userInput;
    char query[MAX_SIZE];
    int charsRead;

    while (sscanf(current, "%s%n", query, &charsRead) == 1) {\
        double score = idfScore(root, query, pageCount);
        printf("IDF(%s): %.4f\n", query, score);

        PageNode* cursor = root->next;
        while (cursor != NULL) {
            cursor->score += score * tfScore(cursor, query);
            cursor = cursor->next;
        }
        current += charsRead;
    }

    topPages(root);

    PageNode* cursor = root->next;
    while (cursor != NULL) {
        cursor->score = 0.0;
        cursor = cursor->next;
    }
}

int contains(const PageNode* pNode, const char *addr) {
  if(pNode == NULL) { return 0; }
  if(!strncmp(pNode->address, addr, strlen(addr))) { return 1; }

  return contains(pNode->next, addr);
}

void insertBack(PageNode* pNode, const char *addr, TrieNode* words, int* count) {
  if (pNode->next == NULL) {
    PageNode* newNode = malloc(sizeof(PageNode));

    if (newNode == NULL) { fprintf(stderr, "could not allocate memory\n");  return; }
    strncpy(newNode->address, addr, BUFFER_SIZE);

    newNode->address[BUFFER_SIZE - 1] = '\0';
    newNode->terms = words;
    newNode->score = 0;
    newNode->next = NULL;

    pNode->next = newNode;
  } else { insertBack(pNode->next, addr, words, count); }
}

void printAddresses(const PageNode *pNode) {
  if(pNode == NULL){ return; }

  printf("%s\n", pNode->address);

  if (pNode->terms != NULL) {
    char buffer[10000];
    printTrieContents(pNode->terms, buffer, 0);
  }

  printAddresses(pNode->next);
}

void destroyList(PageNode* pNode){
  //if node does not exist
  if(pNode == NULL) { return; }

  destroyList(pNode->next);
  if (pNode->terms != NULL) { freeTrieMemory(pNode->terms); }

  free(pNode);
}

int getLink(const char* srcAddr, char* link, const int maxLinkLength) {
  const int bufSize = 1000;
  char buffer[bufSize];
  int numLinks = 0;

  snprintf(buffer, bufSize, "curl -s \"%s\" | python3 getLinks.py", srcAddr);
  FILE *pipe = popen(buffer, "r");
  if(pipe == NULL){
    fprintf(stderr, "ERROR: could not open the pipe for command %s\n", buffer);
    return 0;
  }

  fscanf(pipe, "%d\n", &numLinks);
  if(numLinks > 0){
    for(int linkNum = 0; linkNum < numLinks; linkNum++){
      fgets(buffer, bufSize, pipe);

      char* pNewline = strchr(buffer, '\n');
      if(pNewline != NULL) { *pNewline = '\0'; }

      if(linkNum == 0) {
        strncpy(link, buffer, maxLinkLength);
        link[maxLinkLength-1] = '\0';
      }
    }
  }

  pclose(pipe);
  if(numLinks > 0) { return 1; }
  else { return 0; }
}

void traverse(const TrieNode* root, char* string, const char* target, int level, int* n) {
  if (root->wordCount && strncmp(target, string, level) == 0) {

    if (strlen(string) == level) { *n = root->wordCount; return;  }

    for (int i =0; i < 26; i++) {
      if (root->children[i] != NULL) {
        string[level] = i + 'a';
        traverse(root->children[i], string, target, level+1, n);
      }
    }
  }
}