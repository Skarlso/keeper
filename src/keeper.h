#ifndef KEEPER_H
#define KEEPER_H

#define JENKINS_WAR_LATEST_URL "https://updates.jenkins-ci.org/latest/jenkins.war"
#define JENKINS_WAR_FILENAME "jenkins.war"
#define JENKINS_WAR_PATH_ENV_NAME "KEEPER_JENKINS_WAR_PATH"
#define JENKINS_HOST_ENV_NAME "KEEPER_JENKINS_HOST"
#define JENKINS_TOKEN_ENV_NAME "KEEPER_JENKINS_TOKEN"

#define KEEPER_STEP(name, f, params...)                            \
    printf("running step '%s'\n", name);                             \
    if ((f)(params) != 0) {                                        \
        fprintf(stderr, "error while running step '%s'.\n", name); \
        exit(1);                                                   \
    }

#include <stdio.h>

int get_jenkins_version(char *version, const char *token);
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream);
int download_jenkins_war();
int update_jenkins(const char *path);
int safe_shutdown_jenkins();

#endif // !KEEPER_H
