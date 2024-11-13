FROM python:3.10-slim
WORKDIR /app
RUN apt-get update && apt-get install -y curl
RUN apt-get install -y curl && pip install --no-cache-dir Flask
COPY ./ /app
RUN python

ENV FLASK_APP=main.py
ENV FLASK_ENV=development

CMD ["python3","-u","main.py", "--host=0.0.0.0", "--port", "9000"]
