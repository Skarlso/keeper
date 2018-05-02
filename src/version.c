#include "version.h"
#include <stdlib.h>
#include <string.h>

version_t parse_version(const char *version_str) {
    version_t res;

    char *v_str = strndup(version_str, strlen(version_str));
    char *token = strtok(v_str, ".");
    int nums[] = {0, 0, 0};
    int index = 0;
    while (token != NULL)
    {
        int i = atoi (token);
        nums[index++] = i;
        token = strtok(NULL, ".");
    }
    res.major = nums[0];
    res.minor = nums[1];
    res.build = nums[2];
    return res;
}

int compare_versions(const char *v1, const char *v2)
{
    version_t _v1 = parse_version(v1);
    version_t _v2 = parse_version(v2);
    if (_v1.major > _v2.major) {
        return 1;
    } else if (_v1.major < _v2.major) {
        return -1;
    }
    if (_v1.minor > _v2.minor) {
        return 1;
    } else if (_v1.minor < _v2.minor) {
        return -1;
    }
    if (_v1.build > _v2.build) {
        return 1;
    } else if (_v1.build < _v2.build) {
        return -1;
    }
    return 0;
}
