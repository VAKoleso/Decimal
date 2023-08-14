/**
 * все утилитарные функции
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "s21_decimal.h"
#include "simple_decimal.h"

/**
 * bits[3] содержит коэффициент масштабирования и знак, и состоит из следующих
 * частей: Биты от 0 до 15, младшее слово, не используются и должны быть равны
 * нулю. Биты с 16 по 23 должны содержать показатель степени от 0 до 28, который
 * указывает степень 10 для разделения целого числа. Биты с 24 по 30 не
 * используются и должны быть равны нулю. Бит 31 содержит знак; 0 означает
 * положительный, а 1 означает отрицательный.
 *
 *  получается, что scale это третий (второй, если с нуля считать) байт в int
 *  а знак - это четвертый (третий с нуля). Причём чтобы установить знак этому
 * четвертом биту надо присвоить либо (uchar)0 (+) либо (uchar)128 (-) и никаких
 * битовых сдвигов
 */
uchar get_scale(s21_decimal decimal) {
  union converter conv;
  conv.number = decimal.bits[SCALE];
  return conv.bytes[2];
}

void set_scale(s21_decimal *decimal, uchar scale) {
  union converter conv;
  conv.number = (*decimal).bits[SCALE];
  conv.bytes[2] = scale;
  UNUSED(conv.bytes[2]);
  decimal->bits[SCALE] = conv.number;
}

uchar get_sign(s21_decimal decimal) {
  uchar ret;
  if (is_inf(decimal) == TRUE) {
    ret = DEC_PLUS;
    if (decimal.value_type == s21_NEGATIVE_INFINITY) ret = DEC_MINUS;
  } else {
    union converter int_to_char;
    int_to_char.number = decimal.bits[SCALE];
    ret = int_to_char.bytes[3];
  }
  return ret;
}

void set_sign(s21_decimal *decimal, uchar sign) {
  union converter conv;
  conv.number = (*decimal).bits[SCALE];
  conv.bytes[3] = sign;
  UNUSED(conv.bytes[3]);
  decimal->bits[SCALE] = conv.number;
}

int is_zero(s21_decimal decimal) {
  int ret = FALSE;
  if (decimal.bits[LOW] == 0 && decimal.bits[MID] == 0 &&
      decimal.bits[HIGHT] == 0)
    ret = TRUE;
  return ret;
}

int is_nan(s21_decimal decimal) {
  int ret = FALSE;
  if (decimal.value_type == s21_NAN) ret = TRUE;
  return ret;
}

int is_inf(s21_decimal decimal) {
  int ret = FALSE;
  if (decimal.value_type == s21_INFINITY ||
      decimal.value_type == s21_NEGATIVE_INFINITY)
    ret = TRUE;
  return ret;
}

void set_zero(s21_decimal *decimal) {
  decimal->bits[LOW] = 0;
  decimal->bits[MID] = 0;
  decimal->bits[HIGHT] = 0;
  decimal->bits[SCALE] = 0;
  decimal->value_type = s21_NORMAL_VALUE;
}

uchar get_bit(s21_decimal decimal, uchar index) {
  uint mask = 1u << (index % 32);
  return !!(decimal.bits[index / 32] & mask);
}

void set_bit(s21_decimal *decimal, uchar index, uchar bit_value) {
  if (index < 96) {
    uint mask_val = (!!bit_value) << (index % 32);
    decimal->bits[index / 32] |= mask_val;
  }
}

void copy(s21_decimal src, s21_decimal *dst) {
  dst->bits[LOW] = src.bits[LOW];
  dst->bits[MID] = src.bits[MID];
  dst->bits[HIGHT] = src.bits[HIGHT];
  dst->bits[SCALE] = src.bits[SCALE];
  dst->value_type = src.value_type;
}

int is_all_field_is_equal(s21_decimal one, s21_decimal two) {
  int ret = FALSE;
  if (one.value_type == two.value_type && one.bits[LOW] == two.bits[LOW] &&
      one.bits[MID] == two.bits[MID] && one.bits[HIGHT] == two.bits[HIGHT] &&
      one.bits[SCALE] == two.bits[SCALE])
    ret = TRUE;
  return ret;
}

s21_decimal set_infinity(int znak) {
  s21_decimal res = CONST_DEF_DECIMAL;
  if (znak == DEC_MINUS) {
    res.value_type = s21_NEGATIVE_INFINITY;
  } else {
    res.value_type = s21_INFINITY;
  }
  return res;
}

s21_decimal set_nan() {
  s21_decimal res = CONST_DEF_DECIMAL;
  res.value_type = s21_NAN;
  return res;
}

/*
int calc_max_scale(s21_decimal *one, s21_decimal *two, int *max_scale) {
    int ret;

    s21_decimal dec_with_MIN_scale = get_decimal_with_MIN_scale(*one, *two);
    s21_decimal dec_with_MAX_scale = get_decimal_with_MAX_scale(*one, *two);

    int max_scale_of_dec_MIN_scale = get_max_scale(dec_with_MIN_scale);
    if (max_scale_of_dec_MIN_scale < get_scale(dec_with_MAX_scale)) {
        *max_scale = max_scale_of_dec_MIN_scale;
        ret = ALIGMENT_OVERFLOW;
    } else {
        *max_scale = get_scale(dec_with_MAX_scale);
        ret = ALIGMENT_OK;
    }
    return ret;
}
*/

s21_decimal *get_decimal_with_MIN_scale(s21_decimal *one, s21_decimal *two) {
  s21_decimal *ret;
  int scale_one = get_scale(*one);
  int scale_two = get_scale(*two);

  if (scale_one > scale_two)
    ret = two;
  else
    ret = one;
  return ret;
}

s21_decimal *get_decimal_with_MAX_scale(s21_decimal *one, s21_decimal *two) {
  s21_decimal *ret;
  int scale_one = get_scale(*one);
  int scale_two = get_scale(*two);

  if (scale_one < scale_two)
    ret = two;
  else
    ret = one;
  return ret;
}
/*
int get_max_scale(s21_decimal dec_with_MIN_scale) {
    s21_decimal tmp;
    copy(dec_with_MIN_scale, &tmp);
    int appender_scale = 0;
    while (s21_check_bit_max(tmp) != 0) {
        tmp = pow_by_ten(tmp, TRUE);
        appender_scale++;
    }
    return appender_scale + get_scale(dec_with_MIN_scale);

}
*/

s21_decimal s21_decimal_rounded_div_ten(s21_decimal dec) {
  s21_decimal five = CONST_DECIMAL_FIVE;
  s21_decimal edebichka = CONST_DECIMAL_ONE;
  s21_decimal tmp_dec;
  copy(dec, &tmp_dec);
  s21_decimal dived = s21_decimal_div_ten(dec);
  // set_scale(&dived, 0);
  s21_decimal deved_mul_10 = pow_by_ten(dived, TRUE);
  s21_decimal s21_minus = s21_sub(dec, deved_mul_10);
  set_scale(&s21_minus, 0);
  if (s21_is_greater_or_equal(s21_minus, five) == TRUE) {
    int tmp_scale = get_scale(dived);
    set_scale(&dived, 0);
    dived = s21_add(dived, edebichka);
    set_scale(&dived, tmp_scale);
  }
  return dived;
}

int alignment_scale(s21_decimal *one, s21_decimal *two) {
  int ret = SUCCESS;

  s21_decimal tmp_by_ten;
  int scale_one = get_scale(*one);
  int scale_two = get_scale(*two);
  int sub_scale = abs(scale_two - scale_one);
  if (sub_scale > 0) {
    int dec_scale = 0;
    s21_decimal *tmp1 = get_decimal_with_MIN_scale(one, two);
    s21_decimal *tmp2 = get_decimal_with_MAX_scale(one, two);

    for (int i = 0; i < sub_scale && dec_scale == 0; i++) {
      tmp_by_ten = pow_by_ten(*tmp1, TRUE);
      if (tmp_by_ten.value_type != s21_NORMAL_VALUE) {
        dec_scale = sub_scale - i;
      } else {
        copy(tmp_by_ten, tmp1);
      }
    }
    for (int i = 0; i < dec_scale && ret == SUCCESS; i++) {
      tmp_by_ten = s21_decimal_rounded_div_ten(*tmp2);  //, FALSE);
      if (tmp_by_ten.value_type != s21_NORMAL_VALUE ||
          is_zero(tmp_by_ten) == TRUE) {
        ret = CONVERTING_ERROR;
      } else {
        copy(tmp_by_ten, tmp2);
      }
    }
  }
  return ret;
}

s21_decimal pow_by_ten(s21_decimal dec, int add_scale) {
  s21_decimal ret = CONST_DEF_DECIMAL;
  int scale = get_scale(dec);
  s21_decimal shift_by_3 = shift(dec, 3, DIRECT_LEFT);
  s21_decimal shift_by_1 = shift(dec, 1, DIRECT_LEFT);

  if (shift_by_1.value_type != s21_NORMAL_VALUE) {
    copy(shift_by_3, &ret);
  } else {
    ret = s21_add(shift_by_3, shift_by_1);
    if (add_scale == TRUE)
      set_scale(&ret, scale + 1);
    else
      set_scale(&ret, scale - 1);
  }
  return ret;
}

int s21_check_bits_zero(s21_decimal src) {
  int zero = FALSE;
  if (src.bits[HIGHT] == 0 && src.bits[MID] == 0 && src.bits[LOW] == 0) {
    zero = TRUE;
  }
  return zero;
}

int get_counts_of_digits(int number) {
  int count = (number == 0) ? 1 : 0;
  while (number != 0) {
    count++;
    number /= 10;
  }
  return count;
}

static char *value_type_t_names[] = {"s21_NORMAL_VALUE", "s21_INFINITY",
                                     "s21_NEGATIVE_INFINITY", "s21_NAN"};

void print_decimal(s21_decimal decimal) {
  char *str = to_string(decimal);
  printf("%s", str);
  free(str);
}

char *to_string(s21_decimal decimal) {
  int len = 10240;
  char *ret = malloc(len);
  char sign;

  if (get_sign(decimal) == DEC_MINUS)
    sign = '-';
  else
    sign = '+';

  snprintf(ret, len, "\t--------------------------------------\n\tDECIMAL:\n");
  snprintf(ret + strlen(ret), len, "\t bits[LOW]\t%u   \n", decimal.bits[LOW]);
  snprintf(ret + strlen(ret), len, "\t bits[MID]\t%u   \n", decimal.bits[MID]);
  snprintf(ret + strlen(ret), len, "\t bits[HIGHT]\t%u   \n",
           decimal.bits[HIGHT]);
  snprintf(ret + strlen(ret), len, "\t bits[SCALE]\t%c / %d\n", sign,
           get_scale(decimal));
  snprintf(ret + strlen(ret), len, "\n\tBINARY DATA:\n");
  for (int i = 0; i < 4; i++) {
    uint fbits = decimal.bits[i];
    snprintf(ret + strlen(ret), len, "\t ");
    for (uint mask = 0x80000000; mask; mask >>= 1) {
      if ((mask == 0x80 || mask == 0x800000 || mask == 0x8000))
        snprintf(ret + strlen(ret), len, " ");

      snprintf(ret + strlen(ret), len, "%d", !!(fbits & mask));
    }
    snprintf(ret + strlen(ret), len, "%5s|%5s", "", "");
    if (i == 3) {
      snprintf(ret + strlen(ret), len, "Sign: %c / Scale: %d\n", sign,
               get_scale(decimal));
    } else {
      snprintf(ret + strlen(ret), len, "%u", decimal.bits[i]);
      snprintf(ret + strlen(ret), len, "\n");
    }
  }
  snprintf(ret + strlen(ret), len, "\tvalue_type: %s\n",
           value_type_t_names[decimal.value_type]);

  snprintf(ret + strlen(ret), len, "\n");
  return ret;
}

// Деленим 10 уменьшаем scale на 1
s21_decimal s21_decimal_div_ten(s21_decimal src) {
  s21_decimal tmp = CONST_DEF_DECIMAL;
  s21_decimal result = CONST_DEF_DECIMAL;
  int count = 0, bit_src = 0, bit_tmp = 0, flag_res = 0;
  for (int i = 95; i >= 0; i--) {
    bit_src <<= 1;
    bit_src = bit_src + get_bit(src, i);  // двигаем влево и записываем биты
    if (bit_src >= 10) {
      set_bit(&tmp, bit_tmp, 1);
      bit_src -= 10;
      if (!flag_res) {
        flag_res = 1;
      }
    }
    if (flag_res) {
      bit_tmp++;
    }
  }
  for (int i = bit_tmp - 1; i >= 0; i--, count++) {
    set_bit(&result, count, get_bit(tmp, i));
  }
  int scale = get_scale(src);
  if (scale > 0) {
    set_scale(&result, scale - 1);
  }
  return result;
}

// Проверочная функция ВЫЧИТАНИЯ на входные аргументы типа: INFINITY /
// NEGATIVE_INFINITY / NAN

s21_decimal s21_nan_sub(s21_decimal one, s21_decimal two) {
  s21_decimal result = CONST_DEF_DECIMAL;
  if ((one.value_type == s21_NAN || two.value_type == s21_NAN) ||
      (one.value_type == s21_NEGATIVE_INFINITY &&
       two.value_type == s21_NEGATIVE_INFINITY) ||
      (one.value_type == s21_INFINITY && two.value_type == s21_INFINITY)) {
    result.value_type = s21_NAN;
  } else if ((one.value_type == s21_INFINITY &&
              two.value_type == s21_NORMAL_VALUE) ||
             (one.value_type == s21_NORMAL_VALUE &&
              two.value_type == s21_NEGATIVE_INFINITY) ||
             (one.value_type == s21_INFINITY &&
              two.value_type == s21_NEGATIVE_INFINITY)) {
    result.value_type = s21_INFINITY;
  } else if ((one.value_type == s21_NEGATIVE_INFINITY &&
              two.value_type == s21_NORMAL_VALUE) ||
             (one.value_type == s21_NORMAL_VALUE &&
              two.value_type == s21_INFINITY) ||
             (one.value_type == s21_NEGATIVE_INFINITY &&
              two.value_type == s21_INFINITY)) {
    result.value_type = s21_NEGATIVE_INFINITY;
  }
  return result;
}

// Проверочная функция УМНОЖЕНИЯ на входные аргументы типа: INFINITY /
// NEGATIVE_INFINITY / NAN

s21_decimal s21_nan_mul(s21_decimal one, s21_decimal two) {
  s21_decimal result = CONST_DEF_DECIMAL;
  if ((one.value_type == s21_NAN || two.value_type == s21_NAN) ||
      (s21_is_equal(result, one) == TRUE && two.value_type == s21_INFINITY) ||
      (s21_is_equal(result, one) == TRUE &&
       two.value_type == s21_NEGATIVE_INFINITY) ||
      (one.value_type == s21_INFINITY && s21_is_equal(result, two) == TRUE) ||
      (one.value_type == s21_NEGATIVE_INFINITY &&
       s21_is_equal(result, two) == TRUE)) {
    result.value_type = s21_NAN;
  } else if ((one.value_type == s21_INFINITY &&
              two.value_type == s21_INFINITY) ||
             (one.value_type == s21_INFINITY &&
              s21_is_equal(result, two) != TRUE &&
              two.value_type == s21_NORMAL_VALUE &&
              get_sign(two) == DEC_PLUS) ||
             (two.value_type == s21_INFINITY &&
              s21_is_equal(result, one) != TRUE &&
              one.value_type == s21_NORMAL_VALUE &&
              get_sign(one) == DEC_PLUS) ||
             (one.value_type == s21_NEGATIVE_INFINITY &&
              two.value_type == s21_NEGATIVE_INFINITY)) {
    result.value_type = s21_INFINITY;
  } else if ((one.value_type == s21_INFINITY &&
              two.value_type == s21_NEGATIVE_INFINITY) ||
             (one.value_type == s21_INFINITY &&
              s21_is_equal(result, two) != TRUE &&
              two.value_type == s21_NORMAL_VALUE &&
              get_sign(two) == DEC_MINUS) ||
             (two.value_type == s21_INFINITY &&
              s21_is_equal(result, one) != TRUE &&
              one.value_type == s21_NORMAL_VALUE &&
              get_sign(one) == DEC_MINUS) ||
             (one.value_type == s21_NEGATIVE_INFINITY &&
              two.value_type == s21_INFINITY) ||
             (one.value_type == s21_NEGATIVE_INFINITY &&
              s21_is_equal(result, two) != TRUE &&
              two.value_type == s21_NORMAL_VALUE &&
              get_sign(two) == DEC_PLUS) ||
             (two.value_type == s21_NEGATIVE_INFINITY &&
              s21_is_equal(result, one) != TRUE &&
              one.value_type == s21_NORMAL_VALUE &&
              get_sign(one) == DEC_PLUS)) {
    result.value_type = s21_NEGATIVE_INFINITY;
  }
  return result;
}

// Проверочная функция ДЕЛЕНИЯ на входные аргументы типа: INFINITY /
// NEGATIVE_INFINITY / NAN Влад, не забудь проверки обычных чисел, это важно!!!
// Я еще добавил проверку на то, что two is_zero
s21_decimal s21_nan_div(s21_decimal one, s21_decimal two) {
  s21_decimal result = CONST_DEF_DECIMAL;
  if ((one.value_type == s21_NAN || two.value_type == s21_NAN) ||
      (one.value_type == s21_INFINITY && two.value_type == s21_INFINITY) ||
      (one.value_type == s21_INFINITY &&
       two.value_type == s21_NEGATIVE_INFINITY) ||
      (one.value_type == s21_NEGATIVE_INFINITY &&
       two.value_type == s21_NEGATIVE_INFINITY) ||
      (one.value_type == s21_NEGATIVE_INFINITY &&
       two.value_type == s21_INFINITY)) {
    result.value_type = s21_NAN;
  } else if ((one.value_type == s21_INFINITY &&
              s21_is_equal(result, two) == TRUE) ||
             (one.value_type == s21_INFINITY &&
              two.value_type == s21_NORMAL_VALUE &&
              get_sign(two) == DEC_PLUS) ||
             (one.value_type == s21_NEGATIVE_INFINITY &&
              two.value_type == s21_NORMAL_VALUE &&
              get_sign(two) == DEC_MINUS)) {
    result.value_type = s21_INFINITY;
  } else if ((one.value_type == s21_NEGATIVE_INFINITY &&
              s21_is_equal(result, two) == TRUE) ||
             (one.value_type == s21_INFINITY &&
              two.value_type == s21_NORMAL_VALUE &&
              get_sign(two) == DEC_MINUS) ||
             (one.value_type == s21_NEGATIVE_INFINITY &&
              two.value_type == s21_NORMAL_VALUE &&
              get_sign(two) == DEC_PLUS)) {
    result.value_type = s21_NEGATIVE_INFINITY;
  } else if (is_zero(two) == TRUE) {
    if (is_zero(one) == TRUE)
      result.value_type = s21_NAN;
    else
      result = set_infinity(get_sign(one));
  }
  return result;
}

// Проверочная функция ОСТАТОК ОТ ДЕЛЕНИЯ на входные аргументы типа: INFINITY /
// NEGATIVE_INFINITY / NAN Влад, не забудь проверки обычных чисел, это важно!!!

s21_decimal s21_nan_mod(s21_decimal one, s21_decimal two) {
  s21_decimal result = CONST_DEF_DECIMAL;
  if (one.value_type == s21_NAN || two.value_type == s21_NAN ||
      one.value_type == s21_INFINITY || two.value_type == s21_INFINITY ||
      one.value_type == s21_NEGATIVE_INFINITY ||
      two.value_type == s21_NEGATIVE_INFINITY) {
    result.value_type = s21_NAN;
  }
  return result;
}

#ifdef debug_pty
int main() {
  s21_decimal tmp1[] = {
      {{1, 5, 10, 0x000c0000}, s21_NORMAL_VALUE},     // 31
      {{12345, 0, 0, 0x00020000}, s21_NORMAL_VALUE},  // 27
      {{0x7fffffff, 0x7fffffff, 0, 0x800c0000}, s21_NORMAL_VALUE},
      {{2, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                    // 4
      {{12345, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                // 5
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 6
      {{0x7fffffff, 0x7fffffff, 0, 0x800E0000}, s21_NORMAL_VALUE},  // 7
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 8
      {{0xffffffff, 0xffffffff, 0xffffffff, 0}, s21_NORMAL_VALUE},  // 9
      {{0xffffffff, 0xffffffff, 0xffffffff, 0x00000000},
       s21_NORMAL_VALUE},  // 10
      //    {{100,   0, 0,  0},    s21_NORMAL_VALUE},   // 11
  };

  s21_decimal tmp2[] = {
      {{1, 5, 10, 0x000e0000}, s21_NORMAL_VALUE},                   // 31
      {{12345, 0, 0, 0x00040000}, s21_NORMAL_VALUE},                // 27
      {{0x7fffffff, 0x7fffffff, 0, 0x000e0000}, s21_NORMAL_VALUE},  // 31
      {{3, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                    // 4
      {{12345678, 0, 0, 0x80000000}, s21_NORMAL_VALUE},             // 5
      {{0x7fffffff, 0x7fffffff, 0, 0x00280000}, s21_NORMAL_VALUE},  // 6
      {{0x7fffffff, 0x7fffffff, 0, 0x800c0000}, s21_NORMAL_VALUE},  // 7
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 8
      {{1, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                    // 9
      {{50, 0, 0, 0x80020000}, s21_NORMAL_VALUE},                   // 10
      // {{100,   0, 0,  0},    s21_NORMAL_VALUE}, // 11
  };

  // shift_overflow(tmp1[2], 1, DIRECT_LEFT, TRUE);
  // print_decimal(tmp2[0]);
  print_decimal(s21_mul_from_sd(tmp1[0], tmp2[0]));
  // print_decimal(s21_sum_from_sd(tmp1[1], tmp2[1]));
  //  print_decimal(s21_add(tmp1[2], tmp2[2]));
  s21_decimal dec;
  s21_from_float_to_decimal(5.289365, &dec);
  // print_decimal(dec);

  // print_decimal(s21_mod(tmp1[0], tmp2[0]));

  // print_decimal(tmp1[9]);
  // print_decimal(tmp2[2]);
  //  print_decimal_to_numbers(tmp1[2]);
  // UNUSED(invert_sign(&tmp1[0]));
  UNUSED(is_all_field_is_equal(tmp1[0], tmp1[0]));
  // UNUSED(s21_check_bit_max(tmp1[0]));
  // UNUSED(s21_div_pjian(tmp1[0], tmp1[0]));
  UNUSED(s21_floor(tmp1[0]));
  float f;
  UNUSED(s21_from_decimal_to_float(tmp1[0], &f));
  UNUSED(s21_from_float_to_decimal(5, &tmp1[1]));
  UNUSED(s21_from_int_to_decimal(5, &tmp1[1]));
  UNUSED(s21_is_less_or_equal(tmp1[0], tmp1[0]));
  UNUSED(s21_negate(tmp1[0]));
  UNUSED(s21_or(&tmp1[0], &tmp2[0]));
  UNUSED(s21_round(tmp1[0]));
  UNUSED(s21_mod(tmp1[0], tmp2[0]));
}
#endif
