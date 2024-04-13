int kvm_io_bus_unregister_dev(struct kvm *kvm, enum kvm_bus bus_idx,
			      struct kvm_io_device *dev)
{
	int i, j;
	struct kvm_io_bus *new_bus, *bus;

	lockdep_assert_held(&kvm->slots_lock);

	bus = kvm_get_bus(kvm, bus_idx);
	if (!bus)
		return 0;

	for (i = 0; i < bus->dev_count; i++) {
		if (bus->range[i].dev == dev) {
			break;
		}
	}

	if (i == bus->dev_count)
		return 0;

	new_bus = kmalloc(struct_size(bus, range, bus->dev_count - 1),
			  GFP_KERNEL_ACCOUNT);
	if (new_bus) {
		memcpy(new_bus, bus, struct_size(bus, range, i));
		new_bus->dev_count--;
		memcpy(new_bus->range + i, bus->range + i + 1,
				flex_array_size(new_bus, range, new_bus->dev_count - i));
	}

	rcu_assign_pointer(kvm->buses[bus_idx], new_bus);
	synchronize_srcu_expedited(&kvm->srcu);

	/* Destroy the old bus _after_ installing the (null) bus. */
	if (!new_bus) {
		pr_err("kvm: failed to shrink bus, removing it completely\n");
		for (j = 0; j < bus->dev_count; j++) {
			if (j == i)
				continue;
			kvm_iodevice_destructor(bus->range[j].dev);
		}
	}

	kfree(bus);
	return new_bus ? 0 : -ENOMEM;
}