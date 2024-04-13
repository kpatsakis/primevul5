get_inode_and_dev (struct cpio_file_stat *hdr, struct stat *st)
{
  if (renumber_inodes_option)
    {
      if (st->st_nlink > 1)
	{
	  struct inode_val *ival = find_inode_val (st->st_ino,
						   major (st->st_dev),
						   minor (st->st_dev));
	  if (!ival)
	    ival = add_inode (st->st_ino, NULL,
			      major (st->st_dev), minor (st->st_dev));
	  hdr->c_ino = ival->trans_inode;
	}
      else
	hdr->c_ino = next_inode++;
    }
  else
    hdr->c_ino = st->st_ino;
  if (ignore_devno_option)
    {
      hdr->c_dev_maj = 0;
      hdr->c_dev_min = 0;
    }
  else
    {
      hdr->c_dev_maj = major (st->st_dev);
      hdr->c_dev_min = minor (st->st_dev);
    }
}