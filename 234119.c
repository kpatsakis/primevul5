stat_to_cpio (struct cpio_file_stat *hdr, struct stat *st)
{
  get_inode_and_dev (hdr, st);

  /* For POSIX systems that don't define the S_IF macros,
     we can't assume that S_ISfoo means the standard Unix
     S_IFfoo bit(s) are set.  So do it manually, with a
     different name.  Bleah.  */
  hdr->c_mode = (st->st_mode & 07777);
  if (S_ISREG (st->st_mode))
    hdr->c_mode |= CP_IFREG;
  else if (S_ISDIR (st->st_mode))
    hdr->c_mode |= CP_IFDIR;
#ifdef S_ISBLK
  else if (S_ISBLK (st->st_mode))
    hdr->c_mode |= CP_IFBLK;
#endif
#ifdef S_ISCHR
  else if (S_ISCHR (st->st_mode))
    hdr->c_mode |= CP_IFCHR;
#endif
#ifdef S_ISFIFO
  else if (S_ISFIFO (st->st_mode))
    hdr->c_mode |= CP_IFIFO;
#endif
#ifdef S_ISLNK
  else if (S_ISLNK (st->st_mode))
    hdr->c_mode |= CP_IFLNK;
#endif
#ifdef S_ISSOCK
  else if (S_ISSOCK (st->st_mode))
    hdr->c_mode |= CP_IFSOCK;
#endif
#ifdef S_ISNWK
  else if (S_ISNWK (st->st_mode))
    hdr->c_mode |= CP_IFNWK;
#endif
  hdr->c_nlink = st->st_nlink;
  hdr->c_uid = CPIO_UID (st->st_uid);
  hdr->c_gid = CPIO_GID (st->st_gid);
  if (S_ISBLK (st->st_mode) || S_ISCHR (st->st_mode))
    {
      hdr->c_rdev_maj = major (st->st_rdev);
      hdr->c_rdev_min = minor (st->st_rdev);
    }
  else
    {
      hdr->c_rdev_maj = 0;
      hdr->c_rdev_min = 0;
    }
  hdr->c_mtime = st->st_mtime;
  hdr->c_filesize = st->st_size;
  hdr->c_chksum = 0;
  hdr->c_tar_linkname = NULL;
}