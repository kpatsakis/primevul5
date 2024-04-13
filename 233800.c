translate_valuator_class (Display             *xdisplay,
                          ClutterInputDevice  *device,
                          XIValuatorClassInfo *class)
{
  static gboolean atoms_initialized = FALSE;
  ClutterInputAxis i, axis = CLUTTER_INPUT_AXIS_IGNORE;

  if (G_UNLIKELY (!atoms_initialized))
    {
      XInternAtoms (xdisplay,
                    (char **) clutter_input_axis_atom_names, N_AXIS_ATOMS,
                    False,
                    clutter_input_axis_atoms);

      atoms_initialized = TRUE;
    }

  for (i = 0;
       i < N_AXIS_ATOMS;
       i += 1)
    {
      if (clutter_input_axis_atoms[i] == class->label)
        {
          axis = i + 1;
          break;
        }
    }

  _clutter_input_device_add_axis (device, axis,
                                  class->min,
                                  class->max,
                                  class->resolution);

  CLUTTER_NOTE (BACKEND,
                "Added axis '%s' (min:%.2f, max:%.2fd, res:%d) of device %d",
                clutter_input_axis_atom_names[axis],
                class->min,
                class->max,
                class->resolution,
                device->id);
}