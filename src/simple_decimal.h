#ifndef SRC_SIMPLE_DECIMAL_H_
#define SRC_SIMPLE_DECIMAL_H_

#define SZ_SD 256

#define MAX(a, b) ((a) > (b) ? (a) : (b))


enum {SD_OK = 100, SD_OVERFLOW = 111};

typedef char simple_decimal[SZ_SD+2];
#define FIRST_NOL SZ_SD+1

static simple_decimal sd_1  = {1};
static simple_decimal sd_2  = {0, 1};
static simple_decimal sd_5  = {1, 0, 1, 0};
static simple_decimal sd_10 = {0, 1, 0, 1};


void        from_decimal(s21_decimal src, simple_decimal *dst);
int         to_decimal(s21_decimal *dst, simple_decimal *src, int need_scale, int please_one_more_div);
void        init_sd(simple_decimal *dst);
int         sd_is_zero(simple_decimal);
void        copy_sd(simple_decimal src, simple_decimal *dst);
void        print_sd(simple_decimal src);
void        pint_sd_numbers(simple_decimal src);
int         get_sign_after_sub(simple_decimal src);
int         get_first_1_position_from_end(simple_decimal one);
int         get_first_1_position_from_begin(simple_decimal one);
void        invert_del_res_and_delete_minus_one(simple_decimal *tmp_res);

int         get_non_zero_bits_count(simple_decimal one);
void        sd_delete_first_0(simple_decimal *src);
void        sd_cut(simple_decimal one, simple_decimal *res, int start_pos, int count);
int         count_sd_numbers(simple_decimal src);

int         sd_sub(simple_decimal, simple_decimal, simple_decimal *, ...);  
int         sd_sum(simple_decimal, simple_decimal, simple_decimal *, ...);
int         sd_mul(simple_decimal, simple_decimal, simple_decimal *, ...);
int         sd_div(simple_decimal, simple_decimal, simple_decimal *, ...);
int         sd_div_int(simple_decimal, simple_decimal, simple_decimal *, simple_decimal *, int *count_numbers);

int         sd_is_grater_or_equal(simple_decimal one, simple_decimal two);


s21_decimal s21_sum_from_sd(s21_decimal, s21_decimal);
s21_decimal s21_sub_from_sd(s21_decimal, s21_decimal);
s21_decimal s21_mul_from_sd(s21_decimal, s21_decimal);
s21_decimal s21_div_from_sd(s21_decimal, s21_decimal);
s21_decimal s21_mod_from_sd(s21_decimal one, s21_decimal two);

s21_decimal s21_from_sd(int (*func)(simple_decimal, simple_decimal, simple_decimal *, ...), s21_decimal, s21_decimal, int need_scale, int result_sign);
s21_decimal check_s21_sub_from_sd(s21_decimal one, s21_decimal two, int *is_stop);

s21_decimal s21_nan_mul (s21_decimal one, s21_decimal two);
s21_decimal s21_nan_sub (s21_decimal one, s21_decimal two);
s21_decimal s21_nan_div (s21_decimal one, s21_decimal two);
s21_decimal s21_nan_mod (s21_decimal one, s21_decimal two);
#endif
