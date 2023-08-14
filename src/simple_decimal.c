#include "s21_decimal.h"
#include "simple_decimal.h"

#include <stdarg.h>
#include <stdio.h>



// отлажено до проверок на +-INF, не менять основное!
int sd_sub(simple_decimal one, simple_decimal two, simple_decimal *__res, ...) {
  int ret = SD_OK;
  simple_decimal tmp_res;
  init_sd(&tmp_res);

  for (int i = 0; i < SZ_SD; i++) {
    tmp_res[i] = one[i] - two[i];
  }

  for (int i = 0; i < SZ_SD; i++) {
    if (tmp_res[i] < 0) {
      tmp_res[i] += 2;
      if (i + 1 < SZ_SD) {
        tmp_res[i + 1]--;
      } else {
        tmp_res[SZ_SD] = 1;
        ret = SD_OVERFLOW;
      }
    }
  }
  // если возращается последняя 1, это значит, что число отрицательное (?)
  // проверить.
  //
  // if (tmp_res[SZ_SD-1] > 0 /*|| get_first_1_position_from_end(tmp_res) >
  // MAX_BITS*/)
  // ret = SD_OVERFLOW;
  copy_sd(tmp_res, __res);
  return ret;
}

// отлажено полностью!
int sd_sum(simple_decimal one, simple_decimal two, simple_decimal *__res, ...) {
  int ret = SD_OK;
  simple_decimal tmp_res;
  init_sd(&tmp_res);

  for (int i = 0; i < SZ_SD; i++) {
    tmp_res[i] = one[i] + two[i];
  }
  for (int i = 0; i < SZ_SD; i++) {
    if (tmp_res[i] >= 2) {
      tmp_res[i] -= 2;
      if (i + 1 < SZ_SD) tmp_res[i + 1]++;
    }
  }
  if (tmp_res[SZ_SD - 1] >
      0 /* || get_first_1_position_from_end(tmp_res) > MAX_BITS*/)
    ret = SD_OVERFLOW;
  copy_sd(tmp_res, __res);
  return ret;
}

int sd_mul(simple_decimal one, simple_decimal two, simple_decimal *res, ...) {
  int ret = SD_OK;
  simple_decimal tmp, res_tmp;
  init_sd(&res_tmp);
  int one_first = get_first_1_position_from_end(one);
  int two_first = get_first_1_position_from_end(two);

  for (int i = 0; i <= two_first && ret == SD_OK; i++) {
    // copy_sd(res_tmp, res);
    init_sd(&tmp);
    for (int j = 0; j <= one_first; j++) {
      tmp[i + j] = one[j] * two[i];
    }
    ret = sd_sum(res_tmp, tmp, &res_tmp);
  }
  // sd_delete_first_0(&res_tmp);
  if (ret == SD_OK) copy_sd(res_tmp, res);

  return ret;
}

int sd_mod(simple_decimal one, simple_decimal two, simple_decimal *res, ...) {
  simple_decimal tmp_res;
  int count;
  return sd_div_int(one, two, &tmp_res, res, &count);
}

// устал и решил перписать методы sd_cut так, чтобы возращали не перевернутую
// последовальность а правильную
int sd_div_int(simple_decimal one, simple_decimal __two, simple_decimal *__res,
               simple_decimal *__remainder, int *count_numbers) {
  // printf("one  = ");print_sd(one);
  int ret = SD_OK;
  *count_numbers =
      0;  // количество цифр результата в десятичной системе. как считать незнаю.
  /* место для результата */
  simple_decimal tmp_res, tmp_rem, tmp_two;
  init_sd(&tmp_res);
  init_sd(&tmp_rem);
  /* количество цифр в делимом */
  int kol_vo_1 = get_non_zero_bits_count(one);
  /* количество цифр в делителе */
  int kol_vo_2 = get_non_zero_bits_count(__two);

  // в результате деления получается 0
  if (kol_vo_2 > kol_vo_1) {
    copy_sd(one, __remainder);
    init_sd(__res);
  } else {
    // теперь нужно из one взять столько цифр от 0 сколько есть цифр в делимом
    simple_decimal tmp_cutted_one;
    sd_cut(one, &tmp_cutted_one, kol_vo_1, kol_vo_2);
    // printf("tmp_cutted_one=");print_sd(tmp_cutted_one);
    copy_sd(__two, &tmp_two);
    /* стартовая позиция в делителе откуда будем брать цифры */
    int count_cicles = kol_vo_1 - kol_vo_2;

    while (count_cicles >= 0) {
      if (sd_is_grater_or_equal(tmp_cutted_one, tmp_two) == TRUE) {
        tmp_res[0] = 1;

        sd_sub(tmp_cutted_one, tmp_two, &tmp_cutted_one);
        (*count_numbers)++;
      } else {
        tmp_res[0] = 0;
      }
      kol_vo_2++;
      if (count_cicles > 0) {
        sd_mul(tmp_res, sd_2, &tmp_res);
        sd_mul(tmp_two, sd_2, &tmp_two);
        sd_mul(tmp_cutted_one, sd_2, &tmp_cutted_one);
        tmp_cutted_one[0] =
            one[kol_vo_1 - kol_vo_2];  // start_from_end_pos - 1 - i
        // printf("kol_vo_1 - 1 - kol_vo_2=%d, tmp_cutted_one2=", kol_vo_1 - 1 -
        // kol_vo_2);print_sd(tmp_cutted_one);
        copy_sd(__two, &tmp_two);
      }
      count_cicles--;
    }

    // invert_del_res_and_delete_minus_one(&tmp_res);
    //  printf("tmp_res = ");print_sd(tmp_res);
    sd_mul(__two, tmp_res, &tmp_rem);
    sd_sub(one, tmp_rem, &tmp_rem);
    copy_sd(tmp_res, __res);
    copy_sd(tmp_rem, __remainder);
    // printf("tmp_rem = ");print_sd(tmp_rem);
  }
  return ret;
}

int sd_div(simple_decimal one, simple_decimal two, simple_decimal *res, ...) {
  va_list args;
  va_start(args, res);
  int *scale = va_arg(args, int *);
  int *count_numbers = va_arg(args, int *);
  int scale_of_two = va_arg(args, int);
  *scale = 0;
  *count_numbers = 0;
  va_end(args);
  int div_after_div =
      0;  // для числе вроде 0.000 я не сохраню впереди нули, поэтому после
          // деления надо еще поделить на 10 столько, сколько ведущих нулейй
  int is_first = TRUE;
  int tmp_count_numbers;
  simple_decimal remainder;
  simple_decimal res_tmp;
  init_sd(&res_tmp);

  copy_sd(one, &remainder);
  int ret = SD_OK, ret3 = SD_OK, ret4 = SD_OK;
  while (sd_is_zero(remainder) != TRUE && ret == SD_OK &&
         (*scale) <= MAX_SCALE + 18) {
    simple_decimal res_tmp2;
    int ret1 =
        sd_div_int(remainder, two, &res_tmp2, &remainder, &tmp_count_numbers);

    if (is_first == TRUE) {
      if (tmp_count_numbers == 0) res_tmp2[FIRST_NOL] = TRUE;
      *count_numbers = tmp_count_numbers;
    }
    // if ((*scale) == 78)
    //     (*scale) = 78;
    int ret2 = sd_sum(res_tmp2, res_tmp, &res_tmp);

    if (sd_is_zero(res_tmp) == TRUE) div_after_div++;

    if (sd_is_zero(remainder) != TRUE) {
      if ((*scale) < MAX_SCALE + 10) {
        ret3 = sd_mul(res_tmp, sd_10, &res_tmp);
        ret4 = sd_mul(remainder, sd_10, &remainder);
      }
      (*scale)++;
    }

    if (ret1 != SD_OK || ret2 != SD_OK || ret3 != SD_OK || ret4 != SD_OK) {
      ret = SD_OVERFLOW;
    }
    is_first = FALSE;
  }
  /*if (div_after_div > 0)
      div_after_div--;
  if(scale_of_two > 0)
      div_after_div = 0;*/
  // for (int i = 0; i < div_after_div; i++) {
  // sd_div_int(res_tmp, sd_10, &res_tmp, &remainder, &tmp_count_numbers);
  //    sd_mul(res_tmp, sd_10, &res_tmp);
  //}
  for (int i = 0; i < scale_of_two - div_after_div; i++) {
    sd_mul(res_tmp, sd_10, &res_tmp);
    // (*count_numbers)++;
  }
  /*if (scale_of_two == 0 &&  div_after_div != 0)
      (*count_numbers) = div_after_div;
  if (*count_numbers > MAX_BITS)
      ret = SD_OVERFLOW;
  else
      ret = SD_OK;*/
  copy_sd(res_tmp, res);

  // print_sd(*res);
  //  pint_sd_numbers(*res);
  return ret;
}

s21_decimal s21_from_sd(int (*func)(simple_decimal, simple_decimal,
                                    simple_decimal *, ...),
                        s21_decimal one, s21_decimal two, int need_scale,
                        int result_sign) {
  s21_decimal res_decimal = CONST_DEF_DECIMAL;
  simple_decimal sd_one, sd_two, res_sd, tmp2_src;
  // simple_decimal tmp_0_1 = simple_decimal_10;
  int error;
  int scale = -1;
  int count_numbers = -1;

  if (one.value_type != s21_NORMAL_VALUE ||
      two.value_type != s21_NORMAL_VALUE) {
    error = SD_OVERFLOW;
  } else {
    init_sd(&sd_one);
    init_sd(&sd_two);
    init_sd(&res_sd);
    from_decimal(one, &sd_one);
    from_decimal(two, &sd_two);

    while (((error = func(sd_one, sd_two, &res_sd, &scale, &count_numbers,
                          get_scale(two))) != SD_OK) &&
           need_scale > 0 && (get_scale(one) > 0 || get_scale(two) > 0)) {
      // printf("need_scale = %d; get_scale(one)=%d; get_scale(two)=%d\n\n",
      // need_scale, get_scale(one), get_scale(two)); print_sd(res_sd);
      // printf("\n");
      if (get_scale(one) > 0) {
        // print_sd(res_sd);
        sd_mul(sd_one, sd_10, &tmp2_src);
        // print_sd(tmp2_src);
        copy_sd(tmp2_src, &sd_one);
        set_scale(&one, get_scale(one) - 1);
      } else {
        sd_mul(sd_two, sd_10, &tmp2_src);
        copy_sd(tmp2_src, &sd_two);
        set_scale(&two, get_scale(two) - 1);
      }
      need_scale--;
    }
  }

  if (error == SD_OK) {
    // это было деление
    // if (need_scale == -1)
    //    need_scale = scale;
    int added_need_scale = FALSE;
    if (need_scale == -1 && scale != -1 && count_numbers != -1) {
      if (count_numbers == 0) {
        count_numbers = 1;
        added_need_scale = TRUE;
      }
      if (scale + count_numbers > MAX_SCALE + 1)
        need_scale = MAX_SCALE + 1 - count_numbers;
      else
        need_scale = scale;
    } else if (count_numbers == -1) {
      count_numbers = 0;
    }
    // print_sd(res_sd);

    int podelil =
        to_decimal(&res_decimal, &res_sd, need_scale, added_need_scale);
    if (!(scale != -1 && count_numbers != -1))
      need_scale = need_scale - podelil;

    if (need_scale < 0) {
      res_decimal = set_infinity(result_sign);
    } else {
      if (need_scale > MAX_SCALE) need_scale = MAX_SCALE;

      if (is_zero(res_decimal) == FALSE) {
        set_scale(&res_decimal, need_scale);
      }
      set_sign(&res_decimal, result_sign);
    }
  }
  return res_decimal;
}

int sd_is_grater_or_equal(simple_decimal one, simple_decimal two) {
  int ret = FALSE;
  simple_decimal res;
  sd_sub(one, two, &res);
  if (get_sign_after_sub(res) == DEC_PLUS || sd_is_zero(res) == TRUE)
    ret = TRUE;
  return ret;
}

s21_decimal s21_sub_from_sd(s21_decimal one, s21_decimal two) {
  s21_decimal res;
  if (one.value_type == s21_NORMAL_VALUE &&
      two.value_type == s21_NORMAL_VALUE) {
    int is_stop = FALSE;
    res = check_s21_sub_from_sd(one, two, &is_stop);
    if (is_stop != TRUE) {
      int sign_one = get_sign(one);
      int sign_two = get_sign(two);
      if (sign_one != sign_two) {
        set_sign(&two, sign_one);
        res = s21_sum_from_sd(one, two);
      } else {
        alignment_scale(&one, &two);
        set_sign(&one, DEC_PLUS);
        set_sign(&two, DEC_PLUS);
        if (s21_is_less(one, two) == TRUE) {
          int tmp_sign = DEC_PLUS;
          if (sign_two == DEC_PLUS) tmp_sign = DEC_MINUS;

          res = s21_from_sd(sd_sub, two, one, get_scale(one), tmp_sign);
        } else {
          res = s21_from_sd(sd_sub, one, two, get_scale(one), sign_one);
          // set_sign(&res, sign_one);
        }
      }
    }
  } else {
    res = s21_nan_sub(one, two);
  }
  return res;
}

s21_decimal s21_sum_from_sd(s21_decimal one, s21_decimal two) {
  s21_decimal res;
  if (get_sign(one) != get_sign(two)) {
    set_sign(&two, get_sign(one));
    res = s21_sub_from_sd(one, two);
  } else {
    alignment_scale(&one, &two);
    res = s21_from_sd(sd_sum, one, two, get_scale(one), get_sign(one));
    // set_sign(&res, get_sign(one));
  }
  return res;
}

s21_decimal s21_mul_from_sd(s21_decimal one, s21_decimal two) {
  s21_decimal res;
  if (one.value_type == s21_NORMAL_VALUE &&
      two.value_type == s21_NORMAL_VALUE) {
    int result_sign = DEC_PLUS;
    if (get_sign(one) != get_sign(two)) result_sign = DEC_MINUS;

    res = s21_from_sd(sd_mul, one, two, get_scale(one) + get_scale(two),
                      result_sign);
    /*if (get_sign (one) != get_sign(two))
        set_sign(&res, DEC_MINUS);
    else
        set_sign(&res, DEC_PLUS);*/
  } else {
    res = s21_nan_mul(one, two);
  }
  return res;
}

s21_decimal s21_div_from_sd(s21_decimal one, s21_decimal two) {
  s21_decimal res = CONST_DEF_DECIMAL;
  if (s21_is_equal(res, one) == TRUE && s21_is_equal(res, two) == TRUE) {
    res.value_type = s21_NAN;
  } else if (one.value_type == s21_NORMAL_VALUE && get_sign(one) == DEC_PLUS &&
             s21_is_equal(res, two) == TRUE) {
    res.value_type = s21_INFINITY;
  } else if (one.value_type == s21_NORMAL_VALUE && get_sign(one) == DEC_MINUS &&
             s21_is_equal(res, two) == TRUE) {
    res.value_type = s21_NEGATIVE_INFINITY;
  } else if (one.value_type == s21_NORMAL_VALUE &&
             two.value_type == s21_NORMAL_VALUE) {
    int result_sign = DEC_PLUS;
    if (get_sign(one) != get_sign(two)) result_sign = DEC_MINUS;

    res = s21_from_sd(sd_div, one, two, -1, result_sign);
    /*
        if (get_sign (one) != get_sign(two))
            set_sign(&res, DEC_MINUS);
        else
            set_sign(&res, DEC_PLUS);
    */
  } else {
    res = s21_nan_div(one, two);
  }
  return res;
}

s21_decimal s21_mod_from_sd(s21_decimal one, s21_decimal two) {
  s21_decimal res = CONST_DEF_DECIMAL;
  int result_sign = DEC_PLUS;
  if (s21_is_equal(res, two) == TRUE) {
    res.value_type = s21_NAN;
  } else if (one.value_type == s21_NORMAL_VALUE &&
             two.value_type == s21_NORMAL_VALUE) {
    if (get_sign(one) == DEC_MINUS) result_sign = DEC_MINUS;
    res = s21_from_sd(sd_mod, one, two, MAX(get_scale(one), get_scale(two)),
                      result_sign);

    /*if (get_sign (one) == DEC_MINUS)
        set_sign(&res, DEC_MINUS);
    else
        set_sign(&res, DEC_PLUS);
        */
  } else {
    res = s21_nan_mod(one, two);
  }

  return res;
}
