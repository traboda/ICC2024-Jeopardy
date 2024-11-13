from config import Config, safe
from flask import Flask
from database import db
import base64
import os

app = Flask(__name__)
app.config.from_object(Config)
app.jinja_env.filters["safe"] = safe
app.config['SECRET_KEY'] = base64.b64encode(os.urandom(16)).decode('utf-8')

db.init_app(app)

with app.app_context():
    from routes import api_route, app_route
    app.register_blueprint(api_route)
    app.register_blueprint(app_route)

    db.create_all()

if __name__ == '__main__':
    app.run(port=1337, host="0.0.0.0", debug=False)
