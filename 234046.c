arf_stores_inode_p (enum archive_format arf)
{
  switch (arf)
    {
    case arf_tar:
    case arf_ustar:
      return 0;

    default:
      break;
    }
  return 1;
}