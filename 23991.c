void sftp_attributes_free(sftp_attributes file){
  if (file == NULL) {
    return;
  }

  ssh_string_free(file->acl);
  ssh_string_free(file->extended_data);
  ssh_string_free(file->extended_type);

  SAFE_FREE(file->name);
  SAFE_FREE(file->longname);
  SAFE_FREE(file->group);
  SAFE_FREE(file->owner);

  SAFE_FREE(file);
}