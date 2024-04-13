static inline int bcf1_sync_id(bcf1_t *line, kstring_t *str)
{
    // single typed string
    if ( line->d.id && strcmp(line->d.id, ".") ) {
        return bcf_enc_vchar(str, strlen(line->d.id), line->d.id);
    } else {
        return bcf_enc_size(str, 0, BCF_BT_CHAR);
    }
}