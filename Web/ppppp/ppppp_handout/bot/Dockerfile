FROM node:20.2.0-alpine

RUN apk update && apk upgrade
RUN apk add chromium

WORKDIR /app
COPY ./stuff/ /app/
RUN PUPPETEER_SKIP_DOWNLOAD=1 npm install
RUN chmod +x /app/index.js
RUN adduser -D -u 1001 www
RUN chmod 777 /app -R
USER www

CMD NODE_ENV=production /app/index.js
