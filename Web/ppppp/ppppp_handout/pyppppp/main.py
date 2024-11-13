from ppppp import password_manager
from flask import Flask, request
import ujson

app = Flask(__name__)

@app.route('/add-password', methods=['POST'])
def add_password_route():
    data = ujson.loads(request.get_data(as_text=True))
    result = password_manager.add_password(data)
    return "ok"

@app.route('/view-passwords', methods=['GET'])
def view_password_route():
    result = password_manager.view_password() 
    return result

if __name__ == '__main__':
    app.run(host='0.0.0.0',port=5000, debug=True)
