static uint64_t sm501_system_config_read(void *opaque, hwaddr addr,
                                         unsigned size)
{
    SM501State *s = (SM501State *)opaque;
    uint32_t ret = 0;
    SM501_DPRINTF("sm501 system config regs : read addr=%x\n", (int)addr);

    switch (addr) {
    case SM501_SYSTEM_CONTROL:
        ret = s->system_control;
        break;
    case SM501_MISC_CONTROL:
        ret = s->misc_control;
        break;
    case SM501_GPIO31_0_CONTROL:
        ret = s->gpio_31_0_control;
        break;
    case SM501_GPIO63_32_CONTROL:
        ret = s->gpio_63_32_control;
        break;
    case SM501_DEVICEID:
        ret = 0x050100A0;
        break;
    case SM501_DRAM_CONTROL:
        ret = (s->dram_control & 0x07F107C0) | s->local_mem_size_index << 13;
        break;
    case SM501_ARBTRTN_CONTROL:
        ret = s->arbitration_control;
        break;
    case SM501_COMMAND_LIST_STATUS:
        ret = 0x00180002; /* FIFOs are empty, everything idle */
        break;
    case SM501_IRQ_MASK:
        ret = s->irq_mask;
        break;
    case SM501_MISC_TIMING:
        /* TODO : simulate gate control */
        ret = s->misc_timing;
        break;
    case SM501_CURRENT_GATE:
        /* TODO : simulate gate control */
        ret = 0x00021807;
        break;
    case SM501_CURRENT_CLOCK:
        ret = 0x2A1A0A09;
        break;
    case SM501_POWER_MODE_CONTROL:
        ret = s->power_mode_control;
        break;
    case SM501_ENDIAN_CONTROL:
        ret = 0; /* Only default little endian mode is supported */
        break;

    default:
        qemu_log_mask(LOG_UNIMP, "sm501: not implemented system config"
                      "register read. addr=%" HWADDR_PRIx "\n", addr);
    }

    return ret;
}