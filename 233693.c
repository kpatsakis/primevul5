void help(void)
{
	printf(_("%s %s -- get file access control lists\n"),
	       progname, VERSION);
	printf(_("Usage: %s [-%s] file ...\n"),
	         progname, cmd_line_options);
#if !POSIXLY_CORRECT
	if (posixly_correct) {
#endif
		printf(_(
"  -d, --default           display the default access control list\n"));
#if !POSIXLY_CORRECT
	} else {
		printf(_(
"  -a,  --access           display the file access control list only\n"
"  -d, --default           display the default access control list only\n"
"  -c, --omit-header       do not display the comment header\n"
"  -e, --all-effective     print all effective rights\n"
"  -E, --no-effective      print no effective rights\n"
"  -s, --skip-base         skip files that only have the base entries\n"
"  -R, --recursive         recurse into subdirectories\n"
"  -L, --logical           logical walk, follow symbolic links\n"
"  -P, --physical          physical walk, do not follow symbolic links\n"
"  -t, --tabular           use tabular output format\n"
"  -n, --numeric           print numeric user/group identifiers\n"
"  -p, --absolute-names    don't strip leading '/' in pathnames\n"));
	}
#endif
	printf(_(
"  -v, --version           print version and exit\n"
"  -h, --help              this help text\n"));
}