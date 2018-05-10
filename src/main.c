#include "keeper.h"
#include "version.h"
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>
#include <stdbool.h>

int main()
{
    // Initialize curl capabilities
    curl_global_init(CURL_GLOBAL_ALL);

    const char* path = getenv(JENKINS_WAR_PATH_ENV_NAME);
    if (path == NULL) {
        printf("Please set the location of the jenkins war file via %s\n", JENKINS_WAR_PATH_ENV_NAME);
        return 1;
    }
    const char* token = getenv(JENKINS_TOKEN_ENV_NAME);
    if (token == NULL) {
        printf("Please set the host via %s\n", JENKINS_TOKEN_ENV_NAME);
        return 1;
    }

    const char* restart_cmd = getenv(JENKINS_RESTART_COMMAND_ENV_NAME);
    if (restart_cmd == NULL) {
        printf("Please set the start command via %s\n", JENKINS_RESTART_COMMAND_ENV_NAME);
        return 1;
    }

    char latest_jenkins_version[20];
    // In order to modify this variable we pass in the address of the first element of this array
    KEEPER_STEP("get latest jenkins version", *get_latest_jenkins_version, &latest_jenkins_version[0]);
    printf("got latest jenkins version: %s\n", latest_jenkins_version);

    char local_jenkins_version[20];
    // // In order to modify this variable we pass in the address of the first element of this array
    KEEPER_STEP("get local jenkins version", *get_local_jenkins_version, &local_jenkins_version[0], token);
    printf("got local version: %s\n", local_jenkins_version);

    // Download the latest file
    KEEPER_STEP("download latest jenkins war", *download_jenkins_war);

    int res = compare_versions(local_jenkins_version, latest_jenkins_version);
    if (res != -1) {
        printf("no update required\n");
        return 0;
    }

    // // Initiate safe shutdown
    KEEPER_STEP("safe shutdown Jenkins", *safe_shutdown_jenkins, token);

    // // Update the war file
    KEEPER_STEP("updating the jenkins war file", *update_jenkins, path);

    // Starting Jenkins after it has been updated
    KEEPER_STEP("restarting jenkins", *start_jenkins, restart_cmd);

    printf("Jenkins restarted. Happy building.\n");
    curl_global_cleanup();
    return 0;
}
