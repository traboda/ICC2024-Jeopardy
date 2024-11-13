import os

def add_password(data):
    filename = data.get('filename')
    username = data.get('username')
    password = data.get('password')
    url = data.get('url')
    
    if username and password and url:
        entry = username + ',' +password + ',' + url
        with open(os.path.join(os.path.dirname(__file__), filename), 'a') as f:
            f.write(entry + '\n')
        return {'status': 'success', 'message': 'Password added successfully'}
    return {'status': 'error', 'message': 'Invalid input'}

def view_password():
    with open(os.path.join(os.path.dirname(__file__), 'passwords.txt'), 'r') as f:
        lines = f.read()
    return lines

