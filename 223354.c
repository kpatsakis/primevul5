static void daemon_usage(enum logcode F)
{
  print_rsync_version(F);

  rprintf(F,"\n");
  rprintf(F,"Usage: rsync --daemon [OPTION]...\n");
  rprintf(F,"     --address=ADDRESS       bind to the specified address\n");
  rprintf(F,"     --bwlimit=RATE          limit socket I/O bandwidth\n");
  rprintf(F,"     --config=FILE           specify alternate rsyncd.conf file\n");
  rprintf(F," -M, --dparam=OVERRIDE       override global daemon config parameter\n");
  rprintf(F,"     --no-detach             do not detach from the parent\n");
  rprintf(F,"     --port=PORT             listen on alternate port number\n");
  rprintf(F,"     --log-file=FILE         override the \"log file\" setting\n");
  rprintf(F,"     --log-file-format=FMT   override the \"log format\" setting\n");
  rprintf(F,"     --sockopts=OPTIONS      specify custom TCP options\n");
  rprintf(F," -v, --verbose               increase verbosity\n");
  rprintf(F," -4, --ipv4                  prefer IPv4\n");
  rprintf(F," -6, --ipv6                  prefer IPv6\n");
  rprintf(F,"     --help                  show this help screen\n");

  rprintf(F,"\n");
  rprintf(F,"If you were not trying to invoke rsync as a daemon, avoid using any of the\n");
  rprintf(F,"daemon-specific rsync options.  See also the rsyncd.conf(5) man page.\n");
}