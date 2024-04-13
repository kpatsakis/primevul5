sparse_select_optab (struct tar_sparse_file *file)
{
  switch (current_format == DEFAULT_FORMAT ? archive_format : current_format)
    {
    case V7_FORMAT:
    case USTAR_FORMAT:
      return false;

    case OLDGNU_FORMAT:
    case GNU_FORMAT: /*FIXME: This one should disappear? */
      file->optab = &oldgnu_optab;
      break;

    case POSIX_FORMAT:
      file->optab = &pax_optab;
      break;

    case STAR_FORMAT:
      file->optab = &star_optab;
      break;

    default:
      return false;
    }
  return true;
}
