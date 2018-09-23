#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <stdbool.h>
#include "keeper.h"
#include <fcntl.h>
#include <unistd.h>

struct jenkins_version {
  char value[20];
};

size_t version_write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    size_t numbytes = size * nmemb;
    struct jenkins_version *jv = (struct jenkins_version *)userdata;
    char *loc = strstr(ptr, "/download/war/");
    if (loc != NULL && strlen(jv->value) == 0) {
        loc += strlen("/download/war/");
        char *end = strstr(loc, "/jenkins");
        if ((end - loc) < 20) {
            memcpy(jv->value, loc, end - loc);
        }
    }
    return numbytes;
}

int get_latest_jenkins_version(char *version)
{
    CURL *ehndl;
    CURLcode res;

    // Initialize an easy curl which takes care most of the things we will need
    ehndl = curl_easy_init();
    if (!ehndl) {
        fprintf(stderr, "Could not initialize curl.\n");
        return 1;
    }
    printf("getting latest version number: %s\n", JENKINS_WAR_VERSION_URL);
    curl_easy_setopt(ehndl, CURLOPT_URL, JENKINS_WAR_VERSION_URL);
    // Download URLs for Jenkins all redirect
    curl_easy_setopt(ehndl, CURLOPT_FOLLOWLOCATION, 1L);
    // But don't allow too many...
    curl_easy_setopt(ehndl, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(ehndl, CURLOPT_BUFFERSIZE, 1024L);

    struct jenkins_version jv = {""};
    // Setup the file to which we will write to
    curl_easy_setopt(ehndl, CURLOPT_WRITEFUNCTION, version_write_callback);
    curl_easy_setopt(ehndl, CURLOPT_WRITEDATA, &jv);

    // Progress information
    curl_easy_setopt(ehndl, CURLOPT_NOPROGRESS, 1L);

    res = curl_easy_perform(ehndl);
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
        curl_easy_cleanup(ehndl);
        return 1;
    }
    curl_easy_cleanup(ehndl);
    strncpy(version, jv.value, sizeof(jv.value));
    return 0;
}

size_t header_callback(char *buffer, size_t size,
                              size_t nitems, void *userdata)
{
    /* received header is nitems * size long in 'buffer' NOT ZERO TERMINATED */
    /* 'userdata' is set with CURLOPT_HEADERDATA */
    size_t numbytes = size * nitems;
    struct jenkins_version *jv = (struct jenkins_version *)userdata;
    if (strstr(buffer, "X-Jenkins:") != NULL) {
        sscanf(buffer, "X-Jenkins: %s", jv->value);
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

    struct jenkins_version jv = {""};
    curl_easy_setopt(ehndl, CURLOPT_HEADERFUNCTION, header_callback);
    curl_easy_setopt(ehndl, CURLOPT_HEADERDATA, &jv);

    res = curl_easy_perform(ehndl);
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
        curl_easy_cleanup(ehndl);
        return 1;
    }
    curl_easy_cleanup(ehndl);

    if (strlen(jv.value) == 0) {
        fprintf(stderr, "version is empty\n");
        return 1;
    }

    strncpy(version, jv.value, sizeof(jv.value));
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

    int src_fd, dst_fd, n, err;
    int buffer_size = 4096;
    unsigned char buffer[buffer_size];

    src_fd = open("jenkins.war", O_RDONLY);
    dst_fd = open(path, O_CREAT | O_WRONLY, 0666);

    while (1) {
        err = read(src_fd, buffer, buffer_size);
        if (err == -1) {
            printf("error reading source file.\n");
            return 1;
        }
        n = err;

        if (n == 0) break;

        err = write(dst_fd, buffer, n);
        if (err == -1) {
            printf("error writing to destination file under '%s'.\n", path);
            return 1;
        }
    }

    printf("successfully copied jenkins.war\n");
    close(src_fd);
    close(dst_fd);

    return 0;
}

int safe_shutdown_jenkins(const char *token)
{
    CURL *ehndl;
    CURLcode res;

    // Initialize an easy curl which takes care most of the things we will need
    ehndl = curl_easy_init();
    if (!ehndl) {
        fprintf(stderr, "Could not initialize curl.\n");
        return 1;
    }
    curl_easy_setopt(ehndl, CURLOPT_URL, KEEPER_JENKINS_EXIT_URL);
    curl_easy_setopt(ehndl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(ehndl, CURLOPT_USERPWD, token);
    curl_easy_setopt(ehndl, CURLOPT_POST, 1L);
    res = curl_easy_perform(ehndl);
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
        curl_easy_cleanup(ehndl);
        return 1;
    }

    printf("continuously checking until Jenkins is unavailable.\n");
    curl_easy_setopt(ehndl, CURLOPT_URL, KEEPER_JENKINS_HOST);
    curl_easy_setopt(ehndl, CURLOPT_POST, 0L);

    while ((res = curl_easy_perform(ehndl)) == CURLE_OK) {
        sleep(2);
    }
    printf("jenkins successfully shutdown.\n");
    curl_easy_cleanup(ehndl);
    return 0;
}

int start_jenkins(const char *cmd)
{
    if (fork() == 0) {
        system(cmd);
        return 0;
    }
    return 0;
}
