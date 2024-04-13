static int reds_send_link_ack(RedLinkInfo *link)
{
    SpiceLinkHeader header;
    SpiceLinkReply ack;
    RedChannel *channel;
    RedChannelCapabilities *channel_caps;
    BUF_MEM *bmBuf;
    BIO *bio;
    int ret = FALSE;

    header.magic = SPICE_MAGIC;
    header.size = sizeof(ack);
    header.major_version = SPICE_VERSION_MAJOR;
    header.minor_version = SPICE_VERSION_MINOR;

    ack.error = SPICE_LINK_ERR_OK;

    channel = reds_find_channel(link->link_mess->channel_type, 0);
    if (!channel) {
        spice_assert(link->link_mess->channel_type == SPICE_CHANNEL_MAIN);
        spice_assert(reds->main_channel);
        channel = &reds->main_channel->base;
    }

    reds_channel_init_auth_caps(link, channel); /* make sure common caps are set */

    channel_caps = &channel->local_caps;
    ack.num_common_caps = channel_caps->num_common_caps;
    ack.num_channel_caps = channel_caps->num_caps;
    header.size += (ack.num_common_caps + ack.num_channel_caps) * sizeof(uint32_t);
    ack.caps_offset = sizeof(SpiceLinkReply);

    if (!(link->tiTicketing.rsa = RSA_new())) {
        spice_warning("RSA nes failed");
        return FALSE;
    }

    if (!(bio = BIO_new(BIO_s_mem()))) {
        spice_warning("BIO new failed");
        return FALSE;
    }

    RSA_generate_key_ex(link->tiTicketing.rsa, SPICE_TICKET_KEY_PAIR_LENGTH, link->tiTicketing.bn,
                        NULL);
    link->tiTicketing.rsa_size = RSA_size(link->tiTicketing.rsa);

    i2d_RSA_PUBKEY_bio(bio, link->tiTicketing.rsa);
    BIO_get_mem_ptr(bio, &bmBuf);
    memcpy(ack.pub_key, bmBuf->data, sizeof(ack.pub_key));

    if (!sync_write(link->stream, &header, sizeof(header)))
        goto end;
    if (!sync_write(link->stream, &ack, sizeof(ack)))
        goto end;
    if (!sync_write(link->stream, channel_caps->common_caps, channel_caps->num_common_caps * sizeof(uint32_t)))
        goto end;
    if (!sync_write(link->stream, channel_caps->caps, channel_caps->num_caps * sizeof(uint32_t)))
        goto end;

    ret = TRUE;

end:
    BIO_free(bio);
    return ret;
}
