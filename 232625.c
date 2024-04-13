vim_rename(char_u *from, char_u *to)
{
    int		fd_in;
    int		fd_out;
    int		n;
    char	*errmsg = NULL;
    char	*buffer;
#ifdef AMIGA
    BPTR	flock;
#endif
    stat_T	st;
    long	perm;
#ifdef HAVE_ACL
    vim_acl_T	acl;		/* ACL from original file */
#endif
    int		use_tmp_file = FALSE;

    /*
     * When the names are identical, there is nothing to do.  When they refer
     * to the same file (ignoring case and slash/backslash differences) but
     * the file name differs we need to go through a temp file.
     */
    if (fnamecmp(from, to) == 0)
    {
	if (p_fic && STRCMP(gettail(from), gettail(to)) != 0)
	    use_tmp_file = TRUE;
	else
	    return 0;
    }

    /*
     * Fail if the "from" file doesn't exist.  Avoids that "to" is deleted.
     */
    if (mch_stat((char *)from, &st) < 0)
	return -1;

#ifdef UNIX
    {
	stat_T	st_to;

	/* It's possible for the source and destination to be the same file.
	 * This happens when "from" and "to" differ in case and are on a FAT32
	 * filesystem.  In that case go through a temp file name. */
	if (mch_stat((char *)to, &st_to) >= 0
		&& st.st_dev == st_to.st_dev
		&& st.st_ino == st_to.st_ino)
	    use_tmp_file = TRUE;
    }
#endif
#ifdef WIN3264
    {
	BY_HANDLE_FILE_INFORMATION info1, info2;

	/* It's possible for the source and destination to be the same file.
	 * In that case go through a temp file name.  This makes rename("foo",
	 * "./foo") a no-op (in a complicated way). */
	if (win32_fileinfo(from, &info1) == FILEINFO_OK
		&& win32_fileinfo(to, &info2) == FILEINFO_OK
		&& info1.dwVolumeSerialNumber == info2.dwVolumeSerialNumber
		&& info1.nFileIndexHigh == info2.nFileIndexHigh
		&& info1.nFileIndexLow == info2.nFileIndexLow)
	    use_tmp_file = TRUE;
    }
#endif

    if (use_tmp_file)
    {
	char	tempname[MAXPATHL + 1];

	/*
	 * Find a name that doesn't exist and is in the same directory.
	 * Rename "from" to "tempname" and then rename "tempname" to "to".
	 */
	if (STRLEN(from) >= MAXPATHL - 5)
	    return -1;
	STRCPY(tempname, from);
	for (n = 123; n < 99999; ++n)
	{
	    sprintf((char *)gettail((char_u *)tempname), "%d", n);
	    if (mch_stat(tempname, &st) < 0)
	    {
		if (mch_rename((char *)from, tempname) == 0)
		{
		    if (mch_rename(tempname, (char *)to) == 0)
			return 0;
		    /* Strange, the second step failed.  Try moving the
		     * file back and return failure. */
		    mch_rename(tempname, (char *)from);
		    return -1;
		}
		/* If it fails for one temp name it will most likely fail
		 * for any temp name, give up. */
		return -1;
	    }
	}
	return -1;
    }

    /*
     * Delete the "to" file, this is required on some systems to make the
     * mch_rename() work, on other systems it makes sure that we don't have
     * two files when the mch_rename() fails.
     */

#ifdef AMIGA
    /*
     * With MSDOS-compatible filesystems (crossdos, messydos) it is possible
     * that the name of the "to" file is the same as the "from" file, even
     * though the names are different. To avoid the chance of accidentally
     * deleting the "from" file (horror!) we lock it during the remove.
     *
     * When used for making a backup before writing the file: This should not
     * happen with ":w", because startscript() should detect this problem and
     * set buf->b_shortname, causing modname() to return a correct ".bak" file
     * name.  This problem does exist with ":w filename", but then the
     * original file will be somewhere else so the backup isn't really
     * important. If autoscripting is off the rename may fail.
     */
    flock = Lock((UBYTE *)from, (long)ACCESS_READ);
#endif
    mch_remove(to);
#ifdef AMIGA
    if (flock)
	UnLock(flock);
#endif

    /*
     * First try a normal rename, return if it works.
     */
    if (mch_rename((char *)from, (char *)to) == 0)
	return 0;

    /*
     * Rename() failed, try copying the file.
     */
    perm = mch_getperm(from);
#ifdef HAVE_ACL
    /* For systems that support ACL: get the ACL from the original file. */
    acl = mch_get_acl(from);
#endif
    fd_in = mch_open((char *)from, O_RDONLY|O_EXTRA, 0);
    if (fd_in == -1)
    {
#ifdef HAVE_ACL
	mch_free_acl(acl);
#endif
	return -1;
    }

    /* Create the new file with same permissions as the original. */
    fd_out = mch_open((char *)to,
		       O_CREAT|O_EXCL|O_WRONLY|O_EXTRA|O_NOFOLLOW, (int)perm);
    if (fd_out == -1)
    {
	close(fd_in);
#ifdef HAVE_ACL
	mch_free_acl(acl);
#endif
	return -1;
    }

    buffer = (char *)alloc(BUFSIZE);
    if (buffer == NULL)
    {
	close(fd_out);
	close(fd_in);
#ifdef HAVE_ACL
	mch_free_acl(acl);
#endif
	return -1;
    }

    while ((n = read_eintr(fd_in, buffer, BUFSIZE)) > 0)
	if (write_eintr(fd_out, buffer, n) != n)
	{
	    errmsg = _("E208: Error writing to \"%s\"");
	    break;
	}

    vim_free(buffer);
    close(fd_in);
    if (close(fd_out) < 0)
	errmsg = _("E209: Error closing \"%s\"");
    if (n < 0)
    {
	errmsg = _("E210: Error reading \"%s\"");
	to = from;
    }
#ifndef UNIX	    /* for Unix mch_open() already set the permission */
    mch_setperm(to, perm);
#endif
#ifdef HAVE_ACL
    mch_set_acl(to, acl);
    mch_free_acl(acl);
#endif
#if defined(HAVE_SELINUX) || defined(HAVE_SMACK)
    mch_copy_sec(from, to);
#endif
    if (errmsg != NULL)
    {
	EMSG2(errmsg, to);
	return -1;
    }
    mch_remove(from);
    return 0;
}