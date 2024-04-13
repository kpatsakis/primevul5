static inline void rx_desc_set_sar(uint32_t *desc, int sar_idx)
{
    desc[1] = deposit32(desc[1], R_DESC_1_RX_SAR_SHIFT, R_DESC_1_RX_SAR_LENGTH,
                        sar_idx);
    desc[1] |= R_DESC_1_RX_SAR_MATCH;
}