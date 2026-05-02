export const AREA_UNKNOWN = "unknown";
export const AREA_INCOMPLETE = "incomplete";
export const AREA_NUMBER = "number";

export const CC_UNKNOWN = "unknown";
export const CC_INCOMPLETE = "incomplete";
export const CC_RESERVED = "reserved";
export const CC_SPARE = "spare";
export const CC_NUMBER = "number";
export const CC_GEOGRAPHIC = "geographic";
export const CC_GLOBAL = "global";
export const CC_GROUPS = "groups";
export const CC_TRIALS = "trials";

export const RESTRICT_NONE = 0;
export const RESTRICT_COUNTRY = 1;
export const RESTRICT_AREA = 2;

export const INPUT_MODE_NUMBER = 0;
export const INPUT_MODE_DIALING = 1;

export const NUMBER_KIND_UNKNOWN = "unknown";
export const NUMBER_KIND_REGULAR = "regular";
export const NUMBER_KIND_TOLL_FREE = "toll-free";
export const NUMBER_KIND_SHORT = "short";
export const NUMBER_KIND_PREMIUM = "premium";
export const NUMBER_KIND_EMERGENCY = "emergency";
export const NUMBER_KIND_SERVICE = "service";

const NUMBER_KINDS = new Set([
  NUMBER_KIND_UNKNOWN,
  NUMBER_KIND_REGULAR,
  NUMBER_KIND_TOLL_FREE,
  NUMBER_KIND_SHORT,
  NUMBER_KIND_PREMIUM,
  NUMBER_KIND_EMERGENCY,
  NUMBER_KIND_SERVICE,
]);

const CC_TYPES = new Set([
  CC_UNKNOWN,
  CC_INCOMPLETE,
  CC_RESERVED,
  CC_SPARE,
  CC_NUMBER,
  CC_GEOGRAPHIC,
  CC_GLOBAL,
  CC_GROUPS,
  CC_TRIALS,
]);

const AREA_TYPES = new Set([AREA_UNKNOWN, AREA_INCOMPLETE, AREA_NUMBER]);

function validateCountryTag(value) {
  return /^[a-z]{2}_[A-Z]{2}$/.test(value);
}

function validateDigits(value) {
  return /^[0-9]+$/.test(value);
}

function validateRange(value, min, max) {
  if (!/^[0-9]+$/.test(value)) return null;
  const parsed = Number(value);
  return parsed >= min && parsed <= max ? parsed : null;
}

function tokenize(line) {
  const tokens = [];
  let pos = 0;

  while (pos < line.length) {
    while (pos < line.length && /\s/.test(line[pos])) pos++;
    if (pos >= line.length || (tokens.length === 0 && line[pos] === "#")) break;

    if (line[pos] === "\"") {
      let end = pos + 1;
      while (end < line.length && line[end] !== "\"") end++;
      if (end >= line.length) throw new Error("invalid quoting");
      tokens.push(line.slice(pos + 1, end));
      pos = end + 1;
    } else {
      let end = pos;
      while (end < line.length && !/\s/.test(line[end])) end++;
      tokens.push(line.slice(pos, end));
      pos = end;
    }
  }

  return tokens;
}

function appendNumber(value) {
  return String(value);
}

function startsWith(value, prefix) {
  return Boolean(prefix) && value.startsWith(prefix);
}

function alphaToDigit(ch) {
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

function printMask(mask, number) {
  let result = "";
  let numberPos = 0;

  for (let maskPos = 0; maskPos < mask.length && numberPos < number.length; maskPos++) {
    if (mask[maskPos] === "#") result += number[numberPos++];
    else result += mask[maskPos];
  }

  return result;
}

function regexFromPlan(value) {
  return new RegExp(value);
}

export class E164Plan {
  constructor() {
    this.ccTypes = new Map();
    this.countries = new Map();
    this.areaCountries = new Map();
    this.nationalPrefixes = new Map();
    this.internationalPrefixes = new Map();
    this.areas = new Map();
    this.subscribers = new Map();
    this.knownCountryCodes = new Set();
  }

  static fromText(text) {
    const plan = new E164Plan();
    plan.loadText(text);
    return plan;
  }

  loadText(text) {
    const next = new E164Plan();
    const lines = text.split(/\n/);

    for (let i = 0; i < lines.length; i++) {
      const lineNo = i + 1;
      const args = tokenize(lines[i].replace(/\r$/, ""));
      if (args.length === 0) continue;
      next.parseLine(args, lineNo);
    }

    Object.assign(this, next);
  }

  parseLine(args, lineNo) {
    const directive = args[0];
    const fail = (message) => {
      throw new Error(`line ${lineNo}: ${message}`);
    };
    const countryCode = args.length > 1 ? validateRange(args[1], 0, 999) : null;

    if (directive === "cc") {
      if (args.length !== 3) fail("cc requires 2 arguments");
      if (countryCode === null) fail(`invalid country code '${args[1]}'`);
      if (!CC_TYPES.has(args[2])) fail(`invalid country code type '${args[2]}'`);
      this.ccTypes.set(countryCode, args[2]);
      this.knownCountryCodes.add(countryCode);
      return;
    }

    if (directive === "country") {
      if (args.length !== 3) fail("country requires 2 arguments");
      if (countryCode === null) fail(`invalid country code '${args[1]}'`);
      if (!validateCountryTag(args[2])) fail(`invalid country tag '${args[2]}'`);
      this.countries.set(countryCode, args[2]);
      this.knownCountryCodes.add(countryCode);
      return;
    }

    if (directive === "area-country") {
      if (args.length !== 4) fail("area-country requires 3 arguments");
      const areaCode = validateRange(args[2], 0, 9999);
      if (countryCode === null) fail(`invalid country code '${args[1]}'`);
      if (areaCode === null) fail(`invalid area code '${args[2]}'`);
      if (!validateCountryTag(args[3])) fail(`invalid country tag '${args[3]}'`);
      this.areaCountries.set(`${countryCode}:${areaCode}`, args[3]);
      this.knownCountryCodes.add(countryCode);
      return;
    }

    if (directive === "national-prefix" || directive === "international-prefix") {
      if (args.length !== 3) fail(`${directive} requires 2 arguments`);
      if (countryCode === null) fail(`invalid country code '${args[1]}'`);
      if (!validateDigits(args[2])) fail(`invalid ${directive} '${args[2]}'`);
      const map = directive === "national-prefix" ? this.nationalPrefixes : this.internationalPrefixes;
      map.set(countryCode, args[2]);
      this.knownCountryCodes.add(countryCode);
      return;
    }

    if (directive === "area") {
      if (args.length !== 4) fail("area requires 3 arguments");
      const areaCode = validateRange(args[2], 0, 9999);
      if (countryCode === null) fail(`invalid country code '${args[1]}'`);
      if (areaCode === null) fail(`invalid area code '${args[2]}'`);
      if (!AREA_TYPES.has(args[3])) fail(`invalid area type '${args[3]}'`);
      this.areas.set(`${countryCode}:${areaCode}`, args[3]);
      this.knownCountryCodes.add(countryCode);
      return;
    }

    if (directive === "subscriber") {
      if (args.length !== 5 && args.length !== 6) fail("subscriber requires 4 or 5 arguments");
      if (countryCode === null) fail(`invalid country code '${args[1]}'`);
      if (!args[4].includes("#")) fail("invalid subscriber mask");
      if (args.length === 6 && !NUMBER_KINDS.has(args[5])) fail(`invalid number kind '${args[5]}'`);

      const entries = this.subscribers.get(countryCode) || [];
      entries.push({
        regexNdc: args[2] === "*" ? null : regexFromPlan(args[2]),
        regexSn: regexFromPlan(args[3]),
        mask: args[4],
        kind: args[5] || NUMBER_KIND_REGULAR,
      });
      this.subscribers.set(countryCode, entries);
      this.knownCountryCodes.add(countryCode);
      return;
    }

    fail(`unknown directive '${directive}'`);
  }

  ccToType(countryCode) {
    if (countryCode < 0 || countryCode > 999) return CC_UNKNOWN;
    if (this.ccTypes.has(countryCode)) return this.ccTypes.get(countryCode);
    if (this.countries.has(countryCode)) return CC_NUMBER;

    const prefix = String(countryCode);
    for (const code of this.knownCountryCodes) {
      if (String(code).startsWith(prefix)) return CC_INCOMPLETE;
    }
    return CC_UNKNOWN;
  }

  ccToCountry(countryCode) {
    return this.countries.get(countryCode) || null;
  }

  areaToCountry(countryCode, areaCode) {
    return this.areaCountries.get(`${countryCode}:${areaCode}`) || this.ccToCountry(countryCode);
  }

  nationalPrefix(countryCode) {
    return this.nationalPrefixes.get(countryCode) || null;
  }

  internationalPrefix(countryCode) {
    return this.internationalPrefixes.get(countryCode) || null;
  }

  areaToType(countryCode, areaCode) {
    const key = `${countryCode}:${areaCode}`;
    if (this.areas.has(key)) return this.areas.get(key);
    if (this.areaCountries.has(key)) return AREA_NUMBER;

    const prefix = String(areaCode);
    for (const areaKey of this.areas.keys()) {
      const [cc, area] = areaKey.split(":");
      if (Number(cc) === countryCode && area.startsWith(prefix)) return AREA_INCOMPLETE;
    }
    for (const areaKey of this.areaCountries.keys()) {
      const [cc, area] = areaKey.split(":");
      if (Number(cc) === countryCode && area.startsWith(prefix)) return AREA_INCOMPLETE;
    }

    return AREA_UNKNOWN;
  }

  subscriberRules(countryCode) {
    return this.subscribers.get(countryCode) || [];
  }
}

export class E164Number {
  constructor(plan, context = null) {
    this.plan = plan;
    this.init();
    if (context) this.setContext(context);
  }

  init() {
    this.pos = 0;
    this.value = "";
    this.rawPhone = 0;
    this.cc = { type: CC_UNKNOWN, len: 0, value: 0 };
    this.number = { type: AREA_UNKNOWN, ndcLen: 0, ndc: 0, snLen: 0, sn: 0, mask: null, kind: NUMBER_KIND_UNKNOWN };
    this.context = { countryCode: 0, areaCode: 0, restriction: RESTRICT_NONE, acceptAlphanumeric: false, inputMode: INPUT_MODE_NUMBER };
    this.inputCountryExplicit = false;
    this.inputAreaExplicit = false;
  }

  setContext(context) {
    if (!context) {
      this.context = { countryCode: 0, areaCode: 0, restriction: RESTRICT_NONE, acceptAlphanumeric: false, inputMode: INPUT_MODE_NUMBER };
      this.inputCountryExplicit = false;
      this.inputAreaExplicit = false;
      return;
    }

    const inputMode = Number(context.inputMode ?? context.input_mode ?? INPUT_MODE_NUMBER);
    this.context = {
      countryCode: Number(context.countryCode || context.country_code || 0),
      areaCode: Number(context.areaCode || context.area_code || 0),
      restriction: Number(context.restriction || RESTRICT_NONE),
      acceptAlphanumeric: Boolean(context.acceptAlphanumeric ?? context.accept_alphanumeric),
      inputMode: inputMode === INPUT_MODE_DIALING ? INPUT_MODE_DIALING : INPUT_MODE_NUMBER,
    };
    this.inputCountryExplicit = false;
    this.inputAreaExplicit = false;
  }

  setValue(value) {
    if (value === null || value === undefined) {
      const context = this.context;
      this.init();
      this.context = context;
      return;
    }

    const explicitInternational = value[0] === "+";
    const explicitArea = value[0] === "(";
    let input = "";

    for (let i = explicitInternational || explicitArea ? 1 : 0; i < value.length && input.length < 15; i++) {
      const ch = value[i];
      if (/[0-9]/.test(ch)) input += ch;
      else if (this.context.acceptAlphanumeric && /[A-Za-z]/.test(ch)) input += alphaToDigit(ch);
    }

    const context = this.context;
    this.updateInputFlags(input, explicitInternational, explicitArea);
    const inputCountryExplicit = this.inputCountryExplicit;
    const inputAreaExplicit = this.inputAreaExplicit;
    const digits = this.context.inputMode === INPUT_MODE_DIALING
      ? this.normalizeDialingDigits(input)
      : (explicitInternational ? input : this.normalizeContextDigits(input, explicitArea));

    this.init();
    this.context = context;
    this.inputCountryExplicit = inputCountryExplicit;
    this.inputAreaExplicit = inputAreaExplicit;
    this.value = digits.slice(0, 15);
    this.pos = this.value.length;
    this.update();

    if (this.contextRejectsValue()) this.truncate(0);
  }

  addDigit(digit) {
    if (this.pos >= 15 || !/^[0-9]$/.test(digit)) return false;
    this.value += digit;
    this.pos = this.value.length;
    this.rawPhone = this.rawPhone * 10 + Number(digit);
    this.update();
    return true;
  }

  delDigit() {
    if (this.pos === 0) return false;
    this.value = this.value.slice(0, -1);
    this.pos = this.value.length;
    this.rawPhone = Math.floor(this.rawPhone / 10);
    this.update();
    return true;
  }

  updateInputFlags(value, explicitInternational, explicitArea) {
    this.inputCountryExplicit = false;
    this.inputAreaExplicit = false;
    if (value.length === 0) return;
    this.inputCountryExplicit = this.inputHasExplicitCountry(value, explicitInternational);
    this.inputAreaExplicit = this.inputHasExplicitArea(value, explicitArea);
  }

  inputHasExplicitCountry(value, explicitInternational) {
    if (explicitInternational) return true;
    if (this.context.countryCode === 0) return true;
    if (startsWith(value, this.plan.internationalPrefix(this.context.countryCode))) return true;
    return startsWith(value, appendNumber(this.context.countryCode));
  }

  inputHasExplicitArea(value, explicitArea) {
    if (explicitArea) return true;
    if (this.context.areaCode === 0) return true;

    const area = appendNumber(this.context.areaCode);
    if (startsWith(value, area)) return true;

    const prefix = this.plan.nationalPrefix(this.context.countryCode);
    return startsWith(value, prefix) && startsWith(value.slice(prefix.length), area);
  }

  normalizeContextDigits(value, explicitArea) {
    if (value.length === 0 || this.context.countryCode === 0) return value;

    const internationalPrefix = this.plan.internationalPrefix(this.context.countryCode);
    if (startsWith(value, internationalPrefix)) return value.slice(internationalPrefix.length);

    let digits = appendNumber(this.context.countryCode);
    const nationalPrefix = this.plan.nationalPrefix(this.context.countryCode);
    if (startsWith(value, nationalPrefix)) return digits + value.slice(nationalPrefix.length);

    if (this.context.areaCode !== 0 && !explicitArea) {
      const area = appendNumber(this.context.areaCode);
      if (!startsWith(value, area)) digits += area;
    }

    return digits + value;
  }

  normalizeDialingDigits(value) {
    if (value.length === 0) return "";
    if (this.context.countryCode === 0 || this.context.areaCode === 0) return "";

    const internationalPrefix = this.plan.internationalPrefix(this.context.countryCode);
    if (startsWith(value, internationalPrefix)) return value.slice(internationalPrefix.length);

    const country = appendNumber(this.context.countryCode);
    const nationalPrefix = this.plan.nationalPrefix(this.context.countryCode);
    if (startsWith(value, nationalPrefix)) return country + value.slice(nationalPrefix.length);

    const area = appendNumber(this.context.areaCode);
    if (startsWith(value, area)) return "";

    return country + area + value;
  }

  truncate(pos) {
    const nextPos = Math.max(0, Math.min(pos, 15));
    this.value = this.value.slice(0, nextPos);
    this.pos = this.value.length;
    this.refreshRawPhone();
  }

  refreshRawPhone() {
    this.rawPhone = this.value.length === 0 ? 0 : Number(this.value);
  }

  update() {
    this.updateCc();
    if (this.cc.type === CC_NUMBER) this.updateNumber();
  }

  updateCc() {
    if (this.pos === 0) {
      this.cc = { type: CC_UNKNOWN, len: 0, value: 0 };
      return;
    }

    let value = Number(this.value[0]);
    let len = 1;
    while (len < 3 && this.pos > len && this.plan.ccToType(value) === CC_INCOMPLETE) {
      value = value * 10 + Number(this.value[len]);
      len++;
    }

    this.cc = { type: this.plan.ccToType(value), len, value };
    if (this.cc.type === CC_UNKNOWN || this.cc.type === CC_SPARE) this.truncate(this.cc.len);
  }

  updateNumber() {
    const areaDigits = this.pos - this.cc.len;
    this.number = { type: this.plan.areaToType(this.cc.value, 0), ndcLen: 0, ndc: 0, snLen: 0, sn: 0, mask: null, kind: NUMBER_KIND_UNKNOWN };

    if (this.number.type === AREA_UNKNOWN) {
      this.number.type = AREA_NUMBER;
      this.number.snLen = areaDigits;
      this.number.sn = Number(this.value.slice(this.cc.len) || 0);
      this.number.kind = NUMBER_KIND_REGULAR;
      return;
    }

    while (
      areaDigits > this.number.ndcLen &&
      this.number.ndcLen < 4 &&
      this.plan.areaToType(this.cc.value, this.number.ndc) === AREA_INCOMPLETE
    ) {
      this.number.ndc = this.number.ndc * 10 + Number(this.value[this.cc.len + this.number.ndcLen]);
      this.number.ndcLen++;
    }

    this.number.type = this.plan.areaToType(this.cc.value, this.number.ndc);
    if (this.number.type === AREA_UNKNOWN) {
      if (this.number.ndcLen > 0) {
        this.number.ndcLen--;
        this.number.ndc = Math.floor(this.number.ndc / 10);
        if (this.number.ndcLen > 0) this.number.type = AREA_INCOMPLETE;
      }
      this.number.snLen = 0;
      this.truncate(this.cc.len + this.number.ndcLen);
    } else {
      this.number.snLen = this.pos - this.cc.len - this.number.ndcLen;
      this.number.sn = Number(this.value.slice(this.cc.len + this.number.ndcLen) || 0);
      this.truncate(this.cc.len + this.number.ndcLen + this.number.snLen);
    }

    this.applySubscriberRules();
  }

  subscriberCheck(ndc, sn) {
    for (const rule of this.plan.subscriberRules(this.cc.value)) {
      if (ndc && rule.regexNdc && !rule.regexNdc.test(ndc)) continue;
      rule.regexSn.lastIndex = 0;
      if (rule.regexSn.test(sn)) return rule;
    }
    return null;
  }

  applySubscriberRules() {
    const rules = this.plan.subscriberRules(this.cc.value);
    if (rules.length === 0) return;

    const ndc = this.number.ndc ? String(this.number.ndc) : null;
    let sn = this.value.slice(this.cc.len + this.number.ndcLen);
    while (this.number.snLen > 0) {
      const rule = this.subscriberCheck(ndc, sn);
      if (rule !== null) {
        this.number.mask = rule.mask;
        this.number.kind = rule.kind;
        return;
      }
      this.number.snLen--;
      this.truncate(this.pos - 1);
      sn = this.value.slice(this.cc.len + this.number.ndcLen);
    }

    const rule = this.subscriberCheck(ndc, sn);
    this.number.mask = rule?.mask || null;
    this.number.kind = rule?.kind || NUMBER_KIND_UNKNOWN;
  }

  contextRejectsValue() {
    if (this.pos === 0 || this.context.restriction === RESTRICT_NONE) return false;
    if (this.context.countryCode !== 0 && this.cc.value !== this.context.countryCode) return true;
    if (this.context.restriction < RESTRICT_AREA || this.context.areaCode === 0) return false;
    return this.number.ndcLen > 0 && this.number.ndc !== this.context.areaCode;
  }

  numberUsesAreaParentheses() {
    return this.number.kind === NUMBER_KIND_REGULAR;
  }

  getValue() {
    let result = `+${this.value.slice(0, this.cc.len)}`;
    const hasNdc = this.number.ndcLen > 0;
    const parenthesizedArea = hasNdc && this.numberUsesAreaParentheses();

    if (this.cc.type !== CC_INCOMPLETE && this.cc.type !== CC_UNKNOWN) {
      if (parenthesizedArea) result += " (";
      else if (hasNdc) result += " ";
      if (this.number.type !== AREA_UNKNOWN) {
        if (hasNdc) result += String(this.number.ndc);
        if (this.number.type === AREA_NUMBER) {
          if (parenthesizedArea) result += ")";
          result += " ";

          if (this.number.snLen > 0) {
            const number = this.value.slice(this.cc.len + this.number.ndcLen, this.cc.len + this.number.ndcLen + this.number.snLen);
            result += this.number.mask === null ? number : printMask(this.number.mask, number);
          }
        }
      }
    }

    return result;
  }

  getContextValue() {
    if (this.pos === 0) return "";
    let display = this.getValue();
    const restriction = this.context.restriction;
    let countryPrefixRemoved = false;

    if (this.context.countryCode !== 0 && (restriction >= RESTRICT_COUNTRY || !this.inputCountryExplicit)) {
      const prefix = `+${this.context.countryCode}`;
      if (display.startsWith(prefix)) {
        display = display.slice(prefix.length);
        if (display[0] === " ") display = display.slice(1);
        countryPrefixRemoved = true;
      }
    }

    if (countryPrefixRemoved && !this.numberUsesAreaParentheses()) {
      const nationalPrefix = this.plan.nationalPrefix(this.cc.value);
      if (nationalPrefix) return `${nationalPrefix}${display}`;
    }

    if (this.context.areaCode !== 0 && (restriction >= RESTRICT_AREA || !this.inputAreaExplicit)) {
      const prefix = `(${this.context.areaCode})`;
      if (display.startsWith(prefix)) {
        display = display.slice(prefix.length);
        if (display[0] === " ") display = display.slice(1);
      }
    }

    return display;
  }

  getDialingValue() {
    if (this.pos === 0) return "";

    if (
      this.context.countryCode !== 0 &&
      this.context.areaCode !== 0 &&
      this.cc.value === this.context.countryCode
    ) {
      if (
        this.number.ndcLen > 0 &&
        this.number.ndc === this.context.areaCode &&
        this.numberUsesAreaParentheses()
      ) {
        return this.value.slice(this.cc.len + this.number.ndcLen);
      }

      const nationalPrefix = this.plan.nationalPrefix(this.cc.value);
      if (nationalPrefix) return `${nationalPrefix}${this.value.slice(this.cc.len)}`;
    }

    if (this.context.countryCode !== 0) {
      const internationalPrefix = this.plan.internationalPrefix(this.context.countryCode);
      if (internationalPrefix) return `${internationalPrefix}${this.value}`;
    }

    return `+${this.value}`;
  }

  getCountry() {
    if (this.cc.type === CC_UNKNOWN || this.cc.type === CC_INCOMPLETE) return null;
    if (this.cc.type === CC_NUMBER && this.number.ndcLen > 0) {
      return this.plan.areaToCountry(this.cc.value, this.number.ndc);
    }
    return this.plan.ccToCountry(this.cc.value);
  }

  getNumberKind() {
    return this.cc.type === CC_NUMBER ? this.number.kind : NUMBER_KIND_UNKNOWN;
  }

  isComplete() {
    if (this.pos === 0 || this.cc.type !== CC_NUMBER) return false;
    if (this.number.type !== AREA_NUMBER || this.number.snLen === 0) return false;

    for (let i = 0; i < 10; i++) {
      const copy = this.clone();
      copy.addDigit(String(i));
      if (copy.value !== this.value) return false;
    }
    return true;
  }

  clone() {
    const copy = new E164Number(this.plan);
    Object.assign(copy, JSON.parse(JSON.stringify(this)));
    copy.plan = this.plan;
    return copy;
  }
}
