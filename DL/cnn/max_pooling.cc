#include <stdlib.h>

#include "util.h"
#include "max_pooling.h"

struct Pair {
    int i;
    int j;
};

int 
mp_create(MaxPooling *mp) {
    mp->output_width = (mp->input_width - mp->flt_width) / mp->stride + 1;
    mp->output_height = (mp->input_height - mp->flt_height) / mp->stride + 1;

    mp->feature_map.dim3 = mp->nchnl;
    mp->feature_map.dim2 = mp->output_height;
    mp->feature_map.dim1 = mp->output_width;
    mp->feature_map.array = create_3d_array(mp->nchnl, mp->output_height, mp->output_width);

    mp->input_delta.dim3 = mp->nchnl;
    mp->input_delta.dim2 = mp->input_height;
    mp->input_delta.dim1 = mp->input_width;
    mp->input_delta.array = create_3d_array(mp->nchnl, mp->input_height, mp->input_width);

    return 0;
}

void
mp_destroy(MaxPooling *mp) {
    if (NULL != mp->input_delta.array) {
        mp->input_delta.array = destroy_3d_array(mp->input_delta.array,
                mp->input_delta.dim3, mp->input_delta.dim2, mp->input_delta.dim1);
    }
    if (NULL != mp->feature_map.array) {
        mp->feature_map.array = destroy_3d_array(mp->feature_map.array,
                mp->feature_map.dim3, mp->feature_map.dim2, mp->feature_map.dim1);
    }
}

static double
get_matrix_area_max(double **array, size_t roff, size_t coff,
        size_t rwidth, size_t cwidth) {
    double max = array[roff][coff];
    for (size_t i = roff; i < roff + rwidth; i++) {
        for (size_t j = coff; j < coff + cwidth; j++) {
            if (array[i][j] > max) {
                max = array[i][j];
            }
        }
    }
    return max;
}

void
mp_forward(MaxPooling *mp, const Matrix3D &input) {
    for (size_t d = 0; d < mp->nchnl; d++) {
        for (size_t i = 0; i < mp->output_height; i++) {
            for (size_t j = 0; j < mp->output_width; j++) {
                size_t start_i = i * mp->stride;
                size_t start_j = j * mp->stride;
                mp->feature_map.array[d][i][j] = get_matrix_area_max(
                    input.array[d], start_i, start_j, mp->flt_height,
                    mp->flt_width);
            }
        }
    }
}

Pair
get_max_elem_pos(double **array, size_t roff, size_t coff,
        size_t rwidth, size_t cwidth) {
    double max = array[roff][coff];
    Pair pos = {roff, coff};
    for (size_t i = roff; i < roff + rwidth; i++) {
        for (size_t j = coff; j < coff + cwidth; j++) {
            if (array[i][j] > max) {
                max = array[i][j];
                pos.i = i;
                pos.j = j;
            }
        }
    }
    return pos;
}

void
mp_backward(MaxPooling *mp, const Matrix3D &input, const Matrix3D &sm) {
    matrix_set(mp->input_delta, 0.0);

    for (size_t d = 0; d < mp->nchnl; d++) {
        for (size_t i = 0; i < mp->output_height; i++) {
            for (size_t j = 0; j < mp->output_width; j++) {
                size_t start_i = i * mp->stride;
                size_t start_j = j * mp->stride;
                Pair pos = get_max_elem_pos(input.array[d], start_i, start_j,
                               mp->flt_height, mp->flt_width);
                mp->input_delta.array[d][pos.i][pos.j] = sm.array[d][i][j];
            }
        }
    }
}
