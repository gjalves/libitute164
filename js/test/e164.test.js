import { readFileSync } from "node:fs";
import { dirname, join } from "node:path";
import { describe, it } from "node:test";
import assert from "node:assert/strict";
import { fileURLToPath } from "node:url";
import {
  E164Number,
  E164Plan,
  RESTRICT_AREA,
  RESTRICT_COUNTRY,
  RESTRICT_NONE,
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
  });

  it("rejects malformed country tags transactionally", () => {
    const plan = new E164Plan();
    plan.loadText("country 55 pt_BR\n");

    assert.throws(() => plan.loadText("country 55 pt-BR\n"), /invalid country tag/);
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

  it("supports digit editing helpers", () => {
    const plan = loadedPlan();
    const phone = new E164Number(plan);

    for (const digit of "551912345678") phone.addDigit(digit);
    assert.equal(phone.getValue(), "+55 (19) 1234-5678");

    phone.delDigit();
    assert.equal(phone.getValue(), "+55 (19) 1234-567");
  });
});
