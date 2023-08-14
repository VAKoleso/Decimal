#include <limits.h>
#include <math.h>

#include "s21_decimal.h"

// функция прошла вертера 26 03 2022. не трогаем.
int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  set_zero(dst);
  if (src < 0) {
    set_sign(dst, DEC_MINUS);
    src = -src;
  }
  dst->bits[LOW] = src;

  return SUCCESS;
}

// функция прошла вертера 26 03 2022. не трогаем.
int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  *dst = 0;
  int ret = CONVERTING_ERROR;
  if (src.value_type == s21_NORMAL_VALUE) {
    ret = SUCCESS;
    int scale = get_scale(src);
    int count_stepen = 0;
    long long int tmp = 0;

    for (int i = 0; i < MAX_BITS && ret == SUCCESS; i++) {
      if (get_bit(src, i) != 0) {
        tmp += pow(2 * get_bit(src, i), count_stepen);
        if (tmp >= INT_MAX) ret = CONVERTING_ERROR;
      }
      count_stepen++;
    }
    for (int i = 0; i < scale; i++) {
      tmp /= 10;
    }
    //  if (ret == SUCCESS) {
    if (get_sign(src) == DEC_MINUS) tmp *= -1;

    *dst = tmp;
    //  }
  }

  return ret;
}

// функция не прошла вертера. исправлена.
int s21_from_decimal_to_float(s21_decimal src, float *__dst) {
  int ret = SUCCESS;
  if (src.value_type == s21_NORMAL_VALUE) {
    long double tmp_dst = .0;
    ret = SUCCESS;
    uint scale = get_scale(src);
    for (int i = 0; i < 96; i++) {
      if (get_bit(src, i) > 0) tmp_dst += powl(2., i);
    }
    while (scale != 0) {
      tmp_dst /= 10.;
      scale--;
    }
    if (get_sign(src) == DEC_MINUS) tmp_dst = -tmp_dst;
    *__dst = tmp_dst;
  } else {
    ret = CONVERTING_ERROR;
    if (src.value_type == s21_NAN) *__dst = NAN;
    if (src.value_type == s21_INFINITY) *__dst = INFINITY;
    if (src.value_type == s21_NEGATIVE_INFINITY) *__dst = -INFINITY;
  }
  return ret;
}

//  не прошла вертера
int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  int ret = CONVERTING_ERROR;
  union converter_float conv;
  set_zero(dst);
  if (isnan(src)) {
    dst->value_type = s21_NAN;
  } else if (isinf(src) || fabs(src) > 7.92281625E+28) {
    if (src < 0)
      dst->value_type = s21_NEGATIVE_INFINITY;
    else
      dst->value_type = s21_INFINITY;
  }
  /* В задаче есть условие: Если числа слишком малы (0 < |x| < 1e-28), вернуть
     ошибку и значение, равное 0, value_type = 0 это решается вторым параметром
     в следующем if'е
  */
  if (dst->value_type == s21_NORMAL_VALUE &&
      (!(fabs(src) > 0 && fabs(src) < 1e-28))) {
    ret = SUCCESS;

    if (src != 0) {
      int scale = 0;
      while (fabs(src) < 1) {
        src *= 10.;

        scale++;
      }

      while (get_counts_of_digits(src) < 7) {
        src *= 10.;
        scale++;
      }

      conv.fl = src;

      uchar s = (conv.dw >> 31) ? DEC_MINUS : DEC_PLUS; /* Знак */
      int e = (conv.dw >> 23) & 0xFF;                   /* Порядок */
      set_zero(dst);
      int mask = 0x400000;  // маска 0b1000000...00 (23шт)
      e = e - 127;          // он точно > 0
      set_bit(dst, e, 1);

      for (int i = 1; i <= 23 && (e - i) >= 0; i++) {
        set_bit(dst, e - i, !!(mask & conv.dw));
        mask >>= 1;
      }
      set_sign(dst, s);
      set_scale(dst, scale);
    }
  }
  return ret;
}
