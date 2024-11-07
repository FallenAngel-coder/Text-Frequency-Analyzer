#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <wchar.h>
#include <wctype.h>
#include <time.h>

#define DEBUG

#define MAX_WORD_LENGTH 100

void set_utf8_locale();
char* GetText(const char* path);
void ClearText(wchar_t* data);
void ToMultiMap(const wchar_t* data, wchar_t*** words, int** counts, int* size);
void BanList(wchar_t*** words, int** counts, int* size, const char* path, wchar_t*** newWords, int** newCounts, int* newSize);
void SaveResult(wchar_t** words, int* counts, int size, const char* path, const char* path1);

void set_utf8_locale() {
    if (!setlocale(LC_ALL, "uk_UA.UTF-8")) {
        perror("Locale error");
        exit(1);
    }
}

char* GetText(const char* path) {
    FILE* file = fopen(path, "r");
    if (!file) {
        perror("File open error");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* data = (char*)malloc(file_size + 1);
    if (!data) {
        perror("Memory allocation error");
        fclose(file);
        return NULL;
    }

    fread(data, 1, file_size, file);
    data[file_size] = '\0';

    fclose(file);
    return data;
}

void ClearText(wchar_t* data) {
    size_t length = wcslen(data);
    size_t j = 0;

    for (size_t i = 0; i < length; i++) {
        wchar_t ch = data[i];
        if (iswalpha(ch))
            data[j++] = towlower(ch);
        else if (iswspace(ch)) 
            data[j++] = L' ';
    }
    data[j] = L'\0';
}

void ToMultiMap(const wchar_t* data, wchar_t*** words, int** counts, int* size) {
    if (data == NULL)
        return;
      
    wchar_t* temp = _wcsdup(data); 
    wchar_t* token = wcstok(temp, L" ");  
    int capacity = 100;  

    *words = (wchar_t**)malloc(capacity * sizeof(wchar_t*));
    *counts = (int*)malloc(capacity * sizeof(int));
    *size = 0;

    if (!(*words) || !(*counts)) {
        perror("Memory allocation error");
        free(temp);
        exit(1);
    }

    while (token) {
        if (wcslen(token) > 20) {
            token = wcstok(NULL, L" ");
            continue;
        }
        int found = 0;
        for (int i = 0; i < *size; i++) {
            if (wcscmp((*words)[i], token) == 0) {
                (*counts)[i]++;  
                found = 1;
                break;
            }
        }

        if (!found) {
            if (*size >= capacity) {
                capacity *= 2;  
                wchar_t** newWords = (wchar_t**)realloc(*words, capacity * sizeof(wchar_t*));
                int* newCounts = (int*)realloc(*counts, capacity * sizeof(int));
                if (newWords == NULL || newCounts == NULL) {
                    perror("Memory overallocation error");
                    free(*words);
                    free(*counts);
                    free(temp);
                    exit(1);
                }

                *words = newWords;
                *counts = newCounts;
            }
            (*words)[*size] = _wcsdup(token);
            (*counts)[*size] = 1;
            (*size)++;
        }

        token = wcstok(NULL, L" ");  
    }

    free(temp);  
}

void BanList(wchar_t*** words, int** counts, int* size, const char* path, wchar_t*** newWords, int** newCounts, int* newSize) {
    char* data = GetText(path);
    if (!data) return;

    wchar_t* wdata = (wchar_t*)malloc((strlen(data) + 1) * sizeof(wchar_t));
    mbstowcs(wdata, data, strlen(data) + 1);
    ClearText(wdata);
    wchar_t** banWords;
    int* banCounts;
    int banSize;

    ToMultiMap(wdata, &banWords, &banCounts, &banSize);

    *newWords = (wchar_t**)malloc(*size * sizeof(wchar_t*) * 2);
    *newCounts = (int*)malloc(*size * sizeof(int) + 8);
    *newSize = 0;

    for (int i = 0; i < *size; i++) {
        int banned = 0;
        for (int j = 0; j < banSize; j++) {
            if (wcscmp((*words)[i], banWords[j]) == 0) {
                banned = 1;
                break;
            }
        }
        if (!banned) {
            (*newWords)[*newSize] = _wcsdup((*words)[i]);
            (*newCounts)[*newSize] = (*counts)[i];
            (*newSize)++;
        }
    }

    for (int i = 0; i < banSize; i++) free(banWords[i]);
    free(banWords);
    free(banCounts);
    free(data);
}

void SaveResult(wchar_t** words, int* counts, int size, const char* path, const char* path1) {
    FILE* streamr = fopen(path, "w");
    FILE* streamm = fopen(path1, "w");

    if (!streamr || !streamm) {
        perror("Writing error");
        if (streamr) fclose(streamr);
        if (streamm) fclose(streamm);
        return;
    }

    fprintf(streamr, "\xEF\xBB\xBF");  
    fprintf(streamm, "\xEF\xBB\xBF");  
  
    int totalWords = 0;
    for (int i = 0; i < size; i++) 
        totalWords += counts[i];  
    fprintf(streamr, "Unique words: %.5f%%\n", ((float)size / totalWords) * 100);

    for (int i = 0; i < size; i++) {
        float frequencyPercentage = ((float)counts[i] / totalWords) * 100;
        fwprintf(streamr, L"%s: %.5f%%\n", words[i], frequencyPercentage);
        if (frequencyPercentage > 10) 
            fwprintf(streamm, L"%s: %.5f%%\n", words[i], frequencyPercentage);
        
    }

    fclose(streamr);
    fclose(streamm);
}


int main(int argc, char* argv[]) {
    set_utf8_locale();
    const char* inputPath;
    const char* banlistPath;
    const char* resultPath;
    const char* result1Path;
    if (argc >= 5) {
        inputPath = argv[1];
        banlistPath = argv[2];
        resultPath = argv[3];
        result1Path = argv[4];
    }
    else {
        printf("Input files path in next order: file banlist result result1: ");
        char inputBuffer[1024];
        fgets(inputBuffer, sizeof(inputBuffer), stdin);

        char inputFile[256], banlistFile[256], resultFile[256], result1File[256];
        if (sscanf(inputBuffer, "%255s %255s %255s %255s", inputFile, banlistFile, resultFile, result1File) != 4) {
            fprintf(stderr, "Incorrect input\n");
            return 1;
        }

        inputPath = _strdup(inputFile);
        banlistPath = _strdup(banlistFile);
        resultPath = _strdup(resultFile);
        result1Path = _strdup(result1File);
    }
#ifdef DEBUG
    clock_t begin = clock();
#endif
    char* data = GetText(inputPath);
    if (!data) return 1;

    wchar_t* wdata = (wchar_t*)malloc((strlen(data) + 1) * sizeof(wchar_t));
    mbstowcs(wdata, data, strlen(data) + 1);
    ClearText(wdata);

    wchar_t** words;
    int* counts;
    int size;
    ToMultiMap(wdata, &words, &counts, &size);

    wchar_t** filteredWords;
    int* filteredCounts;
    int filteredSize;
    BanList(&words, &counts, &size, banlistPath, &filteredWords, &filteredCounts, &filteredSize);

    SaveResult(filteredWords, filteredCounts, filteredSize, resultPath, result1Path);
#ifdef DEBUG
    clock_t end = clock();
    double elapsed_ms = (double)(end - begin) / CLOCKS_PER_SEC * 1000;
    printf("\nThe time: %.2f ms\n", elapsed_ms);
#endif
    for (int i = 0; i < size; i++) free(words[i]);
    free(words);
    free(counts);
    for (int i = 0; i < filteredSize; i++) free(filteredWords[i]);
    free(filteredWords);
    free(filteredCounts);
    free(wdata);
    free(data);
    if (argc < 5) {
        free((void*)inputPath);
        free((void*)banlistPath);
        free((void*)resultPath);
        free((void*)result1Path);
    }
    return 0;
}
