
# libITU-T-E164

`libITU-T-E164` is a C library for handling phone numbers in the ITU-T E.164 standard, including support for global, geographic, group numbers, and more. The library provides a simple interface for initialization, configuration, and phone number manipulation, focusing on compliance with international specifications.

It is intended for applications that need to normalize, validate, format, and
derive dialing strings from telephone numbers while keeping regulatory
numbering data outside the compiled library. A browser demo is published at
https://gjalves.github.io/libitute164/js/demo/.

## Features

- **ITU-T E.164 standard support**:
  - Number types, including `GEOGRAPHIC`, `GLOBAL`, `GROUPS`, and `TRIALS`.
  - Differentiation between national, global, and subscriber codes.
  
- **Number manipulation**:
  - Structure initialization.
  - Setting values.
  - Real-time addition and removal of digits.

- **Validation and masking**:
  - Determination of number and area code types.
  - Masks and regular expressions associated with national and international numbers.

## JavaScript Browser Package

The repository also includes a browser-first JavaScript implementation in
`js/`. It reads the same `e164-plan.txt` format and keeps the loaded plan in an
explicit `E164Plan` instance, so browser applications can fetch or bundle the
plan text themselves:

```js
import { E164Number, E164Plan, INPUT_MODE_NUMBER, RESTRICT_AREA } from "@itute164/e164";

const planText = await fetch("/e164-plan.txt").then((response) => response.text());
const plan = E164Plan.fromText(planText);

const phone = new E164Number(plan, {
  countryCode: 55,
  areaCode: 19,
  carrierCode: 15,
  restriction: RESTRICT_AREA,
  acceptAlphanumeric: true,
  inputMode: INPUT_MODE_NUMBER,
});

phone.setValue("912345678");
phone.value;             // "5519912345678"
phone.getValue();        // "+55 (19) 91234-5678"
phone.getContextValue(); // "91234-5678"
phone.getDialingValue(); // "912345678"
phone.getCountry();      // "pt_BR"
```

The browser input behavior is available as a Web Component:

```js
import { E164Plan } from "@itute164/e164";
import "@itute164/e164/input-element";

const planText = await fetch("/e164-plan.txt").then((response) => response.text());
document.querySelector("itute164-input").plan = E164Plan.fromText(planText);
```

```html
<itute164-input
  country-code="55"
  area-code="19"
  carrier-code="15"
  input-mode="number"
  restriction="area"
  show-details>
</itute164-input>
```

Run the JavaScript tests with:

```sh
cd js
npm test
```

The published browser demo is available at
https://gjalves.github.io/libitute164/js/demo/.

For local development, the browser demo in `js/demo/` loads
`data/e164-plan.txt` with `fetch`, so it must be served over HTTP:

```sh
python3 -m http.server 8000
```

Then open `http://localhost:8000/js/demo/`.

### JavaScript API

`E164Plan.fromText(text)` parses the external numbering plan and returns an
`E164Plan`. `plan.loadText(text)` replaces an existing plan transactionally:
if parsing fails, the previous plan remains active. Lookup helpers include
`ccToType(countryCode)`, `areaToType(countryCode, areaCode)`,
`ccToCountry(countryCode)`, `areaToCountry(countryCode, areaCode)`,
`nationalPrefix(countryCode)`, and `internationalPrefix(countryCode)`.

`new E164Number(plan, context)` creates a number bound to a plan. The context
fields are `countryCode`, `areaCode`, `carrierCode`, `restriction`,
`acceptAlphanumeric`, and `inputMode`. `carrierCode` may be set when a country
requires a carrier selection code for long-distance dialing. Use
`RESTRICT_NONE`, `RESTRICT_COUNTRY`, and `RESTRICT_AREA` for
the restriction value. Use `INPUT_MODE_NUMBER` for logical number entry and
`INPUT_MODE_DIALING` for dial-string entry from a known origin; dialing mode
requires both `countryCode` and `areaCode`.

`E164Number` exposes the normalized digits in `phone.value` and provides
`setValue(value)`, `setContext(context)`, `getValue()`, `getContextValue()`,
`getDialingValue()`, `getCountry()`, `isComplete()`, `addDigit(digit)`, and
`delDigit()`.

## Data Structures

### `itu_t_e164_t`
Main structure for manipulating ITU-T E.164 numbers.

```c
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
        struct {
            enum itu_t_area_type_enum type;
            uint8_t ndc_len;
            uint32_t ndc;
            uint8_t sn_len;
            uint64_t sn;
            const char *mask;
            enum itu_t_e164_number_kind_enum kind;
        } number;
        struct {
            uint64_t gsn;
        } geographic;
        struct {
            uint8_t ic_len;
            uint32_t ic;
            uint8_t sn_len;
            uint64_t sn;
        } global;
        struct {
            uint8_t gic;
            uint8_t sn_len;
            uint64_t sn;
        } groups;
        struct {
            uint8_t tic;
            uint8_t sn_len;
            uint64_t sn;
        } trials;
    };
} itu_t_e164_t;
```

### `itu_t_e164_cc_t`
Structure for representing country codes in the ITU-T E.164 standard.

```c
typedef struct {
    enum itu_t_e164_type_enum type;
    uint8_t len;
    uint16_t value;
} itu_t_e164_cc_t;
```

## API Reference

### Number Lifecycle

```c
void itu_t_e164_init(itu_t_e164_t *e164);
void itu_t_e164_set_context(itu_t_e164_t *e164, const itu_t_e164_context_t *context);
void itu_t_e164_set_value(itu_t_e164_t *e164, const char *value);
ssize_t itu_t_e164_get_value(itu_t_e164_t *e164, char *buffer, ssize_t size);
ssize_t itu_t_e164_get_context_value(itu_t_e164_t *e164, char *buffer, ssize_t size);
ssize_t itu_t_e164_get_dialing_value(itu_t_e164_t *e164, char *buffer, ssize_t size);
int itu_t_e164_get_country_code(const itu_t_e164_t *e164);
int itu_t_e164_get_area_code(const itu_t_e164_t *e164);
int itu_t_e164_get_carrier_code(const itu_t_e164_t *e164);
const char *itu_t_e164_get_carrier_name(const itu_t_e164_t *e164);
ssize_t itu_t_e164_get_national_value(const itu_t_e164_t *e164, char *buffer, ssize_t size);
ssize_t itu_t_e164_get_subscriber_value(const itu_t_e164_t *e164, char *buffer, ssize_t size);
int itu_t_e164_add_digit(itu_t_e164_t *e164, char digit);
int itu_t_e164_del_digit(itu_t_e164_t *e164);
```

`itu_t_e164_init()` zeroes an `itu_t_e164_t` instance. Call it before using a
number.

`itu_t_e164_set_context()` sets or clears the default locality and input
policy. Passing `NULL` clears the context. A context is stored per
`itu_t_e164_t` instance.

`itu_t_e164_set_value()` parses a formatted or unformatted input string,
normalizes it into `e164->value`, and refreshes the derived fields in
`e164->cc` and `e164->number`. Passing `NULL` clears the number while keeping
the current context. Input that exceeds the valid length or fails regulatory
rules is truncated to the accepted prefix. With context restrictions, values
outside the restricted country or area are rejected.

`itu_t_e164_get_value()` writes the complete international presentation, such
as `+55 (19) 91234-5678`. It returns the number of bytes that would be written
by the internal formatter. When the number is empty, the historical complete
presentation is `+`.

`itu_t_e164_get_context_value()` writes the presentation relative to the
current context. In an area-restricted Brazilian context, for example,
`+55 (19) 91234-5678` is presented as `91234-5678`.

The component getters expose parsed pieces for applications that need to store
or route them separately: `country_code`/DDI, `area_code`/DDD or NDC,
`carrier_code`/CSP from context, optional carrier name from the numbering
plan, the national value without DDI, and the subscriber value without DDI or
DDD/NDC. Textual getters preserve significant zeroes.

`itu_t_e164_add_digit()` and `itu_t_e164_del_digit()` are low-level digit
editing helpers. They update the same derived fields as `set_value()`, but
they accept digits only and do not apply formatted input prefixes like `+` or
`(`.

### Context

```c
typedef struct {
    uint16_t country_code;
    uint32_t area_code;
    uint8_t restriction;
    uint8_t accept_alphanumeric;
    uint8_t input_mode;
    uint16_t carrier_code;
} itu_t_e164_context_t;
```

`country_code` is the default DDI. `area_code` is the default DDD/NDC inside
that country. Leaving `country_code` as zero disables default locality.
`carrier_code` selects the long-distance carrier when the numbering plan
requires one for dialing strings.

`restriction` controls whether the context is only a default or also a policy:

```c
#define ITU_T_E164_CONTEXT_RESTRICT_NONE 0
#define ITU_T_E164_CONTEXT_RESTRICT_COUNTRY 1
#define ITU_T_E164_CONTEXT_RESTRICT_AREA 2
```

`ITU_T_E164_CONTEXT_RESTRICT_NONE` keeps the policy open. Local input uses the
default context, but explicit `+` or `(` input can escape it.

`ITU_T_E164_CONTEXT_RESTRICT_COUNTRY` accepts only numbers in the context
country. `ITU_T_E164_CONTEXT_RESTRICT_AREA` accepts only numbers in the
context country and area.

When `accept_alphanumeric` is non-zero, `itu_t_e164_set_value()` accepts
letters and maps them through the telephone keypad before validation:
`ABC -> 2`, `DEF -> 3`, `GHI -> 4`, `JKL -> 5`, `MNO -> 6`, `PQRS -> 7`,
`TUV -> 8`, and `WXYZ -> 9`.

`input_mode` controls how input text is interpreted:

```c
#define ITU_T_E164_INPUT_MODE_NUMBER 0
#define ITU_T_E164_INPUT_MODE_DIALING 1
```

`ITU_T_E164_INPUT_MODE_NUMBER` treats input as a logical phone number.
`ITU_T_E164_INPUT_MODE_DIALING` treats input as a dial string from the current
context, including national or international prefixes and carrier selection
codes where the plan defines them.

### Lookup Helpers

```c
enum itu_t_e164_type_enum itu_t_e164_cc_2_type(int country_code);
enum itu_t_area_type_enum itu_t_e164_area_2_type(int country_code, int area_code);
const char *itu_t_e164_cc_2_country(int country_code);
const char *itu_t_e164_area_2_country(int country_code, int area_code);
const char *itu_t_e164_get_country(const itu_t_e164_t *e164);
enum itu_t_e164_number_kind_enum itu_t_e164_get_number_kind(const itu_t_e164_t *e164);
const char *itu_t_e164_number_kind_name(enum itu_t_e164_number_kind_enum kind);
const char *itu_t_e164_cc_2_national_prefix(int country_code);
const char *itu_t_e164_cc_2_international_prefix(int country_code);
int itu_t_e164_cc_2_carrier_code_length(int country_code);
int itu_t_e164_cc_has_carrier_code(int country_code, int carrier_code);
const char *itu_t_e164_cc_2_carrier_name(int country_code, int carrier_code);
struct cc_regex *itu_t_e164_cc_subscriber_regex(int country_code);
```

These functions expose the active numbering plan. Built-in tables are used
when no external rule overrides the requested entry. Country tags use the
`ll_CC` form, such as `pt_BR`; missing country tags and prefixes return
`NULL`. `itu_t_e164_area_2_country()` returns an area-specific country tag
when one exists and otherwise falls back to the country-code tag.
`itu_t_e164_get_country()` applies the same lookup to an already parsed
number. `itu_t_e164_get_number_kind()` returns classification metadata from
the subscriber rule that matched the parsed number, such as `regular`,
`toll-free`, `short`, `premium`, `emergency`, or `service`.

### External Numbering Plans
The library can load numbering plan data from a text file at runtime. Loaded
rules override the built-in tables; missing rules continue to use the built-in
fallbacks. The plan is kept in memory after loading; the library does not
watch or re-read the file automatically. Call one of the load functions again
to reload it, or `itu_t_e164_reset_plan()` to discard the loaded plan. Reloads
are transactional: if parsing fails, the previously active plan remains in use.

```c
int itu_t_e164_load_plan_file(const char *path);
int itu_t_e164_load_plan_fp(FILE *fp);
int itu_t_e164_load_plan_memory(const char *data, size_t size);
int itu_t_e164_load_default_plan(void);
const char *itu_t_e164_plan_error(void);
void itu_t_e164_reset_plan(void);
```

`itu_t_e164_load_plan_file()` loads from a filesystem path.
`itu_t_e164_load_plan_fp()` reads from an existing `FILE *`.
`itu_t_e164_load_plan_memory()` reads exactly `size` bytes from memory; the
buffer does not need to be NUL-terminated.

All load functions return `0` on success and `-1` on failure. On failure,
`itu_t_e164_plan_error()` returns a human-readable error string. Successful
loads clear the previous error. `itu_t_e164_reset_plan()` discards the active
external plan and returns lookups to built-in fallback data.

Plan files are UTF-8-compatible text files using one directive per line.
Fields are separated by ASCII whitespace. Empty lines are ignored. Lines whose
first non-whitespace token starts with `#` are comments. Double quotes can be
used around a field that contains spaces, which is mainly useful for masks.

```text
cc 55 number
country 55 pt_BR
area-country 1 416 en_CA
national-prefix 55 0
international-prefix 55 00
carrier-code-length 55 2
carrier-code 55 15 Vivo
area 55 19 number
subscriber 55 * ^9[0-9]{0,8}$ #####-####
subscriber 598 * ^[0-9]{0,8}$ "# ### ####"
```

Supported directives:

```text
cc <country-code> <unknown|incomplete|reserved|spare|number|geographic|global|groups|trials>
country <country-code> <ll_CC>
area-country <country-code> <area-code> <ll_CC>
national-prefix <country-code> <digits>
international-prefix <country-code> <digits>
carrier-code-length <country-code> <digits>
carrier-code <country-code> <digits> [name]
area <country-code> <area-code> <unknown|incomplete|number>
subscriber <country-code> <ndc-regex|*> <subscriber-regex> <mask> [kind]
```

`country-code` must be in the range `0..999`. `area-code` must be in the range
`0..9999`. Country tags must use the `ll_CC` form, such as `pt_BR`. Dial
prefixes must contain digits only.

The `country` directive stores one tag per country code and should only be used
when the country code alone identifies the country. Shared E.164 country codes
should omit `country` unless a reliable default exists. Use `area-country` for
numbering plans where the national destination code identifies the country or
territory, such as NANP `+1` NPAs.

`cc` defines how a country code is classified. `area` defines the national
destination code hierarchy for countries whose type is `number`. Use
`incomplete` entries for prefixes that need more digits before they become a
complete area code.

`carrier-code-length` marks countries where long-distance dialing includes a
carrier selection code after the national or international prefix. Repeated
`carrier-code` entries optionally restrict accepted carrier codes for that
country; if no carrier codes are listed, any code with the configured length is
accepted. A `carrier-code` entry may include an optional display name for the
carrier; quote it when it contains spaces.

`subscriber` defines validation and formatting for the subscriber part. The
NDC regex is matched against the area code; use `*` to match any area code.
The subscriber regex is matched against the subscriber number. The mask must
contain at least one `#`; each `#` consumes one subscriber digit and any other
character is copied literally to the formatted output. Subscriber rules are
evaluated in file order for the country. The optional `kind` field classifies
the matched number and defaults to `regular`; accepted values are `unknown`,
`regular`, `toll-free`, `short`, `premium`, `emergency`, and `service`.

Default locality is application context, not numbering-plan data. Set it on an
`itu_t_e164_t` instance to accept national or local input without `+`:

```c
itu_t_e164_context_t context = {
    .country_code = 55,
    .area_code = 19,
    .restriction = ITU_T_E164_CONTEXT_RESTRICT_AREA,
};

itu_t_e164_set_context(&e164, &context);
itu_t_e164_set_value(&e164, "912345678");    /* +55 (19) 91234-5678 */
itu_t_e164_set_value(&e164, "019912345678"); /* +55 (19) 91234-5678 */
```

See the Context section for the restriction and alphanumeric input policy.

The repository includes `data/e164-plan.txt` and a validator:

```bash
tools/plan-check data/e164-plan.txt
make check-plan
make check
make sanitize
make ci
make benchmark
```

`make benchmark` builds and runs `tools/bench-validate`, a microbenchmark for
the phone-number validation path. Pass an iteration count to the tool directly,
for example `tools/bench-validate 1000000`. For comparable results, build with
the same compiler flags each time, such as `make clean && make CFLAGS='-O2 -g'
benchmark`.

`itu_t_e164_load_default_plan()` first checks the `LIBITUTE164_PLAN`
environment variable, then `/etc/libitute164/e164-plan.txt`, then
`/usr/share/libitute164/e164-plan.txt`.

## Usage Example

```c
#include "libitute164.h"

int main() {
    itu_t_e164_t number;
    itu_t_e164_init(&number);

    itu_t_e164_set_value(&number, "+1234567890");
    char buffer[16];
    itu_t_e164_get_value(&number, buffer, sizeof(buffer));
    printf("Number: %s\n", buffer);

    itu_t_e164_add_digit(&number, '1');
    itu_t_e164_del_digit(&number);

    return 0;
}
```

## Installation

Clone the repository and compile the library:

```bash
git clone https://github.com/yourproject/libITU-T-E164.git
cd libITU-T-E164
make
```

Include the library in your project by adding `libitute164.h` and linking the generated binary.

## Contribution

Contributions are welcome! Follow these steps to submit improvements or fixes:

1. Fork the repository.
2. Create a branch for your changes: `git checkout -b my-improvement`.
3. Push your changes: `git push origin my-improvement`.
4. Open a Pull Request.

## License

This project is licensed under the [BSD 3-Clause License](LICENSE).
