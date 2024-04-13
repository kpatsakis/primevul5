scm_stat2scm (struct stat_or_stat64 *stat_temp)
{
  SCM ans = scm_c_make_vector (18, SCM_UNSPECIFIED);
  
  SCM_SIMPLE_VECTOR_SET(ans, 0, scm_from_ulong (stat_temp->st_dev));
  SCM_SIMPLE_VECTOR_SET(ans, 1, scm_from_ino_t_or_ino64_t (stat_temp->st_ino));
  SCM_SIMPLE_VECTOR_SET(ans, 2, scm_from_ulong (stat_temp->st_mode));
  SCM_SIMPLE_VECTOR_SET(ans, 3, scm_from_ulong (stat_temp->st_nlink));
  SCM_SIMPLE_VECTOR_SET(ans, 4, scm_from_ulong (stat_temp->st_uid));
  SCM_SIMPLE_VECTOR_SET(ans, 5, scm_from_ulong (stat_temp->st_gid));
#ifdef HAVE_STRUCT_STAT_ST_RDEV
  SCM_SIMPLE_VECTOR_SET(ans, 6, scm_from_ulong (stat_temp->st_rdev));
#else
  SCM_SIMPLE_VECTOR_SET(ans, 6, SCM_BOOL_F);
#endif
  SCM_SIMPLE_VECTOR_SET(ans, 7, scm_from_off_t_or_off64_t (stat_temp->st_size));
  SCM_SIMPLE_VECTOR_SET(ans, 8, scm_from_ulong (stat_temp->st_atime));
  SCM_SIMPLE_VECTOR_SET(ans, 9, scm_from_ulong (stat_temp->st_mtime));
  SCM_SIMPLE_VECTOR_SET(ans, 10, scm_from_ulong (stat_temp->st_ctime));
#ifdef HAVE_STRUCT_STAT_ST_BLKSIZE
  SCM_SIMPLE_VECTOR_SET(ans, 11, scm_from_ulong (stat_temp->st_blksize));
#else
  SCM_SIMPLE_VECTOR_SET(ans, 11, scm_from_ulong (4096L));
#endif
#ifdef HAVE_STRUCT_STAT_ST_BLOCKS
  SCM_SIMPLE_VECTOR_SET(ans, 12, scm_from_blkcnt_t_or_blkcnt64_t (stat_temp->st_blocks));
#else
  SCM_SIMPLE_VECTOR_SET(ans, 12, SCM_BOOL_F);
#endif
  {
    int mode = stat_temp->st_mode;
    
    if (S_ISREG (mode))
      SCM_SIMPLE_VECTOR_SET(ans, 13, scm_sym_regular);
    else if (S_ISDIR (mode))
      SCM_SIMPLE_VECTOR_SET(ans, 13, scm_sym_directory);
#ifdef S_ISLNK
    /* systems without symlinks probably don't have S_ISLNK */
    else if (S_ISLNK (mode))
      SCM_SIMPLE_VECTOR_SET(ans, 13, scm_sym_symlink);
#endif
    else if (S_ISBLK (mode))
      SCM_SIMPLE_VECTOR_SET(ans, 13, scm_sym_block_special);
    else if (S_ISCHR (mode))
      SCM_SIMPLE_VECTOR_SET(ans, 13, scm_sym_char_special);
    else if (S_ISFIFO (mode))
      SCM_SIMPLE_VECTOR_SET(ans, 13, scm_sym_fifo);
#ifdef S_ISSOCK
    else if (S_ISSOCK (mode))
      SCM_SIMPLE_VECTOR_SET(ans, 13, scm_sym_sock);
#endif
    else
      SCM_SIMPLE_VECTOR_SET(ans, 13, scm_sym_unknown);

    SCM_SIMPLE_VECTOR_SET(ans, 14, scm_from_int ((~S_IFMT) & mode));

    /* the layout of the bits in ve[14] is intended to be portable.
       If there are systems that don't follow the usual convention,
       the following could be used:

       tmp = 0;
       if (S_ISUID & mode) tmp += 1;
       tmp <<= 1;
       if (S_IRGRP & mode) tmp += 1;
       tmp <<= 1;
       if (S_ISVTX & mode) tmp += 1;
       tmp <<= 1;
       if (S_IRUSR & mode) tmp += 1;
       tmp <<= 1;
       if (S_IWUSR & mode) tmp += 1;
       tmp <<= 1;
       if (S_IXUSR & mode) tmp += 1;
       tmp <<= 1;
       if (S_IWGRP & mode) tmp += 1;
       tmp <<= 1;
       if (S_IXGRP & mode) tmp += 1;
       tmp <<= 1;
       if (S_IROTH & mode) tmp += 1;
       tmp <<= 1;
       if (S_IWOTH & mode) tmp += 1;
       tmp <<= 1;
       if (S_IXOTH & mode) tmp += 1; 

       SCM_SIMPLE_VECTOR_SET(ans, 14, scm_from_int (tmp));
       
       */
  }  
#ifdef HAVE_STRUCT_STAT_ST_ATIM
  SCM_SIMPLE_VECTOR_SET(ans, 15, scm_from_long (stat_temp->st_atim.tv_nsec));
#else
  SCM_SIMPLE_VECTOR_SET(ans, 15, SCM_I_MAKINUM (0));
#endif
#ifdef HAVE_STRUCT_STAT_ST_MTIM
  SCM_SIMPLE_VECTOR_SET(ans, 16, scm_from_long (stat_temp->st_mtim.tv_nsec));
#else
  SCM_SIMPLE_VECTOR_SET(ans, 16, SCM_I_MAKINUM (0));
#endif
#ifdef HAVE_STRUCT_STAT_ST_CTIM
  SCM_SIMPLE_VECTOR_SET(ans, 17, scm_from_ulong (stat_temp->st_ctim.tv_sec));
#else
  SCM_SIMPLE_VECTOR_SET(ans, 17, SCM_I_MAKINUM (0));
#endif

  return ans;
}