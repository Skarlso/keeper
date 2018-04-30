#include "keeper.h"
#include <stdlib.h>
#include <curl/curl.h>


int main()
{
    // Initialize curl capabilities
    curl_global_init(CURL_GLOBAL_ALL);

    const char* path = getenv(JENKINS_WAR_PATH_ENV_NAME);
    if (path == NULL) {
        printf("Please set the location of the jenkins war file via %s\n", JENKINS_WAR_PATH_ENV_NAME);
        return 1;
    }
    const char* host = getenv(JENKINS_HOST_ENV_NAME);
    if (host == NULL) {
        printf("Please set the host via %s\n", JENKINS_HOST_ENV_NAME);
        return 1;
    }
    const char* token = getenv(JENKINS_TOKEN_ENV_NAME);
    if (token == NULL) {
        printf("Please set the host via %s\n", JENKINS_TOKEN_ENV_NAME);
        return 1;
    }

    char version[20];
    // In order to modify this variable we pass in the address of the first element of this array
    KEEPER_STEP("get Jenkins version", *get_jenkins_version, &version[0], token);
    printf("got version: %s\n", version);


    // Download the latest file
    // KEEPER_STEP("download latest jenkins war", *download_jenkins_war);

    // Update the war file
    KEEPER_STEP("updating the jenkins war file", *update_jenkins, path);

    // Initiate safe shutdown
    KEEPER_STEP("safe shutdown Jenkins", *safe_shutdown_jenkins, host);

    curl_global_cleanup();
    return 0;
}
