
#include <math.h>
#include <stdio.h>

#include "s21_decimal.h"
#include "simple_decimal.h"

void from_decimal(s21_decimal src, simple_decimal *dst) {
  init_sd(dst);
  for (int i = 0; i < MAX_BITS; i++) {
    (*dst)[i] = get_bit(src, i);
  }
}

int to_decimal(s21_decimal *dst, simple_decimal *src, int need_scale,
               int please_one_more_div) {
  int podelil = 0;
  set_zero(dst);
  int one_first, count_numbers, is_divided_by_10 = FALSE;
  simple_decimal remainder, prev_remainder;
  init_sd(&remainder);
  // UNUSED(need_scale);
  UNUSED(count_numbers);
  UNUSED(remainder);
  // int skolko_delit = 29 - need_scale;
  // while((one_first = get_first_1_position_from_end(*src)) >= MAX_BITS) {
  //  copy_sd(remainder, &prev_remainder);
  // sd_div_int(*src, sd_10, src, &remainder, &count_numbers);
  //    // is_divided_by_10 = TRUE;
  //}
  // while((one_first = count_sd_numbers(*src)) > skolko_delit) {
  while ((one_first = get_first_1_position_from_end(*src)) >= MAX_BITS ||
         need_scale - podelil > MAX_SCALE) {
    copy_sd(remainder, &prev_remainder);
    sd_div_int(*src, sd_10, src, &remainder, &count_numbers);
    is_divided_by_10 = TRUE;
    podelil++;
  }
  if (please_one_more_div == TRUE) {
    sd_div_int(*src, sd_10, src, &remainder, &count_numbers);
    // is_divided_by_10 = TRUE;
    //  sd_mul(*src, sd_10, src);sd_mul(*src, sd_10, src);
    podelil--;
  }
  // if (need_scale - podelil >= MAX_SCALE) {// это число 0.12235678.... делим
  // его еще раз, чтобы получить 0 первый, и округлеть
  //     sd_div_int(*src, sd_10, src, &remainder, &count_numbers);
  //     is_divided_by_10 = TRUE;
  // }
  //  printf("\npodelil = %d", podelil);
  if (is_divided_by_10 == TRUE) {  // округляем
    if (sd_is_grater_or_equal(remainder, sd_5) == TRUE) sd_sum(*src, sd_1, src);
  }

  // if (one_first < MAX_BITS)
  //     one_first = MAX_BITS - 1;

  /*for (int i = MAX_BITS - 1; i >= 0; i--) {
      set_bit(dst, i, (*src)[one_first]);
      one_first--;
  }*/
  for (int i = 0; i < MAX_BITS; i++) {
    set_bit(dst, i, (*src)[i]);
    // one_first--;
  }
  return podelil;
}

void init_sd(simple_decimal *dst) {
  for (int i = 0; i < SZ_SD + 2; i++) {
    (*dst)[i] = 0;
  }
}

void copy_sd(simple_decimal src, simple_decimal *dst) {
  for (int i = 0; i < SZ_SD + 2; i++) {
    (*dst)[i] = src[i];
  }
}

int get_non_zero_bits_count(simple_decimal one) {
  return get_first_1_position_from_end(one) + 1;
}

int get_first_1_position_from_end(simple_decimal one) {
  int ret = 0;
  for (int i = SZ_SD - 1; i >= 0 && ret == 0; i--) {
    if (one[i] != 0) ret = i;
  }
  return ret;
}

/**
 * Проверяторы
 */
s21_decimal check_s21_sub_from_sd(s21_decimal one, s21_decimal two,
                                  int *is_stop) {
  *is_stop = TRUE;
  int sgn_one = get_sign(one);
  s21_decimal res = CONST_DEF_DECIMAL;
  if (s21_is_equal(one, two) == TRUE) {
    set_zero(&res);

  } else if (is_nan(one) == TRUE || is_nan(two) == TRUE) {
    set_nan(&res);

  } else if (is_inf(one) == TRUE && is_inf(two) == TRUE) {
    if (one.value_type == two.value_type)
      set_nan(&res);
    else
      res = set_infinity(sgn_one);

  } else if (is_inf(one) == TRUE) {
    res = set_infinity(sgn_one);

  } else if (is_inf(two) == TRUE) {
    if (two.value_type == s21_INFINITY)
      res = set_infinity(DEC_MINUS);
    else
      res = set_infinity(DEC_PLUS);

  } else {
    *is_stop = FALSE;
  }
  return res;
}

// я предполагаю, что при вычитании первый символ == 1 означает, что полученное
// число отрицательное
int get_sign_after_sub(simple_decimal src) {
  int ret = DEC_PLUS;
  if (src[SZ_SD] == 1) ret = DEC_MINUS;
  return ret;
}

int sd_is_zero(simple_decimal src) {
  int ret = TRUE;
  for (int i = 0; i < SZ_SD && ret == TRUE; i++) {
    if (src[i] != 0) ret = FALSE;
  }
  return ret;
}
// устал и решил перписать методы sd_cut так, чтобы возращали не перевернутую
// последовальность а правильную
void sd_cut(simple_decimal one, simple_decimal *res, int start_from_end_pos,
            int count) {
  init_sd(res);
  // int real_start = start_pos - count;
  for (int i = 0; i < count; i++) {
    (*res)[count - 1 - i] = one[start_from_end_pos - 1 - i];
  }
}
