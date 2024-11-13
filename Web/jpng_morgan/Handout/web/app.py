from flask import Flask, request, redirect, url_for, render_template, render_template_string,send_from_directory, session
import os
import random
import zipfile
import logging
import tarfile
from PIL import Image
from functools import wraps

logging.basicConfig(level=logging.WARNING)

app = Flask(__name__)
app.secret_key = hex(random.getrandbits(20))
app.config['UPLOAD_FOLDER'] = '/tmp/uploads'
app.config['EXTRACTED_FOLDER'] = '/tmp/extracted'
app.config['CONVERTED_FOLDER'] = '/tmp/Converted_folder'

os.makedirs(app.config['UPLOAD_FOLDER'], exist_ok=True)
os.makedirs(app.config['EXTRACTED_FOLDER'], exist_ok=True)
os.makedirs(app.config['CONVERTED_FOLDER'], exist_ok=True)

users = {}

def login_required(f):
    @wraps(f)
    def decorated_function(*args, **kwargs):
        if 'logged_in' not in session:
            return redirect(url_for('login'))
        return f(*args, **kwargs)
    return decorated_function


def premium_required(f):
    @wraps(f)
    def decorated_function(*args, **kwargs):
        if session.get('user_type') != 'premium':
            return render_template_string('This feature is available for premium users only.')
        return f(*args, **kwargs)
    return decorated_function

@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        username = request.form['username']
        password = request.form['password']
        if username in users and users[username]['password'] == password:
            session['logged_in'] = True
            session['username'] = username
            session['user_type'] = users[username]['type']
            session['tokens'] = users[username].get('tokens', 50) 
            return redirect(url_for('index'))
        else:
            return render_template('login.html', error_message='Invalid username or password.')
    return render_template('login.html')

@app.route('/register', methods=['GET', 'POST'])
def register():
    if request.method == 'POST':
        username = request.form['username']
        password = request.form['password']
        if username and password:
            users[username] = {'password': password, 'type': 'normal', 'tokens': 50} 
            session['username'] = username
            session['user_type'] = 'normal'
            session['tokens'] = 50
            return redirect(url_for('index'))
        else:
            return render_template_string('Please enter a valid username and password.')
    return render_template('register.html')

@app.route('/logout')
@login_required
def logout():
    session.clear()
    return redirect(url_for('login'))

@app.route('/upgrade-to-premium')
@login_required
def upgrade_to_premium():
    return render_template('upgrade_to_premium.html')


@app.route('/process-upgrade', methods=['POST'])
@login_required
def process_upgrade():
    if session.get('tokens', 0) < 100:
        return render_template('message.html',message="No enough tokens : (")

    username = session.get('username')
    if username:
        session['user_type'] = 'premium'
        session['tokens'] = 0 
        return render_template('message.html',message="Congratulations! You have been upgraded to a premium user.")
    return redirect(url_for('index'))

@app.route('/')
@login_required
def index():
    return render_template('index.html', user_type=session['user_type'])

@app.route('/upload', methods=['GET', 'POST'])
@login_required
def upload_file():
    if request.method == 'POST':
        if 'file' not in request.files:
            return render_template('message.html',message="No file part")

        file = request.files['file']

        if file.filename == '':
            return render_template('message.html',message="No file selected")

        if session['user_type'] == 'premium':
            if file.filename.endswith('.zip'):
                multiple_image(file)
            elif file.filename.endswith('.tar') or file.filename.endswith('.tar.gz'):
                tar_upload(file)
            else:
                return render_template('message.html',message="Error in reading the file")
        else:
            if not file.filename.endswith(('.png', '.jpg', '.jpeg')):
                return render_template('message.html',message="Please upload png , jpeg files . To upload tar file, please purchase premium")
            single_image(file)

        return redirect(url_for('file_history'))

    return render_template('upload.html')

def single_image(file):
    path = os.path.join(app.config['UPLOAD_FOLDER'], file.filename)
    file.save(path)
    convert_image(path)

def multiple_image(zip_file):
    path = os.path.join(app.config['UPLOAD_FOLDER'], zip_file.filename)
    zip_file.save(path)
    extract_from_archive(path)

    for root, dirs, files in os.walk(app.config['EXTRACTED_FOLDER']):
        for file in files:
            if file.endswith(('.png', '.jpg', '.jpeg')):
                file_path = os.path.join(root, file)
                convert_image(file_path) 

def tar_upload(tar_file):
    path = os.path.join(app.config['UPLOAD_FOLDER'], tar_file.filename)
    tar_file.save(path)

    if tarfile.is_tarfile(path):
        extractdir = app.config["EXTRACTED_FOLDER"]  
        os.makedirs(extractdir, exist_ok=True)  

        with tarfile.open(path, 'r:*') as tar: 
            for tarinfo in tar:
                try:
                    tar.extract(tarinfo, path=extractdir)
                    if tarinfo.isfile() and tarinfo.name.endswith(('.png', '.jpg', '.jpeg')):
                        extracted_file_path = os.path.join(extractdir, tarinfo.name)
                        convert_image(extracted_file_path) 
                except Exception as e:
                    logging.error(f"Error extracting {tarinfo.name}: {e}")

    else:
        logging.warning(f'File {path} is not a valid tar file.')

def convert_image(image_path):
    try:
        img = Image.open(image_path)
        if img.mode == 'RGBA':
            img = img.convert('RGB')

        jpeg_name = os.path.splitext(os.path.basename(image_path))[0] + '.jpeg'
        jpeg_path = os.path.join(app.config['CONVERTED_FOLDER'], jpeg_name)
        img.save(jpeg_path)

        logging.info(f'Successfully processed image: {image_path} and saved as {jpeg_name}')
    except Exception as e:
        logging.error(f'Error processing image {image_path}: {e}')

def extract_from_archive(zip_file_path):
    with zipfile.ZipFile(zip_file_path, 'r') as zip_ref:
        zip_ref.extractall(app.config['EXTRACTED_FOLDER'])

@app.route('/file-history')
@login_required
def file_history():
    files = os.listdir(app.config['CONVERTED_FOLDER'])
    return render_template('file_history.html', files=files)

@app.route('/download/<filename>')
@login_required
def download_file(filename):
    return send_from_directory(app.config['CONVERTED_FOLDER'], filename)

if __name__ == '__main__':
    app.run(host='0.0.0.0', debug=False, port=5000)
