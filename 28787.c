lseek_or_error (struct tar_sparse_file *file, off_t offset)
{
  if (file->seekable
      ? lseek (file->fd, offset, SEEK_SET) < 0
      : ! dump_zeros (file, offset))
    {
      seek_diag_details (file->stat_info->orig_file_name, offset);
      return false;
    }
  return true;
}
