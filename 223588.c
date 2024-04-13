static void sm501_system_config_write(void *opaque, hwaddr addr,
                                      uint64_t value, unsigned size)
{
    SM501State *s = (SM501State *)opaque;
    SM501_DPRINTF("sm501 system config regs : write addr=%x, val=%x\n",
                  (uint32_t)addr, (uint32_t)value);

    switch (addr) {
    case SM501_SYSTEM_CONTROL:
        s->system_control &= 0x10DB0000;
        s->system_control |= value & 0xEF00B8F7;
        break;
    case SM501_MISC_CONTROL:
        s->misc_control &= 0xEF;
        s->misc_control |= value & 0xFF7FFF10;
        break;
    case SM501_GPIO31_0_CONTROL:
        s->gpio_31_0_control = value;
        break;
    case SM501_GPIO63_32_CONTROL:
        s->gpio_63_32_control = value & 0xFF80FFFF;
        break;
    case SM501_DRAM_CONTROL:
        s->local_mem_size_index = (value >> 13) & 0x7;
        /* TODO : check validity of size change */
        s->dram_control &= 0x80000000;
        s->dram_control |= value & 0x7FFFFFC3;
        break;
    case SM501_ARBTRTN_CONTROL:
        s->arbitration_control = value & 0x37777777;
        break;
    case SM501_IRQ_MASK:
        s->irq_mask = value & 0xFFDF3F5F;
        break;
    case SM501_MISC_TIMING:
        s->misc_timing = value & 0xF31F1FFF;
        break;
    case SM501_POWER_MODE_0_GATE:
    case SM501_POWER_MODE_1_GATE:
    case SM501_POWER_MODE_0_CLOCK:
    case SM501_POWER_MODE_1_CLOCK:
        /* TODO : simulate gate & clock control */
        break;
    case SM501_POWER_MODE_CONTROL:
        s->power_mode_control = value & 0x00000003;
        break;
    case SM501_ENDIAN_CONTROL:
        if (value & 0x00000001) {
            qemu_log_mask(LOG_UNIMP, "sm501: system config big endian mode not"
                          " implemented.\n");
        }
        break;

    default:
        qemu_log_mask(LOG_UNIMP, "sm501: not implemented system config"
                      "register write. addr=%" HWADDR_PRIx
                      ", val=%" PRIx64 "\n", addr, value);
    }
}