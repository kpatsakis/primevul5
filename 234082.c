inode_val_compare (const void *val1, const void *val2)
{
  const struct inode_val *ival1 = val1;
  const struct inode_val *ival2 = val2;
  return ival1->inode == ival2->inode
         && ival1->major_num == ival2->major_num
         && ival1->minor_num == ival2->minor_num;
}