#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define DEBUG

#define MAX_WORD_LENGTH 100

char* GetText(const char* path);
void ClearText(char* data);
void ToMultiMap(const char* data, char*** words, int** counts, int* size);
void BanList(char*** words, int** counts, int* size, const char* path, char*** newWords, int** newCounts, int* newSize);
void ToMap(char** words, int* counts, int size, char*** newWords, int** newCounts, int* newSize);
void SaveResult(char** words, int* counts, int size, char** newWords, int* newCounts, int newSize, const char* path, const char* path1);

char* GetText(const char* path) {
    FILE* file = fopen(path, "r");
    if (!file) {
        perror("Failed to open file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file); 
    fseek(file, 0, SEEK_SET); 

    char* data = (char*)malloc(file_size + 1);
    if (!data) {
        perror("Failed to allocate memory for file content");
        fclose(file);
        return NULL;
    }
    size_t read_size = fread(data, 1, file_size, file);

    data[file_size] = '\0';

    fclose(file);
    return data;
}


void ClearText(char* data) {
    size_t length = strlen(data);
    for (size_t i = 0; i < length; i++) {
        data[i] = tolower(data[i]);
        if ((data[i] < 'a' || data[i] > 'z') && data[i] != ' ') {
            memmove(&data[i], &data[i + 1], length - i);
            length--;
            i--;
        }
    }
}

void ToMultiMap(const char* data, char*** words, int** counts, int* size) {
    char* temp = _strdup(data);
    if (!temp) return;

    char* token = strtok(temp, " ");
    int capacity = 10;
    *words = (char**)malloc(capacity * sizeof(char*));
    *counts = (int*)malloc(capacity * sizeof(int));
    *size = 0;

    if (!*words || !*counts) {
        perror("Failed to allocate memory in ToMultiMap");
        free(temp);
        return;
    }

    while (token != NULL) {
        if (*size >= capacity) {
            capacity *= 2;
            *words = (char**)realloc(*words, capacity * sizeof(char*));
            *counts = (int*)realloc(*counts, capacity * sizeof(int));
            if (!*words || !*counts) {
                perror("Failed to reallocate memory in ToMultiMap");
                free(temp);
                return;
            }
        }
        (*words)[*size] = _strdup(token);
        if (!(*words)[*size]) {
            perror("Failed to duplicate token");
            free(temp);
            return;
        }
        (*counts)[*size] = *size;
        (*size)++;
        token = strtok(NULL, " ");
    }
    free(temp);
}

void BanList(char*** words, int** counts, int* size, const char* path, char*** newWords, int** newCounts, int* newSize) {
    char* data1 = GetText(path);
    if (!data1) return;

    char** banWords;
    int* banCounts; 
    int banSize;
    ToMultiMap(data1, &banWords, &banCounts, &banSize);

    for (int i = 0; i < banSize; i++) {
        ClearText(banWords[i]);
    }

    int capacity = 10;
    *newWords = (char**)malloc(capacity * sizeof(char*));
    *newCounts = (int*)malloc(capacity * sizeof(int));
    *newSize = 0;

    //if (!*newWords || !*newCounts) {
    //    perror("Failed to allocate memory in BanList");
    //    free(data1);
    //    for (int k = 0; k < banSize; k++) {
    //        free(banWords[k]);
    //    }
    //    free(banWords);
    //    return;
    //}

    for (int i = 0; i < *size; i++) {
        char* normalizedWord = _strdup((*words)[i]);
        if (!normalizedWord) {
            perror("Failed to duplicate word");
            continue;
        }
        ClearText(normalizedWord);  

        int found = 0;
        for (int j = 0; j < banSize; j++) {
            if (strcmp(normalizedWord, banWords[j]) == 0) {  
                found = 1;
                break;
            }
        }
        if (!found) {
            if (*newSize >= capacity) {
                capacity *= 2;
                *newWords = (char**)realloc(*newWords, capacity * sizeof(char*));
                *newCounts = (int*)realloc(*newCounts, capacity * sizeof(int));
                if (!*newWords || !*newCounts) {
                    free(data1);
                    for (int k = 0; k < banSize; k++) {
                        free(banWords[k]);
                    }
                    free(banWords);
                    return;
                }
            }
            (*newWords)[*newSize] = normalizedWord;
            (*newCounts)[*newSize] = (*counts)[i];
            (*newSize)++;
        }
        else {
            free(normalizedWord);  
        }
    }

    for (int i = 0; i < banSize; i++) {
        free(banWords[i]);
    }
    free(banWords);
    free(banCounts);  
    free(data1);
}

typedef struct HashEntry {
    char* word;
    int count;
    struct HashEntry* next; 
} HashEntry;

typedef struct {
    HashEntry** table;
    int size;
    int capacity;
} HashTable;

unsigned int hash(const char* str, int capacity) {
    unsigned int hashValue = 0;
    while (*str)
        hashValue = (hashValue * 31) + (*str++); 
    return hashValue % capacity;
}

HashTable* createHashTable(int capacity) {
    HashTable* hashTable = (HashTable*)malloc(sizeof(HashTable));
    if (!hashTable) {
        perror("Failed to allocate memory for hash table");
        return NULL;
    }
    hashTable->size = 0;
    hashTable->capacity = capacity;
    hashTable->table = (HashEntry**)calloc(capacity, sizeof(HashEntry*));
    if (!hashTable->table) {
        perror("Failed to allocate memory for hash table table");
        free(hashTable);
        return NULL;
    }
    return hashTable;
}

void insertOrUpdate(HashTable* hashTable, const char* word) {
    unsigned int index = hash(word, hashTable->capacity);
    HashEntry* entry = hashTable->table[index];

    while (entry) {
        if (strcmp(entry->word, word) == 0) {
            entry->count++;
            return; 
        }
        entry = entry->next;
    }

    HashEntry* newEntry = (HashEntry*)malloc(sizeof(HashEntry));
    if (!newEntry) {
        perror("Failed to allocate memory for new hash entry");
        return;
    }
    newEntry->word = _strdup(word);
    if (!newEntry->word) {
        perror("Failed to allocate memory for new word");
        free(newEntry);
        return;
    }
    newEntry->count = 1;
    newEntry->next = hashTable->table[index];
    hashTable->table[index] = newEntry;
    hashTable->size++;
}

void hashTableToArray(HashTable* hashTable, char*** newWords, int** newCounts, int* newSize) {
    *newWords = (char**)malloc(hashTable->size * sizeof(char*));
    *newCounts = (int*)malloc(hashTable->size * sizeof(int));
    *newSize = hashTable->size;

    if (!*newWords || !*newCounts) {
        perror("Failed to allocate memory in hashTableToArray");
        return;
    }

    int index = 0;
    for (int i = 0; i < hashTable->capacity; i++) {
        HashEntry* entry = hashTable->table[i];
        while (entry) {
            (*newWords)[index] = entry->word;
            (*newCounts)[index] = entry->count;
            index++;
            entry = entry->next;
        }
    }
}

void ToMap(char** words, int* counts, int size, char*** newWords, int** newCounts, int* newSize) {
    HashTable* hashTable = createHashTable(size * 2);
    if (!hashTable) return;

    for (int i = 0; i < size; i++) 
        insertOrUpdate(hashTable, words[i]);

    hashTableToArray(hashTable, newWords, newCounts, newSize);
}

void SaveResult(char** words, int* counts, int size, char** newWords, int* newCounts, int newSize, const char* path, const char* path1) {
    FILE* streamr = fopen(path, "w");
    FILE* streamm = fopen(path1, "w");
    if (!streamr || !streamm) {
        perror("Failed to open result files");
        if (streamr) fclose(streamr);
        if (streamm) fclose(streamm);
        return;
    }

    float totalSize = (float)size;
    float uniquePercentage = ((float)newSize / totalSize) * 100;
    fprintf(streamr, "Unique words: %.5f%%\n", uniquePercentage);

    for (int i = 0; i < newSize; i++) {
        float frequencyPercentage = ((float)newCounts[i] / totalSize) * 100;
        fprintf(streamr, "%s: %.5f%%\n", newWords[i], frequencyPercentage);

        if (frequencyPercentage > 10.0)
            fprintf(streamm, "%s: %.5f%%\n", newWords[i], frequencyPercentage);
    }

    fclose(streamm);
    fclose(streamr);

    char* data = GetText(path1);
    if (data) {
        if (*data)
            printf("Most used: \n%s", data);
        else
            printf("No words that have > 10%% usage\n");
        free(data);
    }
    else
        printf("Error reading data from file: %s\n", path1);
}

int main(int argc, char* argv[]) {
#ifdef DEBUG
    clock_t begin = clock();
#endif

    if (argc < 5) return 1;

    char* data = GetText(argv[1]);
    if (!data) return 1;

    ClearText(data);

    char** multWords;
    int* multCounts;
    int multSize;
    ToMultiMap(data, &multWords, &multCounts, &multSize);

    char** filteredWords;
    int* filteredCounts;
    int filteredSize;
    BanList(&multWords, &multCounts, &multSize, argv[2], &filteredWords, &filteredCounts, &filteredSize);

    char** finalWords;
    int* finalCounts;
    int finalSize;
    ToMap(filteredWords, filteredCounts, filteredSize, &finalWords, &finalCounts, &finalSize);

    SaveResult(multWords, multCounts, filteredSize, finalWords, finalCounts, finalSize, argv[3], argv[4]);

    for (int i = 0; i < multSize; i++) free(multWords[i]);
    free(multWords);
    free(multCounts);

    for (int i = 0; i < filteredSize; i++) free(filteredWords[i]);
    free(filteredWords);
    free(filteredCounts);

    for (int i = 0; i < finalSize; i++) free(finalWords[i]);
    free(finalWords);
    free(finalCounts);

    free(data);

#ifdef DEBUG
    clock_t end = clock();
    double elapsed_ms = (double)(end - begin) / CLOCKS_PER_SEC * 1000;
    printf("\nThe time: %.2f ms\n", elapsed_ms);
#endif

    return 0;
}
