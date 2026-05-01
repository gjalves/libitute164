import { E164Plan } from "../src/index.js";
import "../src/input-element.js";

const input = document.querySelector("itute164-input");

try {
  const planText = await fetch("../../data/e164-plan.txt").then((response) => {
    if (!response.ok) throw new Error(`could not load plan: ${response.status}`);
    return response.text();
  });
  input.plan = E164Plan.fromText(planText);
} catch (error) {
  input.setAttribute("data-error", error.message);
}
