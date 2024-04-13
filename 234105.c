read_in_binary (struct cpio_file_stat *file_hdr,
		struct old_cpio_header *short_hdr,
		int in_des)
{
  file_hdr->c_magic = short_hdr->c_magic;

  tape_buffered_read (((char *) short_hdr) + 6, in_des,
		      sizeof *short_hdr - 6 /* = 20 */);

  /* If the magic number is byte swapped, fix the header.  */
  if (file_hdr->c_magic == swab_short ((unsigned short) 070707))
    {
      static int warned = 0;

      /* Alert the user that they might have to do byte swapping on
	 the file contents.  */
      if (warned == 0)
	{
	  error (0, 0, _("warning: archive header has reverse byte-order"));
	  warned = 1;
	}
      swab_array ((char *) short_hdr, 13);
    }

  file_hdr->c_dev_maj = major (short_hdr->c_dev);
  file_hdr->c_dev_min = minor (short_hdr->c_dev);
  file_hdr->c_ino = short_hdr->c_ino;
  file_hdr->c_mode = short_hdr->c_mode;
  file_hdr->c_uid = short_hdr->c_uid;
  file_hdr->c_gid = short_hdr->c_gid;
  file_hdr->c_nlink = short_hdr->c_nlink;
  file_hdr->c_rdev_maj = major (short_hdr->c_rdev);
  file_hdr->c_rdev_min = minor (short_hdr->c_rdev);
  file_hdr->c_mtime = (unsigned long) short_hdr->c_mtimes[0] << 16
                      | short_hdr->c_mtimes[1];
  file_hdr->c_filesize = (unsigned long) short_hdr->c_filesizes[0] << 16
                      | short_hdr->c_filesizes[1];
  read_name_from_file (file_hdr, in_des, short_hdr->c_namesize);

  /* In binary mode, the amount of space allocated in the header for
     the filename is `c_namesize' rounded up to the next short-word,
     so we might need to drop a byte.  */
  if (file_hdr->c_namesize % 2)
    tape_toss_input (in_des, 1L);

  /* HP/UX cpio creates archives that look just like ordinary archives,
     but for devices it sets major = 0, minor = 1, and puts the
     actual major/minor number in the filesize field.  See if this
     is an HP/UX cpio archive, and if so fix it.  We have to do this
     here because process_copy_in() assumes filesize is always 0
     for devices.  */
  switch (file_hdr->c_mode & CP_IFMT)
    {
      case CP_IFCHR:
      case CP_IFBLK:
#ifdef CP_IFSOCK
      case CP_IFSOCK:
#endif
#ifdef CP_IFIFO
      case CP_IFIFO:
#endif
	if (file_hdr->c_filesize != 0
	    && file_hdr->c_rdev_maj == 0
	    && file_hdr->c_rdev_min == 1)
	  {
	    file_hdr->c_rdev_maj = major (file_hdr->c_filesize);
	    file_hdr->c_rdev_min = minor (file_hdr->c_filesize);
	    file_hdr->c_filesize = 0;
	  }
	break;
      default:
	break;
    }
}