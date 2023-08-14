#include "s21_decimal.h"

// функция прошла вертера 26 03 2022. не трогаем.
s21_decimal s21_round(s21_decimal src) {
  s21_decimal res_round = src;
  int scale = get_scale(res_round);
  int scale_tmp = get_scale(res_round);
  if (res_round.value_type == s21_NORMAL_VALUE && scale > 0) {
    int znak = get_sign(res_round);  // запоминаем знак
    set_sign(&res_round, 0);         // убираем знак
    s21_decimal tmp = src;
    for (; scale > 1; scale--) {
      res_round = s21_decimal_div_ten(res_round);  // делим на 10 до вида 123,4
    }
    set_scale(&res_round, 1);
    // Заменить на mod
    for (; scale_tmp > 0; scale_tmp--) {
      tmp = s21_decimal_div_ten(tmp);  // делим на 10 до вида 123
    }
    set_scale(&tmp, 0);
    s21_decimal ostatok_mod = s21_sub(res_round, tmp);  // 123,4-123 остаток 0,4
    //
    set_scale(&ostatok_mod, 0);                  // Остаток вид 4
    res_round = s21_decimal_div_ten(res_round);  // делим на 10 до вида 123
    int ostatok_int = 0;
    if (s21_from_decimal_to_int(ostatok_mod, &ostatok_int) == 0) {
      if (ostatok_int > 4 && ostatok_int < 10) {
        s21_decimal decimal_one = CONST_DECIMAL_ONE;
        res_round = s21_add(res_round, decimal_one);
      }
    }
    set_sign(&res_round, znak);
  } else if (src.value_type != s21_NORMAL_VALUE) {
    set_zero(&res_round);
    res_round.value_type = src.value_type;
  }
  return res_round;
}

// функция прошла вертера 26 03 2022. не трогаем.
// Округляет указанное Decimal число до ближайшего целого числа в сторону
// отрицательной бесконечности.
s21_decimal s21_floor(s21_decimal src) {
  s21_decimal res = CONST_DEF_DECIMAL;
  if (res.value_type == s21_NORMAL_VALUE) {
    int znak = get_sign(src);
    res = s21_truncate(src);
    set_sign(&res, DEC_PLUS);
    s21_decimal one = CONST_DECIMAL_ONE;
    if (znak == DEC_MINUS && s21_is_not_equal(src, res) == TRUE) {
      res = s21_add(res, one);
    }
    if (s21_check_bits_zero(res) == FALSE) {
      set_sign(&res, znak);
    }
  } else {
    set_zero(&res);
  }
  return res;
}

// функция прошла вертера 26 03 2022. не трогаем.
// Возвращает результат умножения указанного Decimal на -1

s21_decimal s21_negate(s21_decimal dec) {
  s21_decimal res = dec;
  if (res.value_type == s21_NORMAL_VALUE) {
    if (get_sign(dec) == DEC_MINUS) {
      set_sign(&res, DEC_PLUS);
    } else {
      set_sign(&res, DEC_MINUS);
    }
  } else if (res.value_type == s21_NAN) {
    set_zero(&res);
    if (get_sign(dec) == DEC_MINUS) {
      set_sign(&res, DEC_PLUS);
    } else {
      set_sign(&res, DEC_MINUS);
    }
    res.value_type = s21_NAN;
  } else if (res.value_type == s21_INFINITY) {
    set_zero(&res);
    res = set_infinity(DEC_MINUS);
  } else if (res.value_type == s21_NEGATIVE_INFINITY) {
    set_zero(&res);
    res = set_infinity(DEC_PLUS);
  }
  return res;
}

// функция прошла вертера 26 03 2022. не трогаем.
//  Возвращает целые цифры указанного Decimal числа; любые дробные цифры
//  отбрасываются.

s21_decimal s21_truncate(s21_decimal src) {
  s21_decimal truncate = src;
  if (src.value_type == s21_NORMAL_VALUE) {
    int scale = get_scale(src);
    int znak = get_sign(src);
    if (scale > 0 && scale < 29) {
      for (; scale > 0; scale--) {
        truncate = s21_decimal_div_ten(truncate);
      }
    }
    set_sign(&truncate, znak);
  } else if (src.value_type == s21_INFINITY) {
    truncate = set_infinity(DEC_PLUS);
  } else if (src.value_type == s21_NAN) {
    set_zero(&truncate);
    truncate.value_type = s21_NAN;
  } else if (src.value_type == s21_NEGATIVE_INFINITY) {
    truncate = set_infinity(DEC_MINUS);
  }
  return truncate;
}
