find_inode_val (ino_t node_num, unsigned long major_num,
		 unsigned long minor_num)
{
  struct inode_val sample;
  
  if (!hash_table)
    return NULL;
  
  sample.inode = node_num;
  sample.major_num = major_num;
  sample.minor_num = minor_num;
  return hash_lookup (hash_table, &sample);
}