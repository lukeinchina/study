#ifndef DL_CNN_UTIL_H_
#define DL_CNN_UTIL_H_

#include "types.h"

inline double relu_activator(double x) {
    return x > 0 ? x : 0;
}

inline double relu_derivator(double x) {
    return x > 0 ? 1 : 0;
}

/* 根据步长和填充宽度 计算输出大小 */
size_t calc_output_size(size_t ninput, size_t nflt, size_t npadding, size_t nstride);


double ** create_2d_array(size_t row, size_t col);
double ** destroy_2d_array(double **array, size_t row, size_t col);

/* 第1维是最低维 */
double ***create_3d_array(size_t dim3, size_t dim2, size_t dim1);
double ***destroy_3d_array(double ***array, size_t dim3, size_t dim2, size_t dim1);

/* 设置随机权重在[-0.001, +0.001] 之间。 硬编码 */
void set_random_value(double **array, size_t row, size_t col);
void set_random_value(Matrix2D m);
void set_random_value(Matrix3D m);

/* m1, m2 中每个元素相加，写入m. */
void matrix_add(Matrix2D m1, Matrix2D m2, Matrix2D m);
void matrix_add(Matrix3D m1, Matrix3D m2, Matrix3D m);
void matrix_elem_wise(Matrix3D m, double (*op)(double));
double matrix_elem_sum(Matrix2D m);

void print_2d_array(double **array, size_t row, size_t col);

#endif
