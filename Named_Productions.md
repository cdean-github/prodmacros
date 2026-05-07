- [Getting started](#getting-started)
- [Macro directories](#macro-directories)
  - [Sidebar: Comparing to main](#sidebar-comparing-to-main)
- [Rules](#rules)
- [Autopilot](#autopilot)
  - [Adapt or create rules](#adapt-or-create-rules)
- [Add to the automated productions](#add-to-the-automated-productions)
- [Update the branch and tag its tip](#update-the-branch-and-tag-its-tip)
- [Problem Solving While Running](#problem-solving-while-running)
- [Appendix: Complete yaml files](#appendix-complete-yaml-files)
- [Appendix: Job Exit Codes](#appendix-job-exit-codes)
  - [Contents of `rules/run3oo_calo_physics_pro001_pcdb001_v001.yaml`](#contents-of-rulesrun3oo_calo_physics_pro001_pcdb001_v001yaml)
  - [Contents of `pilots/autopilot_run3oo_calo_physics_pro001_pcdb001_v001.yaml`](#contents-of-pilotsautopilot_run3oo_calo_physics_pro001_pcdb001_v001yaml)

## Getting started

First, define your production type and tag parameters in your terminal. 
**(Adjust "calo", "tracking", or "streaming" and the tags as needed):**

```bash
export PROD_TYPE="calo" # or "tracking"
export PROD_BUILD="pro001"
export PROD_DBTAG="pcdb001"
export PROD_VERSION="v001"

# The following variables are derived automatically:
export PROD_TAG="run3oo_${PROD_TYPE}_${PROD_BUILD}_${PROD_DBTAG}_${PROD_VERSION}"
export DIR_NAME="dir_${PROD_TAG}"
export BRANCH_NAME="branch_${PROD_TAG}"
```

Now, clone the repository and set up your branches. **You can copy/paste this exactly as is:**

```bash
cd Production2026
git clone git@github.com:sPHENIX-Collaboration/prodmacros.git $PROD_TAG
cd $PROD_TAG
```

Start by checking out a new branch, with a slightly different name to not confuse git.
Then make a copy of the appropriate directory, again slightly differently named.
```bash
git checkout -b branch_run3oo_calo_pro001_pcdb001_v001
git branch --show-current
branch_run3oo_calo_pro001_pcdb001_v001
cp -r run3oo dir_run3oo_calo_pro001_pcdb001_v001
git add dir_run3oo_calo_pro001_pcdb001_v001
```
Optional: The autopilot control and the general instructions shouldn't be edited here. No other directories are needed at this point either, so we can delete all of it.
```bash
git rm -r Named_Productions.md active_productions.txt run* 
```

A production needs:
* One or more directories with root macros and steering scripts
* A `yaml` file with production rules
* An autopilot `yaml` file that instantiates these rules, to be used by cron jobs. 
These three live in respective subdirectories.

After renaming the template contents some more, we have:
```bash
cd dir_run3oo_calo_pro001_pcdb001_v001
ls -1
calo_code
pilots
rules
streaming_code
tracking_code
triggered_code
```

Optional: We can delete unneeded directories, in this case tracking and streaming code. It makes sense to delete the branch's copy of this markdown file as well to avoid accidental spaghettification; production specific comments should go into a dedicated README file.
```bash
git rm -rf tracking_code streaming_code
touch README_run3oo_calo_pro001_pcdb001_v001.md
```

## Macro directories
The `main` branch which we used as a starting point should always be up to date. If you need to make general changes to macros, please do so in the `main` branch first, then come back here and merge them. For production specific "one-off" changes, please adjust code in
```bash
triggered_code
calo_code
```
These directories contain one or more Fun4All macros accompanied by a shell wrapper:
```bash
find triggered_code calo_code
> find triggered_code calo_code
triggered_code
triggered_code/Fun4All_Prdf_Combiner.C
triggered_code/run_eventcombine.sh
calo_code
calo_code/Fun4All_Year2_Fitting.C
calo_code/run_fitting.sh
```

### Sidebar: Comparing to main
To compare a current file, use `git diff branch-name -- filename`. For example,
```diff
git diff main:`run3oo/`triggered_code/Fun4All_Prdf_Combiner.C ./triggered_code/Fun4All_Prdf_Combiner.C
diff --git a/run3oo/triggered_code/Fun4All_Prdf_Combiner.C b/dir_run3oo_calo_pro001_pcdb001_v001/triggered_code/Fun4All_Prdf_Combiner.C
index ec93f7e..1a0f63e 100644
--- a/run3oo/triggered_code/Fun4All_Prdf_Combiner.C
+++ b/dir_run3oo_calo_pro001_pcdb001_v001/triggered_code/Fun4All_Prdf_Combiner.C
@@ -26,6 +26,7 @@ void Fun4All_Prdf_Combiner(int nEvents = 0,
                            const std::string &outbase = "delme",
                            const std::string &outdir = "/sphenix/data/data02/sphnxpro/scratch/kolja/test")
 {
+  std::cout << "hello world" << std::endl;
   Fun4AllServer *se = Fun4AllServer::instance();
   se->Verbosity(1);
   se->VerbosityDownscale(100000);
```

To compare committed changes, you need to use the following syntax instead:
```bash
git diff branch1:path/to/file1 branch2:path/to/file2
````


## Rules
Start from the appropriate template:
```bash
git mv run3oo_calo_physics_PROD_TAG_VERSION.yaml run3oo_calo_physics_pro001_pcdb001_v001.yaml
```
You can pretty much guess what changes are needed in this file from that `mv` command.

In this example, we need rules for event combining, i.e. `DST_TRIGGERED_EVENT`, and wave form fitting, `DST_CALOFITTING`.
The individual names can be freely chosen; by convention, adorn the `dsttype` value with a prefix and postfix. We will use two rules ("top nodes"), `pro001_TRIGGERED_EVENT_run3oo` and `pro001_CALOFITTING_run3oo`.

Critical fields to adapt are `build`, `dbtag`, and version, which then need to properly trickle down into the next step's `intriplet`:
```yaml
#__________________________________________________________________________________
pro001_TRIGGERED_EVENT_run3oo:
  params:
    dsttype:      DST_TRIGGERED_EVENT
    build:        pro001
    dbtag:        pcdb001
    version:      1
    period:       run3oo
    dataset:      run3oo
    physicsmode:  physics


[...]
#__________________________________________________________________________________
pro001_CALOFITTING_run3oo:
  params:
    dsttype:      DST_CALOFITTING
    build:        pro001
    dbtag:        pcdb001
    version:      1
    period:       run3oo
    dataset:      run3oo
    physicsmode:  physics

  input:
    intriplet:   pro001_pcdb001_v001   # This is the output triplet of the previous rule!

[...]
#__________________________________________________________________________________
```
Note that `build`, `tag`, and `version` of the two steps don't need to be the same, the connection is made via the `intriplet`. However, if they do differ, please name the base directories, branch, git tag, etc. wisely.


<!-- # filesystem: # redirect log while data03 is full
#   outdir:      "/sphenix/lustre01/sphnxpro/{prodmode}/{period}/{physicsmode}/{outtriplet}/{leafdir}/{rungroup}"
#   finaldir:    "/sphenix/lustre01/sphnxpro/{prodmode}/{period}/{physicsmode}/{outtriplet}/{leafdir}/{rungroup}"
#   logdir:     "/sphenix/sim/sim02/sphnxpro/{prodmode}/{period}/{physicsmode}/{outtriplet}/{leafdir}/{rungroup}/log"
#   histdir:  "/sphenix/data/data02/sphnxpro/{prodmode}/{period}/{physicsmode}/{outtriplet}/{leafdir}/{rungroup}/hist"
#   condor:       "/tmp/sphenixprod/sphnxpro/{prodmode}/{period}/{physicsmode}/{outtriplet}/{leafdir}/{rungroup}/log" -->


It is a good idea to look over the other fields as well. The full contents are in the [Appendix](#appendix-complete-yaml-files). Of particular interest are `dataset` and `physicsmode` for things like cosmics, and `request_memory`, which allows you to specify which RAM image sizes to try successively before condor gives up. Example:
```yaml
    request_memory:         2 GB, 3 GB, 5 GB
```

You can now submit jobs manually using
```bash
create_submission.py --rule pro001_TRIGGERED_EVENT_run3oo --config rules/run3oo_calo_physics_pro001_pcdb001_v001.yaml --runs 82503 --andgo
```
You could also periodically run `dstspider.py` and `histspider.py` with the same arguments. However, especially for spiders, we want to put this job on autopilot.

## Autopilot
Start from the appropriate template.
```bash
cd pilots
git mv autopilot_run3oo_calo_physics_PROD_TAG_VERSION.yaml autopilot_run3oo_calo_physics_pro001_pcdb001_v001.yaml
```

### Adapt or create rules
The file needs a top node for any submission host you'd want to run this production on. It starts with paths:
```yaml
sphnxprod01:
  defaultlocations:
    prodbase:   /sphenix/u/sphnxpro/Production2026/sphenixprod
    configbase: /sphenix/u/sphnxpro/Production2026/run3oo_calo_pro001_pcdb001_v001/dir_run3oo_calo_pro001_pcdb001_v001/rules
    submitdir:  /sphenix/data/data03/sphnxpro/production/run3oo/submission/{rule}
```
Most important here is to change `configbase`. Note that the production submission installation at `prodbase` can also be individualized. `submitdir` is a location for helper caches, so make sure it's not in danger of being full.

Now add an entry for each of the rules we want to run, ex.:
```yaml
  # Event combining
  pro001_TRIGGERED_EVENT_run3oo:
    config: run3oo_calo_physics_pro001_pcdb001_v001.yaml
    runlist: /sphenix/u/sphnxpro/Production2026/run3oo_calo_pro001_pcdb001_v001/dir_run3oo_calo_pro001_pcdb001_v001/runlist_run3oo_calo_pro001
    submit: on
[...]

  # Waveform fitting
  pro001_CALOFITTING_run3oo:
    config: run3oo_calo_physics_pro001_pcdb001_v001.yaml
    runlist: /sphenix/u/sphnxpro/Production2026/run3oo_calo_pro001_pcdb001_v001/dir_run3oo_calo_pro001_pcdb001_v001/runlist_run3oo_calo_pro001
    submit: on
[...]
```

Instead of `runlist`, you can also specify a range with e.g. `runs: [79000 80000]`. The full file in the [Appendix](#appendix-complete-yaml-files) shows additional parameters to control the spider(s), monitoring, priority, etc. Also shown is how to run submission and/or spidering of the same job type from multiple submit hosts.

## Add to the automated productions
The autopilot is run with `production_control.py --steer /path/to/autopilot.yaml`. Instead of adding one such line to the `crontab` of all relevant submission hosts, a master script checks on a text file for all productions that should be run. You can double check that it is active on a given node, and which conterol file it is reading, with
```bash
crontab -l
05,35 * * * * /sphenix/u/sphnxpro/Production2026/sphenixprod/master_production_control.sh /sphenix/u/sphnxpro/Production2026/active_productions.txt 180 >& /dev/null 
```
The `180` is the time interval in seconds between invocations of each line in the list.
(To generate more complex `cron` time expressions, see [crontab.guru](https://crontab.guru/)).

To add this production to the list, edit `/sphenix/u/sphnxpro/Production2026/active_productions.txt` which is symlinked to the `prodmacros` repo in the same directory. Important: We should have deleted the local copy of this file early on, but either way, make sure you edit the correct global one. It should look something like:
```bash
cat /sphenix/u/sphnxpro/Production2026/active_productions.txt
# This file lists the active production steering files for the master cron job.
# One full path per line. Lines starting with # are ignored.
/sphenix/u/sphnxpro/Production2026/run3oo_calo_pro001_pcdb001_v001/dir_run3oo_calo_pro001_pcdb001_v001/pilots/autopilot_run3oo_calo_physics_pro001_pcdb001_v001.yaml
...
```


## Update the branch and tag its tip
To preserve what we're doing, now commit all changes and create a tag. 
Double check we're not changing main:
```bash
git branch --show-current
branch_run3oo_calo_pro001_pcdb001_v001
```
Commit everything with a reasonable message
```bash
git add .
git commit -a -m "Setup for calo production using prod.001 and pcdb001"
```
And create an annotated lightweight tag, again reusing the name we've given this production. Then push everything to github.
```bash
git tag -a tag_run3oo_calo_pro001_pcdb001_v001 -m "Setup for calo production using prod.001 and pcdb001"
git push --follow-tags
```

If you need to make corrections later, create and tag a new tag by appending `_fixN`. Ex.:
```
git tag -a tag_run3oo_calo_pro001_pcdb001_v001_fix1 -m "Added ZDC fix"
git push --follow-tags
```

<!-- ############################################################################### -->
<!-- ############################################################################### -->
<!-- ############################################################################### -->


## Appendix: Complete yaml files

### Contents of `rules/run3oo_calo_physics_pro001_pcdb001_v001.yaml`

```yaml
#______________________________________________________________________________
pro001_TRIGGERED_EVENT_run3oo:
  params:
    dsttype:      DST_TRIGGERED_EVENT
    build:        pro.001
    dbtag:        pcdb001
    version:      1
    period:       run3oo
    dataset:      run3oo
    physicsmode:  physics

  input:
    db:             rawr
    table:          datasets
    min_run_time:   300
    min_run_events: 100000

  job:
    script:                 run_eventcombine.sh
    log:                   '{condor}/{logbase}.condor'
    neventsper:             10000
    payload:                [../triggered_code/*]
    request_memory:         2 GB, 3 GB, 5 GB
    request_xferslots:     '3'    
    batch_name:            '{rule_name}_{dataset}_{outtriplet}'
    priority:              '90'

#______________________________________________________________________________
pro001_CALOFITTING_run3oo:
  params:
    dsttype:      DST_CALOFITTING
    build:        pro.001
    dbtag:        pcdb001
    version:      1
    period:       run3oo
    dataset:      run3oo
    physicsmode:  physics

  input:
    db:           fcr
    table:        datasets
    intriplet:    pro001_pcdb001_v001  # This is the output triplet of the previous rule

  job:
    script:                 run_fitting.sh
    log:                   '{condor}/{logbase}.condor'
    neventsper:             100000
    payload:                [../calo_code/*]
    request_memory:         2500MB, 4GB, 6GB
    request_cpus:          '1'
    batch_name:            '{rule_name}_{dataset}_{outtriplet}'
    priority:              '60'

###############################################################################

```

### Contents of `pilots/autopilot_run3oo_calo_physics_pro001_pcdb001_v001.yaml`

```yaml
################################# Prod03 #######################################
### Standard full production 
sphnxprod02:
  defaultlocations:
    prodbase:   /sphenix/u/sphnxpro/Production2026/sphenixprod
    configbase: /sphenix/u/sphnxpro/Production2026/run3oo_calo_pro001_pcdb001_v001/dir_run3oo_calo_pro001_pcdb001_v001/rules
    submitdir:  /sphenix/data/data02/sphnxpro/production/run3oo/submission/{rule}

  # Event combining
  pro001_TRIGGERED_EVENT_run3oo:
    config: run3oo_calo_physics_pro001_pcdb001_v001.yaml
    runlist: /sphenix/u/sphnxpro/Production2026/run3oo_calo_pro001_pcdb001_v001/dir_run3oo_calo_pro001_pcdb001_v001/runlist_run3oo_calo_pro001
    # runs: [82374 82703]
    #jobprio: 90
    jobprio: 110
    submit: on
    dstspider: on
    finishmon: on

  # Waveform fitting
  pro001_CALOFITTING_run3oo:
    config: run3oo_calo_physics_pro001_pcdb001_v001.yaml
    runlist: /sphenix/u/sphnxpro/Production2026/run3oo_calo_pro001_pcdb001_v001/dir_run3oo_calo_pro001_pcdb001_v001/runlist_run3oo_calo_pro001
    jobprio: 110
    submit: on
    dstspider: on
    finishmon: on

###############################################################################
```

## Appendix: Job Exit Codes

All job scripts report a final exit code via `common_runscript_finish.sh`, which records it in the production database. Codes are designed to identify the failure stage at a glance:

| Code | Stage | Meaning |
|------|-------|---------|
| 0 | — | Success |
| 2 | Setup | Bad arguments or configuration error |
| 3 | Setup | Unsupported OS / environment setup failed |
| 10 | Input | No input files found (DB query returned empty) |
| 11 | Input | Remote file health check failed (missing or wrong size) |
| 20 | Stage-in | Input file copy failed (dd retries exhausted or source missing) |
| 21 | Stage-in | Input file md5 mismatch after copy |
| 30 | Stage-out | Output file not found (macro produced no output) |
| 31 | Stage-out | Output file copy failed (dd retries exhausted) |
| 111 | Input | Streaming: wrong number of GL1 or detector list files |
| other | Macro | Propagated directly from `root.exe` exit code |
