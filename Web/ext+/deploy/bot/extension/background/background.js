const cache = {}; // Fast Fast...
let userRules = [];
console.log("Rules:", chrome.declarativeNetRequest.getDynamicRules());

function addRule(urlPattern, id = false) {
  const ruleId = id ? id : Math.floor(Math.random() * 1e9);

  const rule = {
    id: ruleId,
    priority: 1,
    action: {
      type: "redirect",
      redirect: { extensionPath: "/blocked/blocked.html" },
    },
    condition: { regexFilter: urlPattern, resourceTypes: ["main_frame"] },
  };

  chrome.declarativeNetRequest.updateDynamicRules(
    {
      addRules: [rule],
      removeRuleIds: [],
    },
    () => {
      if (chrome.runtime.lastError) {
        console.error("Failed to add rule:", chrome.runtime.lastError);
      } else {
        console.log("Rule added with ID:", ruleId);
        userRules.push(ruleId); // Track rules!!
      }
    }
  );

  return ruleId;
}

addRule("^(http|https)://web:1337/report.*", 1); // Under construction(lol)...

function deleteRule(ruleId) {
  chrome.declarativeNetRequest.updateDynamicRules(
    {
      addRules: [],
      removeRuleIds: [ruleId],
    },
    () => {
      console.log("Rules:", chrome.declarativeNetRequest.getDynamicRules());
      if (chrome.runtime.lastError) {
        console.error("Failed to delete rule:", chrome.runtime.lastError);
      } else {
        console.log("Rule deleted with ID:", ruleId);
        userRules = userRules.filter((id) => id !== ruleId);
      }
    }
  );
}

function parseQueryString(queryString, options = {}) {
  const params = new URLSearchParams(queryString);
  const parsedParams = {};

  const {
    allowNestedObjects = true,
    allowedKeys = [],
    convertBooleans = false,
    convertNumbers = false,
    allowEmptyValues = false,
  } = options;

  const isAllowedKey = (key) =>
    allowedKeys.length === 0 || allowedKeys.includes(key);

  const blocklist = ["__proto__", "constructor", "prototype"];

  const isBlockedKey = (key) => blocklist.includes(key);

  for (const [key, value] of params.entries()) {
    if (!allowEmptyValues && !value) continue;

    const keyParts = key.split(/\[|\]/).filter((k) => k);

    let current = parsedParams;

    for (let i = 0; i < keyParts.length; i++) {
      let part = keyParts[i];

      if (isBlockedKey(part)) throw new Error(`Blocked key: ${part}`);

      part = part.trim();

      if (i === keyParts.length - 1) {
        let finalValue = value;
        if (convertBooleans)
          finalValue =
            value === "true" ? true : value === "false" ? false : value;
        if (convertNumbers && !isNaN(finalValue))
          finalValue = parseFloat(finalValue);

        if (current[part]) {
          if (!Array.isArray(current[part])) current[part] = [current[part]];
          current[part].push(finalValue.trim());
        } else {
          current[part] = finalValue.trim();
        }
      } else {
        if (!allowNestedObjects || !isAllowedKey(part))
          throw new Error(`Not allowed: ${part}`);
        if (!current[part]) current[part] = {};
        current = current[part];
      }
    }
  }

  return parsedParams;
}

async function login(userId, password) {
  if (cache[userId]) {
    console.log(`User ${userId} found in cache.`);
    return cache[userId];
  }

  try {
    const response = await fetch("http://web:1337/api/login", {
      method: "POST",
      headers: { "Content-Type": "application/x-www-form-urlencoded" },
      body: new URLSearchParams({ userId, password }).toString(),
    });

    const result = await response.json();
    if (result.success) {
      console.log(`User logged in successfully: ${userId}`);
      await chrome.storage.local.set({ authToken: result.token });
      cache[userId] = { authToken: result.token };
      return result;
    } else {
      console.error("Invalid login credentials.");
      return false;
    }
  } catch (error) {
    console.error("Error during login:", error);
    return false;
  }
}

async function register(userId, password) {
  if (cache[userId]) {
    console.log(`User ${userId} already exists in cache.`);
    return false;
  }

  try {
    const response = await fetch("http://web:1337/api/register", {
      method: "POST",
      headers: { "Content-Type": "application/x-www-form-urlencoded" },
      body: new URLSearchParams({ userId, password }).toString(),
    });

    const result = await response.json();
    if (result.success) {
      console.log(`User registered successfully: ${userId}`);
      await chrome.storage.local.set({ authToken: result.token });
      return result;
    } else {
      console.error("Registration failed:", result.message);
      return false;
    }
  } catch (error) {
    console.error("Error during registration:", error);
    return false;
  }
}

async function checkAdmin(userId, password) {
  if (cache[userId]) {
    return cache[userId].isAdmin;
  }
  const result = await login(userId, password);
  console.log("result:", result.isAdmin, result);
  return result ? result.isAdmin : false;
}

async function checkLocalUser(userId, password) {
  try {
    const response = await fetch("http://web:1337/api/local", {
      method: "POST",
      headers: { "Content-Type": "application/x-www-form-urlencoded" },
      body: new URLSearchParams({ userId, password }).toString(),
    });

    const result = await response.json();
    if (result.success) {
      console.log("Local user detected.");
      return result;
    } else {
      console.error("Not a local user.");
      return false;
    }
  } catch (error) {
    console.error("Error during local user check:", error);
    return false;
  }
}

chrome.runtime.onStartup.addListener(async () => {
  const result = await chrome.storage.local.get("authToken");
  const authToken = result.authToken;
  console.log(authToken ? "User is logged in." : "User is not logged in.");
});

chrome.runtime.onMessageExternal.addListener(async function (
  request,
  sender,
  sendResponse
) {
  console.log("Sender:", sender.url);
  const url = new URL(sender.url);

  if (url.hostname === "bot" && url.port === "1338") {
    const { userId, password, flag } = request.data;
    checkLocalUser(userId, password).then((isLocal) => {
      if (isLocal) {
        const flagRegex = `^(http|https)://web:1337/.*flag=${flag}$`;
        console.log(flagRegex);
        addRule(flagRegex);
        console.log("Flag rule set.");
        sendResponse({ success: true });
      } else {
        sendResponse({ success: false });
      }
    });
  } else if (request.data?.action) {
    const action = request.data.action;

    console.log(action, ":", request.data);

    if (action === "bulk") {
      console.log("Bulk Add");
      const queryString = new URL(sender.url).search;
      const parsedParams = parseQueryString(queryString);
      for (const [key, value] of Object.entries(parsedParams)) {
        if (typeof value === "string" && value.trim()) {
          try {
            new RegExp(value);
            addRule(value);
          } catch (error) {
            console.error("Invalid regex:", value, error);
            sendResponse({
              success: false,
              message: `Invalid regex pattern: ${value}`,
            });
            return;
          }
        }
      }
      console.log(userRules);
      sendResponse({ success: true, message: "Rules added successfully!" });
    } else if (action === "delete") {
      console.log("Delete");

      const { userId, password, id } = request.data;

      if (userId && password && id) {
        checkAdmin(userId, password).then((isAdmin) => {
          if (isAdmin) {
            try {
              console.log("Deleting rule...");
              const ruleIdToDelete = userRules.find((ruleId) => {
                console.log("matched:", ruleId);
                return ruleId === id;
              });
              if (ruleIdToDelete) {
                deleteRule(ruleIdToDelete);
              }
              console.log("Rule deleted:", id);
              sendResponse({
                success: true,
                message: "Rule deleted successfully!",
              });
            } catch (error) {
              console.error("Invalid regex:", regex, error);
              sendResponse({
                success: false,
                message: "Invalid regex pattern.",
              });
            }
          } else {
            sendResponse({
              success: false,
              message: "You are not authorized to delete this rule.",
            });
          }
        });
      }
    } else if (action === "login") {
      const { userId, password } = request.data;
      login(userId, password).then((result) => {
        sendResponse({ success: !!result });
      });
    } else if (action === "register") {
      const { userId, password } = request.data;
      register(userId, password).then((result) => {
        console.log("Registered!");
        sendResponse({ success: !!result });
      });
    } else {
      sendResponse({ success: false, message: "Invalid action." });
    }
  }
  return true;
});

chrome.runtime.onMessage.addListener((message, sender, sendResponse) => {
  console.log(`Message received from: ${JSON.stringify(sender)}`);

  if (message.data.action === "addRule") {
    const regexPattern = message.data.regex;
    console.log("Add rule");
    try {
      new RegExp(regexPattern);
      addRule(regexPattern);
      sendResponse({ success: true, message: `Rule added: ${regexPattern}` });
    } catch (error) {
      console.error("Invalid regex:", regexPattern, error);
      sendResponse({ success: false, message: "Invalid regex." });
    }
  }
});
