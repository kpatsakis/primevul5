lr_get_best_checksum(const LrMetalink *metalink,
                     GSList **checksums)
{
    gboolean ret;
    LrChecksumType ch_type;
    gchar *ch_value;

    // From the metalink itself
    ret = lr_best_checksum(metalink->hashes, &ch_type, &ch_value);
    if (ret)
    {
        LrDownloadTargetChecksum *dtch;
        dtch = lr_downloadtargetchecksum_new(ch_type, ch_value);
        *checksums = g_slist_prepend(*checksums, dtch);
        g_debug("%s: Expected checksum for repomd.xml: (%s) %s",
                __func__, lr_checksum_type_to_str(ch_type), ch_value);
    }

    // From the alternates entries
    for (GSList *elem = metalink->alternates; elem; elem = g_slist_next(elem))
    {
        LrMetalinkAlternate *alt = elem->data;
        ret = lr_best_checksum(alt->hashes, &ch_type, &ch_value);
        if (ret) {
            LrDownloadTargetChecksum *dtch;
            dtch = lr_downloadtargetchecksum_new(ch_type, ch_value);
            *checksums = g_slist_prepend(*checksums, dtch);
            g_debug("%s: Expected alternate checksum for repomd.xml: (%s) %s",
                    __func__, lr_checksum_type_to_str(ch_type), ch_value);
        }
    }
}