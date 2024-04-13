tar_sparse_init (struct tar_sparse_file *file)
{
  memset (file, 0, sizeof *file);

  if (!sparse_select_optab (file))
    return false;

  if (file->optab->init)
    return file->optab->init (file);

  return true;
}
