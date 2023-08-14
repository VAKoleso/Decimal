#include "s21_decimal.h"

// Равно
// функция прошла вертера 26 03 2022. не трогаем.
int s21_is_equal(s21_decimal one, s21_decimal two) {
  int res = FALSE;
  if ((one.value_type == s21_INFINITY && two.value_type == s21_INFINITY) ||
      (one.value_type == s21_NEGATIVE_INFINITY &&
       two.value_type == s21_NEGATIVE_INFINITY)) {
    res = TRUE;
  } else if (one.value_type == s21_INFINITY || two.value_type == s21_INFINITY ||
             one.value_type == s21_NEGATIVE_INFINITY ||
             two.value_type == s21_NEGATIVE_INFINITY ||
             one.value_type == s21_NAN || two.value_type == s21_NAN) {
    res = FALSE;
  } else if (is_zero(one) == TRUE && is_zero(two) == TRUE) {
    res = TRUE;
  } else if (get_sign(one) != get_sign(two)) {
    res = FALSE;
  } else {
    res = TRUE;
    s21_decimal left = one;
    s21_decimal right = two;
    if (get_scale(left) != get_scale(right)) {
      alignment_scale(&left, &right);
    }
    for (int i = 95; i >= 0 && res == TRUE; i--) {
      int left_bit = get_bit(left, i);
      int right_bit = get_bit(right, i);
      if (left_bit != right_bit) {
        res = FALSE;
      }
    }
  }
  return res;
}

// функция прошла вертера 26 03 2022. не трогаем.
//  Не равно
int s21_is_not_equal(s21_decimal one, s21_decimal two) {
  int res = TRUE;
  if (s21_is_equal(one, two) == TRUE) {
    res = FALSE;
  }
  return res;
}

// не прошли
// Больше или равно
int s21_is_greater_or_equal(s21_decimal one, s21_decimal two) {
  int res = FALSE;
  if (s21_is_greater(one, two) == TRUE || s21_is_equal(one, two) == TRUE) {
    res = TRUE;
  }
  return res;
}

// Больше
int s21_is_greater(s21_decimal one, s21_decimal two) {
  s21_decimal left = one;
  s21_decimal right = two;
  int res = FALSE;
  int sign_left = get_sign(left);
  int sign_right = get_sign(right);

  if ((left.value_type == s21_INFINITY && right.value_type == s21_INFINITY) ||
      (left.value_type == s21_NEGATIVE_INFINITY &&
       right.value_type == s21_NEGATIVE_INFINITY) ||
      (left.value_type == s21_NAN || right.value_type == s21_NAN)) {
    res = FALSE;
  } else if ((left.value_type != s21_NEGATIVE_INFINITY &&
              right.value_type == s21_NEGATIVE_INFINITY) ||
             (left.value_type == s21_INFINITY &&
              right.value_type != s21_INFINITY)) {
    res = TRUE;
  } else if ((left.value_type != s21_INFINITY &&
              right.value_type == s21_INFINITY) ||
             (left.value_type == s21_NEGATIVE_INFINITY &&
              right.value_type != s21_NEGATIVE_INFINITY)) {
    res = FALSE;
  } else if (is_zero(one) == TRUE && is_zero(two) == TRUE) {
    res = FALSE;
  } else if (sign_left == DEC_PLUS && sign_right == DEC_MINUS) {
    res = TRUE;
  } else if (sign_left == DEC_MINUS && sign_right == DEC_PLUS) {
    res = FALSE;
  } else if (sign_left == sign_right) {  // если знаки равны проходим по битам
    if (get_scale(left) != get_scale(right)) {
      alignment_scale(&left, &right);
    }
    int flag = FALSE;
    for (int i = MAX_BITS - 1; i >= 0; i--) {  // начинаем со старших
      int left_bit = get_bit(left, i);
      int right_bit = get_bit(right, i);
      if (left_bit == 1 && right_bit == 0) {  // если в левом числе бит = 1
        res = TRUE;
        i = 0;
        flag = TRUE;
      } else if (left_bit == 0 && right_bit == 1) {
        res = FALSE;
        i = 0;
        flag = TRUE;
      }
    }
    if (sign_left == DEC_MINUS &&
        flag == TRUE) {  // если знаки минусы, то большее меньше и наоборот
      if (res == FALSE) {
        res = TRUE;
      } else {
        res = FALSE;
      }
    }
  }
  return res;
}

// Меньше
int s21_is_less(s21_decimal one, s21_decimal two) {
  s21_decimal left = one;
  s21_decimal right = two;
  int res = FALSE;
  int sign_left = get_sign(left);
  int sign_right = get_sign(right);
  if (left.value_type == s21_NEGATIVE_INFINITY &&
      (right.value_type == s21_NORMAL_VALUE ||
       right.value_type == s21_INFINITY)) {
    res = TRUE;
  } else if ((left.value_type == s21_NAN || right.value_type == s21_NAN) ||
             ((left.value_type == s21_NORMAL_VALUE ||
               left.value_type == s21_INFINITY) &&
              right.value_type == s21_NEGATIVE_INFINITY)) {
    res = FALSE;
  } else if (is_zero(one) == TRUE && is_zero(two) == TRUE) {
    res = FALSE;
  } else if (sign_left == DEC_MINUS && sign_right == DEC_PLUS) {
    res = TRUE;
  } else if (sign_left == DEC_PLUS && sign_right == DEC_MINUS) {
    res = FALSE;
  } else {
    res = !s21_is_greater_or_equal(one, two);
  }
  return res;
}

// Меньше или равно
int s21_is_less_or_equal(s21_decimal one, s21_decimal two) {
  int res = FALSE;
  if (s21_is_less(one, two) == TRUE || s21_is_equal(one, two) == TRUE) {
    res = TRUE;
  }
  return res;
}
