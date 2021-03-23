#ifndef DL_LSTM_TYPES_H_
#define DL_LSTM_TYPES_H_

/* 
 * 第1维是最低维
 */
struct Matrix2D {
    size_t dim2;
    size_t dim1;
    double **array;
};

/* 
 * 第1维是最低维，第3维是最高维 
 */
struct Matrix3D{
    size_t dim3;
    size_t dim2;
    size_t dim1;
    double ***array;
};

typedef double (*actvr_func)(double);

#endif
