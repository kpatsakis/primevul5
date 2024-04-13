link_to_maj_min_ino (char *file_name, int st_dev_maj, int st_dev_min,
		     ino_t st_ino)
{
  int	link_res;
  char *link_name;
  link_res = -1;
  /* Is the file a link to a previously copied file?  */
  link_name = find_inode_file (st_ino,
			       st_dev_maj,
			       st_dev_min);
  if (link_name == NULL)
    add_inode (st_ino, file_name,
	       st_dev_maj,
	       st_dev_min);
  else
    link_res = link_to_name (file_name, link_name);
  return link_res;
}