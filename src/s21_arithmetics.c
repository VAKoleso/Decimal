#include "s21_decimal.h"
#include "simple_decimal.h"

// функция прошла вертера 26 03 2022. не трогаем.
s21_decimal s21_add(s21_decimal one, s21_decimal two) {
  int overflow_scale = 0;
  return s21_add_overflow(one, two, &overflow_scale);
}

// функция прошла вертера 26 03 2022. не трогаем.
s21_decimal s21_add_overflow(s21_decimal one, s21_decimal two,
                             int *overflow_scale) {
  s21_decimal result = CONST_DEF_DECIMAL;
  s21_decimal a1 = one;
  s21_decimal b1 = two;
  if (get_scale(a1) != get_scale(b1)) {
    alignment_scale(&a1, &b1);
  }
  int sc = get_scale(a1);
  if (one.value_type == s21_NORMAL_VALUE &&
      two.value_type == s21_NORMAL_VALUE) {
    if (get_sign(one) == get_sign(two)) {
      int next = 0;
      for (int i = 0; i < MAX_BITS; i++) {
        int a_bits = get_bit(a1, i);
        int b_bits = get_bit(b1, i);
        if ((a_bits == 0) && (b_bits == 0)) {
          if (next == 1) {
            set_bit(&result, i, 1);
            next = 0;
          } else {
            set_bit(&result, i, 0);
          }
        }
        if (a_bits ^ b_bits) {
          if (next == 1) {
            set_bit(&result, i, 0);
            next = 1;
          } else {
            set_bit(&result, i, 1);
          }
        }
        if (a_bits & b_bits) {
          if (next == 1) {
            set_bit(&result, i, 1);
          } else {
            set_bit(&result, i, 0);
          }
          next = 1;
        }
        if (i == (MAX_BITS - 1) &&
            (((a_bits == 1) || (b_bits == 1)) && (next == 1))) {
          if (*overflow_scale > 0) {
            a1 = s21_decimal_div_ten(a1);
            b1 = s21_decimal_div_ten(b1);
            (*overflow_scale)--;
            result = s21_add_overflow(a1, b1, overflow_scale);
          } else {
            set_zero(&result);
            result.value_type = s21_INFINITY;
          }
        }
      }
      if (get_sign(one) == DEC_MINUS) {
        set_sign(&result, DEC_MINUS);
        if (result.value_type == s21_INFINITY) {
          set_sign(&result, DEC_PLUS);
          result.value_type = s21_NEGATIVE_INFINITY;
        }
      } else {
        set_sign(&result, DEC_PLUS);
      }
    }
    if (get_sign(one) != get_sign(two)) {
      if (get_sign(one) == DEC_PLUS) {
        set_sign(&two, DEC_PLUS);
        result = s21_sub(one, two);
      } else {
        set_sign(&one, DEC_PLUS);
        result = s21_sub(two, one);
      }
    }
  } else {
    if ((one.value_type == s21_NEGATIVE_INFINITY) ||
        (two.value_type == s21_NEGATIVE_INFINITY)) {
      result.value_type = s21_NEGATIVE_INFINITY;
    }
    if ((one.value_type == s21_INFINITY) || (two.value_type == s21_INFINITY)) {
      result.value_type = s21_INFINITY;
      if ((one.value_type == s21_NEGATIVE_INFINITY) ||
          (two.value_type == s21_NEGATIVE_INFINITY)) {
        result.value_type = s21_NAN;
      }
    }
    if ((one.value_type == s21_NAN) || (two.value_type == s21_NAN)) {
      result.value_type = s21_NAN;
    }
  }
  set_scale(&result, sc);

  return result;
}

// функция прошла вертера 26 03 2022. не трогаем.
// Вычитание
s21_decimal s21_sub(s21_decimal one_t, s21_decimal two_t) {
  s21_decimal one = one_t;
  s21_decimal two = two_t;

  s21_decimal result = CONST_DEF_DECIMAL;
  if (s21_is_less(one, two) == TRUE) {
    one = two_t;
    two = one_t;
  }

  if (get_scale(one) != get_scale(two)) {
    alignment_scale(&one, &two);
  }

  s21_decimal decimal_not = s21_not(one);
  s21_decimal decimal_hor = s21_exclude_or(&one, &two);
  s21_decimal decimal_and = s21_and(&decimal_not, &two);
  s21_decimal decimal_shifted = shift(decimal_and, 1, DIRECT_LEFT);

  if (one_t.value_type == s21_NAN || two_t.value_type == s21_NAN) {
    result.value_type = s21_NAN;
  } else if (one_t.value_type == s21_NEGATIVE_INFINITY &&
             two_t.value_type == s21_NEGATIVE_INFINITY) {
    result.value_type = s21_NAN;
  } else if (one_t.value_type == s21_INFINITY &&
             two_t.value_type == s21_INFINITY) {
    result.value_type = s21_NAN;
  } else if (one_t.value_type == s21_NEGATIVE_INFINITY &&
             two_t.value_type == s21_INFINITY) {
    result.value_type = s21_NEGATIVE_INFINITY;
  } else if (one_t.value_type == s21_INFINITY &&
             two_t.value_type == s21_NEGATIVE_INFINITY) {
    result.value_type = s21_INFINITY;
  } else if (one_t.value_type == s21_NEGATIVE_INFINITY &&
             two_t.value_type != s21_NEGATIVE_INFINITY &&
             two.value_type != s21_NAN) {
    result.value_type = s21_NEGATIVE_INFINITY;
  } else if (one_t.value_type == s21_INFINITY &&
             two_t.value_type != s21_INFINITY && two.value_type != s21_NAN) {
    result.value_type = s21_INFINITY;
  } else if (one_t.value_type == s21_NORMAL_VALUE &&
             two_t.value_type == s21_NEGATIVE_INFINITY) {
    result.value_type = s21_INFINITY;
  } else if (two_t.value_type == s21_INFINITY) {
    result.value_type = s21_NEGATIVE_INFINITY;
  } else {
    if (is_zero(two_t) == 0) {
      result = one_t;
    } else if (get_sign(one) != get_sign(two)) {
      s21_decimal tmp_one = one;
      set_sign(&tmp_one, DEC_PLUS);
      s21_decimal tmp_two = two;
      set_sign(&tmp_two, DEC_PLUS);
      result = s21_add(tmp_one, tmp_two);
      if (s21_is_less(one_t, two_t) == TRUE && get_sign(one_t) == DEC_MINUS &&
          get_sign(two_t) == DEC_PLUS) {
        set_sign(&result, DEC_MINUS);
      }
      if (result.value_type == s21_INFINITY &&
          s21_is_less(one_t, two_t) == TRUE)
        result.value_type = s21_NEGATIVE_INFINITY;
    } else {
      result = s21_sub(decimal_hor, decimal_shifted);
      if (s21_is_greater(one_t, two_t) == FALSE &&
          get_sign(one_t) == DEC_PLUS && get_sign(two_t) == DEC_PLUS) {
        set_sign(&result, DEC_MINUS);
      }
    }
    set_scale(&result, get_scale(one));
    if (result.value_type == s21_INFINITY ||
        result.value_type == s21_NEGATIVE_INFINITY)
      set_sign(&result, DEC_PLUS);
  }
  return result;
}

// не прошла проверку
s21_decimal s21_mul(s21_decimal unm_one, s21_decimal unm_two) {
  return s21_mul_from_sd(unm_one, unm_two);
}

// не прошла проверку
s21_decimal s21_mod(s21_decimal one, s21_decimal two) {
  return s21_mod_from_sd(one, two);
}

// не прошла проверку
s21_decimal s21_div(s21_decimal one, s21_decimal two) {
  alignment_scale(&one, &two);
  return s21_div_from_sd(one, two);
}
