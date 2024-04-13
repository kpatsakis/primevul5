static sftp_statvfs_t sftp_parse_statvfs(sftp_session sftp, ssh_buffer buf) {
	sftp_statvfs_t  statvfs;
  uint64_t tmp;
  int ok = 0;

  statvfs = malloc(sizeof(struct sftp_statvfs_struct));
  if (statvfs == NULL) {
    ssh_set_error_oom(sftp->session);
    return NULL;
  }
  ZERO_STRUCTP(statvfs);

  /* try .. catch */
  do {
    /* file system block size */
    if (buffer_get_u64(buf, &tmp) != sizeof(uint64_t)) {
      break;
    }
    statvfs->f_bsize = ntohll(tmp);

    /* fundamental fs block size */
    if (buffer_get_u64(buf, &tmp) != sizeof(uint64_t)) {
      break;
    }
    statvfs->f_frsize = ntohll(tmp);

    /* number of blocks (unit f_frsize) */
    if (buffer_get_u64(buf, &tmp) != sizeof(uint64_t)) {
      break;
    }
    statvfs->f_blocks = ntohll(tmp);

    /* free blocks in file system */
    if (buffer_get_u64(buf, &tmp) != sizeof(uint64_t)) {
      break;
    }
    statvfs->f_bfree = ntohll(tmp);

    /* free blocks for non-root */
    if (buffer_get_u64(buf, &tmp) != sizeof(uint64_t)) {
      break;
    }
    statvfs->f_bavail = ntohll(tmp);

    /* total file inodes */
    if (buffer_get_u64(buf, &tmp) != sizeof(uint64_t)) {
      break;
    }
    statvfs->f_files = ntohll(tmp);

    /* free file inodes */
    if (buffer_get_u64(buf, &tmp) != sizeof(uint64_t)) {
      break;
    }
    statvfs->f_ffree = ntohll(tmp);

    /* free file inodes for to non-root */
    if (buffer_get_u64(buf, &tmp) != sizeof(uint64_t)) {
      break;
    }
    statvfs->f_favail = ntohll(tmp);

    /* file system id */
    if (buffer_get_u64(buf, &tmp) != sizeof(uint64_t)) {
      break;
    }
    statvfs->f_fsid = ntohll(tmp);

    /* bit mask of f_flag values */
    if (buffer_get_u64(buf, &tmp) != sizeof(uint64_t)) {
      break;
    }
    statvfs->f_flag = ntohll(tmp);

    /* maximum filename length */
    if (buffer_get_u64(buf, &tmp) != sizeof(uint64_t)) {
      break;
    }
    statvfs->f_namemax = ntohll(tmp);

    ok = 1;
  } while(0);

  if (!ok) {
    SAFE_FREE(statvfs);
    ssh_set_error(sftp->session, SSH_FATAL, "Invalid statvfs structure");
    return NULL;
  }

  return statvfs;
}