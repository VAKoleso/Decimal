#include <check.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "../s21_decimal.h"
#include "../simple_decimal.h"

#define comparison_test_count 18

static char *char_boolean[] = {"TRUE", "FALSE"};

#define count_decimals 11
s21_decimal decimals[] = {
    {{0, 0, 0, 0}, s21_NORMAL_VALUE},
    {{0, 0, 0, 0x80000000}, s21_NORMAL_VALUE},
    {{5, 0, 0, 0}, s21_NORMAL_VALUE},
    {{6, 0, 0, 0x80010000}, s21_NORMAL_VALUE},
    {{0x7fffffff, 0, 0, 0}, s21_NORMAL_VALUE},
    {{0x7fffffff, 0, 0, 0x80000000}, s21_NORMAL_VALUE},
    {{0x7fffffff, 0, 0, 0x80000000}, s21_NAN},
    {{0x7fffffff, 0, 0, 0x80000000}, s21_INFINITY},
    {{0x7fffffff, 0, 0, 0x00000000}, s21_NEGATIVE_INFINITY},
    {{0x7fffffff, 0, 0, 0x00000000}, s21_NEGATIVE_INFINITY},
    {{0xffffffff, 0xffffffff, 0xffffffff, 0x801C0000}, s21_NORMAL_VALUE},
};
void print_comparision_error(s21_decimal dec1, s21_decimal dec2, char *f_name,
                             int i, int res, int need_res);
void print_error2(s21_decimal dec1, s21_decimal res_dec, s21_decimal etalon_dec,
                  char *f_name, int i);
void print_error4(s21_decimal dec1, s21_decimal dec2, s21_decimal res_dec,
                  s21_decimal etalon_dec, char *f_name, int i);

START_TEST(test_s21_from_float_to_decimal) {
  const int count_tests = 12;
  float test[] = {0,
                  5,
                  -0.6,
                  INT_MAX,
                  -INT_MAX,
                  NAN,
                  INFINITY,
                  -INFINITY,
                  -7.9228162514264337593543950335,
                  0,
                  -39614081257132168796771975168.,
                  79228162514264337593543950334.};  // , FLT_MIN, FLT_MAX,
                                                    // -FLT_MIN, -FLT_MAX};

  s21_decimal res[] = {
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},
      {{5, 0, 0, 0}, s21_NORMAL_VALUE},
      {{6, 0, 0, 0x80010000}, s21_NORMAL_VALUE},
      {{(float)INT_MAX, 0, 0, 0}, s21_NORMAL_VALUE},
      {{(float)INT_MAX, 0, 0, 0x80000000}, s21_NORMAL_VALUE},
      {{0, 0, 0, 0}, s21_NAN},
      {{0x7fffffff, 0, 0, 0x80000000}, s21_INFINITY},
      {{0x7fffffff, 0, 0, 0x80000000}, s21_NEGATIVE_INFINITY},
      {{79228160, 0, 0, 0x80070000}, s21_NORMAL_VALUE},
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},
      {{0x7fffffff, 0x7fffffff, 0x7fffffff, 0x80000000}, s21_NORMAL_VALUE},
      {{0, 0, 0, 0x80000000}, s21_INFINITY},
  };
  int s21_FROM_FLOAT_to_decimal_res[] = {
      SUCCESS,          SUCCESS,          SUCCESS,          SUCCESS,
      SUCCESS,          CONVERTING_ERROR, CONVERTING_ERROR, CONVERTING_ERROR,
      CONVERTING_ERROR, SUCCESS,          SUCCESS};
  int s21_FROM_DEC_to_float_res[] = {
      SUCCESS, SUCCESS,          SUCCESS,          SUCCESS,
      SUCCESS, CONVERTING_ERROR, CONVERTING_ERROR, CONVERTING_ERROR,
      SUCCESS, SUCCESS,          SUCCESS};

  //  s21_FROM_DECIMAL_to_float
  for (int i = 0; i < count_tests - 1; i++) {
    float tmp;
    int ret = s21_from_decimal_to_float(res[i], &tmp);
    if (ret != s21_FROM_DEC_to_float_res[i] ||
        ((fabsf(tmp - test[i]) > 0.1) && !(isnan(tmp) && isnan(test[i])))) {
      print_decimal(res[i]);

      ck_abort_msg(
          "s21_from_decimal_to_float check error. Test number = %d. Output "
          "float = %f, fabsf(tmp - test[i]) = %f",
          i, tmp, fabsf(tmp - test[i]));
    }
  }

  //  s21_FROM_FLOAT_to_decimal
  for (int i = 0; i < count_tests - 2; i++) {
    s21_decimal tmp = CONST_DEF_DECIMAL;
    int ret = s21_from_float_to_decimal(test[i], &tmp);
    if (ret != s21_FROM_FLOAT_to_decimal_res[i] ||
        is_all_field_is_equal(tmp, res[i]) == FALSE) {
      if (s21_is_equal(tmp, res[i]) == FALSE) {
        print_decimal(res[i]);
        print_decimal(tmp);
        ck_abort_msg("s21_from_float_to_decimal check error");
      }
    }
  }
}
END_TEST

// ТЕСТ Сложения
// 1. 5 + 3
// 2. 3 + 5
// 3. -5 + 3
// 4. -3 + 5
// 5. 5 + (-3)
// 6. 3 + (-5)
// 7. -5 + (-3)
// 8. -3 + (-5)
// 9. 5.3 + 3.5
// 10. 3.5 + 5.3
// 11. -5.3 + 3.5
// 12. -3.5 + 5.3
// 13. 5.3 + (-3.5)
// 14. 3.5 + (-5.3)
// 15. -5.3 + (-3.5)
// 16. -3.5 + (-5.3)
// 17. 5 + 0
// 18. -5 + 0
// 19. -5.3 + 0
// 20. 5.3 + 0
// 21. 0 + 5
// 22. 0 + (-5)
// 23. 0 + 5.3
// 24. 0 + (-5.3)

// 25. Проверка 0 + 0
// 26. Проверка 0  + 12345
// 27. Проверка на scale
// 28. Сложение отрицательного числа и положительного
// 29. Сложение отрицательного числа с отрицательным
// 30. Сложение 0 и числа с большим скейлом
// 31. Сложение отрицательного числа с большим скейлом с положительным числом с
// большим скейлом
// 32. Сложение s21_INFINITY и s21_NEGATIVE_INFINITY
// 33. Проверка на переполнение в s21_INFINITY
// 34. Проверка на переполнение в s21_NEGATIVE_INFINITY
// 35. NAN + NAN
// 36. NAN + INFINITY
// 37. NAN + NEGATIVE_INFINITY
// 38. NEGATIVE_INFINITY + NAN
// 39. NEGATIVE_INFINITY + INFINITY
// 40. NEGATIVE_INFINITY + NEGATIVE_INFINITY
// 41. INFINITY + NAN
// 42. INFINITY + NEGATIVE_INFINITY
// 43. INFINITY + INFINITY
// 44. Проверка на переполнение с округлением скейл
// 45. 12345 + INFINITY
// 46. 12345 + NEGATIVE_INFINITY
// 47. 12345 + NAN
// 48. NEGATIVE_INFINITY + 12345
// 49. INFINITY + 12345
// 50. NAN + 12345

START_TEST(test_s21_add) {
  s21_decimal tmp1[] = {
      {{5, 0, 0, 0}, s21_NORMAL_VALUE},                             // 1
      {{3, 0, 0, 0}, s21_NORMAL_VALUE},                             // 2
      {{5, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                    // 3
      {{3, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                    // 4
      {{5, 0, 0, 0}, s21_NORMAL_VALUE},                             // 5
      {{3, 0, 0, 0}, s21_NORMAL_VALUE},                             // 6
      {{5, 0, 0, 0x8000000}, s21_NORMAL_VALUE},                     // 7
      {{3, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                    // 8
      {{53, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                   // 9
      {{35, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                   // 10
      {{53, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                   // 11
      {{35, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                   // 12
      {{53, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                   // 13
      {{35, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                   // 14
      {{53, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                   // 15
      {{35, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                   // 16
      {{5, 0, 0, 0}, s21_NORMAL_VALUE},                             // 17
      {{5, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                    // 18
      {{53, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                   // 19
      {{53, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                   // 20
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 21
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 22
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 23
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 24
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 25
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 26
      {{12345, 0, 0, 0x00020000}, s21_NORMAL_VALUE},                // 27
      {{12345, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                // 28
      {{12345, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                // 29
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 30
      {{0x7fffffff, 0x7fffffff, 0, 0x800c0000}, s21_NORMAL_VALUE},  // 31
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 32
      {{0xffffffff, 0xffffffff, 0xffffffff, 0}, s21_NORMAL_VALUE},  // 33
      {{0xffffffff, 0xffffffff, 0xffffffff, 0x80000000},
       s21_NORMAL_VALUE},                                           // 34
      {{0, 0, 0, 0}, s21_NAN},                                      // 35
      {{0, 0, 0, 0}, s21_NAN},                                      // 36
      {{0, 0, 0, 0}, s21_NAN},                                      // 37
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 38
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 39
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 40
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 41
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 42
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 43
      {{0xFFFFFFFE, 0xffffffff, 0xffffffff, 0}, s21_NORMAL_VALUE},  // 44
      {{12345, 0, 0, 0}, s21_NORMAL_VALUE},                         // 45
      {{12345, 0, 0, 0}, s21_NORMAL_VALUE},                         // 46
      {{12345, 0, 0, 0}, s21_NORMAL_VALUE},                         // 47
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 48
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 49
      {{0, 0, 0, 0}, s21_NAN},                                      // 50
  };

  s21_decimal tmp2[] = {
      {{3, 0, 0, 0}, s21_NORMAL_VALUE},                             // 1
      {{5, 0, 0, 0}, s21_NORMAL_VALUE},                             // 2
      {{3, 0, 0, 0}, s21_NORMAL_VALUE},                             // 3
      {{5, 0, 0, 0}, s21_NORMAL_VALUE},                             // 4
      {{3, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                    // 5
      {{5, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                    // 6
      {{3, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                    // 7
      {{5, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                    // 8
      {{35, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                   // 9
      {{53, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                   // 10
      {{35, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                   // 11
      {{53, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                   // 12
      {{35, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                   // 13
      {{53, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                   // 14
      {{35, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                   // 15
      {{53, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                   // 16
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 17
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 18
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 19
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 20
      {{5, 0, 0, 0}, s21_NORMAL_VALUE},                             // 21
      {{5, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                    // 22
      {{53, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                   // 23
      {{53, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                   // 24
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 25
      {{12345, 0, 0, 0}, s21_NORMAL_VALUE},                         // 26
      {{12345, 0, 0, 0x00030000}, s21_NORMAL_VALUE},                // 27
      {{24690, 0, 0, 0x00000000}, s21_NORMAL_VALUE},                // 28
      {{12345678, 0, 0, 0x80000000}, s21_NORMAL_VALUE},             // 29
      {{0x7fffffff, 0x7fffffff, 0, 0x000e0000}, s21_NORMAL_VALUE},  // 30
      {{0x7fffffff, 0x7fffffff, 0, 0x000e0000}, s21_NORMAL_VALUE},  // 31
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 32
      {{10, 0, 0, 0}, s21_NORMAL_VALUE},                            // 33
      {{50, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                   // 34
      {{0, 0, 0, 0}, s21_NAN},                                      // 35
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 36
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 37
      {{0, 0, 0, 0}, s21_NAN},                                      // 38
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 39
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 40
      {{0, 0, 0, 0}, s21_NAN},                                      // 41
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 42
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 43
      {{6, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                    // 44
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 45
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 46
      {{0, 0, 0, 0}, s21_NAN},                                      // 47
      {{12345, 0, 0, 0}, s21_NORMAL_VALUE},                         // 48
      {{12345, 0, 0, 0}, s21_NORMAL_VALUE},                         // 49
      {{12345, 0, 0, 0}, s21_NORMAL_VALUE},                         // 50
  };
  s21_decimal res[] = {
      {{8, 0, 0, 0}, s21_NORMAL_VALUE},                                // 1
      {{8, 0, 0, 0}, s21_NORMAL_VALUE},                                // 2
      {{2, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                       // 3
      {{2, 0, 0, 0}, s21_NORMAL_VALUE},                                // 4
      {{2, 0, 0, 0}, s21_NORMAL_VALUE},                                // 5
      {{2, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                       // 6
      {{8, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                       // 7
      {{8, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                       // 8
      {{88, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                      // 9
      {{88, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                      // 10
      {{18, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                      // 11
      {{18, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                      // 12
      {{18, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                      // 13
      {{18, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                      // 14
      {{88, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                      // 15
      {{88, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                      // 16
      {{5, 0, 0, 0}, s21_NORMAL_VALUE},                                // 17
      {{5, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                       // 18
      {{53, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                      // 19
      {{53, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                      // 20
      {{5, 0, 0, 0}, s21_NORMAL_VALUE},                                // 21
      {{5, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                       // 22
      {{53, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                      // 23
      {{53, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                      // 24
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                                // 25
      {{12345, 0, 0, 0}, s21_NORMAL_VALUE},                            // 26
      {{135795, 0, 0, 0x00030000}, s21_NORMAL_VALUE},                  // 27
      {{12345, 0, 0, 0x00000000}, s21_NORMAL_VALUE},                   // 28
      {{12358023, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                // 29
      {{0x7fffffff, 0x7fffffff, 0, 0x000e0000}, s21_NORMAL_VALUE},     // 30
      {{0x7FFFFF9D, 0x7FFFFFCE, 0x31, 0x800e0000}, s21_NORMAL_VALUE},  // 31
      {{0, 0, 0, 0}, s21_NAN},                                         // 32
      {{0, 0, 0, 0}, s21_INFINITY},                                    // 33
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                           // 34
      {{0, 0, 0, 0}, s21_NAN},                                         // 35
      {{0, 0, 0, 0}, s21_NAN},                                         // 36
      {{0, 0, 0, 0}, s21_NAN},                                         // 37
      {{0, 0, 0, 0}, s21_NAN},                                         // 38
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                           // 39
      {{0, 0, 0, 0}, s21_NAN},                                         // 40
      {{0, 0, 0, 0}, s21_NAN},                                         // 41
      {{0, 0, 0, 0}, s21_NAN},                                         // 42
      {{0, 0, 0, 0}, s21_INFINITY},                                    // 43
      {{0xffffffff, 0xffffffff, 0xffffffff}, 0},                       // 44
      {{0, 0, 0, 0}, s21_INFINITY},                                    // 45
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                           // 46
      {{0, 0, 0, 0}, s21_NAN},                                         // 47
      {{0, 0, 0, 0}, s21_INFINITY},                                    // 48
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                           // 49
      {{0, 0, 0, 0}, s21_NAN},                                         // 50
  };

  for (int i = 0; i < 50; i++) {
    s21_decimal ret = s21_add(tmp1[i], tmp2[i]);
    if (is_all_field_is_equal(ret, res[i]) == FALSE) {
      print_error4(tmp1[i], tmp2[i], ret, res[i], "s21_add", i);
    }
  }
}
END_TEST

// ТЕСТ ВЫЧИТАНИЯ
// 1. 5 - 3
// 2. 3 - 5
// 3. -5 - 3
// 4. -3 - 5
// 5. 5 - (-3)
// 6. 3 - (-5)
// 7. -5 - (-3)
// 8. -3 - (-5)
// 9. 5.3 - 3.5
// 10. 3.5 - 5.3
// 11. -5.3 - 3.5
// 12. -3.5 - 5.3
// 13. 5.3 - (-3.5)
// 14. 3.5 - (-5.3)
// 15. -5.3 - (-3.5)
// 16. -3.5 - (-5.3)
// 17. 5 - 0
// 18. -5 - 0
// 19. -5.3 - 0
// 20. 5.3 - 0
// 21. 0 - 5
// 22. 0 - (-5)
// 23. 0 - 5.3
// 24. 0 - (-5.3)

// 25. Проверка 0 - 0
// 26. Проверка 0  - 12345
// 27. Проверка на scale
// 28. Вычитание из отрицательного числа  положительного
// 29. Вычитание из отрицательного числа  отрицательного
// 30. Вычитание из 0  числа с большим скейлом
// 31. Вычитание из положительного числа с большим скейлом  отрицательного числа
// с большим скейлом
// 32. Вычитание s21_INFINITY и s21_NEGATIVE_INFINITY
// 33. Проверка на переполнение в s21_INFINITY
// 34. Проверка на переполнение в s21_NEGATIVE_INFINITY
// 35. NAN - NAN
// 36. NAN - INFINITY
// 37. NAN - NEGATIVE_INFINITY
// 38. NEGATIVE_INFINITY - NAN
// 39. NEGATIVE_INFINITY - INFINITY
// 40. NEGATIVE_INFINITY - NEGATIVE_INFINITY
// 41. INFINITY - NAN
// 42. INFINITY - NEGATIVE_INFINITY
// 43. INFINITY - INFINITY
// 44. Проверка на переполнение с округлением скейл
// 45. 12345 - INFINITY
// 46. 12345 - NEGATIVE_INFINITY
// 47. 12345 - NAN
// 48. NEGATIVE_INFINITY - 12345
// 49. INFINITY - 12345
// 50. NAN - 12345

START_TEST(test_s21_sub) {
  s21_decimal tmp1[] = {
      {{2, 0, 0, 0}, s21_NORMAL_VALUE},               // 1
      {{5, 0, 0, 0}, s21_NORMAL_VALUE},               // 1
      {{3, 0, 0, 0}, s21_NORMAL_VALUE},               // 2
      {{5, 0, 0, 0x80000000}, s21_NORMAL_VALUE},      // 3
      {{3, 0, 0, 0x80000000}, s21_NORMAL_VALUE},      // 4
      {{5, 0, 0, 0}, s21_NORMAL_VALUE},               // 5
      {{3, 0, 0, 0}, s21_NORMAL_VALUE},               // 6
      {{5, 0, 0, 0x80000000}, s21_NORMAL_VALUE},      // 7
      {{3, 0, 0, 0x80000000}, s21_NORMAL_VALUE},      // 8
      {{53, 0, 0, 0x00010000}, s21_NORMAL_VALUE},     // 9
      {{35, 0, 0, 0x00010000}, s21_NORMAL_VALUE},     // 10
      {{53, 0, 0, 0x80010000}, s21_NORMAL_VALUE},     // 11
      {{35, 0, 0, 0x80010000}, s21_NORMAL_VALUE},     // 12
      {{53, 0, 0, 0x00010000}, s21_NORMAL_VALUE},     // 13
      {{35, 0, 0, 0x00010000}, s21_NORMAL_VALUE},     // 14
      {{53, 0, 0, 0x80010000}, s21_NORMAL_VALUE},     // 15
      {{35, 0, 0, 0x80010000}, s21_NORMAL_VALUE},     // 16
      {{5, 0, 0, 0}, s21_NORMAL_VALUE},               // 17
      {{5, 0, 0, 0x80000000}, s21_NORMAL_VALUE},      // 18
      {{53, 0, 0, 0x80010000}, s21_NORMAL_VALUE},     // 19
      {{53, 0, 0, 0x00010000}, s21_NORMAL_VALUE},     // 20
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},               // 21
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},               // 22
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},               // 23
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},               // 24
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},               // 25
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},               // 26
      {{12345, 0, 0, 0x00020000}, s21_NORMAL_VALUE},  // 27
      {{12345, 0, 0, 0x80000000}, s21_NORMAL_VALUE},  // 28
      {{12345, 0, 0, 0x80000000}, s21_NORMAL_VALUE},  // 29
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},               // 30
      {{0x7fffffff, 0x7fffffff, 0x7fffffff, 0x000E0000},
       s21_NORMAL_VALUE},                                           // 31
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 32
      {{0xffffffff, 0xffffffff, 0xffffffff, 0}, s21_NORMAL_VALUE},  // 33
      {{0xffffffff, 0xffffffff, 0xffffffff, 0x80000000},
       s21_NORMAL_VALUE},                                           // 34
      {{0, 0, 0, 0}, s21_NAN},                                      // 35
      {{0, 0, 0, 0}, s21_NAN},                                      // 36
      {{0, 0, 0, 0}, s21_NAN},                                      // 37
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 38
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 39
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 40
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 41
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 42
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 43
      {{0xffffffff, 0xffffffff, 0xffffffff, 0}, s21_NORMAL_VALUE},  // 44
      {{12345, 0, 0, 0}, s21_NORMAL_VALUE},                         // 45
      {{12345, 0, 0, 0}, s21_NORMAL_VALUE},                         // 46
      {{12345, 0, 0, 0}, s21_NORMAL_VALUE},                         // 47
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 48
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 49
      {{0, 0, 0, 0}, s21_NAN},                                      // 50
  };

  s21_decimal tmp2[] = {
      {{3, 0, 0, 0}, s21_NORMAL_VALUE},                             // 1
      {{3, 0, 0, 0}, s21_NORMAL_VALUE},                             // 1
      {{5, 0, 0, 0}, s21_NORMAL_VALUE},                             // 2
      {{3, 0, 0, 0}, s21_NORMAL_VALUE},                             // 3
      {{5, 0, 0, 0}, s21_NORMAL_VALUE},                             // 4
      {{3, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                    // 5
      {{5, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                    // 6
      {{3, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                    // 7
      {{5, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                    // 8
      {{35, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                   // 9
      {{53, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                   // 10
      {{35, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                   // 11
      {{53, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                   // 12
      {{35, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                   // 13
      {{53, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                   // 14
      {{35, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                   // 15
      {{53, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                   // 16
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 17
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 18
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 19
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 20
      {{5, 0, 0, 0}, s21_NORMAL_VALUE},                             // 21
      {{5, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                    // 22
      {{53, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                   // 23
      {{53, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                   // 24
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 25
      {{12345, 0, 0, 0}, s21_NORMAL_VALUE},                         // 26
      {{12345, 0, 0, 0x00030000}, s21_NORMAL_VALUE},                // 27
      {{24690, 0, 0, 0}, s21_NORMAL_VALUE},                         // 28
      {{12345678, 0, 0, 0x80000000}, s21_NORMAL_VALUE},             // 29
      {{0x7fffffff, 0x7fffffff, 0, 0x000c0000}, s21_NORMAL_VALUE},  // 30
      {{0x7fffffff, 0x7fffffff, 0, 0x800c0000}, s21_NORMAL_VALUE},  // 31
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 32
      {{10, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                   // 33
      {{50, 0, 0, 0}, s21_NORMAL_VALUE},                            // 34
      {{0, 0, 0, 0}, s21_NAN},                                      // 35
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 36
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 37
      {{0, 0, 0, 0}, s21_NAN},                                      // 38
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 39
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 40
      {{0, 0, 0, 0}, s21_NAN},                                      // 41
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 42
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 43
      {{6, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                    // 44
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 45
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 46
      {{0, 0, 0, 0}, s21_NAN},                                      // 47
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 48
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 49
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 50
  };
  s21_decimal res[] = {
      {{1, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                    // 1
      {{2, 0, 0, 0}, s21_NORMAL_VALUE},                             // 1
      {{2, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                    // 2
      {{8, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                    // 3
      {{8, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                    // 4
      {{8, 0, 0, 0}, s21_NORMAL_VALUE},                             // 5
      {{8, 0, 0, 0}, s21_NORMAL_VALUE},                             // 6
      {{2, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                    // 7
      {{2, 0, 0, 0}, s21_NORMAL_VALUE},                             // 8
      {{18, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                   // 9
      {{18, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                   // 10
      {{88, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                   // 11
      {{88, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                   // 12
      {{88, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                   // 13
      {{88, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                   // 14
      {{18, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                   // 15
      {{18, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                   // 16
      {{5, 0, 0, 0}, s21_NORMAL_VALUE},                             // 17
      {{5, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                    // 18
      {{53, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                   // 19
      {{53, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                   // 20
      {{5, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                    // 21
      {{5, 0, 0, 0}, s21_NORMAL_VALUE},                             // 22
      {{53, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                   // 23
      {{53, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                   // 24
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 25
      {{12345, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                // 26
      {{111105, 0, 0, 0x00030000}, s21_NORMAL_VALUE},               // 27
      {{37035, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                // 28
      {{12333333, 0, 0, 0}, s21_NORMAL_VALUE},                      // 29
      {{0x7fffffff, 0x7fffffff, 0, 0x800c0000}, s21_NORMAL_VALUE},  // 30
      {{0x7FFFFF9B, 0x7FFFFFCD, 0x80000031, 0x000e0000},
       s21_NORMAL_VALUE},                                           // 31
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 32
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 33
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 34
      {{0, 0, 0, 0}, s21_NAN},                                      // 35
      {{0, 0, 0, 0}, s21_NAN},                                      // 36
      {{0, 0, 0, 0}, s21_NAN},                                      // 37
      {{0, 0, 0, 0}, s21_NAN},                                      // 38
      {{0, 0, 0, 0}, s21_NAN},                                      // 39
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 40
      {{0, 0, 0, 0}, s21_NAN},                                      // 41
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 42
      {{0, 0, 0, 0}, s21_NAN},                                      // 43
      {{0xFFFFFFFE, 0xffffffff, 0xffffffff, 0}, s21_NORMAL_VALUE},  // 44
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 45
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 46
      {{0, 0, 0, 0}, s21_NAN},                                      // 47
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 48
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 49
      {{0, 0, 0, 0}, s21_NAN},                                      // 50
  };

  for (int i = 0; i < 50; i++) {
    s21_decimal ret = s21_sub_from_sd(tmp1[i], tmp2[i]);
    if (is_all_field_is_equal(ret, res[i]) == FALSE) {
      print_error4(tmp1[i], tmp2[i], ret, res[i], "s21_sub", i);
    }
  }
}
END_TEST

/*
START_TEST(test_s21_mul2) {
  s21_decimal tmp1[] = {
      {{4294967295, 4294967295, 2147483647, 0}, s21_NORMAL_VALUE}, //1
  };
  s21_decimal tmp2[] = {
      {{1, 0, 0, 0x00010000}, s21_NORMAL_VALUE}, //1
  };
  s21_decimal tmp3[] = {
      {{10, 0, 0, 0}, s21_NORMAL_VALUE}, //1
  };
  s21_decimal etalon[] = {
      tmp1[0], //1
  };
    for (int i = 0; i < 1; i++) {
    s21_decimal ret1 = s21_mul_pjian(tmp1[i], tmp2[i]);
    s21_decimal ret2 = s21_mul_pjian(ret1, tmp2[3]);
    if (is_all_field_is_equal(ret1, etalon[i]) == FALSE) {
      print_error4(tmp1[i], tmp2[i], ret1, etalon[i], "s21_mul2", i);
    }
  }
}
END_TEST
*/

// ТЕСТ Умножение
// 1. 5 * 3
// 2. 3 * 5
// 3. -5 * 3
// 4. -3 * 5
// 5. 5 * (-3)
// 6. 3 * (-5)
// 7. -5 * (-3)
// 8. -3 * (-5)
// 9. 5.3 * 3.5
// 10. 3.5 * 5.3
// 11. -5.3 * 3.5
// 12.-3.5 * 5.3
// 13. 5.3 * (-3.5)
// 14.3.5 * (-5.3)
// 15.-5.3 * (-3.5)
// 16.-3.5 * (-5.3)
// 17. 5 * 0
// 18. -5 * 0
// 19. -5.3 * 0
// 20. 5.3 * 0
// 21. 0 * 5
// 22. 0 * (-5)
// 23. 0 * 5.3
// 24. 0 * (-5.3)

// 25. Проверка 0 * 0
// 26. Проверка 0  * 12345
// 27. Проверка на scale
// 28. Умножение отрицательного числа и положительного
// 29. Умножение отрицательного числа с отрицательным
// 30. Умножение 0 и числа с большим скейлом
// 31. Умножение отрицательного числа с большим скейлом с положительным числом с
// большим скейлом
// 32. Умножение s21_INFINITY и s21_NEGATIVE_INFINITY
// 33. Проверка на переполнение в s21_INFINITY
// 34. Проверка на переполнение в s21_NEGATIVE_INFINITY
// 35. NAN * NAN
// 36. NAN * INFINITY
// 37. NAN * NEGATIVE_INFINITY
// 38. NEGATIVE_INFINITY * NAN
// 39. NEGATIVE_INFINITY * INFINITY
// 40. NEGATIVE_INFINITY * NEGATIVE_INFINITY
// 41. INFINITY * NAN
// 42. INFINITY * NEGATIVE_INFINITY
// 43. INFINITY * INFINITY
// 44. Проверка на переполнение с округлением скейл (Пока не придумал)
// 45. 12345 * INFINITY
// 46. 12345 * NEGATIVE_INFINITY
// 47. 12345 * NAN
// 48. NEGATIVE_INFINITY * 12345
// 49. INFINITY * 12345
// 50. NAN * 12345

START_TEST(test_s21_mul) {
  s21_decimal tmp1[] = {
      {{5, 0, 0, 0}, s21_NORMAL_VALUE},                             // 1
      {{3, 0, 0, 0}, s21_NORMAL_VALUE},                             // 2
      {{5, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                    // 3
      {{3, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                    // 4
      {{5, 0, 0, 0}, s21_NORMAL_VALUE},                             // 5
      {{3, 0, 0, 0}, s21_NORMAL_VALUE},                             // 6
      {{5, 0, 0, 0x8000000}, s21_NORMAL_VALUE},                     // 7
      {{3, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                    // 8
      {{53, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                   // 9
      {{35, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                   // 10
      {{53, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                   // 11
      {{35, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                   // 12
      {{53, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                   // 13
      {{35, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                   // 14
      {{53, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                   // 15
      {{35, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                   // 16
      {{5, 0, 0, 0}, s21_NORMAL_VALUE},                             // 17
      {{5, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                    // 18
      {{53, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                   // 19
      {{53, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                   // 20
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 21
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 22
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 23
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 24
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 25
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 26
      {{12345, 0, 0, 0x00020000}, s21_NORMAL_VALUE},                // 27
      {{12345, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                // 28
      {{12345, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                // 29
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 30
      {{0x7fffffff, 0x7fffffff, 0, 0x800c0000}, s21_NORMAL_VALUE},  // 31
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 32
      {{0xffffffff, 0xffffffff, 0xffffffff, 0}, s21_NORMAL_VALUE},  // 33
      {{0xffffffff, 0xffffffff, 0xffffffff, 0x80000000},
       s21_NORMAL_VALUE},                                           // 34
      {{0, 0, 0, 0}, s21_NAN},                                      // 35
      {{0, 0, 0, 0}, s21_NAN},                                      // 36
      {{0, 0, 0, 0}, s21_NAN},                                      // 37
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 38
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 39
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 40
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 41
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 42
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 43
      {{12345, 0, 0, 0}, s21_NORMAL_VALUE},                         // 44
      {{12345, 0, 0, 0}, s21_NORMAL_VALUE},                         // 45
      {{12345, 0, 0, 0}, s21_NORMAL_VALUE},                         // 46
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 47
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 48
      {{0, 0, 0, 0}, s21_NAN},                                      // 49
      {{4294967295, 4294967295, 2147483647, 0}, s21_NORMAL_VALUE},  // 50
      {{0xFFFFFFF6, 0xFFFFFFFF, 0x9, 0x10000}, s21_NORMAL_VALUE},   // 51 (15)
      {{0x82E25AED, 0xF9E825AD, 0xB03EF094, 0x800E0000},
       s21_NORMAL_VALUE},  // 52 (16)
      {{0x82E25AED, 0xF9E825AD, 0xB03EF094, 0x800E0000},
       s21_NORMAL_VALUE},  // 53 (17)
      {{0x910AF4CA, 0xC045D5F2, 0x27E41B00, 0x150000},
       s21_NORMAL_VALUE},  // 54 (19)
      {{0x910AF4CA, 0xC045D5F2, 0x27E41B00, 0x80150000},
       s21_NORMAL_VALUE},  // 55 (20)
      {{0x910AF4CA, 0xC045D5F2, 0x27E41B00, 0x150000},
       s21_NORMAL_VALUE},  // 56 (21)
      {{0x910AF4CA, 0xC045D5F2, 0x27E41B00, 0x80150000},
       s21_NORMAL_VALUE},                                           // 57 (22)
      {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}, s21_NORMAL_VALUE},  // 58 (26)
      {{0xFE90D95F, 0x846421E8, 0x1, 0x130000}, s21_NORMAL_VALUE},  // 59 (29)
      {{0x54403D8C, 0x8B520559, 0x1FA981AD, 0x800C0000},
       s21_NORMAL_VALUE},  // 60 (30)
      {{0xB0F708D, 0x96B05951, 0x11A17B81, 0x1C0000},
       s21_NORMAL_VALUE},                                             // 61 (31)
      {{0xAC281855, 0x34291EBC, 0x4, 0x80140000}, s21_NORMAL_VALUE},  // 62 (32)
      {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80000000},
       s21_NORMAL_VALUE},                                    // 63 (33)
      {{0x58431906, 0x72D605, 0x0, 0x0}, s21_NORMAL_VALUE},  // 64 (34)
      {{0xB13B45F4, 0x3883AC81, 0x19595C18, 0x80130000},
       s21_NORMAL_VALUE},                           // 65 (35)
      {{12345, 0, 0, 0}, s21_NORMAL_VALUE},         // 66 (44)
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},        // 67 (46)
      {{0, 0, 0, 0}, s21_INFINITY},                 // 68 (48)
      {{1, 0, 0, 0x1C0000}, s21_NORMAL_VALUE},      // 69 (49)
      {{5, 0, 0, 0x140000}, s21_NORMAL_VALUE},      // 70 (50)
      {{0x61F, 0, 0, 0x1C0000}, s21_NORMAL_VALUE},  // 71 (51)
  };
  s21_decimal tmp2[] = {
      {{3, 0, 0, 0}, s21_NORMAL_VALUE},                             // 1
      {{5, 0, 0, 0}, s21_NORMAL_VALUE},                             // 2
      {{3, 0, 0, 0}, s21_NORMAL_VALUE},                             // 3
      {{5, 0, 0, 0}, s21_NORMAL_VALUE},                             // 4
      {{3, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                    // 5
      {{5, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                    // 6
      {{3, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                    // 7
      {{5, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                    // 8
      {{35, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                   // 9
      {{53, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                   // 10
      {{35, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                   // 11
      {{53, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                   // 12
      {{35, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                   // 13
      {{53, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                   // 14
      {{35, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                   // 15
      {{53, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                   // 16
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 17
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 18
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 19
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 20
      {{5, 0, 0, 0}, s21_NORMAL_VALUE},                             // 21
      {{5, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                    // 22
      {{53, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                   // 23
      {{53, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                   // 24
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 25
      {{12345, 0, 0, 0}, s21_NORMAL_VALUE},                         // 26
      {{12345, 0, 0, 0x00040000}, s21_NORMAL_VALUE},                // 27
      {{15, 0, 0, 0}, s21_NORMAL_VALUE},                            // 28
      {{15, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                   // 29
      {{0x7fffffff, 0x7fffffff, 0, 0x000c0000}, s21_NORMAL_VALUE},  // 30
      {{0x7fffffff, 0x7fffffff, 0, 0x800e0000}, s21_NORMAL_VALUE},  // 31
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 32
      {{10, 0, 0, 0}, s21_NORMAL_VALUE},                            // 33
      {{50, 0, 0, 0}, s21_NORMAL_VALUE},                            // 34
      {{0, 0, 0, 0}, s21_NAN},                                      // 35
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 36
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 37
      {{0, 0, 0, 0}, s21_NAN},                                      // 38
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 39
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 40
      {{0, 0, 0, 0}, s21_NAN},                                      // 41
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 42
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 43
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 44
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 45
      {{0, 0, 0, 0}, s21_NAN},                                      // 46
      {{12345, 0, 0, 0}, s21_NORMAL_VALUE},                         // 47
      {{12345, 0, 0, 0}, s21_NORMAL_VALUE},                         // 48
      {{12345, 0, 0, 0}, s21_NORMAL_VALUE},                         // 49
      {{2, 0, 0, 0}, s21_NORMAL_VALUE},                             // 50
      {{0x398BA562, 0x0, 0x0, 0x0}, s21_NORMAL_VALUE},              // 51 (15)
      {{0x340972E, 0, 0, 0}, s21_NORMAL_VALUE},                     // 52 (16)
      {{0x481B1F98, 0xFBFD896D, 0x46D77, 0x80180000},
       s21_NORMAL_VALUE},  // 53 (17)
      {{0x11CDD185, 0xF5BC6FC0, 0x2D50F3F, 0x100000},
       s21_NORMAL_VALUE},  // 54 (19)
      {{0x11CDD185, 0xF5BC6FC0, 0x2D50F3F, 0x100000},
       s21_NORMAL_VALUE},  // 55 (20)
      {{0x11CDD185, 0xF5BC6FC0, 0x2D50F3F, 0x80100000},
       s21_NORMAL_VALUE},  // 56 (21)
      {{0x11CDD185, 0xF5BC6FC0, 0x2D50F3F, 0x80100000},
       s21_NORMAL_VALUE},                         // 57 (22)
      {{1, 0, 0, 0x80000000}, s21_NORMAL_VALUE},  // 58 (26)
      {{0xD4AE3D8A, 0x741427A3, 0x84144, 0x180000},
       s21_NORMAL_VALUE},                                        // 59 (29)
      {{0x65F24DAC, 0x7A9AB, 0, 0x80060000}, s21_NORMAL_VALUE},  // 60 (30)
      {{0x17254C02, 0xF5159B, 0, 0x110000}, s21_NORMAL_VALUE},   // 61 (31)
      {{0x13EDF788, 0xC3C80BD9, 4, 0}, s21_NORMAL_VALUE},        // 62 (32)
      {{1, 0, 0, 0}, s21_NORMAL_VALUE},                          // 63 (33)
      {{0xBE118597, 0xD93D12C6, 0x59648, 0x110000},
       s21_NORMAL_VALUE},  // 64 (34)
      {{0xE6259369, 0x38DAE2B4, 0xBB5FD11E, 0x800B0000},
       s21_NORMAL_VALUE},                               // 65 (35)
      {{0, 0, 0, 0}, s21_NAN},                          // 66 (44)
      {{0, 0, 0, 0}, s21_INFINITY},                     // 67 (46)
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},            // 68 (48)
      {{1, 0, 0, 0x20000}, s21_NORMAL_VALUE},           // 69 (49)
      {{0x159, 0, 0, 0xD0000}, s21_NORMAL_VALUE},       // 70 (50)
      {{0x2FB06A91, 0, 0, 0xC0000}, s21_NORMAL_VALUE},  // 71 (51)
  };
  s21_decimal res[] = {
      {{15, 0, 0, 0x00000000}, s21_NORMAL_VALUE},         // 1
      {{15, 0, 0, 0x00000000}, s21_NORMAL_VALUE},         // 2
      {{15, 0, 0, 0x80000000}, s21_NORMAL_VALUE},         // 3
      {{15, 0, 0, 0x80000000}, s21_NORMAL_VALUE},         // 4
      {{15, 0, 0, 0x80000000}, s21_NORMAL_VALUE},         // 5
      {{15, 0, 0, 0x80000000}, s21_NORMAL_VALUE},         // 6
      {{15, 0, 0, 0x80000000}, s21_NORMAL_VALUE},         // 7
      {{15, 0, 0, 0}, s21_NORMAL_VALUE},                  // 8
      {{1855, 0, 0, 0x00020000}, s21_NORMAL_VALUE},       // 9
      {{1855, 0, 0, 0x00020000}, s21_NORMAL_VALUE},       // 10
      {{1855, 0, 0, 0x80020000}, s21_NORMAL_VALUE},       // 11
      {{1855, 0, 0, 0x80020000}, s21_NORMAL_VALUE},       // 12
      {{1855, 0, 0, 0x80020000}, s21_NORMAL_VALUE},       // 13
      {{1855, 0, 0, 0x80020000}, s21_NORMAL_VALUE},       // 14
      {{1855, 0, 0, 0x00020000}, s21_NORMAL_VALUE},       // 15
      {{1855, 0, 0, 0x00020000}, s21_NORMAL_VALUE},       // 16
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                   // 17
      {{0, 0, 0, 0x80000000}, s21_NORMAL_VALUE},          // 18
      {{0, 0, 0, 0x80000000}, s21_NORMAL_VALUE},          // 19
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                   // 20
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                   // 21
      {{0, 0, 0, 0x80000000}, s21_NORMAL_VALUE},          // 22
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                   // 23
      {{0, 0, 0, 0x80000000}, s21_NORMAL_VALUE},          // 24
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                   // 25
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                   // 26
      {{152399025, 0, 0, 0x00060000}, s21_NORMAL_VALUE},  // 27
      {{185175, 0, 0, 0x80000000}, s21_NORMAL_VALUE},     // 28
      {{185175, 0, 0, 0}, s21_NORMAL_VALUE},              // 29
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                   // 30
      {{3569111941, 2697198339, 461168601, 0x00100000},
       s21_NORMAL_VALUE},                                           // 31
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 32
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 33
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 34
      {{0, 0, 0, 0}, s21_NAN},                                      // 35
      {{0, 0, 0, 0}, s21_NAN},                                      // 36
      {{0, 0, 0, 0}, s21_NAN},                                      // 37
      {{0, 0, 0, 0}, s21_NAN},                                      // 38
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 39
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 40
      {{0, 0, 0, 0}, s21_NAN},                                      // 41
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 42
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 43
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 44
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 45
      {{0, 0, 0, 0}, s21_NAN},                                      // 46
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 47
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 48
      {{0, 0, 0, 0}, s21_NAN},                                      // 49
      {{0xFFFFFFFE, 0xFFFFFFFF, 0xFFFFFFFF, 0}, s21_NORMAL_VALUE},  // 50
      {{0xC6745A9E, 0xFFFFFFFF, 0x398BA561, 0}, s21_NORMAL_VALUE},  // 51 (15)
      {{0x30E2CD56, 0xDEE7F37B, 0x602AFE4D, 0x80060000},
       s21_NORMAL_VALUE},  // 52 (16)
      {{0x71CFE5CF, 0x444ED799, 0x5E569C1C, 0xD0000},
       s21_NORMAL_VALUE},  // 53 (17)
      {{0x14990EE, 0xAA854F8F, 0x22F75A1A, 0xA0000},
       s21_NORMAL_VALUE},  // 54 (19)
      {{0x14990EE, 0xAA854F8F, 0x22F75A1A, 0x800A0000},
       s21_NORMAL_VALUE},  // 55 (20)
      {{0x14990EE, 0xAA854F8F, 0x22F75A1A, 0x800A0000},
       s21_NORMAL_VALUE},  // 56 (21)
      {{0x14990EE, 0xAA854F8F, 0x22F75A1A, 0x000A0000},
       s21_NORMAL_VALUE},  // 57 (22)
      {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80000000},
       s21_NORMAL_VALUE},  // 58 (26)
      {{0x8249A386, 0xA152457, 0x5A3EBE68, 0x1B0000},
       s21_NORMAL_VALUE},  // 59 (29)
      {{0xA227D809, 0x6311F76A, 0x444A97A9, 0x30000},
       s21_NORMAL_VALUE},  // 60 (30)
      {{0x2DAD13C3, 0xE5720A43, 0xC29A1AA, 0x1C0000},
       s21_NORMAL_VALUE},  // 61 (31)
      {{0x2C4462FE, 0x8FE90452, 0xDC3B3573, 0x80090000},
       s21_NORMAL_VALUE},  // 62 (32)
      {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80000000},
       s21_NORMAL_VALUE},  // 63 (33)
      {{0x28F18B9E, 0x7CEB09B3, 0x468B476B, 0x40000},
       s21_NORMAL_VALUE},  // 64 (34)
      {{0x4BEE0E5C, 0xDBCD1F3C, 0x92FF7E60, 0x20000},
       s21_NORMAL_VALUE},                       // 65 (35)
      {{0, 0, 0, 0}, s21_NAN},                  // 66 (44)
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},    // 67 (46)
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},    // 68 (48)
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},         // 69 (49)
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},         // 70 (50)
      {{1, 0, 0, 0x1C0000}, s21_NORMAL_VALUE},  // 71 (51)
  };

  for (int i = 0; i < 71; i++) {
    if (i == 31) {
      int a = 5;
    }
    s21_decimal ret = s21_mul(tmp1[i], tmp2[i]);
    if (is_all_field_is_equal(ret, res[i]) == FALSE) {
      print_error4(tmp1[i], tmp2[i], ret, res[i], "s21_mul", i);
    }
  }
}
END_TEST

// ТЕСТ Деление
// 1. 3 / 2
// 2. 2 / 3
// 3. -3 / 2
// 4. -2 / 3
// 5. 3 / (-2)
// 6. 2 / (-3)
// 7. -3 / (-2)
// 8. -2 / (-3)
// 9. 3.2 / 2.3
// 10. 2.3 / 3.2
// 11. 3 / 0
// 12. -3 / 0
// 13. -3.2 / 0
// 14. 3.2 / 0
// 15. 0 / 3
// 16. 0 / (-3)
// 17. 0 / 3.2
// 18. 0 / (-3.2)
// 19. Проверка 0 / 0
// 20. Проверка 0  / 12345
// 21. Проверка на scale
// 22. Деление отрицательного числа и положительного
// 23. Деление отрицательного числа с отрицательным
// 24. Деление 0 и числа с большим скейлом
// 25. Деление отрицательного числа с большим скейлом с положительным числом с
// большим скейлом
// 26. Деление s21_INFINITY и s21_NEGATIVE_INFINITY
// 27. Проверка на переполнение в s21_INFINITY
// 28. Проверка на переполнение в s21_NEGATIVE_INFINITY
// 29. NAN / NAN
// 30. NAN / INFINITY
// 31. NAN / NEGATIVE_INFINITY
// 32. NEGATIVE_INFINITY / NAN
// 33. NEGATIVE_INFINITY / INFINITY
// 34. NEGATIVE_INFINITY / NEGATIVE_INFINITY
// 35. INFINITY / NAN
// 36. INFINITY / NEGATIVE_INFINITY
// 37. INFINITY / INFINITYa
// 38.  1 / 0
// 39. -1 / 0
// 40. 12345 / INFINITY
// 41. 12345 / NEGATIVE_INFINITY
// 42. 12345 / NAN
// 43. NEGATIVE_INFINITY / 12345
// 44. INFINITY / 12345
// 45. NAN / 12345

START_TEST(test_s21_div) {
  s21_decimal tmp1[] = {
      {{10, 0, 0, 0}, s21_NORMAL_VALUE},                            // 1
      {{3, 0, 0, 0}, s21_NORMAL_VALUE},                             // 2
      {{2, 0, 0, 0}, s21_NORMAL_VALUE},                             // 3
      {{3, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                    // 4
      {{3, 0, 0, 0}, s21_NORMAL_VALUE},                             // 5
      {{3, 0, 0, 0x8000000}, s21_NORMAL_VALUE},                     // 6
      {{32, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                   // 7
      {{23, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                   // 8
      {{3, 0, 0, 0}, s21_NORMAL_VALUE},                             // 9
      {{3, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                    // 10
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 11
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 12
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 13
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 14
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 15
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 16
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                             // 17
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 18
      {{0xffffffff, 0xffffffff, 0xffffffff, 0}, s21_NORMAL_VALUE},  // 19
      {{0, 0, 0, 0}, s21_NAN},                                      // 20
      {{0, 0, 0, 0}, s21_NAN},                                      // 21
      {{0, 0, 0, 0}, s21_NAN},                                      // 22
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 23
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 24
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 25
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 26
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 27
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 28
      {{12345, 0, 0, 0}, s21_NORMAL_VALUE},                         // 29
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 30
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                        // 31
      {{0, 0, 0, 0}, s21_NAN},                                      // 32
      {{0, 0, 0, 0}, s21_INFINITY},                                 // 33
      {{2, 0, 0, 0}, s21_NORMAL_VALUE},                             // 34 (1)
      {{65658654, 0, 0, 0}, s21_NORMAL_VALUE},                      // 35 (9)
  };

  s21_decimal tmp2[] = {
      {{2, 0, 0, 0}, s21_NORMAL_VALUE},               // 1
      {{2, 0, 0, 0}, s21_NORMAL_VALUE},               // 2
      {{3, 0, 0, 0}, s21_NORMAL_VALUE},               // 3
      {{2, 0, 0, 0}, s21_NORMAL_VALUE},               // 4
      {{2, 0, 0, 0x80000000}, s21_NORMAL_VALUE},      // 5
      {{2, 0, 0, 0x80000000}, s21_NORMAL_VALUE},      // 6
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},               // 7
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},               // 8
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},               // 9
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},               // 10
      {{32, 0, 0, 0x00010000}, s21_NORMAL_VALUE},     // 11
      {{32, 0, 0, 0x80010000}, s21_NORMAL_VALUE},     // 12
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},               // 13
      {{12345, 0, 0, 0}, s21_NORMAL_VALUE},           // 14
      {{12345, 0, 0, 0x00030000}, s21_NORMAL_VALUE},  // 15
      {{9, 0, 0, 0}, s21_NORMAL_VALUE},               // 16
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},          // 17
      {{01, 0, 0, 0x80010000}, s21_NORMAL_VALUE},     // 18
      {{0, 0, 0, 0}, s21_NAN},                        // 19
      {{0, 0, 0, 0}, s21_INFINITY},                   // 20
      {{0, 0, 0, 0}, s21_NAN},                        // 21
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},          // 22
      {{0, 0, 0, 0}, s21_INFINITY},                   // 23
      {{0, 0, 0, 0}, s21_NAN},                        // 24
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},          // 25
      {{0, 0, 0, 0}, s21_INFINITY},                   // 26
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},               // 27
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},               // 28
      {{0, 0, 0, 0}, s21_NAN},                        // 29
      {{0, 0, 0, 0}, s21_NAN},                        // 30
      {{12345, 0, 0, 0}, s21_NORMAL_VALUE},           // 31
      {{12345, 0, 0, 0}, s21_NORMAL_VALUE},           // 32
      {{12345, 0, 0, 0}, s21_NORMAL_VALUE},           // 33
      {{3, 0, 0, 0}, s21_NORMAL_VALUE},               // 34 (1)
      {{5, 0, 0, 0}, s21_NORMAL_VALUE},               // 35 (9)
  };
  s21_decimal res[] = {
      {{5, 0, 0, 0}, s21_NORMAL_VALUE},                                   // 1
      {{0x0000000F, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                 // 2
      {{0xAAAAAAB, 0x296E0196, 0x158A8994, 0x1C0000}, s21_NORMAL_VALUE},  // 3
      {{0x0000000F, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                 // 4
      {{0x0000000F, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                 // 5
      {{0x0000000F, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                 // 6
      {{0, 0, 0, 0}, s21_INFINITY},                                       // 7
      {{0, 0, 0, 0}, s21_INFINITY},                                       // 8
      {{0, 0, 0, 0}, s21_INFINITY},                                       // 9
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                              // 10
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                                   // 11
      {{0, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                          // 12
      {{0, 0, 0, 0}, s21_NAN},                                            // 13
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                                   // 14
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                                   // 15
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                                   // 16
      {{0, 0, 0, 0x00000000}, s21_NAN},                                   // 17
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                              // 18
      {{0, 0, 0, 0}, s21_NAN},                                            // 19
      {{0, 0, 0, 0}, s21_NAN},                                            // 20
      {{0, 0, 0, 0}, s21_NAN},                                            // 21
      {{0, 0, 0, 0}, s21_NAN},                                            // 22
      {{0, 0, 0, 0}, s21_NAN},                                            // 23
      {{0, 0, 0, 0}, s21_NAN},                                            // 24
      {{0, 0, 0, 0}, s21_NAN},                                            // 25
      {{0, 0, 0, 0}, s21_NAN},                                            // 26
      {{0, 0, 0, 0}, s21_INFINITY},                                       // 27
      {{0, 0, 0, 0}, s21_INFINITY},                                       // 28
      {{0, 0, 0, 0}, s21_NAN},                                            // 29
      {{0, 0, 0, 0}, s21_NAN},                                            // 30
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                              // 31
      {{0, 0, 0, 0}, s21_NAN},                                            // 32
      {{0, 0, 0, 0}, s21_INFINITY},                                       // 33
      {{0xAAAAAAB, 0x296E0196, 0x158A8994, 0x1C0000},
       s21_NORMAL_VALUE},                              // 34 (1)
      {{0x7D3BE3C, 0, 0, 0x10000}, s21_NORMAL_VALUE},  // 35 (9)
  };
  for (int i = 0; i < 34; i++) {
    if (i == 13) {
      int a = 5;
    }
    s21_decimal ret = s21_div(tmp1[i], tmp2[i]);
    if (is_all_field_is_equal(ret, res[i]) == FALSE) {
      print_error4(tmp1[i], tmp2[i], ret, res[i], "s21_div", i);
    }
  }
}
END_TEST

// ТЕСТ Остаток от деления
// 1. Проверка 0 % 0
// 2. Проверка 0  % 12345
// 3. Проверка 5 % 3
// 4. Проверка 5.6 % 3.3
// 5. Проверка -5.6 % 3.3
// 6. Проверка -5.6 % -3.3
// 7. Проверка -5.6 % 3.3
// 8. Деление s21_INFINITY и s21_NEGATIVE_INFINITY
// 9. Проверка на переполнение в s21_INFINITY
// 10. Проверка на переполнение в s21_NEGATIVE_INFINITY
// 11. NAN % NAN
// 12. NAN % INFINITY
// 13. NAN % NEGATIVE_INFINITY
// 14. NEGATIVE_INFINITY % NAN
// 15. NEGATIVE_INFINITY % INFINITY
// 16. NEGATIVE_INFINITY % NEGATIVE_INFINITY
// 17. INFINITY % NAN
// 18. INFINITY % NEGATIVE_INFINITY
// 19. INFINITY % INFINITY
// 20. 12345 / INFINITY
// 21. 12345 / NEGATIVE_INFINITY
// 22. 12345 / NAN
// 23. NEGATIVE_INFINITY / 12345
// 24. INFINITY / 12345
// 25. NAN / 12345

START_TEST(test_s21_mod) {
  s21_decimal tmp1[] = {
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                    // 1
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                    // 2
      {{5, 0, 0, 0}, s21_NORMAL_VALUE},                    // 3
      {{56, 0, 0, 0x00010000}, s21_NORMAL_VALUE},          // 4
      {{56, 0, 0, 0x80010000}, s21_NORMAL_VALUE},          // 5
      {{56, 0, 0, 0x80010000}, s21_NORMAL_VALUE},          // 6
      {{56, 0, 0, 0x00010000}, s21_NORMAL_VALUE},          // 7
      {{0, 0, 0, 0}, s21_INFINITY},                        // 8
      {{0xffffffff, 0xffffffff, 0, 0}, s21_NORMAL_VALUE},  // 9
      {{0xffffffff, 0xffffffff, 0xffffffff, 0x80000000},
       s21_NORMAL_VALUE},                                          // 10
      {{0, 0, 0, 0}, s21_NAN},                                     // 11
      {{0, 0, 0, 0}, s21_NAN},                                     // 12
      {{0, 0, 0, 0}, s21_NAN},                                     // 13
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                       // 14
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                       // 15
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                       // 16
      {{0, 0, 0, 0}, s21_INFINITY},                                // 17
      {{0, 0, 0, 0}, s21_INFINITY},                                // 18
      {{0, 0, 0, 0}, s21_INFINITY},                                // 19
      {{1, 0, 0, 0}, s21_NORMAL_VALUE},                            // 20
      {{1, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                   // 21
      {{12345, 0, 0, 0}, s21_NORMAL_VALUE},                        // 22
      {{12345, 0, 0, 0}, s21_NORMAL_VALUE},                        // 23
      {{12345, 0, 0, 0}, s21_NORMAL_VALUE},                        // 24
      {{0, 0, 0, 0}, s21_INFINITY},                                // 25
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},                       // 26
      {{0, 0, 0, 0}, s21_NAN},                                     // 27
      {{2, 0, 0, 0}, s21_NORMAL_VALUE},                            // 28 (6)
      {{0xFFFFFFF6, 0xFFFFFFFF, 0x9, 0x10000}, s21_NORMAL_VALUE},  // 29 (15)
      {{0x82E25AED, 0xF9E825AD, 0xB03EF094, 0x800E0000},
       s21_NORMAL_VALUE},  // 30 (16)
      {{0x82E25AED, 0xF9E825AD, 0xB03EF094, 0x800E0000},
       s21_NORMAL_VALUE},  // 31 (17)
      {{0x4EE43976, 0x4B35A1D9, 0x19B974BF, 0xF0000},
       s21_NORMAL_VALUE},  // 32 (18)
      {{0x910AF4CA, 0xC045D5F2, 0x27E41B00, 0x150000},
       s21_NORMAL_VALUE},  // 33 (19)
      {{0x910AF4CA, 0xC045D5F2, 0x27E41B00, 0x80150000},
       s21_NORMAL_VALUE},  // 34 (20)
      {{0x910AF4CA, 0xC045D5F2, 0x27E41B00, 0x150000},
       s21_NORMAL_VALUE},  // 35 (21)
      {{0x910AF4CA, 0xC045D5F2, 0x27E41B00, 0x80150000},
       s21_NORMAL_VALUE},                                             // 36 (22)
      {{0x3EFC5BC9, 0x3FD5E08B, 0x4745, 0x90000}, s21_NORMAL_VALUE},  // 37 (23)
      {{0xBC490AE6, 0xE0E, 0x0, 0x80000000}, s21_NORMAL_VALUE},       // 38 (24)
      {{0x54403D8C, 0x8B520559, 0x1FA981AD, 0x800C0000},
       s21_NORMAL_VALUE},  // 39 (30)
      {{0xB0F708D, 0x96B05951, 0x11A17B81, 0x1C0000},
       s21_NORMAL_VALUE},                                    // 40 (31)
      {{0x58431906, 0x72D605, 0x0, 0x0}, s21_NORMAL_VALUE},  // 41 (34)
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                      // 42 (39)
      {{0x1, 0, 0, 0}, s21_NORMAL_VALUE},                    // 43 (44)
      {{0x1, 0, 0, 0x80000000}, s21_NORMAL_VALUE},           // 44 (50)
      {{0x5, 0x0, 0x0, 0x140000}, s21_NORMAL_VALUE},         // 45 (66)
  };

  s21_decimal tmp2[] = {
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                  // 1
      {{12345, 0, 0, 0}, s21_NORMAL_VALUE},              // 2
      {{3, 0, 0, 0}, s21_NORMAL_VALUE},                  // 3
      {{33, 0, 0, 0x00010000}, s21_NORMAL_VALUE},        // 4
      {{33, 0, 0, 0x00010000}, s21_NORMAL_VALUE},        // 5
      {{33, 0, 0, 0x80010000}, s21_NORMAL_VALUE},        // 6
      {{33, 0, 0, 0x80010000}, s21_NORMAL_VALUE},        // 7
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},             // 8
      {{01, 0, 0, 0x00000000}, s21_NORMAL_VALUE},        // 9
      {{01, 0, 0, 0x80010000}, s21_NORMAL_VALUE},        // 10
      {{0, 0, 0, 0}, s21_NAN},                           // 11
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},             // 12
      {{0, 0, 0, 0}, s21_INFINITY},                      // 13
      {{0, 0, 0, 0}, s21_NAN},                           // 14
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},             // 15
      {{0, 0, 0, 0}, s21_INFINITY},                      // 16
      {{0, 0, 0, 0}, s21_NAN},                           // 17
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},             // 18
      {{0, 0, 0, 0}, s21_INFINITY},                      // 19
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                  // 20
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                  // 21
      {{0, 0, 0, 0}, s21_INFINITY},                      // 22
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},             // 23
      {{0, 0, 0, 0}, s21_NAN},                           // 24
      {{12345, 0, 0, 0}, s21_INFINITY},                  // 25
      {{12345, 0, 0, 0}, s21_NEGATIVE_INFINITY},         // 26
      {{12345, 0, 0, 0}, s21_NAN},                       // 27
      {{0x2D, 0x0, 0x0, 0x80020000}, s21_NORMAL_VALUE},  // 28 (6)
      {{0x398BA562, 0x0, 0x0, 0x0}, s21_NORMAL_VALUE},   // 29 (15)
      {{0x340972E, 0x0, 0x0, 0x0}, s21_NORMAL_VALUE},    // 30 (16)
      {{0x481B1F98, 0xFBFD896D, 0x46D77, 0x80180000},
       s21_NORMAL_VALUE},                                // 31 (17)
      {{0x4E, 0x0, 0x0, 0x80020000}, s21_NORMAL_VALUE},  // 32 (18)
      {{0x11CDD185, 0xF5BC6FC0, 0x2D50F3F, 0x100000},
       s21_NORMAL_VALUE},  // 33 (19)
      {{0x11CDD185, 0xF5BC6FC0, 0x2D50F3F, 0x100000},
       s21_NORMAL_VALUE},  // 34 (20)
      {{0x11CDD185, 0xF5BC6FC0, 0x2D50F3F, 0x80100000},
       s21_NORMAL_VALUE},  // 35 (21)
      {{0x11CDD185, 0xF5BC6FC0, 0x2D50F3F, 0x80100000},
       s21_NORMAL_VALUE},                                          // 36 (22)
      {{0x45, 0x0, 0x0, 0x20000}, s21_NORMAL_VALUE},               // 37 (23)
      {{0x8, 0x0, 0x0, 0x10000}, s21_NORMAL_VALUE},                // 38 (24)
      {{0x65F24DAC, 0x7A9AB, 0x0, 0x80060000}, s21_NORMAL_VALUE},  // 39 (30)
      {{0x17254C02, 0xF5159B, 0x0, 0x110000}, s21_NORMAL_VALUE},   // 40 (31)
      {{0xBE118597, 0xD93D12C6, 0x59648, 0x110000},
       s21_NORMAL_VALUE},                               // 41 (34)
      {{0x0, 0x0, 0x0, 0x0}, s21_NORMAL_VALUE},         // 42 (39)
      {{0x0, 0x0, 0x0, 0x0}, s21_NORMAL_VALUE},         // 43 (44)
      {{0x0, 0x0, 0x0, 0x0}, s21_NORMAL_VALUE},         // 44 (50)
      {{0x159, 0x0, 0x0, 0x190000}, s21_NORMAL_VALUE},  // 45 (66)
  };
  s21_decimal res[] = {
      {{0, 0, 0, 0}, s21_NAN},                                        // 1
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                               // 2
      {{2, 0, 0, 0}, s21_NORMAL_VALUE},                               // 3
      {{23, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                     // 4
      {{23, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                     // 5
      {{23, 0, 0, 0x80010000}, s21_NORMAL_VALUE},                     // 6
      {{23, 0, 0, 0x00010000}, s21_NORMAL_VALUE},                     // 7
      {{0, 0, 0, 0}, s21_NAN},                                        // 8
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},                               // 9
      {{0, 0, 0, 0x80000000}, s21_NORMAL_VALUE},                      // 10
      {{0, 0, 0, 0}, s21_NAN},                                        // 11
      {{0, 0, 0, 0}, s21_NAN},                                        // 12
      {{0, 0, 0, 0}, s21_NAN},                                        // 13
      {{0, 0, 0, 0}, s21_NAN},                                        // 14
      {{0, 0, 0, 0}, s21_NAN},                                        // 15
      {{0, 0, 0, 0}, s21_NAN},                                        // 16
      {{0, 0, 0, 0}, s21_NAN},                                        // 17
      {{0, 0, 0, 0}, s21_NAN},                                        // 18
      {{0, 0, 0, 0}, s21_NAN},                                        // 19
      {{0, 0, 0, 0}, s21_NAN},                                        // 20
      {{0, 0, 0, 0}, s21_NAN},                                        // 21
      {{12345, 0, 0, 0}, s21_NORMAL_VALUE},                           // 22
      {{12345, 0, 0, 0}, s21_NORMAL_VALUE},                           // 23
      {{0, 0, 0, 0}, s21_NAN},                                        // 24
      {{0, 0, 0, 0}, s21_NAN},                                        // 25
      {{0, 0, 0, 0}, s21_NAN},                                        // 26
      {{0, 0, 0, 0}, s21_NAN},                                        // 27
      {{0x14, 0x0, 0x0, 0x20000}, s21_NORMAL_VALUE},                  // 28 (6)
      {{0xAA083616, 0x1, 0x0, 0x10000}, s21_NORMAL_VALUE},            // 29 (15)
      {{0xBD425AED, 0x9FAA782, 0xD9, 0x800E0000}, s21_NORMAL_VALUE},  // 30 (16)
      {{0x73CE5B20, 0x4C75EEC, 0xE23E, 0x80180000},
       s21_NORMAL_VALUE},                                       // 31 (17)
      {{0x4CC67976, 0x1EAE7, 0x0, 0xF0000}, s21_NORMAL_VALUE},  // 32 (18)
      {{0x910AF4CA, 0xC045D5F2, 0x27E41B00, 0x150000},
       s21_NORMAL_VALUE},  // 33 (19)
      {{0x910AF4CA, 0xC045D5F2, 0x27E41B00, 0x80150000},
       s21_NORMAL_VALUE},  // 34 (20)
      {{0x910AF4CA, 0xC045D5F2, 0x27E41B00, 0x150000},
       s21_NORMAL_VALUE},  // 35 (21)
      {{0x910AF4CA, 0xC045D5F2, 0x27E41B00, 0x150000},
       s21_NORMAL_VALUE},                                   // 36 (22)
      {{0x1C9A2AC9, 0x0, 0x0, 0x90000}, s21_NORMAL_VALUE},  // 37 (23)
      {{0x4, 0x0, 0x0, 0x80010000}, s21_NORMAL_VALUE},      // 38 (24)
      {{0xACBA508C, 0xD2B824C5, 0x24, 0x800C0000},
       s21_NORMAL_VALUE},  // 39 (30)
      {{0xB0F708D, 0x96B05951, 0x11A17B81, 0x1C0000},
       s21_NORMAL_VALUE},  // 40 (31)
      {{0x4CB9A4EA, 0x37C5B6DF, 0x2D172, 0x110000},
       s21_NORMAL_VALUE},                            // 41 (34)
      {{0x0, 0x0, 0x0, 0x0}, s21_NAN},               // 42 (39)
      {{0x0, 0x0, 0x0, 0x0}, s21_NAN},               // 43 (44)
      {{0x0, 0x0, 0x0, 0x0}, s21_NAN},               // 44 (50)
      {{0x5F, 0x0, 0x0, 190000}, s21_NORMAL_VALUE},  // 45 в(66)
  };

  for (int i = 0; i < 21; i++) {
    s21_decimal ret = s21_mod(tmp1[i], tmp2[i]);
    if (is_all_field_is_equal(ret, res[i]) == FALSE) {
      print_error4(tmp1[i], tmp2[i], ret, res[i], "s21_mod", i);
    }
  }
}
END_TEST

START_TEST(test_s21_from_int_to_decimal) {
  const int count_tests = 8;

  int test[] = {0, -0, 5, -5, INT_MAX, -INT_MAX, 0, -67, 67};
  s21_decimal res[] = {
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},
      {{0, 0, 0, 0x80000000}, s21_NORMAL_VALUE},
      {{5, 0, 0, 0}, s21_NORMAL_VALUE},
      {{5, 0, 0, 0x80000000}, s21_NORMAL_VALUE},
      {{0x7fffffff, 0, 0, 0}, s21_NORMAL_VALUE},
      {{0x7fffffff, 0, 0, 0x80000000}, s21_NORMAL_VALUE},
      {{9, 0, 0, 0x80020000}, s21_NORMAL_VALUE},
      {{6789, 0, 0, 0x80020000},
       s21_NORMAL_VALUE},  //  Если в числе типа decimal есть дробная часть, то
                           //  её следует отбросить (например, 0.9 преобразуется
                           //  0)
      {{6789, 0, 0, 0x00020000},
       s21_NORMAL_VALUE},  //  Если в числе типа decimal есть дробная часть, то
                           //  её следует отбросить (например, 0.9 преобразуется
                           //  0)
  };
  //  s21_FROM_INT_to_decimal
  for (int i = 0; i < count_tests - 2; i++) {
    s21_decimal tmp;
    int ret = s21_from_int_to_decimal(test[i], &tmp);
    if (ret != SUCCESS || (is_all_field_is_equal(tmp, res[i]) == FALSE &&
                           is_zero(res[i]) == FALSE && is_zero(tmp) == FALSE)) {
      print_decimal(tmp);
      ck_abort_msg(
          "s21_from_int_to_decimal check error. Input int = %d. Return code = "
          "%d.\nDecimal:\n",
          test[i], ret);
    }
  }

  int etalon_ret[] = {SUCCESS, SUCCESS,          SUCCESS,
                      SUCCESS, CONVERTING_ERROR, CONVERTING_ERROR,
                      SUCCESS, SUCCESS,          SUCCESS};
  //  s21_FROM_DECIMAL_to_int
  for (int i = 0; i < count_tests; i++) {
    int tmp;
    int ret = s21_from_decimal_to_int(res[i], &tmp);
    if (ret != etalon_ret[i] || tmp != test[i]) {
      print_decimal(res[i]);
      ck_abort_msg(
          "s21_from_decimal_to_int check error. Test number =%d, Output int = "
          "%d. Test int = %d. Return code = %d.\nDecimal:\n",
          i, tmp, test[i], ret);
    }
  }
}
END_TEST

START_TEST(test_s21_negate) {
  s21_decimal res[] = {
      {{0, 0, 0, 0x80000000}, s21_NORMAL_VALUE},
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},
      {{5, 0, 0, 0x80000000}, s21_NORMAL_VALUE},
      {{6, 0, 0, 0x00010000}, s21_NORMAL_VALUE},
      {{0x7fffffff, 0, 0, 0x80000000}, s21_NORMAL_VALUE},
      {{0x7fffffff, 0, 0, 0}, s21_NORMAL_VALUE},
      {{0, 0, 0, 0}, s21_NAN},
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},
      {{0, 0, 0, 0}, s21_INFINITY},
      {{0, 0, 0, 0}, s21_INFINITY},
      {{0xffffffff, 0xffffffff, 0xffffffff, 0x001C0000}, s21_NORMAL_VALUE},
  };

  for (int i = 0; i < count_decimals; i++) {
    s21_decimal ret = s21_negate(decimals[i]);
    if (is_all_field_is_equal(ret, res[i]) == FALSE) {
      print_error2(decimals[i], ret, res[i], "s21_negate", i);
    }
  }
}
END_TEST

/**
 * Comparision
 */
#define comparison_test_count 18

void comparision_test(int (*func)(s21_decimal a, s21_decimal b), char *f_name,
                      int *need_res) {
  static const s21_decimal dec[6] = {
      {{10, 100, 1000, 0}, s21_NORMAL_VALUE},           // 0
      {{10, 100, 1000, 0x00020000}, s21_NORMAL_VALUE},  // 1 scale = 2
      {{10, 100, 1000, 0x80000000}, s21_NORMAL_VALUE},  // 2  -
      {{10, 100, 1000, 0}, s21_NAN},                    // 3
      {{10, 100, 1000, 0}, s21_NEGATIVE_INFINITY},      // 4
      {{10, 100, 1000, 0}, s21_INFINITY}                // 5
  };
  s21_decimal params[comparison_test_count][2] = {
      {dec[0], dec[0]},  // 1. два одинаковых без scale:
      {dec[1], dec[1]},  // 2. два одинаковых со scale:
      {dec[2], dec[2]},  // 3. два одинаковых со знаком:
      {dec[2], dec[0]},  // 4. два одинаковых с разными знаками:
      {dec[0], dec[2]},  // 5. наоборот знаки
      {dec[0], dec[1]},  // 6. одинаковые знаки, разные scale
      {dec[1], dec[0]},  // 7. наоборот scale
      {dec[2], dec[3]},  // 8.  dec, NAN
      {dec[3], dec[3]},  // 9.  NAN, NAN
      {dec[5], dec[4]},  // 10.  INF, -INF
      {dec[4], dec[5]},  // 11. -INF, INF
      {dec[5], dec[5]},  // 12.  INF, INF
      {dec[4], dec[4]},  // 13. -INF, -INF
      {dec[3], dec[2]},  // 14.  NAN, dec
      {dec[2], dec[5]},  // 15.  dec, INF
      {dec[5], dec[2]},  // 16.  INF, dec
      {dec[2], dec[4]},  // 17.  dec, -INF
      {dec[4], dec[2]},  // 18.  -INF, dec
  };

  for (int i = 0; i < comparison_test_count; i++) {
    int r1 = func(params[i][0], params[i][1]);
    if (r1 != need_res[i])
      print_comparision_error(params[i][0], params[i][1], f_name, i, r1,
                              need_res[i]);
  }
}

// Меньше или равно

START_TEST(test_s21_is_less_or_equal) {
  int need_res[comparison_test_count] = {
      TRUE,  // 1. два одинаковых без scale:
      TRUE,  // 2. два одинаковых со scale:
      TRUE,  // 3. два одинаковых со знаком:
      TRUE,  // 4. два одинаковых с разными знаками:
      FALSE,  // 5. наоборот знаки
      FALSE,  // 6. одинаковые знаки, разные scale
      TRUE,   // 7. наоборот scale
      FALSE,  // 8.  dec, NAN
      FALSE,  // 9.  NAN, NAN
      FALSE,  // 10.  INF, -INF
      TRUE,   // 11. -INF, INF
      TRUE,   // 12.  INF, INF
      TRUE,   // 13. -INF, -INF
      FALSE,  // 14.  NAN, dec
      TRUE,   // 15.  dec, INF
      FALSE,  // 16.  INF, dec
      FALSE,  // 17.  dec, -INF
      TRUE    // 18  -INF, dec
  };
  comparision_test(s21_is_less_or_equal, "s21_is_less_or_equal", need_res);
}
END_TEST

// Меньше

START_TEST(test_s21_is_less) {
  int need_res[comparison_test_count] = {
      FALSE,  // два одинаковых без scale:
      FALSE,  // два одинаковых со scale:
      FALSE,  // два одинаковых со знаком:
      TRUE,   // два одинаковых с разными знаками:
      FALSE,  // наоборот знаки
      FALSE,  // одинаковые знаки, разные scale
      TRUE,   // наоборот scale
      FALSE,  //  dec, NAN
      FALSE,  //  NAN, NAN
      FALSE,  //  INF, -INF
      TRUE,   // -INF, INF
      FALSE,  //  INF, INF
      FALSE,  // -INF, -INF
      FALSE,  //  NAN, dec
      TRUE,   //  dec, INF
      FALSE,  //  INF, dec
      FALSE,  //  dec, -INF
      TRUE    //  -INF, dec
  };
  comparision_test(s21_is_less, "s21_is_less", need_res);
}
END_TEST

//  Больше
START_TEST(test_s21_is_greater) {
  int need_res[comparison_test_count] = {
      FALSE,  // 1. два одинаковых без scale:
      FALSE,  // 2. два одинаковых со scale:
      FALSE,  // 3. два одинаковых со знаком:
      FALSE,  // 4. два одинаковых с разными знаками:
      TRUE,   // 5. наоборот знаки
      TRUE,   // 6. одинаковые знаки, разные scale
      FALSE,  // 7. наоборот scale
      FALSE,  // 8.  dec, NAN
      FALSE,  // 9.  NAN, NAN
      TRUE,   // 10  INF, -INF
      FALSE,  // 11. -INF, INF
      FALSE,  // 12.  INF, INF
      FALSE,  // 13. -INF, -INF
      FALSE,  // 14.  NAN, dec
      FALSE,  // 15.  dec, INF
      TRUE,   // 16  INF, dec
      TRUE,   // 17.  dec, -INF
      FALSE   // 18.  -INF, dec
  };
  comparision_test(s21_is_greater, "s21_is_greater", need_res);
}
END_TEST

//  Больше или равно
START_TEST(test_s21_is_greater_or_equal) {
  int need_res[comparison_test_count] = {
      TRUE,   // 1. два одинаковых без scale:
      TRUE,   // 2. два одинаковых со scale:
      TRUE,   // 3. два одинаковых со знаком:
      FALSE,  // 4. два одинаковых с разными знаками:
      TRUE,   // 5. наоборот знаки
      TRUE,   // 6. одинаковые знаки, разные scale
      FALSE,  // 7. наоборот scale
      FALSE,  // 8.  dec, NAN
      FALSE,  // 9.  NAN, NAN
      TRUE,   // 10.  INF, -INF
      FALSE,  // 11. -INF, INF
      TRUE,   // 12.  INF, INF
      TRUE,   // 13. -INF, -INF
      FALSE,  // 14.  NAN, dec
      FALSE,  // 15.  dec, INF
      TRUE,   // 16.  INF, dec
      TRUE,   // 17.  dec, -INF
      FALSE   //  -INF, dec
  };
  comparision_test(s21_is_greater_or_equal, "s21_is_greater_or_equal",
                   need_res);
}
END_TEST

//  Равно

START_TEST(test_s21_is_equal) {
  int need_res[comparison_test_count] = {
      TRUE,   // 1. два одинаковых без scale:
      TRUE,   // 2. два одинаковых со scale:
      TRUE,   // 3. два одинаковых со знаком:
      FALSE,  // 4. два одинаковых с разными знаками:
      FALSE,  // 5. наоборот знаки
      FALSE,  // 6. одинаковые знаки, разные scale
      FALSE,  // 7. наоборот scale
      FALSE,  // 8.  dec, NAN
      FALSE,  // 9.  NAN, NAN
      FALSE,  // 10.  INF, -INF
      FALSE,  // 11. -INF, INF
      TRUE,   // 12.  INF, INF
      TRUE,   // 13. -INF, -INF
      FALSE,  // 14.  NAN, dec
      FALSE,  // 15.  dec, INF
      FALSE,  // 16.  INF, dec
      FALSE,  // 17.  dec, -INF
      FALSE   // 18.  -INF, dec
  };
  comparision_test(s21_is_equal, "s21_is_equal", need_res);
}
END_TEST

//  Не равно
START_TEST(test_s21_is_not_equal) {
  int need_res[comparison_test_count] = {
      FALSE,  // 1. два одинаковых без scale:
      FALSE,  // 2. два одинаковых со scale:
      FALSE,  // 3. два одинаковых со знаком:
      TRUE,  // 4. два одинаковых с разными знаками:
      TRUE,  // 5. наоборот знаки
      TRUE,  // 6. одинаковые знаки, разные scale
      TRUE,  // 7. наоборот scale
      TRUE,  // 8.  dec, NAN
      TRUE,  // 9.  NAN, NAN
      TRUE,  // 10.  INF, -INF
      TRUE,  // 11. -INF, INF
      FALSE,  // 12.  INF, INF
      FALSE,  // 13. -INF, -INF
      TRUE,   // 14.  NAN, dec
      TRUE,   // 15.  dec, INF
      TRUE,   // 16.  INF, dec
      TRUE,   // 17.  dec, -INF
      TRUE    // 18.  -INF, dec
  };
  comparision_test(s21_is_not_equal, "s21_is_not_equal", need_res);
}
END_TEST

START_TEST(test_s21_floor) {
  s21_decimal tmp1[] = {
      {{29, 0, 0, 0x00010000}, s21_NORMAL_VALUE},        // 1
      {{15, 0, 0, 0x00010000}, s21_NORMAL_VALUE},        // 2
      {{03, 0, 0, 0x00010000}, s21_NORMAL_VALUE},        // 3
      {{05, 0, 0, 0x80010000}, s21_NORMAL_VALUE},        // 4
      {{29, 0, 0, 0x80010000}, s21_NORMAL_VALUE},        // 5
      {{51928374, 0, 0, 0x00070000}, s21_NORMAL_VALUE},  // 6
      {{51928374, 0, 0, 0x80070000}, s21_NORMAL_VALUE},  // 7
      {{0, 0, 0, 0}, s21_INFINITY},                      // 8
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},             // 9
      {{0, 0, 0, 0}, s21_NAN},                           // 10
  };

  s21_decimal res[] = {
      {{2, 0, 0, 0}, s21_NORMAL_VALUE},           // 1
      {{1, 0, 0, 0}, s21_NORMAL_VALUE},           // 2
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},           // 3
      {{1, 0, 0, 0x80000000}, s21_NORMAL_VALUE},  // 4
      {{3, 0, 0, 0x80000000}, s21_NORMAL_VALUE},  // 5
      {{5, 0, 0, 0x00000000}, s21_NORMAL_VALUE},  // 6
      {{6, 0, 0, 0x80000000}, s21_NORMAL_VALUE},  // 7
      {{0, 0, 0, 0}, s21_INFINITY},               // 8
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},      // 9
      {{0, 0, 0, 0}, s21_NAN},                    // 10
  };

  for (int i = 0; i < 10; i++) {
    s21_decimal ret = s21_floor(tmp1[i]);
    if (is_all_field_is_equal(ret, res[i]) == FALSE) {
      print_error2(tmp1[i], ret, res[i], "s21_floor", i);
    }
  }
}
END_TEST

START_TEST(test_s21_round) {
  s21_decimal tmp1[] = {
      {{29, 0, 0, 0x00010000}, s21_NORMAL_VALUE},        // 1
      {{15, 0, 0, 0x00010000}, s21_NORMAL_VALUE},        // 2
      {{03, 0, 0, 0x00010000}, s21_NORMAL_VALUE},        // 3
      {{05, 0, 0, 0x80010000}, s21_NORMAL_VALUE},        // 4
      {{29, 0, 0, 0x80010000}, s21_NORMAL_VALUE},        // 5
      {{51928374, 0, 0, 0x00070000}, s21_NORMAL_VALUE},  // 6
      {{51928374, 0, 0, 0x80070000}, s21_NORMAL_VALUE},  // 7
      {{100, 0, 0, 0}, s21_INFINITY},                    // 8
      {{178, 0, 0, 0}, s21_NEGATIVE_INFINITY},           // 9
      {{199, 0, 0, 0}, s21_NAN},                         // 10
  };

  s21_decimal res[] = {
      {{3, 0, 0, 0}, s21_NORMAL_VALUE},           // 1
      {{2, 0, 0, 0}, s21_NORMAL_VALUE},           // 2
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},           // 3
      {{1, 0, 0, 0x80000000}, s21_NORMAL_VALUE},  // 4
      {{3, 0, 0, 0x80000000}, s21_NORMAL_VALUE},  // 5
      {{5, 0, 0, 0x00000000}, s21_NORMAL_VALUE},  // 6
      {{5, 0, 0, 0x80000000}, s21_NORMAL_VALUE},  // 7
      {{0, 0, 0, 0}, s21_INFINITY},               // 8
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},      // 9
      {{0, 0, 0, 0}, s21_NAN},                    // 10
  };

  for (int i = 0; i < 10; i++) {
    s21_decimal ret = s21_round(tmp1[i]);
    if (is_all_field_is_equal(ret, res[i]) == FALSE) {
      print_error2(tmp1[i], ret, res[i], "s21_round", i);
    }
  }
}
END_TEST

START_TEST(test_s21_truncate) {
  s21_decimal tmp1[] = {
      {{29, 0, 0, 0x00010000}, s21_NORMAL_VALUE},        // 1
      {{15, 0, 0, 0x00010000}, s21_NORMAL_VALUE},        // 2
      {{03, 0, 0, 0x00010000}, s21_NORMAL_VALUE},        // 3
      {{05, 0, 0, 0x80010000}, s21_NORMAL_VALUE},        // 4
      {{29, 0, 0, 0x80010000}, s21_NORMAL_VALUE},        // 5
      {{51928374, 0, 0, 0x00070000}, s21_NORMAL_VALUE},  // 6
      {{51928374, 0, 0, 0x80070000}, s21_NORMAL_VALUE},  // 7
      {{100, 0, 0, 0}, s21_INFINITY},                    // 8
      {{178, 0, 0, 0}, s21_NEGATIVE_INFINITY},           // 9
      {{199, 0, 0, 0}, s21_NAN},                         // 10
  };

  s21_decimal res[] = {
      {{2, 0, 0, 0}, s21_NORMAL_VALUE},           // 1
      {{1, 0, 0, 0}, s21_NORMAL_VALUE},           // 2
      {{0, 0, 0, 0}, s21_NORMAL_VALUE},           // 3
      {{0, 0, 0, 0x80000000}, s21_NORMAL_VALUE},  // 4
      {{2, 0, 0, 0x80000000}, s21_NORMAL_VALUE},  // 5
      {{5, 0, 0, 0x00000000}, s21_NORMAL_VALUE},  // 6
      {{5, 0, 0, 0x80000000}, s21_NORMAL_VALUE},  // 7
      {{0, 0, 0, 0}, s21_INFINITY},               // 8
      {{0, 0, 0, 0}, s21_NEGATIVE_INFINITY},      // 9
      {{0, 0, 0, 0}, s21_NAN},                    // 10
  };

  for (int i = 0; i < 10; i++) {
    s21_decimal ret = s21_truncate(tmp1[i]);
    if (is_all_field_is_equal(ret, res[i]) == FALSE) {
      print_error2(tmp1[i], ret, res[i], "s21_truncate", i);
    }
  }
}
END_TEST

int main() {
  Suite *s1 = suite_create("Core");
  TCase *tc1_1 = tcase_create("Core");
  SRunner *sr = srunner_create(s1);
  int nf;
  srunner_set_fork_status(sr, CK_NOFORK);
  suite_add_tcase(s1, tc1_1);

  tcase_add_test(tc1_1, test_s21_add);
  tcase_add_test(tc1_1, test_s21_mul);
  tcase_add_test(tc1_1, test_s21_from_int_to_decimal);
  tcase_add_test(tc1_1, test_s21_from_float_to_decimal);
  tcase_add_test(tc1_1, test_s21_negate);
  tcase_add_test(tc1_1, test_s21_floor);
  tcase_add_test(tc1_1, test_s21_round);
  tcase_add_test(tc1_1, test_s21_truncate);
  tcase_add_test(tc1_1, test_s21_is_less_or_equal);
  tcase_add_test(tc1_1, test_s21_is_less);
  tcase_add_test(tc1_1, test_s21_sub);
  tcase_add_test(tc1_1, test_s21_div);
  tcase_add_test(tc1_1, test_s21_mod);
  tcase_add_test(tc1_1, test_s21_is_greater);
  tcase_add_test(tc1_1, test_s21_is_greater_or_equal);
  tcase_add_test(tc1_1, test_s21_is_equal);
  tcase_add_test(tc1_1, test_s21_is_not_equal);

  srunner_run_all(sr, CK_ENV);
  nf = srunner_ntests_failed(sr);
  srunner_free(sr);

  return nf == 0 ? 0 : 1;
}

// ошибка сравнения
void print_comparision_error(s21_decimal dec1, s21_decimal dec2, char *f_name,
                             int i, int res, int need_res) {
  char *fmt =
      "\n   Comparision function name: %s\n   test_number (start from 1)=%d "
      "\n   Result=%s(%d)\n   Need result=%s(%d) "
      "\n   Input decimal: \n%s "
      "\n   Result decimal: \n%s";
  char *p_d1 = to_string(dec1);
  char *p_d2 = to_string(dec2);
  ck_abort_msg(fmt, f_name, i + 1, char_boolean[res], res,
               char_boolean[need_res], need_res, p_d1, p_d2);
  free(p_d1);
  free(p_d2);
}

// еррор по 3 параметрам: два входных, один выходной, один эталонный
void print_error2(s21_decimal dec1, s21_decimal res_dec, s21_decimal etalon_dec,
                  char *f_name, int i) {
  char *fmt =
      "\n   Error function name: %s\n   test_number (start from 1)=%d"
      "\n   Input ONE decimal: \n%s"
      "\n   Result decimal: \n%s"
      "\n   ETALON Result decimal: \n%s";
  char *p_d1 = to_string(dec1);
  char *p_res = to_string(res_dec);
  char *p_et = to_string(etalon_dec);
  ck_abort_msg(fmt, f_name, i + 1, p_d1, p_res, p_et);
  free(p_d1);
  free(p_res);
  free(p_et);
}

// еррор по 4 параметрам: два входных, один выходной, один эталонный
void print_error4(s21_decimal dec1, s21_decimal dec2, s21_decimal res_dec,
                  s21_decimal etalon_dec, char *f_name, int i) {
  char *fmt =
      "\n   Error function name: %s\n   test_number (start from 1)=%d"
      "\n   Input ONE decimal: \n%s"
      "\n   Input TWO decimal: \n%s"
      "\n   Result decimal: \n%s"
      "\n   ETALON Result decimal: \n%s";

  char *p_d1 = to_string(dec1);
  char *p_d2 = to_string(dec2);
  char *p_res = to_string(res_dec);
  char *p_et = to_string(etalon_dec);

  ck_abort_msg(fmt, f_name, i + 1, p_d1, p_d2, p_res, p_et);
  printf(fmt, f_name, i + 1, p_d1, p_d2, p_res, p_et);
  free(p_d1);
  free(p_d2);
  free(p_res);
  free(p_et);
}
