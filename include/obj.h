#ifndef OBJ_H
#define OBJ_H

#include "vec.h"

typedef struct obj_t obj_t;

typedef enum {
    INT,
    FLOAT,
    STRING,
    TUPLE,
    VECTOR,
} obj_type_t;

typedef struct {
    obj_t *x;
    obj_t *y;
} tuple_t;

typedef union {
    int v_int;
    float v_float;
    char *v_str;
    tuple_t v_tuple;
    vec_t *v_vec;
} obj_data_t;

struct obj_t {
    obj_data_t data;
    obj_type_t type;
    size_t ref_count;
};

/* Generates function prototypes for basic object types. */
#define OBJ_CREATE_FUNC(type)                                           \
    /* Allocate and return a new `type` `obj_t`, or NULL on failure. */ \
    obj_t *obj_create_##type(type data);

OBJ_CREATE_FUNC(int)
OBJ_CREATE_FUNC(float)

/* Allocate and return a new STRING `obj_t`, or NULL on failure. */
obj_t *obj_create_string(const char *data);

/* Allocate and return a new TUPLE `obj_t`, or NULL on failure. */
obj_t *obj_create_tuple(obj_t *restrict x, obj_t *restrict y);

/* Allocate and return a new VECTOR `obj_t`, or NULL on failure. */
obj_t *obj_create_vec(size_t capacity);

/* Free the `obj_t` instance and all associated memory. */
void obj_destroy(obj_t *obj);

/* Print the contents of the given `obj_t` to stdout. */
void obj_debug_print(const obj_t *obj);

/* Increment `ref_count` of `obj`. */
void obj_inc_ref(obj_t *obj);

/* Decrement `ref_count` of `obj`. If `ref_count` becomes 0, `obj` is freed. */
void obj_dec_ref(obj_t *obj);

/* Wrapper functions for vector operations on `obj_t` values.  */
obj_t *obj_vec_t_at(const vec_t *vec, size_t pos);
obj_t *obj_vec_t_front(const vec_t *vec);
obj_t *obj_vec_t_back(const vec_t *vec);
int obj_vec_t_insert(vec_t *vec, size_t pos, obj_t *value);
obj_t *obj_vec_t_remove(vec_t *vec, size_t pos);
int obj_vec_t_push_back(vec_t *vec, obj_t *value);
obj_t *obj_vec_t_pop_back(vec_t *vec);

#endif  // OBJ_H
