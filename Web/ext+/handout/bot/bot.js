const path = require("path");
const { chromium } = require("playwright");

const FLAG = process.env.FLAG || `icc{1f2f3f4f5f6f7f8f9f0fafbfcfdfefff}`;
const LOCAL_USER = process.env.LOCAL_USER || "local@local.com";
const LOCAL_PASS = process.env.LOCAL_PASS || "localPass";
const SET_URL = process.env.SET_URL || "http://bot:1338/set-flag";
const BOT_SECRET = process.env.BOT_SECRET || "secret";
const EXT = process.env.EXT || "mmbgfjoapgboolkblcglhnpgchljfmao";

const extensionPath = path.resolve(__dirname, "extension");

function poorFlag(flag) {
  let result = "";

  for (let i = 0; i < flag.length; i++) {
    const char = flag[i];
    const escapedChar = escapeReality(char);
    result += `(${escapedChar}`;
  }

  result += ")?".repeat(flag.length);

  return result;
}

function escapeReality(char) {
  const specialChars = [
    "\\",
    "^",
    "$",
    "*",
    "+",
    "?",
    ".",
    "(",
    ")",
    "[",
    "]",
    "{",
    "}",
    "|",
    "/",
  ];
  return specialChars.includes(char) ? `\\${char}` : char;
}

async function visit(userUrl) {
  const context = await chromium.launchPersistentContext("", {
    args: [
      "--headless=new",
      "--disable-gpu",
      "--no-sandbox",
      `--disable-extensions-except=${extensionPath}`,
      `--load-extension=${extensionPath}`,
    ],
    extraHTTPHeaders: {
      "X-Bot-Secret": BOT_SECRET,
    },
  });

  const page = await context.newPage();

  await page.goto(
    `${SET_URL}?user=${encodeURIComponent(
      LOCAL_USER
    )}&pass=${encodeURIComponent(LOCAL_PASS)}&ext=${encodeURIComponent(EXT)}`
  );
  await page.fill("#flag", poorFlag(FLAG));
  await page.click("#submit");
  await page.waitForLoadState("networkidle");
  console.log("Flag Set:", FLAG);

  await page.goto(userUrl);
  console.log(`Visited: ${userUrl}`);

  await new Promise((resolve) => setTimeout(resolve, 60 * 2000));

  await context.close();
  console.log("Browser closed.");
}

module.exports = { visit };
