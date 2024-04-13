static void piix_ide_register_types(void)
{
    type_register_static(&piix3_ide_info);
    type_register_static(&piix3_ide_xen_info);
    type_register_static(&piix4_ide_info);
}