void ssh_bind_set_fd(ssh_bind sshbind, socket_t fd) {
  sshbind->bindfd = fd;
}