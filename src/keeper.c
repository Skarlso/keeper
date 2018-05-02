#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <stdbool.h>
#include "keeper.h"

int get_latest_jenkins_version(char *version)
{
    // Parse https://updates.jenkins-ci.org/download/war/<version is ordered>/jenkins.war
    // to get the latest version of the war file.


    return 0;
}

struct jenkins_headers {
  char value[20];
};

size_t header_callback(char *buffer, size_t size,
                              size_t nitems, void *userdata)
{
    /* received header is nitems * size long in 'buffer' NOT ZERO TERMINATED */
    /* 'userdata' is set with CURLOPT_HEADERDATA */
    size_t numbytes = size * nitems;
    struct jenkins_headers *jh = (struct jenkins_headers *)userdata;
    if (strstr(buffer, "X-Jenkins:") != NULL) {
        sscanf(buffer, "X-Jenkins: %s", jh->value);
    }
    return numbytes;
}

int get_local_jenkins_version(char *version, const char *token)
{
    // Declarations
    CURL *ehndl;
    CURLcode res;

    // Initialize an easy curl which takes care most of the things we will need
    ehndl = curl_easy_init();
    if (!ehndl) {
        fprintf(stderr, "Could not initialize curl.\n");
        return 1;
    }
    // TODO: Extract this into a handler constructor function.
    curl_easy_setopt(ehndl, CURLOPT_URL, KEEPER_JENKINS_HOST);
    // Don't need the body
    curl_easy_setopt(ehndl, CURLOPT_NOBODY, 1L);

    struct jenkins_headers jh = {""};
    curl_easy_setopt(ehndl, CURLOPT_HEADERFUNCTION, header_callback);
    curl_easy_setopt(ehndl, CURLOPT_HEADERDATA, &jh);

    res = curl_easy_perform(ehndl);
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
        curl_easy_cleanup(ehndl);
        return 1;
    }
    curl_easy_cleanup(ehndl);

    if (strlen(jh.value) == 0) {
        fprintf(stderr, "version is empty\n");
        return 1;
    }

    strncpy(version, jh.value, sizeof(jh.value));
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
int update_jenkins(const char *path, bool update_needed)
{
    printf("got path %s for jenkins war file.\n", path);
    if (!update_needed)
        return 0;

    return 0;
}

int safe_shutdown_jenkins(bool update_needed)
{
    if (!update_needed)
        return 0;

    return 0;
}
