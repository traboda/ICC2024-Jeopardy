FROM python:3.9-slim

WORKDIR /app
RUN pip install flask==3.0.1 ujson==4.0.1
COPY . .
RUN mv flag.txt /flag.txt 
EXPOSE 5000
CMD ["python","main.py"]