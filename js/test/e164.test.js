import { readFileSync } from "node:fs";
import { dirname, join } from "node:path";
import { describe, it } from "node:test";
import assert from "node:assert/strict";
import { fileURLToPath } from "node:url";
import {
  E164Number,
  E164Plan,
  INPUT_MODE_DIALING,
  RESTRICT_AREA,
  RESTRICT_COUNTRY,
  RESTRICT_NONE,
  NUMBER_KIND_REGULAR,
  NUMBER_KIND_TOLL_FREE,
} from "../src/index.js";

const __dirname = dirname(fileURLToPath(import.meta.url));
const planText = readFileSync(join(__dirname, "../../data/e164-plan.txt"), "utf8");

function loadedPlan() {
  return E164Plan.fromText(planText);
}

describe("E164Plan", () => {
  it("loads the shared text plan and resolves country tags", () => {
    const plan = loadedPlan();

    assert.equal(plan.ccToCountry(55), "pt_BR");
    assert.equal(plan.ccToCountry(1), null);
    assert.equal(plan.areaToCountry(1, 416), "en_CA");
    assert.equal(plan.areaToCountry(1, 469), "en_US");
    assert.equal(plan.areaToCountry(1, 800), null);
    assert.equal(plan.nationalPrefix(55), "0");
    assert.equal(plan.internationalPrefix(55), "00");
    assert.equal(plan.carrierCodeLength(55), 2);
    assert.equal(plan.hasCarrierCode(55, 15), true);
    assert.equal(plan.hasCarrierCode(55, 55), false);
  });

  it("rejects malformed country tags transactionally", () => {
    const plan = new E164Plan();
    plan.loadText("country 55 pt_BR\n");

    assert.throws(() => plan.loadText("country 55 pt-BR\n"), /invalid country tag/);
    assert.equal(plan.ccToCountry(55), "pt_BR");
  });

  it("rejects unknown directives transactionally", () => {
    const plan = new E164Plan();
    plan.loadText("cc 55 number\ncountry 55 pt_BR\n");

    assert.throws(() => plan.loadText("cc 55 number\nbogus 55\n"), /unknown directive/);
    assert.equal(plan.ccToCountry(55), "pt_BR");
  });
});

describe("E164Number", () => {
  it("formats complete international numbers", () => {
    const plan = loadedPlan();
    const phone = new E164Number(plan);

    phone.setValue("+551912345678");
    assert.equal(phone.getValue(), "+55 (19) 1234-5678");
    assert.equal(phone.value, "551912345678");
    assert.equal(phone.getCountry(), "pt_BR");

    phone.setValue("+542966123456");
    assert.equal(phone.getValue(), "+54 (2966) 123456");

    phone.setValue("+59812345678");
    assert.equal(phone.getValue(), "+598 1 234 5678");
  });

  it("accepts Brazilian 0800 numbers from a default local context", () => {
    const plan = loadedPlan();
    const phone = new E164Number(plan, {
      countryCode: 55,
      areaCode: 19,
      restriction: RESTRICT_NONE,
    });

    phone.setValue("08000101010");
    assert.equal(phone.value, "558000101010");
    assert.equal(phone.getValue(), "+55 800 010 10 10");
    assert.equal(phone.getContextValue(), "0800 010 10 10");
    assert.equal(phone.getCountry(), "pt_BR");
    assert.equal(phone.getNumberKind(), NUMBER_KIND_TOLL_FREE);

    phone.setValue("080001010101");
    assert.equal(phone.value, "558000101010");
    assert.equal(phone.getNumberKind(), NUMBER_KIND_TOLL_FREE);
  });

  it("handles countries without subscriber masks", () => {
    const plan = loadedPlan();
    const phone = new E164Number(plan);

    phone.setValue("+31123456");
    assert.equal(phone.getValue(), "+31 123456");
    assert.equal(phone.value, "31123456");
    assert.equal(phone.getCountry(), "nl_NL");
    assert.equal(phone.getNumberKind(), NUMBER_KIND_REGULAR);
  });

  it("clears values without clearing context", () => {
    const plan = loadedPlan();
    const phone = new E164Number(plan, {
      countryCode: 55,
      areaCode: 19,
      restriction: RESTRICT_AREA,
    });

    phone.setValue("912345678");
    assert.equal(phone.value, "5519912345678");

    phone.setValue(null);
    assert.equal(phone.value, "");
    assert.equal(phone.getContextValue(), "");

    phone.setValue("912345678");
    assert.equal(phone.value, "5519912345678");
    assert.equal(phone.getContextValue(), "91234-5678");
  });

  it("truncates long input to the accepted prefix", () => {
    const plan = loadedPlan();
    const phone = new E164Number(plan);

    phone.setValue("+551912345678999999");
    assert.equal(phone.value, "551912345678");
    assert.equal(phone.getValue(), "+55 (19) 1234-5678");
  });

  it("resolves NANP countries by area code, not country code fallback", () => {
    const plan = loadedPlan();
    const phone = new E164Number(plan);

    phone.setValue("+1");
    assert.equal(phone.getValue(), "+1");
    assert.equal(phone.getCountry(), null);

    phone.setValue("+14165550123");
    assert.equal(phone.getValue(), "+1 (416) 555-0123");
    assert.equal(phone.getCountry(), "en_CA");

    phone.setValue("+14691234567");
    assert.equal(phone.getValue(), "+1 (469) 123-4567");
    assert.equal(phone.getCountry(), "en_US");
  });

  it("applies default context and restricted context display", () => {
    const plan = loadedPlan();
    const phone = new E164Number(plan, {
      countryCode: 55,
      areaCode: 19,
      restriction: RESTRICT_NONE,
    });

    phone.setValue("912345678");
    assert.equal(phone.getValue(), "+55 (19) 91234-5678");
    assert.equal(phone.getContextValue(), "91234-5678");
    assert.equal(phone.value, "5519912345678");

    phone.setValue("(19912345678");
    assert.equal(phone.getContextValue(), "(19) 91234-5678");

    phone.setContext({ countryCode: 55, restriction: RESTRICT_COUNTRY });
    phone.setValue("+12010001234");
    assert.equal(phone.value, "");
    assert.equal(phone.getContextValue(), "");

    phone.setContext({ countryCode: 55, areaCode: 19, restriction: RESTRICT_AREA });
    phone.setValue("+5511912345678");
    assert.equal(phone.value, "");
  });

  it("maps alphanumeric input when enabled", () => {
    const plan = loadedPlan();
    const phone = new E164Number(plan, {
      countryCode: 55,
      areaCode: 19,
      restriction: RESTRICT_AREA,
      acceptAlphanumeric: true,
    });

    phone.setValue("9FLOWERS");
    assert.equal(phone.value, "551993569377");
    assert.equal(phone.getValue(), "+55 (19) 93569-377");

    phone.setContext({
      countryCode: 55,
      areaCode: 19,
      restriction: RESTRICT_AREA,
      acceptAlphanumeric: false,
    });
    phone.setValue("9FLOWERS");
    assert.equal(phone.value, "55199");
    assert.equal(phone.getValue(), "+55 (19) 9");
  });

  it("supports dialing input mode from a fixed origin", () => {
    const plan = loadedPlan();
    const phone = new E164Number(plan, {
      countryCode: 55,
      areaCode: 19,
      restriction: RESTRICT_NONE,
      inputMode: INPUT_MODE_DIALING,
      carrierCode: 15,
    });

    phone.setValue("912345678");
    assert.equal(phone.value, "5519912345678");
    assert.equal(phone.getDialingValue(), "912345678");

    phone.setValue("01519912345678");
    assert.equal(phone.value, "5519912345678");
    assert.equal(phone.getDialingValue(), "912345678");

    phone.setValue("19912345678");
    assert.equal(phone.value, "");

    phone.setValue("00551912345678");
    assert.equal(phone.value, "551912345678");
    assert.equal(phone.getDialingValue(), "12345678");

    phone.setValue("001514165550123");
    assert.equal(phone.value, "14165550123");
    assert.equal(phone.getDialingValue(), "001514165550123");

    phone.setValue("0015551912345678");
    assert.equal(phone.value, "551912345678");
    assert.equal(phone.getDialingValue(), "12345678");

    phone.setValue("08000101010");
    assert.equal(phone.value, "558000101010");
    assert.equal(phone.getDialingValue(), "08000101010");

    phone.setContext({
      countryCode: 55,
      areaCode: 19,
      restriction: RESTRICT_NONE,
      inputMode: INPUT_MODE_DIALING,
    });
    phone.setValue("01511912345678");
    assert.equal(phone.value, "5511912345678");
    assert.equal(phone.getDialingValue(), "");

    phone.setContext({ countryCode: 55, inputMode: INPUT_MODE_DIALING });
    phone.setValue("912345678");
    assert.equal(phone.value, "");
  });

  it("allows Brazilian toll-free dialing input incrementally", () => {
    const plan = loadedPlan();
    const phone = new E164Number(plan, {
      countryCode: 55,
      areaCode: 19,
      restriction: RESTRICT_NONE,
      inputMode: INPUT_MODE_DIALING,
      carrierCode: 21,
    });
    let input = "";

    for (const digit of "08000101010") {
      const previousValue = phone.value;
      const candidate = input + digit;

      phone.setValue(candidate);
      if (phone.value === previousValue && previousValue !== "" && phone.isComplete()) {
        phone.setValue(input);
        continue;
      }
      input = candidate;
    }

    assert.equal(input, "08000101010");
    assert.equal(phone.value, "558000101010");
    assert.equal(phone.getContextValue(), "0800 010 10 10");
    assert.equal(phone.getDialingValue(), "08000101010");
  });

  it("limits international dialing input incrementally", () => {
    const plan = loadedPlan();
    const phone = new E164Number(plan, {
      countryCode: 55,
      areaCode: 19,
      restriction: RESTRICT_NONE,
      inputMode: INPUT_MODE_DIALING,
      carrierCode: 21,
    });
    let input = "";

    for (const digit of "00218001234567890123456") {
      const previousValue = phone.value;
      const candidate = input + digit;

      phone.setValue(candidate);
      if (phone.value === previousValue && previousValue !== "" && (phone.isComplete() || phone.pos >= 15)) {
        phone.setValue(input);
        continue;
      }
      input = candidate;
    }

    assert.equal(input, "0021800123456789012");
    assert.equal(phone.value, "800123456789012");
    assert.equal(phone.pos, 15);
  });

  it("supports digit editing helpers", () => {
    const plan = loadedPlan();
    const phone = new E164Number(plan);

    for (const digit of "551912345678") phone.addDigit(digit);
    assert.equal(phone.getValue(), "+55 (19) 1234-5678");

    phone.delDigit();
    assert.equal(phone.getValue(), "+55 (19) 1234-567");

    while (phone.delDigit()) {}
    assert.equal(phone.value, "");
    assert.equal(phone.getValue(), "+");
    assert.equal(phone.delDigit(), false);
  });
});
