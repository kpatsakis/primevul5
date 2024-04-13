sftp_attributes sftp_parse_attr(sftp_session session, ssh_buffer buf,
    int expectname) {
  switch(session->version) {
    case 4:
      return sftp_parse_attr_4(session, buf, expectname);
    case 3:
    case 2:
    case 1:
    case 0:
      return sftp_parse_attr_3(session, buf, expectname);
    default:
      ssh_set_error(session->session, SSH_FATAL,
          "Version %d unsupported by client", session->server_version);
      return NULL;
  }

  return NULL;
}