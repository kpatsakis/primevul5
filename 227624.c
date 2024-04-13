static void msf2_emac_init(Object *obj)
{
    MSF2EmacState *s = MSS_EMAC(obj);

    sysbus_init_irq(SYS_BUS_DEVICE(obj), &s->irq);

    memory_region_init_io(&s->mmio, obj, &emac_ops, s,
                          "msf2-emac", R_MAX * 4);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->mmio);
}