static inline uint32_t sftp_get_new_id(sftp_session session) {
  return ++session->id_counter;
}