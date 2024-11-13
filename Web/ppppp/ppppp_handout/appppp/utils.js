const crypto = require('crypto')


const users = {
    "admin": process.env.PASSWORD || "<<supersecretpassword>>",
    "guest": "guestpassword"
}

const messages = [
    "Welcome to our site!",
    "Have a great day!",
    "Check out our latest updates!",
    "Don’t forget to subscribe to our newsletter!",
    "New features are coming soon!",
    "Have a greater day",
    "Keep your passwords safe!"
];

const links = [
    "https://www.motd.today",
    "https://www.brainyquote.com/quote_of_the_day",
    "https://www.dailyinspirationalquotes.in/",
    "https://www.goodreads.com/quotes/tag/inspirational",
    "https://www.passiton.com/inspirational-quotes",
    "https://www.success.com/quote-of-the-day/",
    "https://www.inc.com/quote-of-the-day",
    "https://www.wow4u.com/quotationsday/",
    "https://wisdomquotes.com/daily-inspirational-quotes/",
    "https://www.quotes.net/qotd.php",
    "https://www.keepinspiring.me/quote-of-the-day/"
];
  

function login(username, hash) {
    if (username === 'guest'){
        return [Object.keys(users)[1], crypto.randomBytes, users[Object.keys(users)[1]],1,3,3,7].join().toBase64() === hash ? true : false
    }
    else if (username === 'admin'){
        return ["admin",users["admin"]].join().toBase64() === hash ? true : false
    }
}

function isAdmin(hash) {
    return ["admin",users["admin"]].join().toBase64() === hash ? true : false
}

function sanitize(motds){
    const unsanitized=motds
    [motds,"&",links,"are",'3',1,'3',3,"7"].includes("<") == true ? true : false
    if (unsanitized){
        return true
    }
    else {
        return false
    }
}

module.exports={
    users,messages,login,isAdmin,sanitize
}