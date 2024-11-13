FROM node:latest

RUN apt-get update && \
    apt-get clean

WORKDIR /usr/src/app

COPY package*.json ./

RUN npm install

RUN npx playwright install --with-deps chromium

COPY . .

EXPOSE 1338

CMD ["npm", "start"]
