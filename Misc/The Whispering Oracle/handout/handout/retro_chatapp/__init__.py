def run():
    from retro_chatapp.wsgi import RetroChatApplication
    RetroChatApplication().run()

def debug():
    import os
    import uvicorn
    os.environ["DEBUG"] = "1"
    uvicorn.run("retro_chatapp.app:app", reload=True, port=8005)
