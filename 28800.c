tar_sparse_member_p (struct tar_sparse_file *file)
{
  if (file->optab->sparse_member_p)
    return file->optab->sparse_member_p (file);
  return false;
}
