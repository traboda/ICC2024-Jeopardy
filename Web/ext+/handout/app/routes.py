from flask import Blueprint, jsonify, request, render_template, session, redirect, url_for, current_app, make_response, g, flash
from models import ExtensionUser, FrontendUser
from functools import wraps
from database import db
import datetime
import base64
import jwt
import re
import os

api_route = Blueprint('api_route', __name__)

EMAIL_REGEX = r'^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+$'

def generate_nonce():
    return base64.b64encode(os.urandom(16)).decode('utf-8')

def extract_youtube_id(url):
    match = re.search(r'(?:v=|\/)([0-9A-Za-z_-]{11}).*', url)
    return match.group(1) if match else None

def token_required(f):
    @wraps(f)
    def wrapper(*args, **kwargs):
        token = request.headers.get('Authorization')
        if not token:
            return jsonify({'message': 'Token is missing!'}), 403
        
        try:
            token = token.split(" ")[1]
            data = jwt.decode(token, current_app.config['SECRET_KEY'], algorithms=['HS256'])
            current_user = data['user_id']
            is_admin = data['is_admin']
            return f(current_user, is_admin, *args, **kwargs)
        except jwt.ExpiredSignatureError:
            return jsonify({'message': 'Token has expired!'}), 401
        except jwt.InvalidTokenError:
            return jsonify({'message': 'Invalid token!'}), 401
    wrapper.__name__ = f.__name__
    return wrapper

def generate_token(user_id, is_admin):
    payload = {
        'user_id': user_id,
        'is_admin': is_admin,
        'exp': datetime.datetime.utcnow() + datetime.timedelta(hours=1)
    }
    return jwt.encode(payload, current_app.config['SECRET_KEY'], algorithm='HS256')

@api_route.route('/api/getExtensionUsers', methods=['GET'])
def get_extension_users():
    users = ExtensionUser.query.all()
    users_list = [{"user_id": user.user_id} for user in users]
    return jsonify({"extension_users": users_list})

@api_route.route('/api/login', methods=['POST'])
def extension_login():
    user_id = request.form.get('userId')
    password = request.form.get('password')
    
    user = ExtensionUser.query.filter_by(user_id=user_id).first()
    
    if user and user.password == password:
        is_admin = (user_id == 'admin')
        token = generate_token(user_id, is_admin)
        return jsonify({"success": True, "isAdmin": is_admin, "token": token}), 200
    else:
        return jsonify({"success": False, "message": "Invalid credentials"}), 401

@api_route.route('/api/register', methods=['POST'])
def register_extension_user():
    user_id = request.form.get('userId')
    password = request.form.get('password')

    if not re.match(EMAIL_REGEX, user_id):
        return jsonify({"success": False, "message": "Invalid email format"}), 400

    if ExtensionUser.query.filter_by(user_id=user_id).first():
        return jsonify({"success": False, "message": "User already exists"}), 400

    new_extension_user = ExtensionUser(user_id=user_id, password=password)
    db.session.add(new_extension_user)
    db.session.commit()

    return jsonify({"success": True, "message": "Extension user registered successfully"}), 201

@api_route.route('/api/local', methods=['POST'])
def is_local():
    user_id = request.form.get('userId')
    password = request.form.get('password')
    local_id = os.environ.get('LOCAL_USER', 'local@local.com')
    local_pass = os.environ.get('LOCAL_PASS', 'localPass')
    print(local_id, user_id)
    print(local_pass, password)
    if user_id == local_id and local_pass == password:
        return jsonify({"success": True}), 200
    else:
        return jsonify({"success": False, "message": "Invalid credentials"}), 401

app_route = Blueprint('app_route', __name__)

@app_route.before_request
def before_request():
    g.nonce = generate_nonce()

@app_route.after_request
def add_csp_header(response):
    csp_policy = {
        "default-src": "'none'",
        "script-src": f"'nonce-{g.nonce}'",
        "style-src": "'self' https://fonts.googleapis.com",
        "img-src": "'self' https://*.youtube.com",
        "font-src": "https://fonts.googleapis.com https://fonts.gstatic.com",
        "connect-src": "'self'",
        "base-uri": "'none'",
        "frame-src": "'self' https://*.youtube.com"
    }
    csp_string = "; ".join([f"{key} {value}" for key, value in csp_policy.items()])
    response.headers['Content-Security-Policy'] = csp_string
    return response

def login_required(f):
    @wraps(f)
    def wrapper(*args, **kwargs):
        if 'user' not in session:
            flash("You need to log in first!", "error")
            return redirect(url_for('app_route.frontend_login'))
        return f(*args, **kwargs)
    wrapper.__name__ = f.__name__
    return wrapper

@app_route.route('/', methods=['GET'])
@login_required
def index():
    videos = [
        {"title": "Lamborghini", "url": "https://www.youtube.com/watch?v=PjRSbIlDc1E"},
        {"title": "Nissan GT", "url": "https://www.youtube.com/watch?v=QmGawXG8mYo"},
        {"title": "Hellcat Dodge", "url": "https://www.youtube.com/watch?v=Mndlkecqubo"},
        {"title": "Lamborghini", "url": "https://www.youtube.com/watch?v=PjRSbIlDc1E"},
        {"title": "Nissan GT", "url": "https://www.youtube.com/watch?v=QmGawXG8mYo"},
        {"title": "Hellcat Dodge", "url": "https://www.youtube.com/watch?v=Mndlkecqubo"},
        {"title": "Lamborghini", "url": "https://www.youtube.com/watch?v=PjRSbIlDc1E"},
        {"title": "Nissan GT", "url": "https://www.youtube.com/watch?v=QmGawXG8mYo"},
        {"title": "Hellcat Dodge", "url": "https://www.youtube.com/watch?v=Mndlkecqubo"},
        {"title": "Lamborghini", "url": "https://www.youtube.com/watch?v=PjRSbIlDc1E"},
        {"title": "Nissan GT", "url": "https://www.youtube.com/watch?v=QmGawXG8mYo"},
        {"title": "Hellcat Dodge", "url": "https://www.youtube.com/watch?v=Mndlkecqubo"}
    ]
    
    for video in videos:
        video_id = extract_youtube_id(video['url'])
        video['video_id'] = video_id
        video['thumbnail'] = f"https://img.youtube.com/vi/{video_id}/hqdefault.jpg"
    
    return render_template('index.html', nonce=g.nonce, user=session.get('user'), videos=videos)

@app_route.route('/watch/<video_id>', methods=['GET'])
@login_required
def watch(video_id):
    embed_url = f"https://www.youtube.com/embed/{video_id}"
    return render_template('watch.html', nonce=g.nonce, embed_url=embed_url, user=session.get('user'))

@app_route.route('/login', methods=['GET', 'POST'])
def frontend_login():
    if request.method == "GET":
        return render_template('login.html', nonce=g.nonce, user=session.get('user'))
    elif request.method == "POST":
        email = request.form.get('email')
        password = request.form.get('password')
        
        user = FrontendUser.query.filter_by(email=email).first()
        
        if user and user.password == password:
            session['user'] = email
            flash("You have successfully logged in!", "success")
            return redirect(url_for('app_route.index'))
        else:
            flash("Invalid email or password. Please try again.", "error")
            return redirect(url_for('app_route.frontend_login'))

@app_route.route('/register', methods=['GET', 'POST'])
def frontend_register():
    if request.method == "POST":
        email = request.form.get('email')
        password = request.form.get('password')

        if not re.match(EMAIL_REGEX, email):
            flash("Invalid email format", "error")
            return redirect(url_for('app_route.frontend_register'))

        if FrontendUser.query.filter_by(email=email).first():
            flash("User already exists", "error")
            return redirect(url_for('app_route.frontend_register'))

        new_user = FrontendUser(email=email, password=password)
        db.session.add(new_user)
        db.session.commit()

        flash("User registered successfully!", "success")
        return redirect(url_for('app_route.frontend_login'))
    
    elif request.method == "GET":
        return render_template('register.html', nonce=g.nonce, user=session.get('user'))

@app_route.route('/profile', methods=['GET'])
@login_required
def profile():
    return render_template('profile.html', nonce=g.nonce, user=session.get('user'))


@app_route.route('/report', methods=['GET', 'POST'])
# @login_required
def report():
    video_ids = [
        "Mndlkecqubo",
        "QmGawXG8mYo",
        "PjRSbIlDc1E",
    ]
    if request.method == "GET":
        return render_template('report.html', nonce=g.nonce, user=session.get('user'))
    elif request.method == "POST":
        car = request.form.get("car")
        video_id = request.form.get("video")
        content = request.form.get("content")

        if video_id not in video_ids:
            car = "Rick got"
            video_id = "dQw4w9WgXcQ"

        return render_template('report.html', car=car, video_id=video_id, nonce=g.nonce, user=session.get('user'))
