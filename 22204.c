static bool sungem_rx_full(SunGEMState *s, uint32_t kick, uint32_t done)
{
    return kick == ((done + 1) & s->rx_mask);
}