add_inode (ino_t node_num, char *file_name, unsigned long major_num,
	   unsigned long minor_num)
{
  struct inode_val *temp;
  struct inode_val *e = NULL;
  
  /* Create new inode record.  */
  temp = (struct inode_val *) xmalloc (sizeof (struct inode_val));
  temp->inode = node_num;
  temp->major_num = major_num;
  temp->minor_num = minor_num;
  temp->file_name = file_name ? xstrdup (file_name) : NULL;

  if (renumber_inodes_option)
    temp->trans_inode = next_inode++;
  else
    temp->trans_inode = temp->inode;

  if (!((hash_table
	 || (hash_table = hash_initialize (0, 0, inode_val_hasher,
					   inode_val_compare, 0)))
	&& (e = hash_insert (hash_table, temp))))
    xalloc_die ();
  return e;
}