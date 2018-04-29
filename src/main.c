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
    // STEP(download_jenkins_war);
    int ret = download_jenkins_war();
    if (ret > 0)
        return ret;

    // Update the war file
    ret = update_jenkins(path);
    if (ret > 0)
        return ret;

    // Initiate safe shutdown

    // Save the current version?

    return 0;
}
