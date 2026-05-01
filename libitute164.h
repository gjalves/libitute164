#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>

enum itu_t_area_type_enum {
    ITU_T_AREA_UNKNOWN,
    ITU_T_AREA_INCOMPLETE,
    ITU_T_AREA_NUMBER,
};

enum itu_t_e123_type_enum {
    ITU_T_E123_UNKNOWN,
    ITU_T_E123_NATIONAL,
    ITU_T_E123_INTERNATIONAL,
};

enum itu_t_e164_type_enum {
    ITU_T_UNKNOWN,
    ITU_T_INCOMPLETE,
    ITU_T_RESERVED,
    ITU_T_SPARE,
    ITU_T_NUMBER,
    ITU_T_GEOGRAPHIC,
    ITU_T_GLOBAL,
    ITU_T_GROUPS,
    ITU_T_TRIALS,
};

// Country_code
typedef struct {
    enum itu_t_e164_type_enum type;
    uint8_t len;
    uint16_t value;
} itu_t_e164_cc_t;

typedef struct {
    uint16_t country_code;
    uint32_t area_code;
    uint8_t restriction;
    uint8_t accept_alphanumeric;
} itu_t_e164_context_t;

#define ITU_T_E164_CONTEXT_RESTRICT_NONE 0
#define ITU_T_E164_CONTEXT_RESTRICT_COUNTRY 1
#define ITU_T_E164_CONTEXT_RESTRICT_AREA 2

typedef struct {
    int pos;
    char value[16];
    uint64_t raw_phone;
    uint64_t raw_extension;
    enum itu_t_e123_type_enum e123_type;
    itu_t_e164_cc_t cc;
    itu_t_e164_context_t context;
    uint8_t input_country_explicit;
    uint8_t input_area_explicit;

    union {
        // ITU_T_NUMBER,
        struct {
            enum itu_t_area_type_enum type;

            // National Destination Code
            uint8_t ndc_len;
            uint32_t ndc;

            // Subscriber Number
            uint8_t sn_len;
            uint64_t sn;

            const char *mask;
        } number;

        // ITU_T_GEOGRAPHIC
        struct {
            // Global Subscriber Number
            uint64_t gsn;
        } geographic;

        // ITU_T_GLOBAL
        struct {
            // Identification Code
            uint8_t ic_len;
            uint32_t ic;

            // Subscriber Number
            uint8_t sn_len;
            uint64_t sn;
        } global;

        // ITU_T_GROUPS
        struct {
            // Group Identification Code
            uint8_t gic;

            // Subscriber Number
            uint8_t sn_len;
            uint64_t sn;
        } groups;

        // ITU_T_TRIALS
        struct {
            // Group Identification Code
            uint8_t tic;

            // Subscriber Number
            uint8_t sn_len;
            uint64_t sn;
        } trials;
    };
} itu_t_e164_t;

struct cc_regex {
    const char *regex_ndc;
    const char *regex_sn;
    const char *mask_sn;
};

enum itu_t_e164_type_enum itu_t_e164_cc_2_type(int country_code);
const char *itu_t_e164_cc_2_country(int country_code);
const char *itu_t_e164_cc_2_national_prefix(int country_code);
const char *itu_t_e164_cc_2_international_prefix(int country_code);
enum itu_t_area_type_enum itu_t_e164_area_2_type(int country_code, int area_code);
struct cc_regex *itu_t_e164_cc_subscriber_regex(int country_code);
int itu_t_e164_load_plan_file(const char *path);
int itu_t_e164_load_plan_fp(FILE *fp);
int itu_t_e164_load_default_plan(void);
const char *itu_t_e164_plan_error(void);
void itu_t_e164_reset_plan(void);
void itu_t_e164_init(itu_t_e164_t *e164);
void itu_t_e164_set_context(itu_t_e164_t *e164, const itu_t_e164_context_t *context);
void itu_t_e164_set_value(itu_t_e164_t *e164, const char *value);
ssize_t itu_t_e164_get_value(itu_t_e164_t *e164, char *buffer, ssize_t size);
ssize_t itu_t_e164_get_context_value(itu_t_e164_t *e164, char *buffer, ssize_t size);
int itu_t_e164_add_digit(itu_t_e164_t *e164, char digit);
int itu_t_e164_del_digit(itu_t_e164_t *e164);
