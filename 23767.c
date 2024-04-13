bmexec (kwset_t kwset, char const *text, size_t size)
{
  /* Help the compiler inline bmexec_trans in two ways, depending on
     whether kwset->trans is null.  */
  return (kwset->trans
          ? bmexec_trans (kwset, text, size)
          : bmexec_trans (kwset, text, size));
}