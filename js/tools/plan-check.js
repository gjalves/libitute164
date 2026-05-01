#!/usr/bin/env node

import { readFileSync } from "node:fs";
import { basename } from "node:path";
import { E164Plan } from "../src/index.js";

const program = basename(process.argv[1]);
const path = process.argv[2];

if (process.argv.length !== 3) {
  console.error(`usage: ${program} <e164-plan.txt>`);
  process.exit(2);
}

try {
  const text = readFileSync(path, "utf8");
  E164Plan.fromText(text);
} catch (error) {
  console.error(`${path}: invalid numbering plan: ${error.message}`);
  process.exit(1);
}
