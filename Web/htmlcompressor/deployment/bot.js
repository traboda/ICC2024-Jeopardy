const puppeteer = require("puppeteer");


function sleep(ms) {
  return new Promise(resolve => setTimeout(resolve, ms));
}

const visit = async (code) => {
    console.log(`start: ${JSON.stringify(code)}`);
    const url = `http://localhost:3000`;
  
    const browser = await puppeteer.launch({
      headless: "new",
      executablePath: "/usr/bin/google-chrome-stable",
      args: ["--no-sandbox"],
    });
     
    const page = await browser.newPage();

    await page.setCookie({
      name: "FLAG",
      value: "icc{did_y0u_us3_4_Chr0me_zero_d4y}",
      domain: "localhost:3000",
      path: "/",
    });


    try {
      await page.goto(url, { timeout: 1000 });
      await sleep(1 * 1000);
      await page.waitForSelector("#htmlcode");
      await page.type("#htmlcode", code);
      await page.waitForSelector("#compress");
      await page.click("#compress");
      await sleep(10 * 1000);
    } catch (e) {
      console.log(e);
    }
    await page.close();
    await browser.close();
    console.log(`end: ${JSON.stringify(code)}`);
  };
  


module.exports = { visit };