static void NormalizeShapeForOutput(TensorShapeProto* shape) {
  for (int i = 0; i < shape->dim_size(); i++) {
    if (shape->dim(i).size() < -1) {
      VLOG(2) << "Normalizing dimension: " << i << " from "
              << shape->dim(i).size() << " to -1";
      shape->mutable_dim(i)->set_size(-1);
    }
  }
}