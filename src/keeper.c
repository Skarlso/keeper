#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include "keeper.h"

int get_latest_jenkins_version(char *version)
{
    // Parse https://updates.jenkins-ci.org/download/war/<version is ordered>/jenkins.war
    // to get the latest version of the war file.
}

int get_local_jenkins_version(char *version, const char *token, const char *host)
{
    // Jenkins version is in the response header called X-Jenkins:
    // X-Jenkins: 2.118
    // http://localhost:8080/api/json
    char v[] = {0};

    // No body needed.
    // Declarations
    CURL *ehndl;
    FILE *file;
    CURLcode res;

    // Initialize an easy curl which takes care most of the things we will need
    ehndl = curl_easy_init();
    if (!ehndl) {
        fprintf(stderr, "Could not initialize curl.\n");
        return 1;
    }
    // TODO: Extract this into a handler constructor function.
    curl_easy_setopt(ehndl, CURLOPT_URL, host);
    // Download URLs for Jenkins all redirect
    curl_easy_setopt(ehndl, CURLOPT_FOLLOWLOCATION, 1L);
    // But don't allow too many...
    curl_easy_setopt(ehndl, CURLOPT_MAXREDIRS, 50L);
    // Don't need the body
    curl_easy_setopt(ehndl, CURLOPT_NOBODY, 1L);

    // TODO: https://curl.haxx.se/libcurl/c/CURLOPT_HEADERFUNCTION.html

    res = curl_easy_perform(ehndl);
    if (res != CURLE_OK) {
        fclose(file);
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
        curl_easy_cleanup(ehndl);
        return 1;
    }
    curl_easy_cleanup(ehndl);

    if (v == NULL) {
        fprintf(stderr, "version is empty\n");
        return 1;
    }

    strncpy(version, v, sizeof(v));
    return 0;
}

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

    // Initialize an easy curl which takes care most of the things we will need
    ehndl = curl_easy_init();
    if (!ehndl) {
        fprintf(stderr, "Could not initialize curl.\n");
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
    fclose(file);
    return 0;
}

// Takes the newly downloaded war file and puts it into the right location
// defined by the environment property KEEPER_JENKINS_WAR_PATH
int update_jenkins(const char *path)
{
    printf("got path %s for jenkins war file.\n", path);
    return 0;
}

int safe_shutdown_jenkins(const char *host)
{
    return 0;
}
