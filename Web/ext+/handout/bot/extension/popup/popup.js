document.addEventListener("DOMContentLoaded", () => {
  const regexInput = document.getElementById("regexInput");
  const addRegexBtn = document.getElementById("addRegex");
  const blockedList = document.getElementById("blockedList");
  const extId = "mmbgfjoapgboolkblcglhnpgchljfmao";

  chrome.storage.sync.get(["blockedPatterns"], function (data) {
    if (data.blockedPatterns) {
      data.blockedPatterns.forEach((pattern) => {
        addPatternToUI(pattern);
      });
    }
  });

  addRegexBtn.addEventListener("click", () => {
    const regexPattern = regexInput.value;

    if (regexPattern) {
      chrome.storage.sync.get(["blockedPatterns"], function (data) {
        let patterns = data.blockedPatterns || [];
        patterns.push(regexPattern);

        chrome.storage.sync.set({ blockedPatterns: patterns }, function () {
          addPatternToUI(regexPattern);
          addBlockingRule(regexPattern);
        });
      });

      regexInput.value = "";
    }
  });

  function addPatternToUI(pattern) {
    const li = document.createElement("li");
    li.textContent = pattern;
    blockedList.appendChild(li);
  }

  function addBlockingRule(pattern) {
    chrome.runtime.sendMessage(
      {
        data: {
          action: "addRule",
          regex: pattern,
        },
      },
      (res) => {
        console.log(res.message);
      }
    );
  }
});
