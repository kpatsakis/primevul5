int ssh_bind_set_callbacks(ssh_bind sshbind, ssh_bind_callbacks callbacks,
    void *userdata){
  if (sshbind == NULL) {
    return SSH_ERROR;
  }
  if (callbacks == NULL) {
    ssh_set_error_invalid(sshbind);
    return SSH_ERROR;
  }
  if(callbacks->size <= 0 || callbacks->size > 1024 * sizeof(void *)){
    ssh_set_error(sshbind,SSH_FATAL,
        "Invalid callback passed in (badly initialized)");
    return SSH_ERROR;
  }
  sshbind->bind_callbacks = callbacks;
  sshbind->bind_callbacks_userdata=userdata;
  return 0;
}