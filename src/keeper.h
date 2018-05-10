#ifndef KEEPER_H
#define KEEPER_H

#define JENKINS_WAR_LATEST_URL "https://updates.jenkins-ci.org/latest/jenkins.war"
#define JENKINS_WAR_VERSION_URL "https://updates.jenkins-ci.org/download/war/"
#define JENKINS_WAR_FILENAME "jenkins.war"
#define JENKINS_WAR_PATH_ENV_NAME "KEEPER_JENKINS_WAR_PATH"
#define KEEPER_JENKINS_HOST "http://localhost:8080/"
#define KEEPER_JENKINS_EXIT_URL "http://localhost:8080/exit"
#define JENKINS_TOKEN_ENV_NAME "KEEPER_JENKINS_TOKEN"
#define JENKINS_RESTART_COMMAND_ENV_NAME "KEEPER_JENKINS_START_CMD"

#define KEEPER_STEP(name, f, params...)                            \
    printf("running step '%s'\n", name);                             \
    if ((f)(params) != 0) {                                        \
        fprintf(stderr, "error while running step '%s'.\n", name); \
        exit(1);                                                   \
    }

#include <stdio.h>
#include <stdbool.h>

int get_local_jenkins_version(char *version, const char *token);
int get_latest_jenkins_version(char *version);
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t header_callback(char *buffer, size_t size, size_t nitems, void *userdata);
size_t version_write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);
int download_jenkins_war();
int update_jenkins(const char *path);
int safe_shutdown_jenkins(const char *token);
int compare_versions(const char *v1, const char *v2);
int start_jenkins(const char *cmd);

#endif // !KEEPER_H
