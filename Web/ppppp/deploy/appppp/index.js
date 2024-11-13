const http = require('http')
const qs = require('qs')
const { users, messages, login, isAdmin, sanitize} = require('./utils.js')
const path = require('path')
const fs = require('fs')
const cookie = require('cookie')
const axios = require('axios')

const PORT = 3000

String.prototype.toBase64 = function() {
    return Buffer.from(this).toString('base64')
}

const parseBody = (req, callback) => {
    let body = ''
    req.on('data', chunk => {
      body += chunk.toString()
    })
    req.on('end', () => {
      callback(qs.parse(body))
    })
  }

const serveStaticFile = (res, filePath, contentType) => {
    fs.readFile(filePath, 'utf8', (err, data) => {
        if (err) {
        res.writeHead(500, { 'Content-Type': 'text/plain' })
        return res.end('Internal Server Error')
        }
        res.writeHead(200, { 'Content-Type': contentType })
        if (filePath == path.join(__dirname,"static/login.html")) {
            const randomMessage = messages.length < 8 ? messages[Math.floor(Math.random() * messages.length)] : (messages[7], messages.pop())
            data = data.replace('{{motd}}',randomMessage)
        }
        res.end(data)
    })
}

const server = http.createServer((req,res) => {
    res.setHeader('Content-Security-Policy', `default-src 'self'; script-src 'self' 'sha256-BAS/ifNgAiYtpqQaJ6WJusP6tdAjyFkmWBEA3MwQhdU='; style-src 'self'`)
    const cookies=cookie.parse(req.headers.cookie||'')
    const hash = cookies.hash

    if (req.method === 'GET' && req.url === '/') {
        serveStaticFile(res, path.join(__dirname, 'static/login.html'), 'text/html')
    } else if (req.method === 'GET' && req.url === '/login.html') {
        serveStaticFile(res, path.join(__dirname, 'static/login.html'), 'text/html')
    } else if (req.method === 'GET' && req.url === '/bootstrap.min.css') {
        serveStaticFile(res, path.join(__dirname, 'static/bootstrap.min.css'), 'text/css');
    } else if (req.method === 'GET' && req.url === '/bootstrap.bundle.min.js') {
        serveStaticFile(res, path.join(__dirname, 'static/bootstrap.bundle.min.js'), 'application/javascript');
    } else if (req.method === 'GET' && req.url === '/login.js') {
        serveStaticFile(res, path.join(__dirname, 'static/login.js'), 'application/javascript');
    } else if (req.method === 'GET' && req.url === '/add-password.js') {
        serveStaticFile(res, path.join(__dirname, 'static/add-password.js'), 'application/javascript')
    } else if (req.method === 'GET' && req.url === '/view-password.js') {
        serveStaticFile(res, path.join(__dirname, 'static/view-password.js'), 'application/javascript')
    } else if (req.method === 'GET' && req.url === '/login.css') {
        serveStaticFile(res, path.join(__dirname, 'static/login.css'), 'text/css');
    } else if (req.method === 'GET' && req.url === '/view-password.css') {
        serveStaticFile(res, path.join(__dirname, 'static/view-password.css'), 'text/css')
    } else if (req.method === 'GET' && req.url === '/add-password.css') {
        serveStaticFile(res, path.join(__dirname, 'static/add-password.css'), 'text/css')
    } else if (req.method === 'GET' && req.url === '/add-password.html') {
        if (isAdmin(hash)){
            serveStaticFile(res, path.join(__dirname, 'static/add-password.html'), 'text/html')
        } else{
            res.writeHead(401, { 'Content-Type': 'text/plain' })
            return res.end('Uh oH')
        }
    } else if (req.method === 'GET' && req.url === '/view-password.html') {
        const hash = cookies.hash
        if (isAdmin(hash)){
            serveStaticFile(res, path.join(__dirname, 'static/view-password.html'), 'text/html')
        } else{
            res.writeHead(401, { 'Content-Type': 'text/plain' })
            return res.end('Uh oH')
        }
    } else if (req.method === 'POST' && req.url === '/login') {
        parseBody(req, ({ username, hash }) => {
            if (login(username,hash)){
                res.writeHead(200, {
                'Content-Type': 'text/plain',
                'Set-Cookie': cookie.serialize('hash', hash)
                });
                res.end('Login successful');
            } else{
                res.writeHead(401, { 'Content-Type': 'text/plain' });
                return res.end('Uh oH');
            }

        })
        
      } else if (req.method === 'POST' && req.url === '/add-password') {
        if (!isAdmin(hash)) {
            res.writeHead(401, { 'Content-Type': 'text/plain' })
            res.end('Uh oH')
        } else {
            let body = ''
            req.on('data', chunk => {
                body += chunk.toString()
            })
            req.on('end', async () => {
                try {
                    const parsedBody = JSON.parse(body)
                    if (parsedBody.filename !== "passwords.txt"){
                        res.writeHead(500, { 'Content-Type': 'text/plain' })
                        res.end('Uh oH')
                    } else {
                        const response = await axios.post('http://python-app:5000/add-password',parsedBody)
                        res.writeHead(200, {'Content-Type': 'text/plain'})
                        res.end('Password saved successfully')
                    }
                } catch (error) {
                    console.log(error)
                    res.writeHead(500, { 'Content-Type': 'text/plain' })
                    res.end('Uh oH');
                }
            })
        }
    }else if (req.method === 'GET' && req.url === '/view-passwords') {
        if (!isAdmin(hash)) {
            res.writeHead(401, { 'Content-Type': 'text/plain' })
            res.end('Uh oH')
        } else {
            axios.get('http://python-app:5000/view-passwords').then(response => {
                const rows = response.data.trim().split('\n').map(line => {
                    const [username, password, url] = line.split(',')
                    return `<tr><td>${username}</td><td>${password}</td><td>${url}</td></tr>`
                }).join('')

                const table = `
                    <table border="1">
                        <tr><th>Username</th><th>Password</th><th>URL</th></tr>
                        ${rows}
                    </table>`

                res.writeHead(200, { 'Content-Type': 'text/plain' })
                res.end(table)
            }).catch(error => {
                console.log(error)
                res.writeHead(500, { 'Content-Type': 'text/plain' })
                res.end("Uh oH")
            })
        }
    } 
    else if (req.method === 'POST' && req.url === '/add-motd') {
        if (!login("guest", hash)) {
            res.writeHead(401, { 'Content-Type': 'text/plain' })
            res.end('Uh oH');
        }
        else {
            parseBody(req, ({ message }) => {
                if(message){
                    messages.push(message)
                    if (sanitize(messages)){
                        messages.pop()
                    }
                    res.writeHead(200, { 'Content-Type': 'text/plain' })
                    res.end('Added motd');
                }
                else {
                    res.writeHead(400, {'Content-Type': 'text/plain'})
                    res.end("Uh oH")
                }
            })
        }
    } else {
        res.writeHead(404, { 'Content-Type': 'text/plain' })
        res.end('Not Found')
      }
})

server.listen(PORT, () => {
    console.log(`Server is running on port ${PORT}`)
  })
