static inline void printMatrix(cairo_matrix_t *matrix){
	printf("%f %f, %f %f (%f %f)\n", matrix->xx, matrix->yx,
			matrix->xy, matrix->yy,
			matrix->x0, matrix->y0);
}
