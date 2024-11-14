/* File: indexPage.c */
/* Author: Britton Wolfe */
/* Date: September 3rd, 2010 */

/* This program indexes a web page, printing out the counts of words on that page */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define ALPHABET_SIZE 26

typedef struct TrieNode
{
  int wordCount;
  struct TrieNode *children[ALPHABET_SIZE];
} TrieNode;

// Function prototypes
TrieNode *createTrieNode();
int indexPage(const char *url);
int addWordOccurrence(TrieNode *root, const char *word, const int wordLength);
void printTrieContents(TrieNode *node, char *buffer, int depth);
int freeTrieMemory(TrieNode *node);
int getText(const char *srcAddr, char *buffer, const int bufSize);

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

// Function to create a new trie node
TrieNode *createTrieNode()
{
  TrieNode *newNode = (TrieNode *)malloc(sizeof(TrieNode));

  if (newNode)
  {
    newNode->wordCount = 0;

    for (int i = 0; i < ALPHABET_SIZE; i++)
    {
      newNode->children[i] = NULL;
    }
  }
  else
  {
    (void)fprintf(stderr, "Failed to allocate memory for node!");
    exit(-1);
  }

  return newNode;
}

/* NOTE: int return values can be used to indicate errors (typically non-zero)
   or success (typically zero return value) */

// Function to index a web page
int indexPage(const char *url)
{

  printf("%s\n", url);

  char buffer[300000];
  int bytesRead = getText(url, buffer, sizeof(buffer));

  if (bytesRead == 0)
  {
    fprintf(stderr, "ERROR while retrieving the text from the URL: %s\n", url);
    return 1;
  }

  TrieNode *root = createTrieNode();
  if (!root)
  {
    fprintf(stderr, "ERROR while trying to create trie node\n");
    return 2;
  }

  char word[100];
  int wordLength = 0;

  // Build words & add occurrences
  for (int i = 0; i < bytesRead; i++)
  {
    if (isalpha(buffer[i]))
    {
      word[wordLength++] = tolower(buffer[i]);
    }
    else if (wordLength > 0)
    {
      word[wordLength] = '\0';
      addWordOccurrence(root, word, wordLength);
      printf("\t%s\n", word);
      wordLength = 0;
    }
  }

  if (wordLength > 0)
  {
    word[wordLength] = '\0';
    addWordOccurrence(root, word, wordLength);
    printf("\t%s\n", word);
  }

  printTrieContents(root, word, 0);
  freeTrieMemory(root);

  return 0;
}

// Function to add word occurrence to trie
int addWordOccurrence(TrieNode *root, const char *word, const int wordLength)
{
  TrieNode *currentNode = root;

  for (int i = 0; i < wordLength; i++)
  {
    int index = word[i] - 'a'; // Convert character to index
    if (!currentNode->children[index])
    {
      currentNode->children[index] = createTrieNode();
      if (!currentNode->children[index])
      {
        fprintf(stderr, "ERROR while allocating memory for trie node.\n");
        return -1;
      }
    }
    currentNode = currentNode->children[index];
  }
  currentNode->wordCount += 1; // Increment word count at the end of the word
  return 0;
}

void printTrieContents(TrieNode *node, char *buffer, int depth)
{
  if (node->wordCount > 0)
  {
    buffer[depth] = '\0';
    printf("%s: %d\n", buffer, node->wordCount);
  }

  for (int i = 0; i < ALPHABET_SIZE; i++)
  {
    if (node->children[i])
    {
      buffer[depth] = 'a' + i;
      printTrieContents(node->children[i], buffer, depth + 1);
    }
  }
}

int freeTrieMemory(TrieNode *node)
{
  for (int i = 0; i < ALPHABET_SIZE; i++)
  {
    if (node->children[i])
    {
      freeTrieMemory(node->children[i]);
    }
  }
  free(node);
  return 0;
}

// You should not need to modify this function
int getText(const char *srcAddr, char *buffer, const int bufSize)
{
  FILE *pipe;
  int bytesRead;

  snprintf(buffer, bufSize, "curl -s \"%s\" | python3 getText.py", srcAddr);

  pipe = popen(buffer, "r");
  if (pipe == NULL)
  {
    fprintf(stderr, "ERROR: could not open the pipe for command %s\n",
            buffer);
    return 0;
  }

  bytesRead = fread(buffer, sizeof(char), bufSize - 1, pipe);
  buffer[bytesRead] = '\0';

  pclose(pipe);

  return bytesRead;
}