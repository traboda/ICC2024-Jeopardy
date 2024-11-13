FROM python:3.8-slim-buster
WORKDIR /app
COPY src/requirements.txt requirements.txt
RUN pip install -r requirements.txt
COPY src /app
CMD ["gunicorn", "-b", ":5000", "app:app"]
