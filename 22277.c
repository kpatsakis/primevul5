static int cleanup_cb(void *rock,
                      const char *key, size_t keylen,
                      const char *data __attribute__((unused)),
                      size_t datalen __attribute__((unused)))
{
    annotate_db_t *d = (annotate_db_t *)rock;

    return cyrusdb_delete(d->db, key, keylen, tid(d), /*force*/1);
}