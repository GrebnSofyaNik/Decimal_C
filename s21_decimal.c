#include "s21_decimal.h"

int s21_is_less(s21_decimal value1, s21_decimal value2) {
  return s21_is_greater(value2, value1);
}

int s21_is_less_or_equal(s21_decimal value1, s21_decimal value2) {
  return s21_is_less(value1, value2) || s21_is_equal(value1, value2);
}

int s21_is_greater(s21_decimal value1, s21_decimal value2) {
  int result = 0;
  unsigned int buff1[6] = {0, 0, 0, 0, 0, 0};
  unsigned int buff2[6] = {0, 0, 0, 0, 0, 0};
  if ((result = s21_sign_compare(value1, value2)) != 0) {
    result = result == -1 ? 1 : 0;
  } else {
    for (int i = 0; i < 3; i++) {
      buff1[i] = value1.bits[i];
      buff2[i] = value2.bits[i];
    }
    s21_shift_scale(value1, value2, buff1, buff2, 6);
    if (s21_is_greater_mem(buff1, buff2, 6)) {
      result = 1;
    }
    if (s21_get_sign(value1) && !s21_is_equal(value1, value2)) {
      result = !result;
    }
  }
  return result && !s21_is_equal(value1, value2);
}

int s21_is_greater_or_equal(s21_decimal value1, s21_decimal value2) {
  return s21_is_greater(value1, value2) || s21_is_equal(value1, value2);
}

int s21_is_equal(s21_decimal value1, s21_decimal value2) {
  int result = 0;
  unsigned int buff1[6] = {0, 0, 0, 0, 0, 0};
  unsigned int buff2[6] = {0, 0, 0, 0, 0, 0};
  if (s21_dec_is_zero(value1) && s21_dec_is_zero(value2)) {
    result = 1;
  } else if ((result = s21_sign_compare(value1, value2)) != 0) {
    result = 0;
  } else {
    for (int i = 0; i < 3; i++) {
      buff1[i] = value1.bits[i];
      buff2[i] = value2.bits[i];
    }
    s21_shift_scale(value1, value2, buff1, buff2, 6);
    if (s21_is_equal_mem(buff1, buff2, 6)) {
      result = 1;
    }
  }
  return result;
}

int s21_is_not_equal(s21_decimal value1, s21_decimal value2) {
  return !s21_is_equal(value1, value2);
}
int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  int status = 0;
  if (dst == NULL) {
    status = 1;
  } else {
    int sign = s21_get_sign(src);
    int scale = s21_get_scale(src);
    unsigned res = 0;
    s21_decimal div_res;
    s21_copy_dec_to_dec(&src, &div_res);
    while (scale != 0) {
      s21_div_int(src.bits, div_res.bits, 10, 3);
      s21_copy_dec_to_dec(&div_res, &src);
      scale--;
    }
    if (div_res.bits[1] != 0 || div_res.bits[2] != 0 ||
        (div_res.bits[0] >> 31 & 1) != 0) {
      status = 1;
    } else {
      res = div_res.bits[0];
      *dst = 0;
      *dst = res;
      if (sign == 1) {
        *dst *= -1;
      }
    }
  }
  return status;
}

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  int status = 0;
  if (dst == NULL) {
    status = 1;
  } else {
    s21_clear_bits(dst->bits, 4);
    if (src < 0) {
      s21_set_sign(dst, 1);
      src = -src;
    }
    dst->bits[0] = src;
  }
  return status;
}

int s21_negate(s21_decimal value, s21_decimal *result) {
  *result = value;
  ((value.bits[3] >> 31 & 1) == 1) ? s21_set_sign(result, 0)
                                   : s21_set_sign(result, 1);
  return 0;
}

int s21_truncate(s21_decimal value, s21_decimal *result) {
  memcpy(result->bits, value.bits, sizeof(unsigned) * 4);
  int scale = s21_get_scale(*result);
  for (int i = 0; i < scale; i++) {
    s21_div_int(result->bits, result->bits, 10, 3);
  }
  s21_set_scale(result, 0);
  return 0;
}

int s21_floor(s21_decimal value, s21_decimal *result) {
  unsigned frac = 0;
  if (s21_get_sign(value) == 0) {
    s21_truncate(value, result);
  } else {
    memcpy(result->bits, value.bits, sizeof(unsigned) * 4);
    int scale = s21_get_scale(*result);
    for (int i = 0; i < scale; i++) {
      frac |= s21_mod_int(result->bits, 10, 3) > 0;
      s21_div_int(result->bits, result->bits, 10, 3);
    }
    if (frac) {
      s21_add_int(result->bits, result->bits, 1, 3);
    }
    s21_set_scale(result, 0);
  }
  return 0;
}

int s21_round(s21_decimal value, s21_decimal *result) {
  unsigned last_digit = 0;
  memcpy(result->bits, value.bits, sizeof(unsigned) * 4);
  int scale = s21_get_scale(*result);
  for (int i = 0; i < scale; i++) {
    last_digit = s21_mod_int(result->bits, 10, 3);
    s21_div_int(result->bits, result->bits, 10, 3);
  }
  if (last_digit >= 5) {
    s21_add_int(result->bits, result->bits, 1, 3);
  }
  s21_set_scale(result, 0);
  return 0;
}

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  int status = 0;
  int scale = s21_get_scale(src);
  if (dst == NULL || scale > 28) {
    status = 1;
  } else {
    double result = 0;
    for (int i = 0; i < 96; i++) {
      if (s21_bit(src, i) != 0) {
        result += pow(2, i);
      }
    }
    while (scale != 0) {
      result /= 10;
      scale--;
    }
    *dst = 0;
    *dst = (float)result;
    int sign = s21_get_sign(src);
    if (sign == 1) {
      *dst *= -1;
    }
  }
  return status;
}

int s21_div(s21_decimal value1, s21_decimal value2, s21_decimal *result) {
  int err = 0;
  s21_clear_bits(result->bits, 4);
  if (s21_dec_is_zero(value2)) {
    err = 3;
  } else {
    int sign = s21_get_sign(value1) != s21_get_sign(value2);
    unsigned buff1[9];
    unsigned buff2[9];
    unsigned buff_result[9];
    s21_clear_bits(buff_result, 9);
    s21_to_scale_mem(value1, buff1, 56, 9);
    s21_to_scale_mem(value2, buff2, 56, 9);
    int scale = -1;
    while (s21_is_greater_mem(buff1, buff2, 9)) {
      s21_mul_int(buff2, buff2, 10, 9);
      scale--;
    }
    while ((!s21_arr_is_zero(buff1, 9) && !s21_arr_is_zero(buff2, 9) &&
            scale < 30) ||
           scale < 0) {
      s21_mul_int(buff_result, buff_result, 10, 9);
      while ((!s21_is_greater_mem(buff2, buff1, 9))) {
        s21_sub_mem(buff1, buff2, buff1, 9);
        buff_result[0]++;
      }
      s21_div_int(buff2, buff2, 10, 9);
      scale++;
    }
    err = s21_mem_to_dec(buff_result, scale, sign, 9, result);
  }

  return err;
}

int s21_mod(s21_decimal value1, s21_decimal value2, s21_decimal *result) {
  int err = 0;
  s21_clear_bits(result->bits, 4);
  if (s21_dec_is_zero(value2)) {
    err = 3;
  } else {
    unsigned buff1[9];
    unsigned buff2[9];
    unsigned buff_result[9];
    s21_clear_bits(buff_result, 9);
    s21_to_scale_mem(value1, buff1, 56, 9);
    s21_to_scale_mem(value2, buff2, 56, 9);
    int scale = -1;
    while (s21_is_greater_mem(buff1, buff2, 9)) {
      s21_mul_int(buff2, buff2, 10, 9);
      scale--;
    }
    while (scale < 0) {
      s21_mul_int(buff_result, buff_result, 10, 9);
      while ((!s21_is_greater_mem(buff2, buff1, 9))) {
        s21_sub_mem(buff1, buff2, buff1, 9);
        buff_result[0]++;
      }
      s21_div_int(buff2, buff2, 10, 9);
      scale++;
    }
    err = s21_mem_to_dec(buff1, 56, s21_get_sign(value1), 9, result);
  }

  return err;
}

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int err = 0;
  int sign;
  int scale = s21_get_scale(value_1) > s21_get_scale(value_2)
                  ? s21_get_scale(value_1)
                  : s21_get_scale(value_2);
  unsigned buff1[6];
  unsigned buff2[6];
  unsigned buff_result[6];
  s21_clear_bits(buff_result, 6);
  s21_to_scale_mem(value_1, buff1, scale, 6);
  s21_to_scale_mem(value_2, buff2, scale, 6);

  if (s21_get_sign(value_1) == s21_get_sign(value_2)) {
    sign = s21_get_sign(value_1);
  } else {
    if (s21_is_greater_mem(buff1, buff2, 6)) {
      sign = s21_get_sign(value_1);
      s21_negate_mem(buff2, buff2, 6);
    } else {
      sign = s21_get_sign(value_2);
      s21_negate_mem(buff1, buff1, 6);
    }
  }
  s21_mem_add(buff1, buff2, buff_result, 6);
  err = s21_mem_to_dec(buff_result, scale, sign, 6, result);
  return err;
}
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_negate(value_2, &value_2);
  return s21_add(value_1, value_2, result);
}
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int err = 0;

  int sign = s21_get_sign(value_1) != s21_get_sign(value_2);
  int scale = s21_get_scale(value_1) + s21_get_scale(value_2);

  unsigned buff1[6];
  unsigned buff2[6];
  unsigned buff_result[6];

  s21_clear_bits(buff_result, 6);
  s21_to_scale_mem(value_1, buff1, s21_get_scale(value_1), 6);
  s21_to_scale_mem(value_2, buff2, s21_get_scale(value_2), 6);

  while (!s21_arr_is_zero(buff1, 6)) {
    if (buff1[0] % 2 == 1) {
      s21_mem_add(buff_result, buff2, buff_result, 6);
    }
    s21_div_int(buff1, buff1, 2, 6);
    s21_mul_int(buff2, buff2, 2, 6);
  }

  err = s21_mem_to_dec(buff_result, scale, sign, 6, result);
  return err;
}

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  int result = 0;
  for (int i = 0; i < 4; i++) {
    dst->bits[i] = 0;
  }
  if ((src != src) | (fabsf(src) > 7.922816e28) | (fabsf(src) == INFINITY) |
      (fabsf(src) < 1e-28)) {
    if (fabsf(src) != 0) {
      result = 1;
    }
  } else {
    char flts[64];
    snprintf(flts, 64, "%.6e", src);
    int sign_int = (src > 0 ? 0 : 1);
    src = fabsf(src);
    char numb[64];
    int j = 0;
    for (unsigned long int i = 0; i < strlen(flts); i++) {
      if (flts[i] == 'e') {
        numb[j] = '\0';
        break;
      }
      if (flts[i] != 46) {
        numb[j] = flts[i];
        j++;
      }
    }
    int _exp10 = 0;
    int _mantissa = 0;
    int tmp = 0;
    sscanf(numb, "%d", &_mantissa);
    sscanf(flts, "%d.%de%d", &tmp, &tmp, &_exp10);
    dst->bits[0] = abs(_mantissa);
    _exp10 = 6 - _exp10;
    while (_exp10 < 0) {
      s21_mod_int(dst->bits, 10, 3);
      s21_mul_int(dst->bits, dst->bits, 10, 3);
      _exp10++;
    }
    if (sign_int) {
      dst->bits[3] = 0x80000000;
    }
    s21_set_scale(dst, _exp10);
  }
  return result;
}

int s21_div_int(const unsigned *value, unsigned *result, unsigned divider,
                int size) {
  if (divider != 0) {
    unsigned long long buf = 0ull;
    unsigned long long div_buf = 0ull;
    unsigned mod = 0;
    for (int i = size - 1; i >= 0; i--) {
      mod = ((unsigned long long)buf + (unsigned long long)value[i]) %
            (unsigned long long)divider;
      div_buf = ((unsigned long long)buf + (unsigned long long)value[i]) /
                (unsigned long long)divider;
      result[i] = (unsigned)div_buf;
      buf = mod;
      buf = buf << 32;
    }
  }
  return !!divider;
}

int s21_sign_compare(
    s21_decimal value1,
    s21_decimal value2) {  // Возвращает 1, если value1 отрицательный,
                           // Возвращает -1, если value2 отрицательный, иначе 0;
  unsigned sign = 0u;
  if (s21_get_sign(value1) > s21_get_sign(value2)) {
    sign = 1;
  } else if (s21_get_sign(value2) > s21_get_sign(value1)) {
    sign = -1;
  }
  return sign;
}

void s21_shift_scale(s21_decimal value1, s21_decimal value2, unsigned *buff1,
                     unsigned *buff2, int size) {
  unsigned scale1 = s21_get_scale(value1);
  unsigned scale2 = s21_get_scale(value2);
  unsigned scale = scale1 > scale2 ? scale1 : scale2;
  s21_to_scale_mem(value1, buff1, scale, size);
  s21_to_scale_mem(value2, buff2, scale, size);
}

void s21_to_scale_mem(s21_decimal value, unsigned *buff, unsigned scale,
                      int size) {
  unsigned shift = scale - s21_get_scale(value);
  s21_clear_bits(buff, size);
  memcpy(buff, value.bits, 3 * sizeof(unsigned));
  for (unsigned i = 0; i < shift; i++) {
    s21_mul_int(buff, buff, 10, size);
  }
}

int s21_is_equal_mem(const unsigned *value1, const unsigned *value2, int size) {
  int flag = 1;
  for (int i = size - 1; i >= 0; i--) {
    flag = flag & (value1[i] == value2[i]);
  }
  return flag;
}

int s21_is_greater_mem(const unsigned *value1, const unsigned *value2,
                       int size) {
  int flag = 0;
  int result = 0;
  for (int i = size - 1; (i >= 0) & (!flag); i--) {
    if (value1[i] > value2[i]) {
      flag = 1;
      result = 1;
    } else if (value1[i] < value2[i]) {
      flag = 1;
      result = 0;
    }
  }
  return result;
}

int s21_mem_add(const unsigned *value1, const unsigned *value2,
                unsigned *result, int size) {
  unsigned long long buf = 0ull;
  for (int i = 0; i < size; i++) {
    buf += (unsigned long long)value1[i] + (unsigned long long)value2[i];
    result[i] = (unsigned)buf;
    buf = buf >> 32;
  }
  return !!buf;
}

int s21_mod_int(const unsigned *value, unsigned divider, int size) {
  unsigned mod = 0;
  if (divider != 0) {
    unsigned long long buf = 0ull;
    for (int i = size - 1; i >= 0; i--) {
      mod =
          (unsigned)(((unsigned long long)buf + (unsigned long long)value[i]) %
                     (unsigned long long)divider);
      buf = (unsigned long long)mod;
      buf = buf << 32;
    }
  }
  return mod;
}

int s21_mul_int(const unsigned *value, unsigned *result, unsigned multiplier,
                int size) {
  unsigned long long int addtiply = 0ull;
  for (int i = 0; i < size; i++) {
    addtiply = addtiply +
               ((unsigned long long)value[i] * (unsigned long long)multiplier);
    result[i] = (unsigned)addtiply;
    addtiply = addtiply >> 32;
  }
  return (addtiply > 0);
}

int s21_add_int(const unsigned *value, unsigned *result, unsigned addtiplier,
                int size) {
  unsigned long long int addtiply = (unsigned long long)addtiplier;
  for (int i = 0; i < size; i++) {
    addtiply = addtiply + (unsigned long long)value[i];
    result[i] = (unsigned)addtiply;
    addtiply = addtiply >> 32;
  }
  return (addtiply > 0);
}

int s21_negate_mem(const unsigned *value, unsigned *result, int size) {
  for (int i = 0; i < size; i++) {
    result[i] = ~value[i];
  }
  s21_add_int(result, result, 1, size);
  return 0;
}

int s21_set_sign(s21_decimal *value, int x) {
  if (x) {
    value->bits[3] |= 0b10000000000000000000000000000000;
  } else {
    value->bits[3] &= 0b01111111111111111111111111111111;
  }
  return 0;
}

void s21_clear_bits(unsigned *result, int size) {
  for (int i = 0; i < size; i++) {
    result[i] = 0;
  }
}

int s21_get_sign(const s21_decimal value) {
  int sign = value.bits[3] >> 31 & 1;
  return !!sign;
}

int s21_get_scale(const s21_decimal value) {
  return (unsigned char)(value.bits[3] >> 16);
}
void s21_set_scale(s21_decimal *value, int scale) {
  value->bits[3] =
      (value->bits[3] & 0xFF00FFFF) | (((unsigned)scale & 0xFF) << 16);
  value->bits[3] =
      (value->bits[3] & 0xFF00FFFF) | (((unsigned)scale & 0xFF) << 16);
}

int s21_dec_is_zero(s21_decimal value) {
  return !(value.bits[0] || value.bits[1] || value.bits[2]);
}

void s21_copy_dec_to_dec(const s21_decimal *value, s21_decimal *result) {
  for (int i = 0; i < 4; i++) {
    result->bits[i] = value->bits[i];
  }
}

int s21_bit(const s21_decimal value, const int num) {
  return (value.bits[num / 32] >> (num % 32)) & 1u;
}

int s21_mem_to_dec(unsigned *src, int scale, int sign, int size,
                   s21_decimal *result) {
  s21_clear_bits(result->bits, 4);
  int err = 0;
  int last_digit = 0;
  while ((!s21_arr_is_zero(&src[3], size - 3) || scale > 28) && scale > 0) {
    last_digit = s21_mod_int(src, 10, size);
    s21_div_int(src, src, 10, size);
    scale--;
  }
  if (last_digit == 5 && (s21_mod_int(src, 10, size) % 2 == 1)) {
    s21_add_int(src, src, 1, size);
  } else if (last_digit > 5) {
    s21_add_int(src, src, 1, size);
  }
  if (!s21_arr_is_zero(&src[3], size - 3)) {
    err = sign + 1;
  } else {
    memcpy(result->bits, src, 3 * sizeof(unsigned));
    result->bits[3] = 0u;
    s21_set_sign(result, sign);
    s21_set_scale(result, scale);
  }
  return err;
}

int s21_arr_is_zero(unsigned *arr, int size) {
  int flag = 1;
  for (int i = 0; i < size; i++) {
    if (arr[i] != 0) {
      flag &= 0;
    }
  }
  return flag;
}

int s21_sub_mem(const unsigned *value1, const unsigned *value2,
                unsigned *result, int size) {
  unsigned value2_copy[size];
  memcpy(value2_copy, value2, size * sizeof(unsigned));
  s21_negate_mem(value2, value2_copy, size);
  s21_mem_add(value1, value2_copy, result, size);
  return 0;
}

int s21_decimal_deserialize(s21_decimal *value, int fd) {
  return read(fd, value->bits, sizeof(s21_decimal));
}

int s21_decimal_serialize(s21_decimal value, int fd) {
  return write(fd, value.bits, sizeof(s21_decimal));
}
