tcp_flags_to_str_first_letter(const struct tcpheader *tcph)
{
    wmem_strbuf_t *buf = wmem_strbuf_new(wmem_packet_scope(), "");
    unsigned i;
    const unsigned flags_count = 12;
    const char first_letters[] = "RRRNCEUAPRSF";

    /* upper three bytes are marked as reserved ('R'). */
    for (i = 0; i < flags_count; i++) {
        if (((tcph->th_flags >> (flags_count - 1 - i)) & 1)) {
            wmem_strbuf_append_c(buf, first_letters[i]);
        } else {
            wmem_strbuf_append(buf, UTF8_MIDDLE_DOT);
        }
    }

    return wmem_strbuf_finalize(buf);
}