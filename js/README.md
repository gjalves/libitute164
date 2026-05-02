# @itute164/e164

Browser-first JavaScript implementation of the libitute164 phone number parser.
It reads the same `e164-plan.txt` format used by the C library and keeps the
loaded plan in memory.

## Browser Usage

```js
import { E164Number, E164Plan, INPUT_MODE_NUMBER, RESTRICT_AREA } from "./src/index.js";

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

console.log(phone.value);             // "5519912345678"
console.log(phone.getValue());        // "+55 (19) 91234-5678"
console.log(phone.getContextValue()); // "91234-5678"
console.log(phone.getDialingValue()); // "912345678"
console.log(phone.getCountry());      // "pt_BR"
console.log(phone.getNumberKind());   // "regular"
```

The package does not fetch the plan by itself. Browser applications should load
or bundle the text file and pass its contents to `E164Plan.fromText()`.

## Web Component

Importing `input-element.js` registers `<itute164-input>`:

```js
import { E164Plan } from "./src/index.js";
import "./src/input-element.js";

const planText = await fetch("/e164-plan.txt").then((response) => response.text());
const plan = E164Plan.fromText(planText);
const input = document.querySelector("itute164-input");

input.plan = plan;
input.addEventListener("itute164-change", (event) => {
  console.log(event.detail.value);
});
```

```html
<itute164-input
  country-code="55"
  area-code="19"
  carrier-code="15"
  input-mode="number"
  restriction="area"
  value="912345678"
  show-details>
</itute164-input>
```

Attributes and properties:

- `plan`
- `value`
- `rawValue`
- `countryCode` / `country-code`
- `areaCode` / `area-code`
- `carrierCode` / `carrier-code`
- `inputMode` / `input-mode`
- `restriction`
- `acceptAlphanumeric` / `accept-alphanumeric`
- `complete`
- `country`
- `numberKind`

The component keeps its own editing buffer, displays the formatted contextual
number in the input, limits extra characters when the number is complete, and
emits `itute164-change` with `{ value, rawValue, displayValue, contextValue,
dialingValue, country, kind, complete }`.

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
- `plan.carrierCodeLength(countryCode)`
- `plan.hasCarrierCode(countryCode, carrierCode)`

### `E164Number`

`new E164Number(plan, context)` creates a number bound to an `E164Plan`.

Context fields:

- `countryCode`
- `areaCode`
- `carrierCode`
- `restriction`
- `acceptAlphanumeric`
- `inputMode`

Restriction constants:

- `RESTRICT_NONE`
- `RESTRICT_COUNTRY`
- `RESTRICT_AREA`

Input mode constants:

- `INPUT_MODE_NUMBER`
- `INPUT_MODE_DIALING`

`INPUT_MODE_NUMBER` is the default logical number-entry mode. It may apply the
configured country or area context while normalizing to E.164. `INPUT_MODE_DIALING`
expects a dial string from a known origin and requires both `countryCode` and
`areaCode`; national and international prefixes from the plan are then treated
as required dialing syntax. When the plan defines carrier selection codes, set
`carrierCode` to generate complete long-distance dialing strings.

Number methods and properties:

- `phone.value`
- `phone.setValue(value)`
- `phone.setContext(context)`
- `phone.getValue()`
- `phone.getContextValue()`
- `phone.getDialingValue()`
- `phone.getCountry()`
- `phone.getNumberKind()`
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
