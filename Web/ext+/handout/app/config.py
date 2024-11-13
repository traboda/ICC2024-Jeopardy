from markupsafe import Markup

class Config:
    SQLALCHEMY_DATABASE_URI = 'sqlite:///users.db'
    SQLALCHEMY_TRACK_MODIFICATIONS = False

def safe(value):
    blocked = [
        "meta",
        "script",
        "img",
        "link",
        "iframe",
        "area",
        "base",
        "canvas",
        "font",
        "form",
        "frameset",
        "math",
    ]

    for block in blocked:
        if block in value.lower():
            print("Blocked:", block)
            value = "cheater"

    return Markup(value)