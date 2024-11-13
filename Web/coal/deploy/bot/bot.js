const puppeteer = require('puppeteer')

module.exports.visit = async function visit(url){
	try{
		browser = await puppeteer.launch({
		    pipe: true,
		    args: [
		        "--no-sandbox",
		        "--disable-setuid-sandbox",
		        "--js-flags=--noexpose_wasm,--jitless",
		        "--ignore-certificate-errors",
		    ],
		    executablePath: "/usr/bin/google-chrome-stable",
		    headless: 'new'
		});
		console.log("launched browser")

		let page = await browser.newPage();

		await page.goto('http://nginx:8000', {'waitUntil': 'networkidle0'});
		await page.type('#loginUsername', 'admin');
		await page.type('#loginPassword', 'yeah_good_luck_guessing_this_password_lmao');
		await Promise.all([
			page.click('#login-button'),
		]);
		await page.close();

		page = await browser.newPage();
		await page.goto(url);
		await new Promise(r => setTimeout(r, 60000));

	}catch(e){ 
		console.log(e) 
		return "failed"
	}
	try{
		await browser.close();
		return "success"
	}catch(e){
		return "error"
	}
	
}
