import { readFileSync } from "node:fs";
import { dirname, join } from "node:path";
import { it } from "node:test";
import assert from "node:assert/strict";
import { fileURLToPath } from "node:url";
import { E164Plan } from "../src/index.js";

const __dirname = dirname(fileURLToPath(import.meta.url));
const planText = readFileSync(join(__dirname, "../../data/e164-plan.txt"), "utf8");

it("exports the web component module", async () => {
  const previousDocument = globalThis.document;
  const previousCustomElements = globalThis.customElements;
  const previousHTMLElement = globalThis.HTMLElement;
  const previousCustomEvent = globalThis.CustomEvent;
  const previousActiveElement = globalThis.document?.activeElement;
  const defined = new Map();
  const createField = () => ({
    value: "",
    checked: false,
    textContent: "",
    selectionStart: 0,
    selectionEnd: 0,
    addEventListener() {},
    removeEventListener() {},
    select() {},
    setSelectionRange(start, end) {
      this.selectionStart = start;
      this.selectionEnd = end;
    },
  });
  const fields = {
    "#country": createField(),
    "#area": createField(),
    "#carrier": createField(),
    "#mode": createField(),
    "#restriction": createField(),
    "#alphanumeric": createField(),
    "#number": createField(),
    "#raw": createField(),
    "#full": createField(),
    "#context": createField(),
    "#dialing": createField(),
    "#detected-country": createField(),
    "#kind": createField(),
    "#status": createField(),
  };

  globalThis.document = {
    activeElement: null,
    createElement() {
      return {
        innerHTML: "",
        content: {
          cloneNode() {
            return {};
          },
        },
      };
    },
  };
  globalThis.customElements = {
    get(name) {
      return defined.get(name);
    },
    define(name, value) {
      defined.set(name, value);
    },
  };
  globalThis.HTMLElement = class {
    constructor() {
      this.attributes = new Map();
      this.isConnected = true;
    }

    attachShadow() {
      this.shadowRoot = {
        append() {},
        querySelector(selector) {
          return fields[selector];
        },
      };
    }

    setAttribute(name, value) {
      this.attributes.set(name, String(value));
    }

    removeAttribute(name) {
      this.attributes.delete(name);
    }

    getAttribute(name) {
      return this.attributes.get(name) ?? null;
    }

    hasAttribute(name) {
      return this.attributes.has(name);
    }

    dispatchEvent(event) {
      this.lastEvent = event;
      return true;
    }
  };
  globalThis.CustomEvent = class {
    constructor(type, options = {}) {
      this.type = type;
      this.bubbles = Boolean(options.bubbles);
      this.detail = options.detail;
    }
  };

  try {
    const module = await import("../src/input-element.js");
    assert.equal(typeof module.Itute164InputElement, "function");
    assert.equal(defined.get("itute164-input"), module.Itute164InputElement);

    const input = new module.Itute164InputElement();
    input.plan = E164Plan.fromText(planText);
    input.countryCode = 55;
    input.areaCode = 19;
    input.restriction = 2;
    input.acceptAlphanumeric = true;
    input.value = "9flowers";

    assert.equal(fields["#number"].value, "9FLOW-ERS");
    assert.equal(input.value, "551993569377");

    input.areaCode = 11;
    assert.equal(input.rawValue, "");
    assert.equal(input.value, "");
    assert.equal(fields["#number"].value, "");

    input.countryCode = 55;
    input.areaCode = 19;
    input.carrierCode = 21;
    input.inputMode = "dialing";
    input.insertPhoneText("021");

    assert.equal(input.rawValue, "021");
    assert.equal(input.value, "");
    assert.equal(fields["#number"].value, "021");

    input.insertPhoneText("19");

    assert.equal(input.rawValue, "02119");
    assert.equal(input.value, "5519");
    assert.equal(fields["#number"].value, "02119");

    input.value = "";
    input.insertPhoneText("00551998259222222222222222");

    assert.equal(input.rawValue, "005519982592222");
    assert.equal(input.value, "5519982592222");
    assert.equal(fields["#number"].value, "005519982592222");
  } finally {
    globalThis.document = previousDocument;
    globalThis.customElements = previousCustomElements;
    globalThis.HTMLElement = previousHTMLElement;
    globalThis.CustomEvent = previousCustomEvent;
    if (globalThis.document && previousActiveElement !== undefined)
      globalThis.document.activeElement = previousActiveElement;
  }
});

it("keeps the shared plan loadable alongside the component module", () => {
  const plan = E164Plan.fromText(planText);
  assert.equal(plan.areaToCountry(1, 469), "en_US");
  assert.equal(plan.areaToCountry(1, 416), "en_CA");
});
