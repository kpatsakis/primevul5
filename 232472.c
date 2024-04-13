static int hidinput_get_battery_property(struct power_supply *psy,
					 enum power_supply_property prop,
					 union power_supply_propval *val)
{
	struct hid_device *dev = power_supply_get_drvdata(psy);
	int value;
	int ret = 0;

	switch (prop) {
	case POWER_SUPPLY_PROP_PRESENT:
	case POWER_SUPPLY_PROP_ONLINE:
		val->intval = 1;
		break;

	case POWER_SUPPLY_PROP_CAPACITY:
		if (dev->battery_status != HID_BATTERY_REPORTED &&
		    !dev->battery_avoid_query) {
			value = hidinput_query_battery_capacity(dev);
			if (value < 0)
				return value;
		} else  {
			value = dev->battery_capacity;
		}

		val->intval = value;
		break;

	case POWER_SUPPLY_PROP_MODEL_NAME:
		val->strval = dev->name;
		break;

	case POWER_SUPPLY_PROP_STATUS:
		if (dev->battery_status != HID_BATTERY_REPORTED &&
		    !dev->battery_avoid_query) {
			value = hidinput_query_battery_capacity(dev);
			if (value < 0)
				return value;

			dev->battery_capacity = value;
			dev->battery_status = HID_BATTERY_QUERIED;
		}

		if (dev->battery_status == HID_BATTERY_UNKNOWN)
			val->intval = POWER_SUPPLY_STATUS_UNKNOWN;
		else if (dev->battery_capacity == 100)
			val->intval = POWER_SUPPLY_STATUS_FULL;
		else
			val->intval = POWER_SUPPLY_STATUS_DISCHARGING;
		break;

	case POWER_SUPPLY_PROP_SCOPE:
		val->intval = POWER_SUPPLY_SCOPE_DEVICE;
		break;

	default:
		ret = -EINVAL;
		break;
	}

	return ret;
}