- [Getting started](#getting-started)
- [Macro directories](#macro-directories)
  - [Sidebar: Comparing to main](#sidebar-comparing-to-main)
- [Rules](#rules)
- [Autopilot](#autopilot)
  - [Adapt or create rules](#adapt-or-create-rules)
- [Add to the automated productions](#add-to-the-automated-productions)
- [Update the branch and tag its tip](#update-the-branch-and-tag-its-tip)
- [Appendix: Complete yaml files](#appendix-complete-yaml-files)
  - [Contents of `rules/run3oo_tracking_physics_pro001_pcdb001_v001.yaml`](#contents-of-rulesrun3oo_tracking_physics_pro001_pcdb001_v001yaml)
  - [Contents of `pilots/autopilot_run3oo_tracking_physics_pro001_pcdb001_v001.yaml`](#contents-of-pilotsautopilot_run3oo_tracking_physics_pro001_pcdb001_v001yaml)

## Getting started

Clone the ProdFlow repository in the right location and into an appropriately named folder:
```bash
cd Production2026
git clone git@github.com:sPHENIX-Collaboration/prodmacros.git run3oo_tracking_pro001_pcdb001_v001
cd run3oo_tracking_pro001_pcdb001_v001
```

Start by checking out a new branch, with a slightly different name to not confuse git.
Then make a copy of the appropriate directory, again slightly differently named.
```bash
git checkout -b branch_run3oo_tracking_pro001_pcdb001_v001
git branch --show-current
branch_run3oo_tracking_pro001_pcdb001_v001
cp -r run3oo dir_run3oo_tracking_pro001_pcdb001_v001
git add dir_run3oo_tracking_pro001_pcdb001_v001
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
cd dir_run3oo_tracking_pro001_pcdb001_v001
ls -1
calo_code
pilots
rules
streaming_code
tracking_code
triggered_code
```

Optional: We can delete unneeded directories, in this case calo and triggered code. It makes sense to delete the branch's copy of this markdown file as well to avoid accidental spaghettification; production specific comments should go into a dedicated README file.
```bash
git rm -rf calo_code triggered_code
touch README_run3oo_tracking_pro001_pcdb001_v001.md
```

## Macro directories
The `main` branch which we used as a starting point should always be up to date. If you need to make general changes to macros, please do so in the `main` branch first, then come back here and merge them. For production specific "one-off" changes, please adjust code in
```bash
streaming_code
tracking_code
```
These directories contain one or more Fun4All macros accompanied by a shell wrapper:
```bash
find streaming_code tracking_code
> find streaming_code tracking_code
streaming_code
streaming_code/Fun4All_SingleStream_Combiner.C
streaming_code/run_parallel_streams.sh
tracking_code
tracking_code/Fun4All_JobA.C
tracking_code/Fun4All_JobC.C
tracking_code/Fun4All_RolloverJob0.C
tracking_code/run_jobA.sh
tracking_code/run_jobC.sh
tracking_code/run_rolloverjob0.sh
```

### Sidebar: Comparing to main
To compare a current file, use `git diff branch-name -- filename`. For example,
```diff
git diff main:`run3oo/`streaming_code/Fun4All_SingleStream_Combiner.C ./streaming_code/Fun4All_SingleStream_Combiner.C
diff --git a/run3oo/streaming_code/Fun4All_SingleStream_Combiner.C b/dir_run3oo_tracking_pro001_pcdb001_v001/streaming_code/Fun4All_SingleStream_Combiner.C
index ec93f7e..1a0f63e 100644
--- a/run3oo/streaming_code/Fun4All_SingleStream_Combiner.C
+++ b/dir_run3oo_tracking_pro001_pcdb001_v001/streaming_code/Fun4All_SingleStream_Combiner.C
@@ -26,6 +26,7 @@ void Fun4All_SingleStream_Combiner(int nEvents = 0,
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
git mv run3oo_tracking_physics_PROD_TAG_VERSION.yaml run3oo_tracking_physics_pro001_pcdb001_v001.yaml
```
You can pretty much guess what changes are needed in this file from that `mv` command.

In this example, we need rules for event combining, i.e. `DST_STREAMING_EVENT`, cluster production, `DST_TRKR_CLUSTER`, seeding, `DST_TRKR_SEED`, and track fitting, `DST_TRKR_TRACKS`.
The individual names can be freely chosen; by convention, adorn the `dsttype` value with a prefix and postfix. We will use four rules ("top nodes"), `pro001_STREAMING_EVENT_run3oo`, `pro001_TRKR_CLUSTER_run3oo`, `pro001_TRKR_SEED_run3oo`, and `pro001_TRKR_TRACKS_run3oo`.

Critical fields to adapt are `build`, `dbtag`, and version, which then need to properly trickle down into the next step's `intriplet`:
```yaml
#__________________________________________________________________________________
pro001_STREAMING_EVENT_run3oo:
  params:
    dsttype:    DST_STREAMING_EVENT
    period:     run3oo
    physicsmode: physics
    dataset:    run3oo
    build:      pro001
    dbtag:      pcdb001
    version:    1

[...]
#__________________________________________________________________________________
pro001_TRKR_CLUSTER_run3oo:
  params:
    dsttype:     DST_TRKR_CLUSTER
    period:      run3oo
    physicsmode: physics
    dataset:     run3oo
    build:       pro001
    dbtag:       pcdb001
    version:     1
  
  input:
    intriplet:   pro001_pcdb001_v001   # This is the output triplet of the previous rule!

[...]
#__________________________________________________________________________________
pro001_TRKR_SEED_run3oo:
  params:
    dsttype:     DST_TRKR_SEED
    period:      run3oo
    physicsmode: physics
    dataset:     run3oo
    build:       pro001
    dbtag:       pcdb001
    version:     1

  input:
    intriplet:   pro001_pcdb001_v001   # This is the output triplet of the previous rule!

[...]
#__________________________________________________________________________________
pro001_TRKR_TRACKS_run3oo:
  params:
    dsttype:     DST_TRKR_TRACKS
    period:      run3oo
    physicsmode: physics
    dataset:     run3oo
    build:       pro001
    dbtag:       pcdb001
    version:     1

  input:
    intriplet:   pro001_pcdb001_v001   # This is the output triplet of the previous rule!

[...]
#__________________________________________________________________________________
```
Note that `build`, `tag`, and `version` of the two steps don't need to be the same, the connection is made via the `intriplet`. However, if they do differ, please name the base directories, branch, git tag, etc. wisely.


<!-- # filesystem: # redirect log while data02 is full
#   outdir:      "/sphenix/lustre01/sphnxpro/{prodmode}/{period}/{physicsmode}/{outtriplet}/{leafdir}/{rungroup}"
#   finaldir:    "/sphenix/lustre01/sphnxpro/{prodmode}/{period}/{physicsmode}/{outtriplet}/{leafdir}/{rungroup}"
#   logdir:     "/sphenix/data/data02/sphnxpro/{prodmode}/{period}/{physicsmode}/{outtriplet}/{leafdir}/{rungroup}/log"
#   histdir:  "/sphenix/data/data02/sphnxpro/{prodmode}/{period}/{physicsmode}/{outtriplet}/{leafdir}/{rungroup}/hist"
#   condor:       "/tmp/sphenixprod/sphnxpro/{prodmode}/{period}/{physicsmode}/{outtriplet}/{leafdir}/{rungroup}/log" -->


It is a good idea to look over the other fields as well. The full contents are in the [Appendix](#appendix-complete-yaml-files). Of particular interest are `dataset` and `physicsmode` for things like cosmics, and `request_memory`, which allows you to specify which RAM image sizes to try successively before condor gives up. Example:
```yaml
    request_memory:         2 GB, 3 GB, 5 GB
```

You can now submit jobs manually using
```bash
create_submission.py --rule pro001_STREAMING_EVENT_run3oo --config rules/run3oo_tracking_physics_pro001_pcdb001_v001.yaml --runs 82503 --andgo
```
You could also periodically run `dstspider.py` and `histspider.py` with the same arguments. However, especially for spiders, we want to put this job on autopilot.

## Autopilot
Start from the appropriate template.
```bash
cd pilots
git mv autopilot_run3oo_tracking_physics_PROD_TAG_VERSION.yaml autopilot_run3oo_tracking_physics_pro001_pcdb001_v001.yaml
```

### Adapt or create rules
The file needs a top node for any submission host you'd want to run this production on. It starts with paths:
```yaml
sphnxprod01:
  defaultlocations:
    submitdir:  /sphenix/data/data02/sphnxpro/production/run3oo/submission/{rule}
    prodbase:   /sphenix/u/sphnxpro/Production2026/sphenixprod
    configbase: /sphenix/u/sphnxpro/Production2026/run3oo_tracking_pro001_pcdb001_v001/dir_run3oo_tracking_pro001_pcdb001_v001/rules
```
Most important here is to change `configbase`. Note that the production submission installation at `prodbase` can also be individualized. `submitdir` is a location for helper caches, so make sure it's not in danger of being full.

Now add an entry for each of the rules we want to run, ex.:
```yaml
  # Event combining
  pro001_STREAMING_EVENT_run3oo:
    config: run3oo_tracking_physics_pro001_pcdb001_v001.yaml
    runlist: /sphenix/u/sphnxpro/Production2026/run3oo_tracking_pro001_pcdb001_v001/dir_run3oo_tracking_pro001_pcdb001_v001/runlist_run3oo_tracking_pro001
    submit: on
[...]

  # Clustering
  pro001_TRKR_CLUSTER_run3oo:
    config: run3oo_tracking_physics_pro001_pcdb001_v001.yaml
    runlist: /sphenix/u/sphnxpro/Production2026/run3oo_tracking_pro001_pcdb001_v001/dir_run3oo_tracking_pro001_pcdb001_v001/runlist_run3oo_tracking_pro001
    submit: on
[...]
  
  # Seeding
  pro001_TRKR_SEED_run3oo:
    config: run3oo_tracking_physics_pro001_pcdb001_v001.yaml
    runlist: /sphenix/u/sphnxpro/Production2026/run3oo_tracking_pro001_pcdb001_v001/dir_run3oo_tracking_pro001_pcdb001_v001/runlist_run3oo_tracking_pro001
    submit: on
[...]
  
  # Track Fitting
  pro001_TRKR_TRACKS_run3oo:
    config: run3oo_tracking_physics_pro001_pcdb001_v001.yaml
    runlist: /sphenix/u/sphnxpro/Production2026/run3oo_tracking_pro001_pcdb001_v001/dir_run3oo_tracking_pro001_pcdb001_v001/runlist_run3oo_tracking_pro001
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
/sphenix/u/sphnxpro/Production2026/run3oo_tracking_pro001_pcdb001_v001/dir_run3oo_tracking_pro001_pcdb001_v001/pilots/autopilot_run3oo_tracking_physics_pro001_pcdb001_v001.yaml
...
```


## Update the branch and tag its tip
To preserve what we're doing, now commit all changes and create a tag. 
Double check we're not changing main:
```bash
git branch --show-current
branch_run3oo_tracking_pro001_pcdb001_v001
```
Commit everything with a reasonable message
```bash
git add .
git commit -a -m "Setup for tracking production using prod.001 and pcdb001"
```
And create an annotated lightweight tag, again reusing the name we've given this production. Then push everything to github.
```bash
git tag -a tag_run3oo_tracking_pro001_pcdb001_v001 -m "Setup for tracking production using prod.001 and pcdb001"
git push --follow-tags
```

If you need to make corrections later, create and tag a new tag by appending `_fixN`. Ex.:
```
git tag -a tag_run3oo_tracking_pro001_pcdb001_v001_fix1 -m "Added TPC fix"
git push --follow-tags
```

## Problem Solving While Running
To extract subsystem and run number from held jobs (adjust the `condor_q` command with more constraints to isolate a HoldReason or JobBatchName), use
```bash
for line in `condor_q -long -held |grep UserLog | sed 's/UserLog = //g' `; do
   file=$(basename "$line" .condor)
   run=$(      echo "$file" | awk -F'[_-]' '{print $(NF-1)+0}')
   subsystem=$(echo "$file" | awk -F'[_-]' '{print $4}')
   echo $subsystem $run
done
```

<!-- ############################################################################### -->
<!-- ############################################################################### -->
<!-- ############################################################################### -->


## Appendix: Complete yaml files

### Contents of `rules/run3oo_trackinging_physics_pro001_pcdb001_v001.yaml`

```yaml
#______________________________________________________________________________________________________________________
pro001_STREAMING_EVENT_run3oo:
  params:
    dsttype:    DST_STREAMING_EVENT
    period:     run3oo
    physicsmode: physics
    dataset:    run3oo
    build:      pro001
    dbtag:      pcdb001
    version:    1

  input:
    db:          rawr
    table:       datasets
    min_run_time:   300
    min_run_events: 100000
    combine_seg0_only: false

  job:
    script:                 run_parallel_streams.sh
    log:                   '{condor}/{logbase}.condor'
    neventsper:             10000
    payload:                [../streaming_code/*]
    request_memory:         3072 MB, 5072 MB, 7072 MB
    request_xferslots:     '3'
    batch_name:            '{rule_name}_{dataset}_{outtriplet}'
    priority:              '90'
    filesystem:
      logdir:   "/sphenix/data/data02/sphnxpro/{prodmode}/{period}/{physicsmode}/{outtriplet}/{leafdir}/{rungroup}/log"
      histdir:  "/sphenix/data/data02/sphnxpro/{prodmode}/{period}/{physicsmode}/{outtriplet}/{leafdir}/{rungroup}/hist"
      condor:            "/tmp/data02/sphnxpro/{prodmode}/{period}/{physicsmode}/{outtriplet}/{leafdir}/{rungroup}/log"

#______________________________________________________________________________________________________________________
pro001_TRKR_CLUSTER_run3oo:
  params:
    dsttype:     DST_TRKR_CLUSTER
    period:      run3oo
    physicsmode: physics
    dataset:     run3oo
    build:       pro001
    dbtag:       pcdb001
    version:     1

  input:
    db:          fcr
    table:       datasets
    intriplet:   pro001_pcdb001_v001
    # cut_segment: 10
  job:
    script:                 run_rolloverjob0.sh
    log:                   '{condor}/{logbase}.condor'
    neventsper:             1000
    payload:                [../tracking_code/*]
    request_memory:         8192 MB, 12092 MB, 16092 MB
    request_cpus:          '1'
    batch_name:            '{rule_name}_{dataset}_{outtriplet}'
    priority:              '60'

#______________________________________________________________________________________________________________________
pro001_TRKR_SEED_run3oo:
  params:
    dsttype:     DST_TRKR_SEED
    period:      run3oo
    physicsmode: physics
    dataset:     run3oo
    build:       pro001
    dbtag:       pcdb001
    version:     1
  input:
    db:          fcr
    table:       datasets
    intriplet:   pro001_pcdb001_v001
    # cut_segment: 10

  job:
    script:                 run_jobA.sh
    log:                   '{condor}/{logbase}.condor'
    neventsper:             1000
    payload:                [../tracking_code/*]
    request_memory:         4096 MB, 6096 MB, 8096 MB
    request_cpus:          '2'
    batch_name:            '{rule_name}_{dataset}_{outtriplet}'
    priority:              '30'

#______________________________________________________________________________________________________________________
pro001_TRKR_TRACKS_run3oo:
  params:
    dsttype:     DST_TRKR_TRACKS
    period:      run3oo
    physicsmode: physics
    dataset:     run3oo
    build:       pro001
    dbtag:       pcdb001
    version:     1
  input:
    db:          fcr
    table:       datasets
    intriplet:   pro001_pcdb001_v001
    # cut_segment: 10

  job:
    script:                 run_jobC.sh
    log:                   '{condor}/{logbase}.condor'
    neventsper:             1000
    payload:                [../tracking_code/*]
    request_memory:         4096 MB, 6096 MB, 8096 MB
    request_cpus:          '2'
    batch_name:            '{rule_name}_{dataset}_{outtriplet}'
    priority:              '30'
```

### Contents of `pilots/autopilot_run3oo_tracking_physics_pro001_pcdb001_v001.yaml`

```yaml
################################# Prod01 #######################################
### Standard full production 
sphnxprod01:
  defaultlocations:
    submitdir:  /sphenix/data/data02/sphnxpro/production/run3oo/submission/{rule}
    prodbase:   /sphenix/u/sphnxpro/Production2026/sphenixprod
    configbase: /sphenix/u/sphnxpro/Production2026/BRANCH/dir_BRANCH

  # STREAMING physics
  pro001_STREAMING_EVENT_run3oo:
    config: run3oo_tracking_physics_pro001_pcdb001_v001.yaml
    runlist: /sphenix/u/sphnxpro/Production2026/run3oo_tracking_pro001_pcdb001_v001/dir_run3oo_tracking_pro001_pcdb001_v001/runlist
    #runs: [82372 82703]
    jobprio: 90
    submit: on
    dstspider: on
    finishmon: on

  # TRKR_CLUSTER physics
  pro001_TRKR_CLUSTER_run3oo:
    config: run3oo_tracking_physics_pro001_pcdb001_v001.yaml
    runlist: /sphenix/u/sphnxpro/Production2026/run3oo_tracking_pro001_pcdb001_v001/dir_run3oo_tracking_pro001_pcdb001_v001/runlist
    #runs: [82372 82703]
    jobprio: 60
    submit: on
    dstspider: on
    finishmon: on
  
  # TRKR_SEED physics
  pro001_TRKR_SEED_run3oo:
    config: run3oo_tracking_physics_pro001_pcdb001_v001.yaml
    runlist: /sphenix/u/sphnxpro/Production2026/run3oo_tracking_pro001_pcdb001_v001/dir_run3oo_tracking_pro001_pcdb001_v001/runlist
    #runs: [82372 82703]
    jobprio: 30
    submit: on
    dstspider: on
    finishmon: on

  # TRKR_TRACKS physics
  pro001_TRKR_TRACKS_run3oo:
    config: run3oo_tracking_physics_pro001_pcdb001_v001.yaml
    runlist: /sphenix/u/sphnxpro/Production2026/run3oo_tracking_pro001_pcdb001_v001/dir_run3oo_tracking_pro001_pcdb001_v001/runlist
    #runs: [82372 82703]
    jobprio: 10
    submit: on
    dstspider: on
    finishmon: on

###############################################################################
```

