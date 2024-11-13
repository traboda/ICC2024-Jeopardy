const params = new URLSearchParams(window.location.search);
const form = document.querySelector("form");
const extId = "mmbgfjoapgboolkblcglhnpgchljfmao";

const showMessage = (message, type) => {
  const element = document.createElement("li");
  if (type === "success") {
    element.classList.add("success");
  } else {
    element.classList.add("error");
  }

  element.innerText = message;
  document.querySelector("#message").appendChild(element);
};

if (params.get("userId") && params.get("pass") && params.get("type")) {
  const userId = params.get("userId");
  const pass = params.get("pass");
  const type = params.get("type");

  if (type === "login") {
    chrome.runtime.sendMessage(extId,
      {
        data: { userId: userId, action: "login", password: pass },
      },
      (res) => {
        if (res.success) {
          showMessage("Logged In!", "success");
        } else {
          showMessage("Failed to Log In.", "error");
        }
      }
    );
  } else {
    chrome.runtime.sendMessage(extId,
      {
        data: { userId: userId, password: pass, action: "register" },
      },
      (res) => {
        if (res.success) {
          showMessage("Registered!", "success");
        } else {
          showMessage("Failed to Register.", "error");
        }
      }
    );
  }
}
