SPICE_GNUC_VISIBLE int spice_server_set_agent_copypaste(SpiceServer *s, int enable)
{
    spice_assert(reds == s);
    agent_copypaste = enable;
    reds->agent_state.write_filter.copy_paste_enabled = agent_copypaste;
    reds->agent_state.read_filter.copy_paste_enabled = agent_copypaste;
    return 0;
}
