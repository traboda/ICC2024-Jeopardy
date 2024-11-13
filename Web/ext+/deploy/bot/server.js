const express = require("express");
const rateLimit = require("express-rate-limit");
const bodyParser = require("body-parser");
const path = require("path");
const { visit } = require("./bot");

const app = express();
const PORT = 1338;
const PAGE_REGEX = /\b(?:https?:\/\/|www\.)[^\s\/$.?#].[^\s]*\b/g;

const BOT_SECRET = process.env.BOT_SECRET || "secret";

app.set("view engine", "ejs");
app.set("views", path.join(__dirname, "views"));

app.use(bodyParser.urlencoded({ extended: true }));

const limiter = rateLimit({
  windowMs: 1 * 60 * 1000,
  max: 5,
  message: "Ayoo chill mann!!",
});

app.use("/visit", limiter);

app.get("/", (req, res) => {
  res.render("index");
});

app.post("/visit", limiter, async (req, res) => {
  const { url } = req.body;
  if (!url || !PAGE_REGEX.test(url)) {
    return res.status(400).send("Invalid URL!");
  }

  try {
    const response = await visit(url);
    res.send("Success!");
  } catch (error) {
    console.error("Error submitting flag:", error);
    res.status(500).send("flag = noooo....");
  }
});

app.get("/set-flag", (req, res) => {
  const botSecret = req.headers["x-bot-secret"];

  if (!botSecret || botSecret !== BOT_SECRET) {
    return res.status(403).send("I guess you are over smart?");
  }

  res.set({
    "Cache-Control": "no-store, no-cache, must-revalidate, proxy-revalidate",
    Pragma: "no-cache",
    Expires: "0",
    "Surrogate-Control": "no-store",
  });

  res.render("flag");
});

app.listen(PORT, () => {
  console.log(`Bot is running at http://bot:${PORT}`);
});
