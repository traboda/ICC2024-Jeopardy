
async function submitChat() {
    const userText = document.getElementById("user_text").value;
    const formData = new FormData();
    formData.append("user_text", userText);

    try {
        const response = await fetch("/api/chat", {
            method: "POST",
            body: formData
        });
        const result = await response.json();
        if (!response.ok) throw new Error(result.error || "Unknown error occurred");
        alert(result.response);
    } catch (error) {
        alert(`Error: ${error.message}`);
    }
}

async function submitAudio() {
    const audioFile = document.getElementById("audio_file").files[0];
    if (!audioFile) {
        alert("Please select an audio file.");
        return;
    }

    const formData = new FormData();
    formData.append("audio", audioFile);

    try {
        const response = await fetch("/api/audio-chat", {
            method: "POST",
            body: formData
        });
        const result = await response.json();
        if (!response.ok) throw new Error(result.error || "Unknown error occurred");
        alert(result.response);
    } catch (error) {
        alert(`Error: ${error.message}`);
    }
}


async function sendMessage(event) {
    event.preventDefault();
    const userInput = document.getElementById("user-input").value;

    // Append user's message to the chat log
    appendMessage("user", userInput);

    const response = await fetch("/api/chat", {
        method: "POST",
        headers: { "Content-Type": "application/x-www-form-urlencoded" },
        body: new URLSearchParams({ user_text: userInput })
    });
    const data = await response.json();
    
    // Append AI response to the chat log
    appendMessage("agent", data.response);
    document.getElementById("user-input").value = "";
}

async function sendAudio(event) {
    event.preventDefault();
    const formData = new FormData();
    formData.append("audio", document.getElementById("audio-input").files[0]);

    // Indicate that audio was sent in chat log
    appendMessage("user", "[Audio message sent]");

    const response = await fetch("/api/audio-chat", {
        method: "POST",
        body: formData
    });
    const data = await response.json();
    appendMessage("user","[Transcription]: " + data.transcription);

    // Append transcription or error message to the chat log
    appendMessage("agent", data.response || "[Error in audio processing]");
    document.getElementById("audio-input").value = "";
}

function appendMessage(sender, message) {
    const chatLog = document.getElementById("chat-log");
    const messageDiv = document.createElement("div");
    messageDiv.className = sender === "user" ? "user-message" : "agent-message";
    messageDiv.textContent = message;
    chatLog.appendChild(messageDiv);
    chatLog.scrollTop = chatLog.scrollHeight;
}

