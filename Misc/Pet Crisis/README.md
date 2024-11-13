# Pet Crisis

### Challenge Description

A new website for pets was recently created, and the owner is very proud of it's security since it is hosted on cloud. The owner is also a tech enthusiast and uses the same account for his personal use. Are you confident enough to break past the security barrier??

### Short Writeup

- The challenge begins with a pets website. All the images of the webpage lead us to a public S3 bucket.
- When we access the bucketusing the console, we get the access key and the secret access key in the metadata of image6
- We configure our aws-cli using these credentials.
- Now we look for roles and permissions.
- We can see that we have a assume role policy named Lamda_Assume and a role called Lambda_Guy. So we assume the role Lambda_Guy.
- We have gotten some new credentials so, we will add these in aws configure.
- Since the name is Lambda_Guy we try to get info about the running lambda function.
- We see a function called exec, so we get the configurations of the function.
- As we can see the flag is in the environment variable of the exec lambda function.

### Flag

icc{i_asked_lambda_to_keep_it_a_secret}

### Author

[Aayushman Singh](https://www.linkedin.com/in/aayushman-singh-814b5128b)
