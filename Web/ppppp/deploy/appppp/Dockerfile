FROM node:latest

WORKDIR /app
COPY . .
RUN npm install http qs cookie axios crypto
EXPOSE 3000
CMD ["node","index.js"]