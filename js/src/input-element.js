import {
  E164Number,
  INPUT_MODE_DIALING,
  INPUT_MODE_NUMBER,
  NUMBER_KIND_UNKNOWN,
  RESTRICT_AREA,
  RESTRICT_COUNTRY,
  RESTRICT_NONE,
} from "./index.js";

const restrictions = new Set([RESTRICT_NONE, RESTRICT_COUNTRY, RESTRICT_AREA]);
const inputModes = new Set([INPUT_MODE_NUMBER, INPUT_MODE_DIALING]);
const contextAttributes = new Set(["country-code", "area-code", "carrier-code", "restriction", "input-mode", "accept-alphanumeric"]);

const template = document.createElement("template");
template.innerHTML = `
  <style>
    :host {
      display: block;
      color: var(--itute164-color, #1f2933);
      font-family: var(--itute164-font-family, system-ui, -apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif);
    }

    * {
      box-sizing: border-box;
    }

    .grid {
      display: grid;
      grid-template-columns: repeat(4, minmax(0, 1fr));
      gap: 12px;
      margin-bottom: 12px;
    }

    label {
      display: grid;
      gap: 6px;
      font-size: 13px;
      font-weight: 600;
      color: var(--itute164-label-color, #52606d);
    }

    .check {
      align-content: end;
      grid-template-columns: auto 1fr;
      gap: 8px;
      padding-bottom: 10px;
    }

    input,
    select {
      width: 100%;
      min-height: 42px;
      border: 1px solid var(--itute164-border-color, #bcccdc);
      border-radius: 6px;
      padding: 8px 10px;
      color: var(--itute164-input-color, #102a43);
      font: inherit;
      font-weight: 500;
      background: var(--itute164-input-background, #ffffff);
    }

    input:focus,
    select:focus {
      outline: 3px solid var(--itute164-focus-color, #9fb3c8);
      outline-offset: 1px;
    }

    dl {
      display: grid;
      grid-template-columns: repeat(2, minmax(0, 1fr));
      gap: 12px;
      margin: 18px 0 0;
    }

    dl div {
      border: 1px solid var(--itute164-border-color, #d9e2ec);
      border-radius: 6px;
      padding: 12px;
      min-height: 70px;
    }

    dt {
      color: var(--itute164-muted-color, #627d98);
      font-size: 12px;
      font-weight: 700;
      text-transform: uppercase;
    }

    dd {
      margin: 8px 0 0;
      overflow-wrap: anywhere;
      font-family: var(--itute164-mono-font-family, ui-monospace, SFMono-Regular, Menlo, Consolas, monospace);
      font-size: 16px;
    }

    :host(:not([show-details])) dl {
      display: none;
    }

    @media (max-width: 680px) {
      .grid,
      dl {
        grid-template-columns: 1fr;
      }
    }
  </style>

  <div class="grid">
    <label>
      DDI
      <input id="country" inputmode="numeric">
    </label>

    <label>
      DDD
      <input id="area" inputmode="numeric">
    </label>

    <label>
      Carrier
      <input id="carrier" inputmode="numeric">
    </label>

    <label>
      Mode
      <select id="mode">
        <option value="0">Number</option>
        <option value="1">Dialing</option>
      </select>
    </label>

    <label>
      Restriction
      <select id="restriction">
        <option value="0">Open</option>
        <option value="1">Country</option>
        <option value="2">Area</option>
      </select>
    </label>

    <label class="check">
      <input id="alphanumeric" type="checkbox">
      Alphanumeric
    </label>
  </div>

  <label>
    Number
    <input id="number" inputmode="tel" autocomplete="tel">
  </label>

  <dl>
    <div>
      <dt>Raw</dt>
      <dd id="raw"></dd>
    </div>
    <div>
      <dt>Full</dt>
      <dd id="full"></dd>
    </div>
    <div>
      <dt>Context</dt>
      <dd id="context"></dd>
    </div>
    <div>
      <dt>Dialing</dt>
      <dd id="dialing"></dd>
    </div>
    <div>
      <dt>Country</dt>
      <dd id="detected-country"></dd>
    </div>
    <div>
      <dt>Kind</dt>
      <dd id="kind"></dd>
    </div>
    <div>
      <dt>Status</dt>
      <dd id="status"></dd>
    </div>
  </dl>
`;

function parseNumber(value) {
  return /^[0-9]+$/.test(value) ? Number(value) : 0;
}

function restrictionFromAttribute(value) {
  if (value === "country") return RESTRICT_COUNTRY;
  if (value === "area") return RESTRICT_AREA;
  if (/^[0-2]$/.test(value || "")) return Number(value);
  return RESTRICT_NONE;
}

function restrictionToAttribute(value) {
  if (value === RESTRICT_COUNTRY) return "country";
  if (value === RESTRICT_AREA) return "area";
  return "none";
}

function modeFromAttribute(value) {
  if (value === "dialing") return INPUT_MODE_DIALING;
  if (/^[0-1]$/.test(value || "")) return Number(value);
  return INPUT_MODE_NUMBER;
}

function modeToAttribute(value) {
  return value === INPUT_MODE_DIALING ? "dialing" : "number";
}

function alphaToPhoneDigit(ch) {
  const code = ch.toUpperCase();
  if (code >= "A" && code <= "C") return "2";
  if (code >= "D" && code <= "F") return "3";
  if (code >= "G" && code <= "I") return "4";
  if (code >= "J" && code <= "L") return "5";
  if (code >= "M" && code <= "O") return "6";
  if (code >= "P" && code <= "S") return "7";
  if (code >= "T" && code <= "V") return "8";
  if (code >= "W" && code <= "Z") return "9";
  return "";
}

export class Itute164InputElement extends HTMLElement {
  static observedAttributes = ["country-code", "area-code", "carrier-code", "restriction", "input-mode", "accept-alphanumeric", "value", "show-details"];

  constructor() {
    super();
    this.attachShadow({ mode: "open" });
    this.shadowRoot.append(template.content.cloneNode(true));

    this.fields = {
      country: this.shadowRoot.querySelector("#country"),
      area: this.shadowRoot.querySelector("#area"),
      carrier: this.shadowRoot.querySelector("#carrier"),
      mode: this.shadowRoot.querySelector("#mode"),
      restriction: this.shadowRoot.querySelector("#restriction"),
      alphanumeric: this.shadowRoot.querySelector("#alphanumeric"),
      number: this.shadowRoot.querySelector("#number"),
      raw: this.shadowRoot.querySelector("#raw"),
      full: this.shadowRoot.querySelector("#full"),
      context: this.shadowRoot.querySelector("#context"),
      dialing: this.shadowRoot.querySelector("#dialing"),
      detectedCountry: this.shadowRoot.querySelector("#detected-country"),
      kind: this.shadowRoot.querySelector("#kind"),
      status: this.shadowRoot.querySelector("#status"),
    };

    this._plan = null;
    this._phone = null;
    this._phoneInput = "";
    this._syncingAttributes = false;
  }

  connectedCallback() {
    this.fields.number.addEventListener("beforeinput", this);
    this.fields.number.addEventListener("keydown", this);
    this.fields.number.addEventListener("focus", this);
    for (const input of [this.fields.country, this.fields.area, this.fields.carrier, this.fields.mode, this.fields.restriction, this.fields.alphanumeric])
      input.addEventListener("input", this);

    this.syncFromAttributes();
    this.rebuildFromContext();
  }

  disconnectedCallback() {
    this.fields.number.removeEventListener("beforeinput", this);
    this.fields.number.removeEventListener("keydown", this);
    this.fields.number.removeEventListener("focus", this);
    for (const input of [this.fields.country, this.fields.area, this.fields.carrier, this.fields.mode, this.fields.restriction, this.fields.alphanumeric])
      input.removeEventListener("input", this);
  }

  attributeChangedCallback(name) {
    if (this._syncingAttributes || !this.isConnected) return;
    this.syncFromAttributes();
    if (contextAttributes.has(name)) this.clearPhoneInput();
    this.rebuildFromContext();
  }

  get plan() {
    return this._plan;
  }

  set plan(plan) {
    this._plan = plan;
    this._phone = plan ? new E164Number(plan) : null;
    this.rebuildFromContext();
  }

  get value() {
    return this._phone?.value || "";
  }

  set value(value) {
    this._phoneInput = value || "";
    this.updateFromInput(this._phoneInput);
  }

  get rawValue() {
    return this._phoneInput;
  }

  get countryCode() {
    return parseNumber(this.fields.country.value);
  }

  set countryCode(value) {
    const nextValue = value ? String(value) : "";
    const changed = this.fields.country.value !== nextValue;
    this.fields.country.value = nextValue;
    this.syncAttribute("country-code", this.fields.country.value);
    if (changed) this.clearPhoneInput();
    this.rebuildFromContext();
  }

  get areaCode() {
    return this.countryCode === 0 ? 0 : parseNumber(this.fields.area.value);
  }

  set areaCode(value) {
    const nextValue = value ? String(value) : "";
    const changed = this.fields.area.value !== nextValue;
    this.fields.area.value = nextValue;
    this.syncAttribute("area-code", this.fields.area.value);
    if (changed) this.clearPhoneInput();
    this.rebuildFromContext();
  }

  get carrierCode() {
    return parseNumber(this.fields.carrier.value);
  }

  set carrierCode(value) {
    const nextValue = value ? String(value) : "";
    const changed = this.fields.carrier.value !== nextValue;
    this.fields.carrier.value = nextValue;
    this.syncAttribute("carrier-code", this.fields.carrier.value);
    if (changed) this.clearPhoneInput();
    this.rebuildFromContext();
  }

  get restriction() {
    return restrictionFromAttribute(this.fields.restriction.value);
  }

  get inputMode() {
    return modeFromAttribute(this.fields.mode.value);
  }

  set inputMode(value) {
    const mode = modeFromAttribute(String(value));
    const changed = modeFromAttribute(this.fields.mode.value) !== mode;
    this.fields.mode.value = String(mode);
    this.syncAttribute("input-mode", modeToAttribute(mode));
    if (changed) this.clearPhoneInput();
    this.rebuildFromContext();
  }

  set restriction(value) {
    const restriction = restrictionFromAttribute(String(value));
    const changed = restrictionFromAttribute(this.fields.restriction.value) !== restriction;
    this.fields.restriction.value = String(restriction);
    this.syncAttribute("restriction", restrictionToAttribute(restriction));
    if (changed) this.clearPhoneInput();
    this.rebuildFromContext();
  }

  get acceptAlphanumeric() {
    return this.fields.alphanumeric.checked;
  }

  set acceptAlphanumeric(value) {
    const nextValue = Boolean(value);
    const changed = this.fields.alphanumeric.checked !== nextValue;
    this.fields.alphanumeric.checked = nextValue;
    this.syncBooleanAttribute("accept-alphanumeric", this.fields.alphanumeric.checked);
    if (changed) this.clearPhoneInput();
    this.rebuildFromContext();
  }

  get complete() {
    return Boolean(this._phone?.isComplete());
  }

  get country() {
    return this._phone?.getCountry() || null;
  }

  get numberKind() {
    return this._phone?.getNumberKind() || NUMBER_KIND_UNKNOWN;
  }

  handleEvent(event) {
    if (event.currentTarget === this.fields.number) {
      this.handleNumberEvent(event);
      return;
    }

    this.syncContextAttributes();
    this.clearPhoneInput();
    this.rebuildFromContext();
  }

  handleNumberEvent(event) {
    if (event.type === "focus") {
      this.fields.number.select();
      return;
    }

    if (event.type === "keydown") {
      if ((event.ctrlKey || event.metaKey) && event.key.toLowerCase() === "a") return;
      if (event.key === "Backspace" || event.key === "Delete") {
        event.preventDefault();
        this.deletePhoneText();
      }
      return;
    }

    if (event.inputType === "deleteContentBackward" || event.inputType === "deleteContentForward") {
      event.preventDefault();
      this.deletePhoneText();
      return;
    }

    if (event.inputType === "insertText" || event.inputType === "insertCompositionText") {
      event.preventDefault();
      this.insertPhoneText(event.data || "");
      return;
    }

    event.preventDefault();
  }

  syncFromAttributes() {
    this.fields.country.value = this.getAttribute("country-code") || "";
    this.fields.area.value = this.getAttribute("area-code") || "";
    this.fields.carrier.value = this.getAttribute("carrier-code") || "";
    this.fields.mode.value = String(modeFromAttribute(this.getAttribute("input-mode")));
    this.fields.restriction.value = String(restrictionFromAttribute(this.getAttribute("restriction")));
    this.fields.alphanumeric.checked = this.hasAttribute("accept-alphanumeric");
    this._phoneInput = this.getAttribute("value") || this._phoneInput || "";
  }

  syncAttribute(name, value) {
    this._syncingAttributes = true;
    if (value) this.setAttribute(name, value);
    else this.removeAttribute(name);
    this._syncingAttributes = false;
  }

  syncBooleanAttribute(name, value) {
    this._syncingAttributes = true;
    if (value) this.setAttribute(name, "");
    else this.removeAttribute(name);
    this._syncingAttributes = false;
  }

  syncContextAttributes() {
    this.syncAttribute("country-code", this.fields.country.value);
    this.syncAttribute("area-code", this.fields.area.value);
    this.syncAttribute("carrier-code", this.fields.carrier.value);
    this.syncAttribute("input-mode", modeToAttribute(modeFromAttribute(this.fields.mode.value)));
    this.syncAttribute("restriction", restrictionToAttribute(restrictionFromAttribute(this.fields.restriction.value)));
    this.syncBooleanAttribute("accept-alphanumeric", this.fields.alphanumeric.checked);
  }

  clearPhoneInput() {
    this._phoneInput = "";
  }

  applyContext() {
    if (!this._phone) return;

    const countryCode = parseNumber(this.fields.country.value);
    const areaCode = countryCode === 0 ? 0 : parseNumber(this.fields.area.value);
    const carrierCode = countryCode === 0 ? 0 : parseNumber(this.fields.carrier.value);
    let restriction = restrictionFromAttribute(this.fields.restriction.value);
    let inputMode = modeFromAttribute(this.fields.mode.value);

    if (!restrictions.has(restriction) || countryCode === 0)
      restriction = RESTRICT_NONE;
    else if (restriction === RESTRICT_AREA && areaCode === 0)
      restriction = RESTRICT_COUNTRY;
    if (!inputModes.has(inputMode))
      inputMode = INPUT_MODE_NUMBER;

    this._phone.setContext({
      countryCode,
      areaCode,
      restriction,
      acceptAlphanumeric: this.fields.alphanumeric.checked,
      inputMode,
      carrierCode,
    });
  }

  prefixAllowed(ch) {
    if (!this._phone) return false;
    if (this._phone.context.inputMode === INPUT_MODE_DIALING) return false;
    if (ch === "+") return this._phone.context.restriction === RESTRICT_NONE;
    if (ch === "(") return this._phone.context.restriction < RESTRICT_AREA;
    return false;
  }

  acceptedPhoneInputChar(ch) {
    if (/^[0-9]$/.test(ch)) return true;
    if (/^[A-Za-z]$/.test(ch)) return this.fields.alphanumeric.checked;
    if ((ch === "+" || ch === "(") && this._phoneInput.length === 0) return this.prefixAllowed(ch);
    return false;
  }

  phoneDisplayValue() {
    if (!this._phone) return "";
    if (this._phone.context.inputMode === INPUT_MODE_DIALING && this._phoneInput.length > 0) return this._phoneInput;
    if (this._phone.pos === 0 && this._phoneInput.length > 0) return this._phoneInput;
    return this.getDisplayContextValue();
  }

  getDisplayContextValue() {
    if (!this._phone || this._phoneInput.length === 0) return this._phone?.getContextValue() || "";

    const value = this._phone.value.split("");
    let valuePos = value.length - 1;

    for (let inputPos = this._phoneInput.length - 1; inputPos >= 0 && valuePos >= 0; inputPos--) {
      const ch = this._phoneInput[inputPos];

      if (/^[0-9]$/.test(ch)) {
        if (value[valuePos] === ch) valuePos--;
        continue;
      }

      if (/^[A-Za-z]$/.test(ch)) {
        const digit = alphaToPhoneDigit(ch);
        if (digit !== "" && value[valuePos] === digit) {
          value[valuePos] = ch.toUpperCase();
          valuePos--;
        }
      }
    }

    const displayPhone = this._phone.clone();
    displayPhone.value = value.join("");
    return displayPhone.getContextValue();
  }

  syncPhoneField() {
    this.fields.number.value = this.phoneDisplayValue();
  }

  phoneSelectionIsFull() {
    return this.fields.number.selectionStart === 0 && this.fields.number.selectionEnd === this.fields.number.value.length;
  }

  normalizePhoneInput(rawInput) {
    let accepted = "";

    if (!this._phone) return "";

    this._phone.setValue("");
    for (const ch of rawInput) {
      if (!this.acceptedPhoneInputChar(ch)) continue;

      const previousValue = this._phone.value;
      const candidate = accepted + ch;

      this._phone.setValue(candidate);
      if (this._phone.value === previousValue && previousValue !== "" && (this._phone.isComplete() || this._phone.pos >= 15)) {
        this._phone.setValue(accepted);
        continue;
      }
      accepted = candidate;
    }

    this._phone.setValue(accepted);
    return accepted;
  }

  updateFromInput(nextInput) {
    this.applyContext();
    this._phoneInput = this.normalizePhoneInput(nextInput);
    this.render();

    if (document.activeElement === this.fields.number)
      this.fields.number.setSelectionRange(this.fields.number.value.length, this.fields.number.value.length);
  }

  rebuildFromContext() {
    this.updateFromInput(this._phoneInput);
  }

  insertPhoneText(text) {
    this.updateFromInput(this.phoneSelectionIsFull() ? text : this._phoneInput + text);
  }

  deletePhoneText() {
    if (this.phoneSelectionIsFull()) {
      this.updateFromInput("");
      return;
    }

    if (this._phoneInput.length === 0) return;
    this.updateFromInput(this._phoneInput.slice(0, -1));
  }

  stateDetail() {
    return {
      value: this._phone?.value || "",
      rawValue: this._phoneInput,
      displayValue: this._phone?.getValue() || "",
      contextValue: this._phone?.getContextValue() || "",
      dialingValue: this._phone?.getDialingValue() || "",
      components: this._phone?.getComponents() || null,
      country: this.country,
      kind: this.numberKind,
      complete: this.complete,
    };
  }

  render() {
    this.syncPhoneField();

    const value = this._phone?.value || "";
    this.fields.raw.textContent = value || "-";
    this.fields.full.textContent = this._phone?.pos === 0 ? "" : this._phone.getValue();
    this.fields.context.textContent = this._phone?.getContextValue() || "";
    this.fields.dialing.textContent = this._phone?.getDialingValue() || "";
    this.fields.detectedCountry.textContent = this.country || "unknown";
    this.fields.kind.textContent = this.numberKind;
    this.fields.status.textContent = this.complete ? "complete" : "incomplete";

    this.dispatchEvent(new CustomEvent("itute164-change", {
      bubbles: true,
      detail: this.stateDetail(),
    }));
  }
}

if (!customElements.get("itute164-input"))
  customElements.define("itute164-input", Itute164InputElement);
