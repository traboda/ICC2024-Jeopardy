# Use the official NGINX Alpine base image
FROM nginx:alpine

# Copy the custom nginx.conf file
COPY nginx.conf /etc/nginx/conf.d/default.conf

# Copy the index.html file to the default NGINX html directory
COPY index.html /usr/share/nginx/html/index.html

# Expose port 8443
EXPOSE 8443

# Start NGINX server
CMD ["nginx", "-g", "daemon off;"]
