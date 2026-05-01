import {
  E164Number,
  E164Plan,
  RESTRICT_AREA,
  RESTRICT_COUNTRY,
  RESTRICT_NONE,
} from "../src/index.js";

const fields = {
  country: document.querySelector("#country"),
  area: document.querySelector("#area"),
  restriction: document.querySelector("#restriction"),
  alphanumeric: document.querySelector("#alphanumeric"),
  number: document.querySelector("#number"),
  raw: document.querySelector("#raw"),
  full: document.querySelector("#full"),
  context: document.querySelector("#context"),
  detectedCountry: document.querySelector("#detected-country"),
  status: document.querySelector("#status"),
};

const restrictions = new Set([RESTRICT_NONE, RESTRICT_COUNTRY, RESTRICT_AREA]);
const planText = await fetch("../../data/e164-plan.txt").then((response) => {
  if (!response.ok) throw new Error(`could not load plan: ${response.status}`);
  return response.text();
});
const plan = E164Plan.fromText(planText);
const phone = new E164Number(plan);

function numericValue(input) {
  return /^[0-9]+$/.test(input.value) ? Number(input.value) : 0;
}

function update() {
  const restriction = Number(fields.restriction.value);
  phone.setContext({
    countryCode: numericValue(fields.country),
    areaCode: numericValue(fields.area),
    restriction: restrictions.has(restriction) ? restriction : RESTRICT_NONE,
    acceptAlphanumeric: fields.alphanumeric.checked,
  });
  phone.setValue(fields.number.value);

  fields.raw.textContent = phone.value || "-";
  fields.full.textContent = phone.pos === 0 ? "" : phone.getValue();
  fields.context.textContent = phone.getContextValue();
  fields.detectedCountry.textContent = phone.getCountry() || "unknown";
  fields.status.textContent = phone.isComplete() ? "complete" : "incomplete";
}

for (const input of Object.values(fields)) {
  if (input instanceof HTMLInputElement || input instanceof HTMLSelectElement)
    input.addEventListener("input", update);
}

update();
