#include <stdio.h>
#include <stdlib.h>

#include "crawler.h"

int main(int argc, char* argv[]) {
    if (argc != 4) {
    (void) fprintf(stderr, "Invalid number of arguments!\nCommand Usage : crawler <urlFile> <maxPages> <seed>");
    return -1;
  }

  srand(atol(argv[3])); 

  // Read in address file
  FILE* addressFile = fopen(argv[1], "r");
  if (addressFile == NULL) {
      (void) fprintf(stderr, "Failed to open file '%s', aborting!", argv[1]);
      return -1;
  }

  int maxPages = atoi(argv[2]);
  int maxHops;
  int index = 0;
  PageNode indexedPages[MAX_PAGES];
  char address[BUFFER_SIZE];

  // Crawl pages
  while(fscanf(addressFile, "%s %d", address, &maxHops) != EOF && index < maxPages) {
    int hops = 0;

    while(hops <= maxHops && indexedPages < maxPages) {
      printf("Indexing...\n%s\n", address);

      TrieNode *root = createTrieNode();
      if(root == NULL) {
        printf("ERROR: Could not allocate memory for TrieNode.\n");
        return -3;
      }

      int termCount = 0;
      indexPage(address, root, &termCount);
      strncpy(indexedPages[index].address, address, BUFFER_SIZE);

      indexedPages[index].root = root;
      indexedPages[index].termCount = termCount;
      index++;
      
      char newUrl[BUFFER_SIZE];
      if(getText(address, newUrl, sizeof(newUrl)) > 0) {
        strncpy(address, newUrl, BUFFER_SIZE);
      } else {
        break;
      }
      hops++;
    }
  }

  for(int i = 0; i < index; i++) {
    freeTrieMemory(indexedPages[i].root);
  }
  
  fclose(addressFile);
  return 0;
}