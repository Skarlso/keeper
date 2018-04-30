#include "keeper.h"
#include <stdlib.h>

int main()
{
    const char* path = getenv("KEEPER_JENKINS_WAR_PATH");
    if (path == NULL) {
        printf("Please set the location of the jenkins war file via %s\n", JENKINS_WAR_PATH_ENV_NAME);
        return 1;
    }

    // Download the latest file
    KEEPER_STEP("download latest jenkins war", *download_jenkins_war);

    // Update the war file
    KEEPER_STEP("updating the jenkins war file", *update_jenkins, path);

    // Initiate safe shutdown
    KEEPER_STEP("safe shutdown Jenkins", *safe_shutdown_jenkins);

    return 0;
}
