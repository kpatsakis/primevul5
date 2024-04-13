static void gem_init(Object *obj)
{
    CadenceGEMState *s = CADENCE_GEM(obj);
    DeviceState *dev = DEVICE(obj);

    DB_PRINT("\n");

    gem_init_register_masks(s);
    memory_region_init_io(&s->iomem, OBJECT(s), &gem_ops, s,
                          "enet", sizeof(s->regs));

    sysbus_init_mmio(SYS_BUS_DEVICE(dev), &s->iomem);

    object_property_add_link(obj, "dma", TYPE_MEMORY_REGION,
                             (Object **)&s->dma_mr,
                             qdev_prop_allow_set_link_before_realize,
                             OBJ_PROP_LINK_STRONG);
}