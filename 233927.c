ssh_bind ssh_bind_new(void) {
  ssh_bind ptr;

  ptr = malloc(sizeof(struct ssh_bind_struct));
  if (ptr == NULL) {
    return NULL;
  }
  ZERO_STRUCTP(ptr);
  ptr->bindfd = SSH_INVALID_SOCKET;
  ptr->bindport= 22;
  ptr->common.log_verbosity = 0;

  return ptr;
}