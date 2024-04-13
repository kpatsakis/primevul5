static void print_rsync_version(enum logcode f)
{
	char *subprotocol = "";
	char const *got_socketpair = "no ";
	char const *have_inplace = "no ";
	char const *hardlinks = "no ";
	char const *prealloc = "no ";
	char const *symtimes = "no ";
	char const *acls = "no ";
	char const *xattrs = "no ";
	char const *links = "no ";
	char const *iconv = "no ";
	char const *ipv6 = "no ";
	STRUCT_STAT *dumstat;

#if SUBPROTOCOL_VERSION != 0
	if (asprintf(&subprotocol, ".PR%d", SUBPROTOCOL_VERSION) < 0)
		out_of_memory("print_rsync_version");
#endif
#ifdef HAVE_SOCKETPAIR
	got_socketpair = "";
#endif
#ifdef HAVE_FTRUNCATE
	have_inplace = "";
#endif
#ifdef SUPPORT_HARD_LINKS
	hardlinks = "";
#endif
#ifdef SUPPORT_PREALLOCATION
	prealloc = "";
#endif
#ifdef SUPPORT_ACLS
	acls = "";
#endif
#ifdef SUPPORT_XATTRS
	xattrs = "";
#endif
#ifdef SUPPORT_LINKS
	links = "";
#endif
#ifdef INET6
	ipv6 = "";
#endif
#ifdef ICONV_OPTION
	iconv = "";
#endif
#ifdef CAN_SET_SYMLINK_TIMES
	symtimes = "";
#endif

	rprintf(f, "%s  version %s  protocol version %d%s\n",
		RSYNC_NAME, RSYNC_VERSION, PROTOCOL_VERSION, subprotocol);
	rprintf(f, "Copyright (C) 1996-2015 by Andrew Tridgell, Wayne Davison, and others.\n");
	rprintf(f, "Web site: http://rsync.samba.org/\n");
	rprintf(f, "Capabilities:\n");
	rprintf(f, "    %d-bit files, %d-bit inums, %d-bit timestamps, %d-bit long ints,\n",
		(int)(sizeof (OFF_T) * 8),
		(int)(sizeof dumstat->st_ino * 8), /* Don't check ino_t! */
		(int)(sizeof (time_t) * 8),
		(int)(sizeof (int64) * 8));
	rprintf(f, "    %ssocketpairs, %shardlinks, %ssymlinks, %sIPv6, batchfiles, %sinplace,\n",
		got_socketpair, hardlinks, links, ipv6, have_inplace);
	rprintf(f, "    %sappend, %sACLs, %sxattrs, %siconv, %ssymtimes, %sprealloc\n",
		have_inplace, acls, xattrs, iconv, symtimes, prealloc);

#ifdef MAINTAINER_MODE
	rprintf(f, "Panic Action: \"%s\"\n", get_panic_action());
#endif

#if SIZEOF_INT64 < 8
	rprintf(f, "WARNING: no 64-bit integers on this platform!\n");
#endif
	if (sizeof (int64) != SIZEOF_INT64) {
		rprintf(f,
			"WARNING: size mismatch in SIZEOF_INT64 define (%d != %d)\n",
			(int) SIZEOF_INT64, (int) sizeof (int64));
	}

	rprintf(f,"\n");
	rprintf(f,"rsync comes with ABSOLUTELY NO WARRANTY.  This is free software, and you\n");
	rprintf(f,"are welcome to redistribute it under certain conditions.  See the GNU\n");
	rprintf(f,"General Public Licence for details.\n");
}