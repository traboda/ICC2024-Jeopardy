const express = require("express");
const { visit } = require("./bot");
const path = require('path');

const app = express()

const PORT = 3000;


app.set('view engine', 'ejs');
app.use(express.static(path.join(__dirname, 'static')));
app.use(express.urlencoded({ extended: false }));

app.use((req, res, next) => {
  res.set("Content-Security-Policy", "default-src 'self'; script-src 'self' 'unsafe-eval' https://cdnjs.cloudflare.com/ajax/libs/html-minifier/4.0.0/htmlminifier.min.js ; style-src 'self' 'unsafe-inline' https://fonts.googleapis.com ; object-src 'none';font-src 'self' https://fonts.googleapis.com https://fonts.gstatic.com;");
  next();
});

function escapeHTML(html,length) {
  let escapedHTML = ""
  const maxLength = Math.min(length, html.length); 
  for (let i = 0; i < maxLength; i++) {
    if (html[i] === '&') {
      escapedHTML += '&amp;';
    } else if (html[i] === '<') {
      escapedHTML += '&lt;';
    } else if (html[i] === '>') {
      escapedHTML += '&gt;';
    } else if (html[i] === '"') {
      escapedHTML += '&quot;';
    } else if (html[i] === "'") {
      escapedHTML += '&#039;';
    } else {
      escapedHTML += html[i]; 
    }
  }
  if (length < html.length) {
    escapedHTML += html.slice(length);
  }
  return escapedHTML;
}


app.get("/",(req,res) => {
    res.render("index", { escapedHtml: '' });
})


app.post("/render", (req, res) => {
  const html = req.body.html;
  const initialLength = parseInt(req.body.initialLength, 10);
  const formattedLength = parseInt(req.body.formattedLength, 10);
  const escapedHtml = escapeHTML(html,formattedLength);

  res.render("index", { 
    title: 'HTML Styler',
    message: 'Hello, World!',
    escapedHtml: escapedHtml,
    initialLength: initialLength,
    formattedLength: formattedLength
  });
  
});

app.get('/redirect', (req, res) => {
  const { url } = req.query; 
  if (url) {
    res.redirect(url); 
  } else {
    res.status(400).send('No URL provided for redirection.');
  }
});

app.post('/share', (req,res) => {
  const html = req.body.html
  console.log("got code ->",html)
  visit(html)
  return res.json({"works":"loll"})
  
})

app.listen(PORT, () => {
  console.log(`Server is running on http://localhost:${PORT}`);
});