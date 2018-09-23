# Keeper

Updated Jenkins if there is a new version available

For security, keeper has to run on the same host as Jenkins does to call localhost.

# Building

```bash
make
```

# Usage

Keeper is intended as a script which runs as a cron job for example. It will safe shutdown Jenkins, which means as long as there
is a job running it won't kill Jenkins off. It will check the current Jenkins' version against the latest available one. If the
latest is newer, it will download that and copy the old WAR over.

After that, it will re-start Jenkins based on an environment property called: `KEEPER_JENKINS_START_CMD`.

# Environment Properties

Keeper employs a couple properties. These are:

```bash
KEEPER_JENKINS_WAR_PATH # The path of the Jenkins war file to overwrite.
KEEPER_JENKINS_TOKEN # Which is used to gather the Jenkins version // This might be obsolete in the next version as it's
essentially not required.
KEEPER_JENKINS_START_CMD # This is so that Keeper knows how to restart Jenkins once it has been updated.
```

# Tests

In the works.

