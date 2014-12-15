#pragma once
#include <stdlib.h>
#include "hash_func.h"

template<typename ElemType>
class DHash
{
    public:
        DHash(long init_size = 16):_ratio(3.0) {
            _curr_table_size = init_size < 16 ? 16 : init_size;

            _list_size = 1024; /* init table size */
            _table_list = (ElemType **)calloc(sizeof(ElemType *) * _list_size);
            assert(NULL != _table_list);

            /* first hash talbe */
            _table_list[0] = (ElemType *)malloc(_init_size * sizeof(ElemType));

            _elem_count = 0;
            _expanding  = false;
        }
        ~DHash(void);
        int Insert(const ElemType &e);
        int Delete(const ElemType &e);
        void Clear(void);
    private:
        void Destroy(void);
    private:
        long  _elem_count;
        const double _ratio;

        long _curr_table_size;

        ElemType **_table_list;
        long _list_size;

        bool _expanding;
};

