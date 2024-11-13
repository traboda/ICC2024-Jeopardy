from fastapi import FastAPI, Form, UploadFile, File, HTTPException
from fastapi.responses import HTMLResponse, JSONResponse
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates
import asyncio
import os
import re
from pydub import AudioSegment

app = FastAPI()
app.mount("/static", StaticFiles(directory="static"), name="static")
templates = Jinja2Templates(directory="templates")

VALID_FILENAME_REGEX = re.compile(r'^[\w,\s-]+\.[A-Za-z]{3,4}$')


# Max file size (4 MB)
MAX_FILE_SIZE = 4 * 1024 * 1024
# Max audio duration (20 seconds)
MAX_AUDIO_DURATION = 20



def sanitize_input(text: str) -> str:
    """Sanitize user input to prevent command injection."""
    if "'" in text or ';' in text or '&' in text or '|' in text:
        raise HTTPException(status_code=400, detail="Invalid input detected.")
    return text.strip()


@app.get("/", response_class=HTMLResponse)
async def index():
    return templates.TemplateResponse("index.html", {"request": {}})


@app.post("/api/chat")
async def chat_response(user_text: str = Form(...)):
    sanitized_text = sanitize_input(user_text)

    # Run the chatbot in a subprocess
    proc = await asyncio.create_subprocess_shell(
        f'python3 chatbot.py "{sanitized_text}"',
        stdout=asyncio.subprocess.PIPE,
        stderr=asyncio.subprocess.DEVNULL
    )
    
    stdout, _ = await proc.communicate()
    
    # Decode output from bytes to string
    return JSONResponse(content={"response": stdout.decode().strip()})





def is_valid_audio_file(file: UploadFile) -> bool:
    if not file.filename or not VALID_FILENAME_REGEX.match(file.filename):
        print(file.filename)
        print(VALID_FILENAME_REGEX)
        raise HTTPException(status_code=400, detail="Invalid file name.")
    if file.size > MAX_FILE_SIZE:
        raise HTTPException(status_code=400, detail="File size exceeds the limit of 4MB.")
    file.file.seek(0)
    return True

@app.post("/api/audio-chat")
async def audio_response(audio: UploadFile = File(...)):

    try:
        if not is_valid_audio_file(audio):
            return JSONResponse(content={"error": "Invalid audio file."}, status_code=400)
        
        audio_file_path = f"/tmp/{audio.filename}"
        with open(audio_file_path, "wb") as buffer:
            buffer.write(await audio.read())
        
        audio_file_path = sanitize_input(audio_file_path)
        proc = await asyncio.create_subprocess_shell(
            f"python3 whisper.py --model tiny.en {audio_file_path} --language English --best_of 5 --beam_size None",
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.PIPE
        )
        stdout, stderr = await proc.communicate()
        if proc.returncode != 0:
            return JSONResponse(content={"error": "Error running Whisper."}, status_code=500)

        transcription = stdout.decode().strip()

        print("Transcription",transcription)
        chatbot_proc = await asyncio.create_subprocess_shell(
            f"python3 chatbot.py '{transcription}'",
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.STDOUT
        )
        chatbot_stdout, chatbot_stderr = await chatbot_proc.communicate()
        return JSONResponse(content={"response": chatbot_stdout.decode().strip(), "transcription": transcription})
    except Exception as e:
        print("err",e)
        return JSONResponse(content={"error": f"Unexpected error: {str(e)}"}, status_code=500)

