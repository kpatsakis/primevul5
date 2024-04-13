virSecuritySELinuxSetHostdevSubsysLabel(virSecurityManager *mgr,
                                        virDomainDef *def,
                                        virDomainHostdevDef *dev,
                                        const char *vroot)

{
    virDomainHostdevSubsysUSB *usbsrc = &dev->source.subsys.u.usb;
    virDomainHostdevSubsysPCI *pcisrc = &dev->source.subsys.u.pci;
    virDomainHostdevSubsysSCSI *scsisrc = &dev->source.subsys.u.scsi;
    virDomainHostdevSubsysSCSIVHost *hostsrc = &dev->source.subsys.u.scsi_host;
    virDomainHostdevSubsysMediatedDev *mdevsrc = &dev->source.subsys.u.mdev;
    virSecuritySELinuxCallbackData data = {.mgr = mgr, .def = def};

    int ret = -1;

    /* Like virSecuritySELinuxSetImageLabelInternal() for a networked
     * disk, do nothing for an iSCSI hostdev
     */
    if (dev->source.subsys.type == VIR_DOMAIN_HOSTDEV_SUBSYS_TYPE_SCSI &&
        scsisrc->protocol == VIR_DOMAIN_HOSTDEV_SCSI_PROTOCOL_TYPE_ISCSI)
        return 0;

    switch ((virDomainHostdevSubsysType)dev->source.subsys.type) {
    case VIR_DOMAIN_HOSTDEV_SUBSYS_TYPE_USB: {
        g_autoptr(virUSBDevice) usb = NULL;

        if (dev->missing)
            return 0;

        usb = virUSBDeviceNew(usbsrc->bus,
                              usbsrc->device,
                              vroot);
        if (!usb)
            return -1;

        ret = virUSBDeviceFileIterate(usb, virSecuritySELinuxSetUSBLabel, &data);
        break;
    }

    case VIR_DOMAIN_HOSTDEV_SUBSYS_TYPE_PCI: {
        g_autoptr(virPCIDevice) pci = NULL;

        if (!virPCIDeviceExists(&pcisrc->addr))
            break;

        pci = virPCIDeviceNew(&pcisrc->addr);

        if (!pci)
            return -1;

        if (pcisrc->backend == VIR_DOMAIN_HOSTDEV_PCI_BACKEND_VFIO) {
            g_autofree char *vfioGroupDev = virPCIDeviceGetIOMMUGroupDev(pci);

            if (!vfioGroupDev)
                return -1;

            ret = virSecuritySELinuxSetHostdevLabelHelper(vfioGroupDev,
                                                          false,
                                                          &data);
        } else {
            ret = virPCIDeviceFileIterate(pci, virSecuritySELinuxSetPCILabel, &data);
        }
        break;
    }

    case VIR_DOMAIN_HOSTDEV_SUBSYS_TYPE_SCSI: {
        virDomainHostdevSubsysSCSIHost *scsihostsrc = &scsisrc->u.host;

        g_autoptr(virSCSIDevice) scsi =
            virSCSIDeviceNew(NULL,
                             scsihostsrc->adapter, scsihostsrc->bus,
                             scsihostsrc->target, scsihostsrc->unit,
                             dev->readonly, dev->shareable);

        if (!scsi)
            return -1;

        ret = virSCSIDeviceFileIterate(scsi,
                                       virSecuritySELinuxSetSCSILabel,
                                       &data);
        break;
    }

    case VIR_DOMAIN_HOSTDEV_SUBSYS_TYPE_SCSI_HOST: {
        g_autoptr(virSCSIVHostDevice) host = virSCSIVHostDeviceNew(hostsrc->wwpn);

        if (!host)
            return -1;

        ret = virSCSIVHostDeviceFileIterate(host,
                                            virSecuritySELinuxSetHostLabel,
                                            &data);
        break;
    }

    case VIR_DOMAIN_HOSTDEV_SUBSYS_TYPE_MDEV: {
        g_autofree char *vfiodev = NULL;

        if (!(vfiodev = virMediatedDeviceGetIOMMUGroupDev(mdevsrc->uuidstr)))
            return ret;

        ret = virSecuritySELinuxSetHostdevLabelHelper(vfiodev, true, &data);
        break;
    }

    case VIR_DOMAIN_HOSTDEV_SUBSYS_TYPE_LAST:
        ret = 0;
        break;
    }

    return ret;
}