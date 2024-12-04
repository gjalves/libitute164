
# libITU-T-E164

`libITU-T-E164` is a C library for handling phone numbers in the ITU-T E.164 standard, including support for global, geographic, group numbers, and more. The library provides a simple interface for initialization, configuration, and phone number manipulation, focusing on compliance with international specifications.

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
    union {
        struct {
            enum itu_t_area_type_enum type;
            uint8_t ndc_len;
            uint32_t ndc;
            uint8_t sn_len;
            uint64_t sn;
            const char *mask;
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

## Main Functions

### Initialization
```c
void itu_t_e164_init(itu_t_e164_t *e164);
```
Initializes an `itu_t_e164_t` structure.

### Setting Values
```c
void itu_t_e164_set_value(itu_t_e164_t *e164, const char *value);
```
Sets the value of a phone number.

### Retrieving Values
```c
ssize_t itu_t_e164_get_value(itu_t_e164_t *e164, char *buffer, ssize_t size);
```
Retrieves the value of a number formatted as a string.

### Digit Manipulation
- **Add digit**:
  ```c
  int itu_t_e164_add_digit(itu_t_e164_t *e164, char digit);
  ```
- **Remove digit**:
  ```c
  int itu_t_e164_del_digit(itu_t_e164_t *e164);
  ```

### Type Determination
```c
enum itu_t_e164_type_enum itu_t_e164_cc_2_type(int country_code);
enum itu_t_area_type_enum itu_t_e164_area_2_type(int country_code, int area_code);
```
Determines the number or area code type based on country or area code.

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
