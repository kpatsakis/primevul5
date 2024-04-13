tcp_option_len_check(proto_item* length_item, packet_info *pinfo, guint len, guint optlen)
{
    if (len != optlen) {
        /* Bogus - option length isn't what it's supposed to be for this option. */
        expert_add_info_format(pinfo, length_item, &ei_tcp_opt_len_invalid,
                               "option length should be %u", optlen);
        return FALSE;
    }

    return TRUE;
}