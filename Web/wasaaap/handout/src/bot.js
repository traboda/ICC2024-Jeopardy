const puppeteer = require("puppeteer");
const crypto = require("crypto");

const FLAG = process.env.FLAG;


async function visit(id) {
  const browser = await puppeteer.launch({
    args: [
        "--no-sandbox",
        "--headless"
    ],
    executablePath: "/usr/bin/google-chrome",
  });

  try {
    const secret = crypto.randomBytes(32).toString('hex');

    let page = await browser.newPage();

		await page.setCookie({
			httpOnly: true,
			name: 'sid',
			value: secret,
			domain: 'localhost',
		});

		page = await browser.newPage();
    visit = `[{"action":"add","content":"${FLAG}","time":1729881873363}]`

    await page.goto(`http://localhost:3000/?s=${btoa(visit)}`);

    await new Promise((resolve) => setTimeout(resolve, 3000));

    await page.goto(
      `http://localhost:3000/?s=${id}`,
      { timeout: 5000 }
    );

    await new Promise((resolve) => setTimeout(resolve, 3000));
    
    await page.close();
    await browser.close();

  } catch (e) {
    console.log(e);
    await browser.close();
  }
}

module.exports = { visit };
