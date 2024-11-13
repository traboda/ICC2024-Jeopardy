#!/usr/bin/env node
const express = require('express')
const childProcess = require('child_process');

const app = express();

app.use(express.urlencoded({ extended: false }));
url = "http://node-app:3000/login.html"

app.get('/report',(req,res)=>{

	res.type('text/plain');
	
	childProcess.spawn('node',['./bot.js',JSON.stringify(url)]);
	res.send('Admin will visit!');
	    	
});

app.get('/', (req, res) => {
	res.type('text/html');
	res.send('<html><body><form action="/report" method="get"><input type="submit" value="Report"></form></body></html>');
});

app.listen(8000);
