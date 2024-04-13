void usage(enum logcode F)
{
  print_rsync_version(F);

  rprintf(F,"\n");
  rprintf(F,"rsync is a file transfer program capable of efficient remote update\n");
  rprintf(F,"via a fast differencing algorithm.\n");

  rprintf(F,"\n");
  rprintf(F,"Usage: rsync [OPTION]... SRC [SRC]... DEST\n");
  rprintf(F,"  or   rsync [OPTION]... SRC [SRC]... [USER@]HOST:DEST\n");
  rprintf(F,"  or   rsync [OPTION]... SRC [SRC]... [USER@]HOST::DEST\n");
  rprintf(F,"  or   rsync [OPTION]... SRC [SRC]... rsync://[USER@]HOST[:PORT]/DEST\n");
  rprintf(F,"  or   rsync [OPTION]... [USER@]HOST:SRC [DEST]\n");
  rprintf(F,"  or   rsync [OPTION]... [USER@]HOST::SRC [DEST]\n");
  rprintf(F,"  or   rsync [OPTION]... rsync://[USER@]HOST[:PORT]/SRC [DEST]\n");
  rprintf(F,"The ':' usages connect via remote shell, while '::' & 'rsync://' usages connect\n");
  rprintf(F,"to an rsync daemon, and require SRC or DEST to start with a module name.\n");
  rprintf(F,"\n");
  rprintf(F,"Options\n");
  rprintf(F," -v, --verbose               increase verbosity\n");
  rprintf(F,"     --info=FLAGS            fine-grained informational verbosity\n");
  rprintf(F,"     --debug=FLAGS           fine-grained debug verbosity\n");
  rprintf(F,"     --msgs2stderr           special output handling for debugging\n");
  rprintf(F," -q, --quiet                 suppress non-error messages\n");
  rprintf(F,"     --no-motd               suppress daemon-mode MOTD (see manpage caveat)\n");
  rprintf(F," -c, --checksum              skip based on checksum, not mod-time & size\n");
  rprintf(F," -a, --archive               archive mode; equals -rlptgoD (no -H,-A,-X)\n");
  rprintf(F,"     --no-OPTION             turn off an implied OPTION (e.g. --no-D)\n");
  rprintf(F," -r, --recursive             recurse into directories\n");
  rprintf(F," -R, --relative              use relative path names\n");
  rprintf(F,"     --no-implied-dirs       don't send implied dirs with --relative\n");
  rprintf(F," -b, --backup                make backups (see --suffix & --backup-dir)\n");
  rprintf(F,"     --backup-dir=DIR        make backups into hierarchy based in DIR\n");
  rprintf(F,"     --suffix=SUFFIX         set backup suffix (default %s w/o --backup-dir)\n",BACKUP_SUFFIX);
  rprintf(F," -u, --update                skip files that are newer on the receiver\n");
  rprintf(F,"     --inplace               update destination files in-place (SEE MAN PAGE)\n");
  rprintf(F,"     --append                append data onto shorter files\n");
  rprintf(F,"     --append-verify         like --append, but with old data in file checksum\n");
  rprintf(F," -d, --dirs                  transfer directories without recursing\n");
  rprintf(F," -l, --links                 copy symlinks as symlinks\n");
  rprintf(F," -L, --copy-links            transform symlink into referent file/dir\n");
  rprintf(F,"     --copy-unsafe-links     only \"unsafe\" symlinks are transformed\n");
  rprintf(F,"     --safe-links            ignore symlinks that point outside the source tree\n");
  rprintf(F,"     --munge-links           munge symlinks to make them safer (but unusable)\n");
  rprintf(F," -k, --copy-dirlinks         transform symlink to a dir into referent dir\n");
  rprintf(F," -K, --keep-dirlinks         treat symlinked dir on receiver as dir\n");
  rprintf(F," -H, --hard-links            preserve hard links\n");
  rprintf(F," -p, --perms                 preserve permissions\n");
  rprintf(F," -E, --executability         preserve the file's executability\n");
  rprintf(F,"     --chmod=CHMOD           affect file and/or directory permissions\n");
#ifdef SUPPORT_ACLS
  rprintf(F," -A, --acls                  preserve ACLs (implies --perms)\n");
#endif
#ifdef SUPPORT_XATTRS
  rprintf(F," -X, --xattrs                preserve extended attributes\n");
#endif
  rprintf(F," -o, --owner                 preserve owner (super-user only)\n");
  rprintf(F," -g, --group                 preserve group\n");
  rprintf(F,"     --devices               preserve device files (super-user only)\n");
  rprintf(F,"     --specials              preserve special files\n");
  rprintf(F," -D                          same as --devices --specials\n");
  rprintf(F," -t, --times                 preserve modification times\n");
  rprintf(F," -O, --omit-dir-times        omit directories from --times\n");
  rprintf(F," -J, --omit-link-times       omit symlinks from --times\n");
  rprintf(F,"     --super                 receiver attempts super-user activities\n");
#ifdef SUPPORT_XATTRS
  rprintf(F,"     --fake-super            store/recover privileged attrs using xattrs\n");
#endif
  rprintf(F," -S, --sparse                turn sequences of nulls into sparse blocks\n");
#ifdef SUPPORT_PREALLOCATION
  rprintf(F,"     --preallocate           allocate dest files before writing them\n");
#else
  rprintf(F,"     --preallocate           pre-allocate dest files on remote receiver\n");
#endif
  rprintf(F," -n, --dry-run               perform a trial run with no changes made\n");
  rprintf(F," -W, --whole-file            copy files whole (without delta-xfer algorithm)\n");
  rprintf(F,"     --checksum-choice=STR   choose the checksum algorithms\n");
  rprintf(F," -x, --one-file-system       don't cross filesystem boundaries\n");
  rprintf(F," -B, --block-size=SIZE       force a fixed checksum block-size\n");
  rprintf(F," -e, --rsh=COMMAND           specify the remote shell to use\n");
  rprintf(F,"     --rsync-path=PROGRAM    specify the rsync to run on the remote machine\n");
  rprintf(F,"     --existing              skip creating new files on receiver\n");
  rprintf(F,"     --ignore-existing       skip updating files that already exist on receiver\n");
  rprintf(F,"     --remove-source-files   sender removes synchronized files (non-dirs)\n");
  rprintf(F,"     --del                   an alias for --delete-during\n");
  rprintf(F,"     --delete                delete extraneous files from destination dirs\n");
  rprintf(F,"     --delete-before         receiver deletes before transfer, not during\n");
  rprintf(F,"     --delete-during         receiver deletes during the transfer\n");
  rprintf(F,"     --delete-delay          find deletions during, delete after\n");
  rprintf(F,"     --delete-after          receiver deletes after transfer, not during\n");
  rprintf(F,"     --delete-excluded       also delete excluded files from destination dirs\n");
  rprintf(F,"     --ignore-missing-args   ignore missing source args without error\n");
  rprintf(F,"     --delete-missing-args   delete missing source args from destination\n");
  rprintf(F,"     --ignore-errors         delete even if there are I/O errors\n");
  rprintf(F,"     --force                 force deletion of directories even if not empty\n");
  rprintf(F,"     --max-delete=NUM        don't delete more than NUM files\n");
  rprintf(F,"     --max-size=SIZE         don't transfer any file larger than SIZE\n");
  rprintf(F,"     --min-size=SIZE         don't transfer any file smaller than SIZE\n");
  rprintf(F,"     --partial               keep partially transferred files\n");
  rprintf(F,"     --partial-dir=DIR       put a partially transferred file into DIR\n");
  rprintf(F,"     --delay-updates         put all updated files into place at transfer's end\n");
  rprintf(F," -m, --prune-empty-dirs      prune empty directory chains from the file-list\n");
  rprintf(F,"     --numeric-ids           don't map uid/gid values by user/group name\n");
  rprintf(F,"     --usermap=STRING        custom username mapping\n");
  rprintf(F,"     --groupmap=STRING       custom groupname mapping\n");
  rprintf(F,"     --chown=USER:GROUP      simple username/groupname mapping\n");
  rprintf(F,"     --timeout=SECONDS       set I/O timeout in seconds\n");
  rprintf(F,"     --contimeout=SECONDS    set daemon connection timeout in seconds\n");
  rprintf(F," -I, --ignore-times          don't skip files that match in size and mod-time\n");
  rprintf(F," -M, --remote-option=OPTION  send OPTION to the remote side only\n");
  rprintf(F,"     --size-only             skip files that match in size\n");
  rprintf(F," -@, --modify-window=NUM     set the accuracy for mod-time comparisons\n");
  rprintf(F," -T, --temp-dir=DIR          create temporary files in directory DIR\n");
  rprintf(F," -y, --fuzzy                 find similar file for basis if no dest file\n");
  rprintf(F,"     --compare-dest=DIR      also compare destination files relative to DIR\n");
  rprintf(F,"     --copy-dest=DIR         ... and include copies of unchanged files\n");
  rprintf(F,"     --link-dest=DIR         hardlink to files in DIR when unchanged\n");
  rprintf(F," -z, --compress              compress file data during the transfer\n");
  rprintf(F,"     --compress-level=NUM    explicitly set compression level\n");
  rprintf(F,"     --skip-compress=LIST    skip compressing files with a suffix in LIST\n");
  rprintf(F," -C, --cvs-exclude           auto-ignore files the same way CVS does\n");
  rprintf(F," -f, --filter=RULE           add a file-filtering RULE\n");
  rprintf(F," -F                          same as --filter='dir-merge /.rsync-filter'\n");
  rprintf(F,"                             repeated: --filter='- .rsync-filter'\n");
  rprintf(F,"     --exclude=PATTERN       exclude files matching PATTERN\n");
  rprintf(F,"     --exclude-from=FILE     read exclude patterns from FILE\n");
  rprintf(F,"     --include=PATTERN       don't exclude files matching PATTERN\n");
  rprintf(F,"     --include-from=FILE     read include patterns from FILE\n");
  rprintf(F,"     --files-from=FILE       read list of source-file names from FILE\n");
  rprintf(F," -0, --from0                 all *-from/filter files are delimited by 0s\n");
  rprintf(F," -s, --protect-args          no space-splitting; only wildcard special-chars\n");
  rprintf(F,"     --address=ADDRESS       bind address for outgoing socket to daemon\n");
  rprintf(F,"     --port=PORT             specify double-colon alternate port number\n");
  rprintf(F,"     --sockopts=OPTIONS      specify custom TCP options\n");
  rprintf(F,"     --blocking-io           use blocking I/O for the remote shell\n");
  rprintf(F,"     --stats                 give some file-transfer stats\n");
  rprintf(F," -8, --8-bit-output          leave high-bit chars unescaped in output\n");
  rprintf(F," -h, --human-readable        output numbers in a human-readable format\n");
  rprintf(F,"     --progress              show progress during transfer\n");
  rprintf(F," -P                          same as --partial --progress\n");
  rprintf(F," -i, --itemize-changes       output a change-summary for all updates\n");
  rprintf(F,"     --out-format=FORMAT     output updates using the specified FORMAT\n");
  rprintf(F,"     --log-file=FILE         log what we're doing to the specified FILE\n");
  rprintf(F,"     --log-file-format=FMT   log updates using the specified FMT\n");
  rprintf(F,"     --password-file=FILE    read daemon-access password from FILE\n");
  rprintf(F,"     --list-only             list the files instead of copying them\n");
  rprintf(F,"     --bwlimit=RATE          limit socket I/O bandwidth\n");
#ifdef HAVE_SETVBUF
  rprintf(F,"     --outbuf=N|L|B          set output buffering to None, Line, or Block\n");
#endif
  rprintf(F,"     --write-batch=FILE      write a batched update to FILE\n");
  rprintf(F,"     --only-write-batch=FILE like --write-batch but w/o updating destination\n");
  rprintf(F,"     --read-batch=FILE       read a batched update from FILE\n");
  rprintf(F,"     --protocol=NUM          force an older protocol version to be used\n");
#ifdef ICONV_OPTION
  rprintf(F,"     --iconv=CONVERT_SPEC    request charset conversion of filenames\n");
#endif
  rprintf(F,"     --checksum-seed=NUM     set block/file checksum seed (advanced)\n");
  rprintf(F," -4, --ipv4                  prefer IPv4\n");
  rprintf(F," -6, --ipv6                  prefer IPv6\n");
  rprintf(F,"     --version               print version number\n");
  rprintf(F,"(-h) --help                  show this help (-h is --help only if used alone)\n");

  rprintf(F,"\n");
  rprintf(F,"Use \"rsync --daemon --help\" to see the daemon-mode command-line options.\n");
  rprintf(F,"Please see the rsync(1) and rsyncd.conf(5) man pages for full documentation.\n");
  rprintf(F,"See http://rsync.samba.org/ for updates, bug reports, and answers\n");
}