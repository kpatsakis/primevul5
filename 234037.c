cpio_to_stat (struct stat *st, struct cpio_file_stat *hdr)
{
  memset (st, 0, sizeof (*st));
  st->st_dev = makedev (hdr->c_dev_maj, hdr->c_dev_min);
  st->st_ino = hdr->c_ino;
  st->st_mode = hdr->c_mode & 0777;
  if (hdr->c_mode & CP_IFREG)
    st->st_mode |= S_IFREG;
  else if (hdr->c_mode & CP_IFDIR)
    st->st_mode |= S_IFDIR;
#ifdef S_IFBLK
  else if (hdr->c_mode & CP_IFBLK)
    st->st_mode |= S_IFBLK;
#endif
#ifdef S_IFCHR
  else if (hdr->c_mode & CP_IFCHR)
    st->st_mode |= S_IFCHR;
#endif
#ifdef S_IFFIFO
  else if (hdr->c_mode & CP_IFIFO)
    st->st_mode |= S_IFIFO;
#endif
#ifdef S_IFLNK
  else if (hdr->c_mode & CP_IFLNK)
    st->st_mode |= S_IFLNK;
#endif
#ifdef S_IFSOCK
  else if (hdr->c_mode & CP_IFSOCK)
    st->st_mode |= S_IFSOCK;
#endif
#ifdef S_IFNWK
  else if (hdr->c_mode & CP_IFNWK)
    st->st_mode |= S_IFNWK;
#endif
  st->st_nlink = hdr->c_nlink;
  st->st_uid = CPIO_UID (hdr->c_uid);
  st->st_gid = CPIO_GID (hdr->c_gid);
  st->st_rdev = makedev (hdr->c_rdev_maj, hdr->c_rdev_min);
  st->st_mtime = hdr->c_mtime;
  st->st_size = hdr->c_filesize;
}