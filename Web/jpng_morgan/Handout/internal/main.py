from flask import Flask, render_template,render_template_string,request

app=Flask(__name__)

blacklist=[
    'import',
    'from',
    'as',
    'eval',
    'compile',
    'open',
    'sys',
    'breakpoint',
    'subprocess',
    'getattr',
    'setattr',
    'delattr',
    'globals',
    'locals',
    'vars',
    'callable',
    'application',
    'dir',
    'help',
    'exec',
    'execfile',
    'input',
    'raw_input',
    'range',
    'call',
    'template',
    '__import__',
    'print',
    'help',
    'map',
    'filter',
    'zip',
    'reduce',
    'os',
    'sys',
    'subprocess',
    'shutil',
    'requests',
    'json',
    '__dict__',
    '__class__',
    '__globals__',
    '__module__',
    '__subclasses__',
    '__name__',
    '<',
    '>',
    'request',
    '$',
    '#',
    '&',
    'config',
    '__',
    'get_flashed_messages',
    '__mro__',
    'help',
    'license',
    'lipsum',
    'script',
    'iframe',
    '\\x',
    '\\u',
    '\\'
    'style',
]

@app.route('/',methods=['GET', 'POST'])
def home():
    if request.method=='POST':
        name=request.form.get('name')
        for i in blacklist:
            if i in name:
                print(i,"found")
                return render_template_string("sus !")
        return render_template_string(name)
    return render_template('index.html')



if __name__=='__main__':
    app.run(host='0.0.0.0',port='9000')
