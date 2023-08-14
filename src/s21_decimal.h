#ifndef SRC_S21_DECIMAL_H_
#define SRC_S21_DECIMAL_H_

#define TRUE    0
#define FALSE   1



#define SUCCESS  0
#define CONVERTING_ERROR    1

#define DIRECT_LEFT 1
#define DIRECT_RIGHT -1

#define ALIGMENT_OVERFLOW   2
#define ALIGMENT_OK         3

typedef unsigned int uint;
typedef unsigned long long ull;
typedef unsigned char uchar;
//  #define unsigned int uint
//  #define ulint   unsigned long long int
#define uchar   unsigned char

typedef enum {
    s21_NORMAL_VALUE        = 0,
    s21_INFINITY            = 1,
    s21_NEGATIVE_INFINITY   = 2,
    s21_NAN                 = 3
}
value_type_t;

typedef struct {
    uint            bits[7];
    value_type_t    value_type;
}
s21_super_decimal;

typedef struct {
    uint            bits[4];
    value_type_t    value_type;
}
s21_decimal;

//  для упрощенного обращения к битам LOW==0, MID==1 и тп
enum {
    LOW, MID, HIGHT, SCALE
};

#define SCALE_SUPER_DECIMAL 6

//  конвертер int -> массив из четырех байтов и обратно.
//  нужеен чтобы scale и знак не заморачиваясь устанавливать
union converter {
    uint    number;
    uchar   bytes[4];
};

union converter_float {
    float   fl;
    uint    dw;
};

union converter_ull {
    ull    number;
    uint   bytes[2];
};



#define DEC_MINUS   128
#define DEC_PLUS    0

#define MAX_BITS    96
#define MAX_SCALE   28

#define CONST_DEF_DECIMAL {{0,  0, 0, 0}, 0}
#define CONST_DECIMAL_ONE {{1,  0, 0, 0}, 0}
#define CONST_DECIMAL_TWO {{2,  0, 0, 0}, 0}
#define CONST_DECIMAL_TEN {{10, 0, 0, 0}, 0}
#define CONST_DECIMAL_FIVE {{5, 0, 0, 0}, 0}

#define CONST_DEF_SUPER_DECIMAL {{0, 0, 0, 0, 0, 0, 0}, 0}


/**
 * Арифметические операторы
 */

//  Сложение
//  //  алгоритм https://answer-id.com/ru/51913387
s21_decimal s21_add(s21_decimal, s21_decimal);
s21_decimal s21_add_overflow(s21_decimal one, s21_decimal two, int *control_overflow);

//  Вычитание
s21_decimal s21_sub(s21_decimal, s21_decimal);
//  Умножение
s21_decimal s21_mul(s21_decimal, s21_decimal);
//  Деление
s21_decimal s21_div(s21_decimal, s21_decimal);
//  Остаток от деления
s21_decimal s21_mod(s21_decimal, s21_decimal);



/**
 * Операторы сравнение
 *  Возвращаемое значение:
 *      0 - TRUE
 *      1 - FALSE
 */

//  Меньше
int s21_is_less(s21_decimal, s21_decimal);
//  Меньше или равно
int s21_is_less_or_equal(s21_decimal, s21_decimal);
//  Больше
int s21_is_greater(s21_decimal, s21_decimal);
//  Больше или равно
int s21_is_greater_or_equal(s21_decimal, s21_decimal);
//  Равно
int s21_is_equal(s21_decimal, s21_decimal);
//  Не равно
int s21_is_not_equal(s21_decimal, s21_decimal);

/**
 * Преобразователи
 *  Возвращаемое значение - код ошибки:
 *      0 - SUCCESS
 *      1 - CONVERTING ERROR
 */

//  из int
int s21_from_int_to_decimal(int src, s21_decimal *dst);
//  из float
int s21_from_float_to_decimal(float src, s21_decimal *dst);
//  В int
int s21_from_decimal_to_int(s21_decimal src, int *dst);
//  В float
int s21_from_decimal_to_float(s21_decimal src, float *dst);


// Округляет указанное Decimal число до ближайшего целого числа в сторону отрицательной бесконечности.
s21_decimal s21_floor(s21_decimal);
// Округляет Decimal до ближайшего целого числа.
s21_decimal s21_round(s21_decimal);
// Возвращает целые цифры указанного Decimal числа; любые дробные цифры отбрасываются.
s21_decimal s21_truncate(s21_decimal);
// Возвращает результат умножения указанного Decimal на -1.
s21_decimal s21_negate(s21_decimal);


/**
 * другие нужные методы
 */
uchar       get_scale(s21_decimal decimal);
void        set_scale(s21_decimal *decimal, uchar scale);
uchar       get_sign(s21_decimal decimal);
void        set_sign(s21_decimal *decimal, uchar sign);
void        invert_sign(s21_decimal *decimal);
uchar       get_bit(s21_decimal decimal, uchar index);
void        set_bit(s21_decimal *decimal, uchar index, uchar bit_value);

/**
 * Битовые операции
 */
s21_decimal s21_and(s21_decimal *one, s21_decimal *two);
s21_decimal s21_or(s21_decimal *one, s21_decimal *two);
s21_decimal s21_exclude_or(s21_decimal *one, s21_decimal *two);
s21_decimal s21_not(s21_decimal one);
s21_decimal shift(s21_decimal dec, int to_shift, int direction);
s21_decimal shift_overflow(s21_decimal dec, int to_shift, int direction, int control_overflow);

/**
 * Проверяторы
 */
int         is_zero(s21_decimal decimal);
int         is_inf(s21_decimal decimal);
int         is_nan(s21_decimal decimal);
int         is_all_field_is_equal(s21_decimal one, s21_decimal two);

/**
 * Установщики значений
 */
void        set_zero(s21_decimal *decimal);
void        copy(s21_decimal src, s21_decimal *dst);
s21_decimal set_infinity(int znak);
s21_decimal set_nan();

/**
 * Печать
 */
void        print_decimal(s21_decimal decimal);
char        *to_string(s21_decimal decimal);
void        print_decimal_to_numbers(s21_decimal decimal);


// Выравнивание scale
// возвращает SUCCESS или CONVERTING_ERROR, в случачае если уравнять не удалось (переполнение)
int         alignment_scale(s21_decimal *one, s21_decimal *two);
int         get_counts_of_digits(int number);

s21_decimal pow_by_ten(s21_decimal dec, int add_scale);
s21_decimal s21_decimal_div_ten(s21_decimal src);

s21_decimal s21_add_pjian(s21_decimal one, s21_decimal two);
s21_decimal s21_mul_pjian(s21_decimal one, s21_decimal two);
s21_decimal s21_div_pjian(s21_decimal one, s21_decimal two);
int         s21_check_bits_zero(s21_decimal src);
int         s21_is_greater_pjian_or_equal_pjian(s21_decimal one, s21_decimal two);


s21_decimal s21_div_r(s21_decimal one, s21_decimal two);

#define UNUSED(x) (void)(x)

#endif  // SRC_S21_DECIMAL_H_
