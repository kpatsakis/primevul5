void ssh_bind_set_blocking(ssh_bind sshbind, int blocking) {
  sshbind->blocking = blocking ? 1 : 0;
}