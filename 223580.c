static void sm501_reset(SM501State *s)
{
    s->system_control = 0x00100000; /* 2D engine FIFO empty */
    /* Bits 17 (SH), 7 (CDR), 6:5 (Test), 2:0 (Bus) are all supposed
     * to be determined at reset by GPIO lines which set config bits.
     * We hardwire them:
     *  SH = 0 : Hitachi Ready Polarity == Active Low
     *  CDR = 0 : do not reset clock divider
     *  TEST = 0 : Normal mode (not testing the silicon)
     *  BUS = 0 : Hitachi SH3/SH4
     */
    s->misc_control = SM501_MISC_DAC_POWER;
    s->gpio_31_0_control = 0;
    s->gpio_63_32_control = 0;
    s->dram_control = 0;
    s->arbitration_control = 0x05146732;
    s->irq_mask = 0;
    s->misc_timing = 0;
    s->power_mode_control = 0;
    s->i2c_byte_count = 0;
    s->i2c_status = 0;
    s->i2c_addr = 0;
    memset(s->i2c_data, 0, 16);
    s->dc_panel_control = 0x00010000; /* FIFO level 3 */
    s->dc_video_control = 0;
    s->dc_crt_control = 0x00010000;
    s->twoD_source = 0;
    s->twoD_destination = 0;
    s->twoD_dimension = 0;
    s->twoD_control = 0;
    s->twoD_pitch = 0;
    s->twoD_foreground = 0;
    s->twoD_background = 0;
    s->twoD_stretch = 0;
    s->twoD_color_compare = 0;
    s->twoD_color_compare_mask = 0;
    s->twoD_mask = 0;
    s->twoD_clip_tl = 0;
    s->twoD_clip_br = 0;
    s->twoD_mono_pattern_low = 0;
    s->twoD_mono_pattern_high = 0;
    s->twoD_window_width = 0;
    s->twoD_source_base = 0;
    s->twoD_destination_base = 0;
    s->twoD_alpha = 0;
    s->twoD_wrap = 0;
}