void sftp_packet_free(sftp_packet packet) {
  if (packet == NULL) {
    return;
  }

  ssh_buffer_free(packet->payload);
  free(packet);
}