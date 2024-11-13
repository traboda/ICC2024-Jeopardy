FROM python:3.10-slim

RUN apt-get update && \
    apt-get install -y cron && \
    apt-get clean && \
    rm -rf /usr/bin/apt*

RUN mkdir -p /user && ln -s /user /app

WORKDIR /app

COPY ./ /app

RUN pip install --no-cache-dir pillow Flask


RUN python -m pip uninstall urllib3 python-socketio http requests httpx aiohttp websockets tornado httplib2 pycurl -y

RUN python -m pip uninstall pip -y

ENV FLASK_APP=app.py
ENV FLASK_ENV=development

EXPOSE 5000

RUN mv cron.sh /cron.sh && \
    chmod +x /cron.sh

RUN echo "*/1 * * * * /cron.sh" > /etc/cron.d/mycron && \
    chmod 0644 /etc/cron.d/mycron && \
    crontab /etc/cron.d/mycron

CMD cron && python3 -u app.py --host=0.0.0.0
