#include "keeper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// Method which writes data to a given file stream
// This is provided so this code also works on Windows.
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

int download_jenkins_war()
{
    // Declarations
    CURL *ehndl;
    FILE *file;
    CURLcode res;
    // Initialize curl capabilities
    curl_global_init(CURL_GLOBAL_ALL);

    // Initialize an easy curl which takes care most of the things we will need
    ehndl = curl_easy_init();
    if (!ehndl) {
        printf("Could not initialize curl.\n");
        return 1;
    }
    printf("downloading newest jenkins war from: %s\n", JENKINS_WAR_LATEST_URL);
    curl_easy_setopt(ehndl, CURLOPT_URL, JENKINS_WAR_LATEST_URL);
    // Download URLs for Jenkins all redirect
    curl_easy_setopt(ehndl, CURLOPT_FOLLOWLOCATION, 1L);
    // But don't allow too many...
    curl_easy_setopt(ehndl, CURLOPT_MAXREDIRS, 50L);

    // Setup the file to which we will write to
    char fName[50] = JENKINS_WAR_FILENAME;
    file = fopen(fName, "wb");
    curl_easy_setopt(ehndl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(ehndl, CURLOPT_WRITEDATA, file);

    // Progress information
    curl_easy_setopt(ehndl, CURLOPT_NOPROGRESS, 0L);

    res = curl_easy_perform(ehndl);
    if (res != CURLE_OK) {
        fclose(file);
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
        curl_easy_cleanup(ehndl);
        return 1;
    }
    curl_easy_cleanup(ehndl);
    curl_global_cleanup();
    fclose(file);
    return 0;
}

// Takes the newly downloaded war file and puts it into the right location
// defined by the environment property KEEPER_JENKINS_WAR_PATH
int update_jenkins(const *char path)
{
    return 0;
}
