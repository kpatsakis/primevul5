copyin_file (struct cpio_file_stat *file_hdr, int in_file_des)
{
  bool existing_dir = false;

  if (!to_stdout_option
      && try_existing_file (file_hdr, in_file_des, &existing_dir) < 0)
    return;

  /* Do the real copy or link.  */
  switch (file_hdr->c_mode & CP_IFMT)
    {
    case CP_IFREG:
      copyin_regular_file (file_hdr, in_file_des);
      break;

    case CP_IFDIR:
      cpio_create_dir (file_hdr, existing_dir);
      break;

    case CP_IFCHR:
    case CP_IFBLK:
#ifdef CP_IFSOCK
    case CP_IFSOCK:
#endif
#ifdef CP_IFIFO
    case CP_IFIFO:
#endif
      copyin_device (file_hdr);
      break;

#ifdef CP_IFLNK
    case CP_IFLNK:
      copyin_link (file_hdr, in_file_des);
      break;
#endif

    default:
      error (0, 0, _("%s: unknown file type"), file_hdr->c_name);
      tape_toss_input (in_file_des, file_hdr->c_filesize);
      tape_skip_padding (in_file_des, file_hdr->c_filesize);
    }
}