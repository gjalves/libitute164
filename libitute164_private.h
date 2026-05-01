#ifndef LIBITUTE164_PRIVATE_H
#define LIBITUTE164_PRIVATE_H

#include "libitute164.h"

int itu_t_e164_plan_cc_lookup(int country_code, enum itu_t_e164_type_enum *type);
int itu_t_e164_plan_area_lookup(int country_code, int area_code, enum itu_t_area_type_enum *type);
struct cc_regex *itu_t_e164_plan_subscriber_regex(int country_code);

#endif
