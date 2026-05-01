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
let phoneInput = fields.number.value;

function numericValue(input) {
  return /^[0-9]+$/.test(input.value) ? Number(input.value) : 0;
}

function applyContext() {
  const countryCode = numericValue(fields.country);
  const areaCode = countryCode === 0 ? 0 : numericValue(fields.area);
  let restriction = Number(fields.restriction.value);

  if (!restrictions.has(restriction) || countryCode === 0)
    restriction = RESTRICT_NONE;
  else if (restriction === RESTRICT_AREA && areaCode === 0)
    restriction = RESTRICT_COUNTRY;

  phone.setContext({
    countryCode,
    areaCode,
    restriction,
    acceptAlphanumeric: fields.alphanumeric.checked,
  });
}

function prefixAllowed(ch) {
  if (ch === "+") return phone.context.restriction === RESTRICT_NONE;
  if (ch === "(") return phone.context.restriction < RESTRICT_AREA;
  return false;
}

function acceptedPhoneInputChar(ch) {
  if (/^[0-9]$/.test(ch)) return true;
  if (/^[A-Za-z]$/.test(ch)) return fields.alphanumeric.checked;
  if ((ch === "+" || ch === "(") && phoneInput.length === 0) return prefixAllowed(ch);
  return false;
}

function phoneDisplayValue() {
  if (phone.pos === 0 && (phoneInput === "+" || phoneInput === "(")) return phoneInput;
  return phone.getContextValue();
}

function syncPhoneField() {
  const display = phoneDisplayValue();
  fields.number.value = display;
}

function phoneSelectionIsFull() {
  return fields.number.selectionStart === 0 && fields.number.selectionEnd === fields.number.value.length;
}

function render() {
  syncPhoneField();
  fields.raw.textContent = phone.value || "-";
  fields.full.textContent = phone.pos === 0 ? "" : phone.getValue();
  fields.context.textContent = phone.getContextValue();
  fields.detectedCountry.textContent = phone.getCountry() || "unknown";
  fields.status.textContent = phone.isComplete() ? "complete" : "incomplete";
}

function normalizePhoneInput(rawInput) {
  let accepted = "";

  phone.setValue("");
  for (const ch of rawInput) {
    if (!acceptedPhoneInputChar(ch)) continue;

    const previousValue = phone.value;
    const candidate = accepted + ch;

    phone.setValue(candidate);
    if (phone.value === previousValue && previousValue !== "") {
      phone.setValue(accepted);
      continue;
    }
    accepted = candidate;
  }

  phone.setValue(accepted);
  return accepted;
}

function updateFromInput(nextInput, selectAll = false) {
  applyContext();
  phoneInput = normalizePhoneInput(nextInput);
  render();

  if (selectAll && document.activeElement === fields.number)
    fields.number.select();
  else if (document.activeElement === fields.number)
    fields.number.setSelectionRange(fields.number.value.length, fields.number.value.length);
}

function rebuildFromContext() {
  updateFromInput(phoneInput);
}

function insertPhoneText(text) {
  updateFromInput(phoneSelectionIsFull() ? text : phoneInput + text);
}

function deletePhoneText() {
  if (phoneSelectionIsFull()) {
    updateFromInput("");
    return;
  }

  if (phoneInput.length === 0) return;
  updateFromInput(phoneInput.slice(0, -1));
}

fields.number.addEventListener("beforeinput", (event) => {
  if (event.inputType === "deleteContentBackward" || event.inputType === "deleteContentForward") {
    event.preventDefault();
    deletePhoneText();
    return;
  }

  if (event.inputType === "insertText" || event.inputType === "insertCompositionText") {
    event.preventDefault();
    insertPhoneText(event.data || "");
    return;
  }

  event.preventDefault();
});

fields.number.addEventListener("keydown", (event) => {
  if ((event.ctrlKey || event.metaKey) && event.key.toLowerCase() === "a") {
    return;
  }

  if (event.key === "Backspace" || event.key === "Delete") {
    event.preventDefault();
    deletePhoneText();
  }
});

fields.number.addEventListener("focus", () => {
  fields.number.select();
});

for (const input of [fields.country, fields.area, fields.restriction, fields.alphanumeric]) {
  input.addEventListener("input", rebuildFromContext);
}

rebuildFromContext();
