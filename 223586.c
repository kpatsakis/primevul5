static void sm501_i2c_write(void *opaque, hwaddr addr, uint64_t value,
                            unsigned size)
{
    SM501State *s = (SM501State *)opaque;
    SM501_DPRINTF("sm501 i2c regs : write addr=%" HWADDR_PRIx
                  " val=%" PRIx64 "\n", addr, value);

    switch (addr) {
    case SM501_I2C_BYTE_COUNT:
        s->i2c_byte_count = value & 0xf;
        break;
    case SM501_I2C_CONTROL:
        if (value & SM501_I2C_CONTROL_ENABLE) {
            if (value & SM501_I2C_CONTROL_START) {
                int res = i2c_start_transfer(s->i2c_bus,
                                             s->i2c_addr >> 1,
                                             s->i2c_addr & 1);
                s->i2c_status |= (res ? SM501_I2C_STATUS_ERROR : 0);
                if (!res) {
                    int i;
                    SM501_DPRINTF("sm501 i2c : transferring %d bytes to 0x%x\n",
                                  s->i2c_byte_count + 1, s->i2c_addr >> 1);
                    for (i = 0; i <= s->i2c_byte_count; i++) {
                        res = i2c_send_recv(s->i2c_bus, &s->i2c_data[i],
                                            !(s->i2c_addr & 1));
                        if (res) {
                            SM501_DPRINTF("sm501 i2c : transfer failed"
                                          " i=%d, res=%d\n", i, res);
                            s->i2c_status |= SM501_I2C_STATUS_ERROR;
                            return;
                        }
                    }
                    if (i) {
                        SM501_DPRINTF("sm501 i2c : transferred %d bytes\n", i);
                        s->i2c_status = SM501_I2C_STATUS_COMPLETE;
                    }
                }
            } else {
                SM501_DPRINTF("sm501 i2c : end transfer\n");
                i2c_end_transfer(s->i2c_bus);
                s->i2c_status &= ~SM501_I2C_STATUS_ERROR;
            }
        }
        break;
    case SM501_I2C_RESET:
        if ((value & SM501_I2C_RESET_ERROR) == 0) {
            s->i2c_status &= ~SM501_I2C_STATUS_ERROR;
        }
        break;
    case SM501_I2C_SLAVE_ADDRESS:
        s->i2c_addr = value & 0xff;
        break;
    case SM501_I2C_DATA ... SM501_I2C_DATA + 15:
        s->i2c_data[addr - SM501_I2C_DATA] = value & 0xff;
        break;
    default:
        qemu_log_mask(LOG_UNIMP, "sm501 i2c : not implemented register write. "
                      "addr=0x%" HWADDR_PRIx " val=%" PRIx64 "\n", addr, value);
    }
}