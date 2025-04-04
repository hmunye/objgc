#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "obj.h"
#include "vec.h"

static obj_t *internal_obj_t_create(void) {
    obj_t *obj = malloc(sizeof(obj_t));
    if (!obj) {
        return NULL;
    }

    return obj;
}

obj_t *obj_create_int(int data) {
    obj_t *obj = internal_obj_t_create();
    if (!obj) {
        return NULL;
    }

    obj->type = INT;
    obj->data.v_int = data;

    return obj;
}

obj_t *obj_create_float(float data) {
    obj_t *obj = internal_obj_t_create();
    if (!obj) {
        return NULL;
    }

    obj->type = FLOAT;
    obj->data.v_float = data;

    return obj;
}

obj_t *obj_create_string(const char *data) {
    if (!data) {
        return NULL;
    }

    obj_t *obj = internal_obj_t_create();
    if (!obj) {
        return NULL;
    }

    obj->type = STRING;

    /* Allocate enough memory for string and null terminator */
    obj->data.v_str = malloc(strlen(data) + 1);
    if (!obj->data.v_str) {
        free(obj);
        return NULL;
    }

    strcpy(obj->data.v_str, data);

    return obj;
}

obj_t *obj_create_tuple(obj_t *restrict x, obj_t *restrict y) {
    if (!x || !y) {
        return NULL;
    }

    obj_t *obj = internal_obj_t_create();
    if (!obj) {
        return NULL;
    }

    obj->type = TUPLE;
    obj->data.v_tuple.x = x;
    obj->data.v_tuple.y = y;

    return obj;
}

obj_t *obj_create_vec(size_t capacity) {
    if (capacity == 0) {
        return NULL;
    }

    obj_t *obj = internal_obj_t_create();
    if (!obj) {
        return NULL;
    }

    obj->type = VECTOR;
    if (!(obj->data.v_vec = vec_t_reserve(capacity))) {
        free(obj);
        return NULL;
    }

    return obj;
}

void obj_destroy(obj_t *obj) {
    if (!obj) {
        return;
    }

    switch (obj->type) {
        case INT:
        case FLOAT:
            break;
        case STRING:
            /* Free dynamically allocated string before freeing the
             * object */
            free(obj->data.v_str);
            obj->data.v_str = NULL;
            break;
        case TUPLE:
            obj_destroy(obj->data.v_tuple.x);
            obj->data.v_tuple.x = NULL;

            obj_destroy(obj->data.v_tuple.y);
            obj->data.v_tuple.y = NULL;
            break;
        case VECTOR: {
            vec_t *vec = obj->data.v_vec;

            if (!vec) {
                break;
            }

            if (vec->data) {
                for (size_t i = 0; i < vec->size; ++i) {
                    obj_t *elem = (obj_t *)(vec->data[i]);

                    /* If marked as removed (NULL), skip iteration */
                    if (!elem) {
                        continue;
                    }

                    switch (elem->type) {
                        case STRING:
                            free(elem->data.v_str);
                            obj->data.v_str = NULL;
                            break;
                        case VECTOR:
                            /* Recursively free nested objects */
                            obj_destroy(elem);
                            /* To avoid double free on `elem` (vector), skip
                             * iteration after recursive call since it already
                             * frees itself */
                            continue;
                        default:
                            break;
                    }

                    free(elem);
                }

                free(vec->data);
            }

            free(vec);
            break;
        }
        default:
            printf("UNKNOWN TYPE");
            return;
    }

    free(obj);
}

void obj_debug_print(const obj_t *restrict obj) {
    if (!obj) {
        return;
    }

    switch (obj->type) {
        case INT:
            printf("%d", obj->data.v_int);
            break;
        case FLOAT:
            printf("%.1f", obj->data.v_float);
            break;
        case STRING:
            printf("\"%s\"", obj->data.v_str ? obj->data.v_str : "null");
            break;
        case TUPLE:
            printf("(");
            obj->data.v_tuple.x ? obj_debug_print(obj->data.v_tuple.x)
                                : (void)printf("null");

            printf(", ");

            obj->data.v_tuple.y ? obj_debug_print(obj->data.v_tuple.y)
                                : (void)printf("null");
            printf(")");

            break;
        case VECTOR: {
            vec_t *vec = obj->data.v_vec;

            if (!vec) {
                printf("LEN: {0} []");
                break;
            }

            printf("LEN: {%zu} [", vec->size);

            for (size_t i = 0; i < vec->size; ++i) {
                obj_t *elem = (obj_t *)(vec->data[i]);
                obj_debug_print(elem);

                if (i < vec->size - 1) {
                    printf(", ");
                }
            }

            printf("]");
            break;
        }
        default:
            printf("UNKNOWN TYPE");
            break;
    }
}
