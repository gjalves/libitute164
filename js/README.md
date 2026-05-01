# @itute164/e164

Browser-first JavaScript implementation of the libitute164 phone number parser.
It reads the same `e164-plan.txt` format used by the C library and keeps the
loaded plan in memory.

## Browser Usage

```js
import { E164Number, E164Plan, RESTRICT_AREA } from "./src/index.js";

const planText = await fetch("/e164-plan.txt").then((response) => response.text());
const plan = E164Plan.fromText(planText);

const phone = new E164Number(plan, {
  countryCode: 55,
  areaCode: 19,
  restriction: RESTRICT_AREA,
  acceptAlphanumeric: true,
});

phone.setValue("912345678");

console.log(phone.value);             // "5519912345678"
console.log(phone.getValue());        // "+55 (19) 91234-5678"
console.log(phone.getContextValue()); // "91234-5678"
console.log(phone.getCountry());      // "pt_BR"
```

The package does not fetch the plan by itself. Browser applications should load
or bundle the text file and pass its contents to `E164Plan.fromText()`.

## API

### `E164Plan`

`E164Plan.fromText(text)` parses a plan and returns an `E164Plan`.

`plan.loadText(text)` replaces the current plan transactionally. If parsing
fails, the previous plan remains active and an exception is thrown.

Lookup helpers:

- `plan.ccToType(countryCode)`
- `plan.areaToType(countryCode, areaCode)`
- `plan.ccToCountry(countryCode)`
- `plan.areaToCountry(countryCode, areaCode)`
- `plan.nationalPrefix(countryCode)`
- `plan.internationalPrefix(countryCode)`

### `E164Number`

`new E164Number(plan, context)` creates a number bound to an `E164Plan`.

Context fields:

- `countryCode`
- `areaCode`
- `restriction`
- `acceptAlphanumeric`

Restriction constants:

- `RESTRICT_NONE`
- `RESTRICT_COUNTRY`
- `RESTRICT_AREA`

Number methods and properties:

- `phone.value`
- `phone.setValue(value)`
- `phone.setContext(context)`
- `phone.getValue()`
- `phone.getContextValue()`
- `phone.getCountry()`
- `phone.isComplete()`
- `phone.addDigit(digit)`
- `phone.delDigit()`

## Demo

The browser demo loads `../data/e164-plan.txt` with `fetch`, so serve the
repository over HTTP:

```sh
python3 -m http.server 8000
```

Open `http://localhost:8000/js/demo/`.

## Tests And Validation

```sh
npm test
npm run check-plan
```

From the repository root:

```sh
make js-test
make js-check-plan
```

## Plan Regex Compatibility

Subscriber rules use JavaScript `RegExp`. The current shared plan intentionally
uses a regex subset that is compatible with both JavaScript `RegExp` and the C
implementation's POSIX extended regular expressions. New plan rules should stay
within that shared subset unless both implementations are updated together.
