static uint64_t sm501_i2c_read(void *opaque, hwaddr addr, unsigned size)
{
    SM501State *s = (SM501State *)opaque;
    uint8_t ret = 0;

    switch (addr) {
    case SM501_I2C_BYTE_COUNT:
        ret = s->i2c_byte_count;
        break;
    case SM501_I2C_STATUS:
        ret = s->i2c_status;
        break;
    case SM501_I2C_SLAVE_ADDRESS:
        ret = s->i2c_addr;
        break;
    case SM501_I2C_DATA ... SM501_I2C_DATA + 15:
        ret = s->i2c_data[addr - SM501_I2C_DATA];
        break;
    default:
        qemu_log_mask(LOG_UNIMP, "sm501 i2c : not implemented register read."
                      " addr=0x%" HWADDR_PRIx "\n", addr);
    }

    SM501_DPRINTF("sm501 i2c regs : read addr=%" HWADDR_PRIx " val=%x\n",
                  addr, ret);
    return ret;
}