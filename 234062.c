find_inode_file (ino_t node_num, unsigned long major_num,
		 unsigned long minor_num)
{
  struct inode_val *ival = find_inode_val (node_num, major_num, minor_num);
  return ival ? ival->file_name : NULL;
}