static void sm501_init(SM501State *s, DeviceState *dev,
                       uint32_t local_mem_bytes)
{
    s->local_mem_size_index = get_local_mem_size_index(local_mem_bytes);
    SM501_DPRINTF("sm501 local mem size=%x. index=%d\n", get_local_mem_size(s),
                  s->local_mem_size_index);

    /* local memory */
    memory_region_init_ram(&s->local_mem_region, OBJECT(dev), "sm501.local",
                           get_local_mem_size(s), &error_fatal);
    memory_region_set_log(&s->local_mem_region, true, DIRTY_MEMORY_VGA);
    s->local_mem = memory_region_get_ram_ptr(&s->local_mem_region);

    /* i2c */
    s->i2c_bus = i2c_init_bus(dev, "sm501.i2c");
    /* ddc */
    I2CDDCState *ddc = I2CDDC(qdev_create(BUS(s->i2c_bus), TYPE_I2CDDC));
    i2c_set_slave_address(I2C_SLAVE(ddc), 0x50);

    /* mmio */
    memory_region_init(&s->mmio_region, OBJECT(dev), "sm501.mmio", MMIO_SIZE);
    memory_region_init_io(&s->system_config_region, OBJECT(dev),
                          &sm501_system_config_ops, s,
                          "sm501-system-config", 0x6c);
    memory_region_add_subregion(&s->mmio_region, SM501_SYS_CONFIG,
                                &s->system_config_region);
    memory_region_init_io(&s->i2c_region, OBJECT(dev), &sm501_i2c_ops, s,
                          "sm501-i2c", 0x14);
    memory_region_add_subregion(&s->mmio_region, SM501_I2C, &s->i2c_region);
    memory_region_init_io(&s->disp_ctrl_region, OBJECT(dev),
                          &sm501_disp_ctrl_ops, s,
                          "sm501-disp-ctrl", 0x1000);
    memory_region_add_subregion(&s->mmio_region, SM501_DC,
                                &s->disp_ctrl_region);
    memory_region_init_io(&s->twoD_engine_region, OBJECT(dev),
                          &sm501_2d_engine_ops, s,
                          "sm501-2d-engine", 0x54);
    memory_region_add_subregion(&s->mmio_region, SM501_2D_ENGINE,
                                &s->twoD_engine_region);

    /* create qemu graphic console */
    s->con = graphic_console_init(dev, 0, &sm501_ops, s);
}