# Underdeveloped

### Challenge Description

Cloudelco is a tech startup focused on cloud infrastructure and digital transformation, though they’re still in the early stages of development. Ironically, they’ve set up some of their services on AWS, their biggest competitor, and the team’s laid-back approach has led to numerous misconfigurations on the platform. Some employees have even accidentally uploaded a half-built version of their website to Docker Hub without securing it properly. Your mission: find the flag.

### Short Writeup

According to the challenge description ,their website has been leaked inside a Docker Image in Docker Hub. Upon searching for the company name “cloudelco” we get their image

Next we can set up the image and run the container and indeed we do get a website. We enumerate the website , but we get nothing. Lets now analyze the docker image. To get the commands that the Dockerfile ran we can run the following command
`sudo docker history --no-trunc --format "{{.CreatedAt}} {{.CreatedBy}}" 9bc48706fbf5`

We see the following commands being executed

```
2024-10-28T17:54:19+05:30 /bin/sh -c wget
https://cloudelcobucket.s3.eu-north-1.amazonaws.com/cloudelco.png -O /var/www/html/assets/cloudelco.png
2024-10-28T17:54:17+05:30 /bin/sh -c wget
https://cloudelcobucket.s3.eu-north-1.amazonaws.com/styles.css -O /var/www/html/styles.css
2024-10-28T17:54:16+05:30 /bin/sh -c wget
https://cloudelcobucket.s3.eu-north-1.amazonaws.com/index.html -O /var/www/html/index.html

```

Looks like the source code and other resources for a
website are being downloaded from a public s3 bucket

```
❯ aws s3 ls s3://cloudelcobucket/
2024-10-28 15:41:56 3930 cloudelco.png
2024-10-28 16:01:19 3256 index.html
2024-10-28 15:53:34 3203 styles.css
2024-10-28 20:20:33 431 undev.tf

```

A terraform script? Hmm.. interesting. Let’s check it out.

```❯ wget
https://cloudelcobucket.s3.eu-north-1.amazonaws.com/undev.tf

```

An AMI ID is hard-coded in the script. It is a public AMI.

- Build EC2 instance using AMI
- Get IP address of EC2 instance from development.sh in home projects folder in home directory. There is also a mention of some secret keys

A website hosted on the EC2 instance. We find that it has :

- Path traversal.
- We see that there is a user named keymaster. This was mentioned in development.sh in the EC2 instance that we booted up from the public AMI.
- Navigate to /home/keymaster/access.keys. We get access keys
- Do `aws configure`
  `❯ aws secretsmanager list-secrets`

There is a secret named “topsecret” stored using the AWS secrets manager service.
`❯ aws secretsmanager get-secret-value --secret-id topsecret`

And we see the flag

### Flag

icc{7h3_53cr37_15_0u7}

### Author

[Anirudh Ajithkumar](https://www.linkedin.com/in/anirudh-ajithkumar-651469285)
[Devnath N S](https://www.linkedin.com/in/devnath-n-s)
