#ifndef KEEPER_H
#define KEEPER_H

#define JENKINS_WAR_LATEST_URL "https://updates.jenkins-ci.org/latest/jenkins.war"
#define JENKINS_WAR_FILENAME "jenkins.war"
#define JENKINS_WAR_PATH_ENV_NAME "KEEPER_JENKINS_WAR_PATH"

#define STEP

#include <stdio.h>

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream);
int download_jenkins_war();
int update_jenkins(const char *path);

#endif // !KEEPER_H
