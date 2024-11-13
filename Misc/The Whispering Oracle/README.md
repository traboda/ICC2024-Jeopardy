# Whispering Secrets

### Challenge Description

Welcome to ChatterBox! You’ve encountered a friendly AI chatbot that seems to know a bit more than it lets on. Your task is to get the chatbot to reveal a hidden flag. However, it’s not as easy as it sounds. The chatbot is designed to keep secrets, so you’ll have to use your creativity and cunning to uncover it.

You can interact with ChatterBox using either text or audio messages. Try different strategies, engage it in conversation, or maybe ask the right questions. But be careful – the chatbot is trained to avoid giving up the flag too easily.

## Short Writeup

There is a web app, which is a chat application

- we look into the source and see it's a simple if , else chat application so no prompt injection or something like that
- in app.py there is a subshell being run
- input is being sanitised through a naive sanitize_input() function
- same subshell is being used to run whisper.py command
- goal is to combine multiple injections to bypass the sanitization input ( combination of uploading a file/file name and text message )

### Flag

icc{1nvrt3d_wh1sp3r}
