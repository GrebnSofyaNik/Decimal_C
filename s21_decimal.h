#ifndef S21_DECIMAL_H
#define S21_DECIMAL_H
#include <fcntl.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  unsigned bits[4];
} s21_decimal;

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

int s21_is_less(s21_decimal value_1, s21_decimal value_2);
int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2);
int s21_is_greater(s21_decimal value_1, s21_decimal value_2);
int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2);
int s21_is_equal(s21_decimal value_1, s21_decimal value_2);
int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2);

int s21_from_int_to_decimal(int src, s21_decimal *dst);
int s21_from_float_to_decimal(float src, s21_decimal *dst);
int s21_from_decimal_to_int(s21_decimal src, int *dst);
int s21_from_decimal_to_float(s21_decimal src, float *dst);

int s21_floor(s21_decimal value, s21_decimal *result);
int s21_round(s21_decimal value, s21_decimal *result);
int s21_truncate(s21_decimal value, s21_decimal *result);
int s21_negate(s21_decimal value, s21_decimal *result);

int s21_mul_int(const unsigned *value, unsigned *result, unsigned multiplier,
                int size);
int s21_div_int(const unsigned *value, unsigned *result, unsigned divider,
                int size);
int s21_mod_int(const unsigned *value, unsigned divider, int size);
int s21_mem_add(const unsigned *value1, const unsigned *value2,
                unsigned *result, int size);
int s21_negate_mem(const unsigned *value, unsigned *result, int size);
int s21_is_equal_mem(const unsigned *value1, const unsigned *value2, int size);
int s21_is_greater_mem(const unsigned *value1, const unsigned *value2,
                       int size);

int s21_get_scale(const s21_decimal value);
void s21_set_scale(s21_decimal *value, int scale);

int s21_sign_compare(s21_decimal value1, s21_decimal value2);
int s21_scale_compare(s21_decimal value1, s21_decimal value2);
void s21_shift_scale(s21_decimal value1, s21_decimal value2, unsigned *buff1,
                     unsigned *buff2, int size);
void s21_fill_with_ones(unsigned *value, int size);
int s21_set_sign(s21_decimal *value, int x);
void s21_clear_bits(unsigned *result, int size);
int s21_get_sign(const s21_decimal value);
void s21_shift_zeroes(s21_decimal *value);
int s21_dec_is_zero(s21_decimal value);
void s21_copy_dec_to_dec(const s21_decimal *value, s21_decimal *result);
int s21_bit(const s21_decimal value, const int num);
int s21_add_int(const unsigned *value, unsigned *result, unsigned addtiplier,
                int size);
int s21_decimal_deserialize(s21_decimal *value, int fd);
int s21_decimal_serialize(s21_decimal value, int fd);
void s21_to_scale_mem(s21_decimal value, unsigned *buff, unsigned scale,
                      int size);
int s21_arr_is_zero(unsigned *arr, int size);
int s21_mem_to_dec(unsigned *src, int scale, int sign, int size,
                   s21_decimal *result);
int s21_sub_mem(const unsigned *value1, const unsigned *value2,
                unsigned *result, int size);

#endif  // S21_DECIMAL_H
