/**
 * dictionary.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Implements a dictionary's functionality.
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "dictionary.h"

uint32_t dictsize = 0; // number of words in loaded dictionary, 0 if not loaded
struct node* hashtable[HASHSIZE]; // hash table

/**
 * @brief: singly linked list of strings
 */
struct node{
	char* val;
	struct node* next;
};

/**
 * @brief: murmur hash
 * @param key: string to be hashed
 * @param length: length of key string
 * @param seed: seed for hash function
 *
 * copied from https://en.wikipedia.org/wiki/MurmurHash
 */
uint32_t murmur3_32(const char *key, uint32_t len, uint32_t seed){
	static const uint32_t c1 = 0xcc9e2d51;
	static const uint32_t c2 = 0x1b873593;
	static const uint32_t r1 = 15;
	static const uint32_t r2 = 13;
	static const uint32_t m = 5;
	static const uint32_t n = 0xe6546b64;

	uint32_t hash = seed;

	const int nblocks = len / 4;
	const uint32_t *blocks = (const uint32_t *) key;
	int i;
	for (i = 0; i < nblocks; i++) {
		uint32_t k = blocks[i];
		k *= c1;
		k = (k << r1) | (k >> (32 - r1));
		k *= c2;

		hash ^= k;
		hash = ((hash << r2) | (hash >> (32 - r2))) * m + n;
	}

	const uint8_t *tail = (const uint8_t *) (key + nblocks * 4);
	uint32_t k1 = 0;

	switch (len & 3) {
	case 3:
		k1 ^= tail[2] << 16;
	case 2:
		k1 ^= tail[1] << 8;
	case 1:
		k1 ^= tail[0];

		k1 *= c1;
		k1 = (k1 << r1) | (k1 >> (32 - r1));
		k1 *= c2;
		hash ^= k1;
	}

	hash ^= len;
	hash ^= (hash >> 16);
	hash *= 0x85ebca6b;
	hash ^= (hash >> 13);
	hash *= 0xc2b2ae35;
	hash ^= (hash >> 16);

	return hash;
}

/**
 * @brief: check if word is in dictionary
 * @param word: word to look for dictionary
 * @return: true if word is in dictionary else false.
 */
bool check(const char* word){
	uint32_t hashkey; // hash table index of word
	struct node* crawler; // to crawl through singly-linked list
	int wordlen = strlen(word); // length of word
	char lowerword[wordlen+1]; // to store lower case of word

	// get hashkey
	for(int i = 0; word[i]; i++)
		lowerword[i] = tolower(word[i]);
	lowerword[wordlen] = '\0';
	hashkey = murmur3_32(lowerword, (uint32_t) strlen(lowerword), HASHSEED) % HASHSIZE;

	// if hash bin is empty, return immediately
	if(!hashtable[hashkey])
		return false;

	else{
		// crawl through hash bin & check for word
		crawler = hashtable[hashkey];
		while(crawler){
			if(strcmp(crawler->val,lowerword) == 0)
				return true;
			else
				crawler = crawler->next;
		}

		// if word is not found, return false
		return false;
	}
}

/**
 * @brief: loads dictionary into memory.
 * @param dictionary: file name of dictionary
 * @return: true if successful, else false.
 */
bool load(const char* dictionary){
	char word[LENGTH + 2]; // buffer to read dictionary words into (2 for \n and \0)
	uint32_t hashkey; // hash table index of word
	struct node* temp; // temporary node for construction

	// open dictionary
	FILE* dictfile = fopen(dictionary, "r");
	if(!dictfile){
		printf("Error opening dictionary at %s.\n", dictionary);
		return false;
	}

	// clear hash table & dictionary size
	unload();
	dictsize  = 0;

	// read word by word & enter into hash table
	while(fgets(word, sizeof(word)/sizeof(char), dictfile)){
		// trim new line character
		if(word[strlen(word)-1] == '\n')
			word[strlen(word)-1] = '\0';

		// get hash key
		hashkey = murmur3_32(word, (uint32_t) strlen(word), HASHSEED) % HASHSIZE;

		// copy  word into a node
		temp = (struct node*) malloc(sizeof(struct node));
		temp->val = (char*) malloc((strlen(word) + 1) * sizeof(char));
		strncpy(temp->val, word, strlen(word));
		temp->val[strlen(word)] = '\0';

		// add to singly linked list
		if(!hashtable[hashkey])
			temp->next = NULL;
		else
			temp->next = hashtable[hashkey];
		hashtable[hashkey] = temp;

		// count dictionary size
		dictsize++;
	}

	// end
	return true;
}

/**
 * @brief: counts number of words in dictionary
 * @return: number of words if dictionary loaded or 0 if not loaded.
 */
unsigned int size(void){
	return dictsize;
}

/**
 * @brief: unloads dictionary from memory
 * @return: true if successful, else false.
 */
bool unload(void){
	struct node* frontcrawler; // to crawl through singly-linked list
	struct node* backcrawler;

	// free each element of hash table
	for(uint32_t i = 0; i < HASHSIZE; i++){
		// free singly linked lists in hash table
		if(hashtable[i]){
			backcrawler = hashtable[i];
			while(backcrawler){
				frontcrawler = backcrawler->next;
				free(backcrawler->val);
				free(backcrawler);
				backcrawler = frontcrawler;
			}
		}
	}

	// end
	return true;
}