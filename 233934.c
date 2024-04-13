int ssh_bind_accept(ssh_bind sshbind, ssh_session session) {
  socket_t fd = SSH_INVALID_SOCKET;
  int rc;
  if (sshbind->bindfd == SSH_INVALID_SOCKET) {
    ssh_set_error(sshbind, SSH_FATAL,
        "Can't accept new clients on a not bound socket.");
    return SSH_ERROR;
  }

  if (session == NULL){
      ssh_set_error(sshbind, SSH_FATAL,"session is null");
      return SSH_ERROR;
  }

  fd = accept(sshbind->bindfd, NULL, NULL);
  if (fd == SSH_INVALID_SOCKET) {
    ssh_set_error(sshbind, SSH_FATAL,
        "Accepting a new connection: %s",
        strerror(errno));
    return SSH_ERROR;
  }
  rc = ssh_bind_accept_fd(sshbind, session, fd);

  if(rc == SSH_ERROR){
#ifdef _WIN32
      closesocket(fd);
#else
      close(fd);
#endif
      ssh_socket_free(session->socket);
  }
  return rc;
}