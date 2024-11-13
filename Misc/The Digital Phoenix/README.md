# The Digital Phoenix

### Challenge Description

A mysterious hacker known as "Phoenix" has been targeting various cybersecurity conferences. Intelligence suggests they will strike at ICC next. Your task is to investigate and prevent the attack by identifying Phoenix's real identity at the event.

### Short Writeup

+ Extract the domain name - phoenix-security.online and 'Project Key' `7h3n3st` from the PCAP
+ CNAME for www.phoenix-security.online points to a GitHub page - phoenix-security832.github.io
+ Visit the github profile - github.com/phoenix-security832 and go through the repos
+ One of the older commits shows a SQLite DB that was removed from the repo - Commit 1f9cc00
+ The DB contains the email address of the Phoenix
+ Send Email to the Phoenix and leak the instagram username
+ Notice the watch the in the story of the account
+ Identify the on-site staff wearing the same watch and ask information about the project `7h3n3st` and get the flag  

### Flag

ICC{TH3_PH03N1X_N3V3R_SL33PS}