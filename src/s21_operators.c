#include "s21_decimal.h"

// Оператор "||"

s21_decimal s21_or(s21_decimal *one, s21_decimal *two) {
  s21_decimal result = CONST_DEF_DECIMAL;
  for (int i = 0; i < MAX_BITS; i++) {
    set_bit(&result, i, get_bit(*one, i) | get_bit(*two, i));
  }
  return result;
}

// Оператор "^"

s21_decimal s21_exclude_or(s21_decimal *one, s21_decimal *two) {
  s21_decimal result = CONST_DEF_DECIMAL;
  for (int i = 0; i < 96; i++) {
    set_bit(&result, i, get_bit(*one, i) ^ get_bit(*two, i));
  }
  return result;
}

// Оператор "~"

s21_decimal s21_not(s21_decimal one) {
  s21_decimal result = CONST_DEF_DECIMAL;
  for (int i = 0; i < 96; i++) {
    if (get_bit(one, i) > 0) {
      set_bit(&result, i, 0);
    } else {
      set_bit(&result, i, 1);
    }
  }
  if (get_sign(one) == DEC_MINUS) {
    set_sign(&result, DEC_PLUS);
  } else {
    set_sign(&result, DEC_MINUS);
  }
  return result;
}

s21_decimal s21_and(s21_decimal *one, s21_decimal *two) {
  s21_decimal result = CONST_DEF_DECIMAL;
  for (int i = 0; i < MAX_BITS; i++) {
    set_bit(&result, i, get_bit(*one, i) & get_bit(*two, i));
  }
  return result;
}

// Смещение

s21_decimal shift_overflow(s21_decimal dec, int to_shift, int direction,
                           int control_overflow) {
  s21_decimal result = CONST_DEF_DECIMAL;
  set_sign(&result, get_sign(dec));
  set_scale(&result, get_scale(dec));
  if (dec.value_type != s21_NORMAL_VALUE || to_shift == 0) {
    copy(dec, &result);
  } else {
    int number_not_zero_bit = 95;
    for (int i = 95; i > 0 && get_bit(dec, i) == 0; i--) {
      number_not_zero_bit = i;
    }
    if (number_not_zero_bit + to_shift >= 96 && direction == DIRECT_LEFT &&
        control_overflow != TRUE) {
      if (get_sign(dec) == DEC_MINUS) {
        result.value_type = s21_NEGATIVE_INFINITY;
        set_sign(&result, DEC_MINUS);
      } else {
        result.value_type = s21_INFINITY;
        set_sign(&result, DEC_PLUS);
      }
    } else {
      int current_bit = number_not_zero_bit;
      while (current_bit >= 0) {
        int tmp = get_bit(dec, current_bit);
        set_bit(&result, current_bit + direction * to_shift, tmp);
        current_bit--;
      }
    }
  }
  return result;
}

s21_decimal shift(s21_decimal dec, int to_shift, int direction) {
  return shift_overflow(dec, to_shift, direction, FALSE);
}
