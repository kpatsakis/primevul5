read_in_old_ascii (struct cpio_file_stat *file_hdr, int in_des)
{
  struct old_ascii_header ascii_header;
  unsigned long dev;

  tape_buffered_read (ascii_header.c_dev, in_des,
		      sizeof ascii_header - sizeof ascii_header.c_magic);
  dev = FROM_OCTAL (ascii_header.c_dev);
  file_hdr->c_dev_maj = major (dev);
  file_hdr->c_dev_min = minor (dev);

  file_hdr->c_ino = FROM_OCTAL (ascii_header.c_ino);
  file_hdr->c_mode = FROM_OCTAL (ascii_header.c_mode);
  file_hdr->c_uid = FROM_OCTAL (ascii_header.c_uid);
  file_hdr->c_gid = FROM_OCTAL (ascii_header.c_gid);
  file_hdr->c_nlink = FROM_OCTAL (ascii_header.c_nlink);
  dev = FROM_OCTAL (ascii_header.c_rdev);
  file_hdr->c_rdev_maj = major (dev);
  file_hdr->c_rdev_min = minor (dev);

  file_hdr->c_mtime = FROM_OCTAL (ascii_header.c_mtime);
  file_hdr->c_filesize = FROM_OCTAL (ascii_header.c_filesize);
  read_name_from_file (file_hdr, in_des, FROM_OCTAL (ascii_header.c_namesize));

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