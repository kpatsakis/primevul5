tar_sparse_dump_header (struct tar_sparse_file *file)
{
  if (file->optab->dump_header)
    return file->optab->dump_header (file);
  return false;
}
