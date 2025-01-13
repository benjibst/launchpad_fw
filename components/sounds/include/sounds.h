#pragma once
extern const size_t s0_len;
extern const unsigned char s0_data[];
extern const size_t s1_len;
extern const unsigned char s1_data[];
extern const size_t s2_len;
extern const unsigned char s2_data[];
extern const size_t s3_len;
extern const unsigned char s3_data[];
extern const size_t s4_len;
extern const unsigned char s4_data[];
const unsigned char *sounds[] = {
    s0_data,
    s1_data,
    s2_data,
    s3_data,
    s4_data,
};
const size_t *const sound_lengths[] = {
    &s0_len,
    &s1_len,
    &s2_len,
    &s3_len,
    &s4_len,
};
const size_t num_sounds = 5;
