#!/usr/bin/env node
const puppeteer = require('puppeteer')

async function visit(url){
	let browser;

	const username = process.env.USERNAME || 'admin';
	const hash = process.env.PASSWORD || '<<supersecretpassword>>';

	if (!username || !hash) {
        console.error('Username and hash must be set as environment variables.');
        return;
    }

	try{
		browser = await puppeteer.launch({
		    pipe: true,
		    args: [
		        "--no-sandbox",
		        "--disable-setuid-sandbox",
		        "--js-flags=--noexpose_wasm,--jitless",
		        "--ignore-certificate-errors",
		    ],
		    executablePath: "/usr/bin/chromium",
		    headless: 'new'
		});

		let page = await browser.newPage();

		await page.goto(url,{ waitUntil: 'domcontentloaded', timeout: 2000 });
		await page.type('#login-username', process.env.USERNAME);
		await page.type('#login-password', process.env.PASSWORD);
		await page.click('#login-button');
		await new Promise(resolve => setTimeout(resolve, 2000));
	}catch(e){ console.log(e) }
	try{await browser.close();}catch(e){}
	process.exit(0)
}

visit(JSON.parse(process.argv[2]))
