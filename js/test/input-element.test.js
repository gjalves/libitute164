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
  const defined = new Map();

  globalThis.document = {
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
    attachShadow() {
      this.shadowRoot = {
        append() {},
        querySelector() {
          return {};
        },
      };
    }
  };

  try {
    const module = await import("../src/input-element.js");
    assert.equal(typeof module.Itute164InputElement, "function");
    assert.equal(defined.get("itute164-input"), module.Itute164InputElement);
  } finally {
    globalThis.document = previousDocument;
    globalThis.customElements = previousCustomElements;
    delete globalThis.HTMLElement;
  }
});

it("keeps the shared plan loadable alongside the component module", () => {
  const plan = E164Plan.fromText(planText);
  assert.equal(plan.areaToCountry(1, 469), "en_US");
  assert.equal(plan.areaToCountry(1, 416), "en_CA");
});
