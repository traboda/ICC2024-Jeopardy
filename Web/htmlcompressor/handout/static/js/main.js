
window.minify = require("html-minifier").minify;

(function() {
    
    function minifyHTML(htmlstring) {
          return window.minify(htmlstring, {
            collapseWhitespace: true,
            minifyCSS: true,
            minifyJS: true,
            keepClosingSlash: true,
            processConditionalComments: true
          });
      }

    function getHtmlInput() {
      return document.querySelector('textarea[name="html"]').value;
    }

    function setOutput(styledHtml) {
      document.querySelector('.output').innerHTML = styledHtml;
    }

    function styleHtml() {
      var html = getHtmlInput();
      let initialLength = html.length;
      var styledHtml = minifyHTML(html)
      let formattedLength = styledHtml.length;
      document.getElementById('initialLength').value = initialLength;
      document.getElementById('formattedLength').value = formattedLength;
      return styledHtml;
    }

    function handleSubmit(event) {
      event.preventDefault();
      var styledHtml = styleHtml();
      document.querySelector('textarea[name="html"]').value = styledHtml;
      event.target.submit();
    }

    function initializeEventListeners() {
      document.querySelector('form').addEventListener('submit', handleSubmit);
      document.querySelector(".share-btn").addEventListener('click', (event) => {
        event.preventDefault(); 
        shareContent();         
      });
    }

    function init() {
      initializeEventListeners();
    }


    function shareContent() {
      const form = document.querySelector('form');
      const htmlContent = form.querySelector('textarea[name="html"]').value;

      const body = new URLSearchParams({
        html: htmlContent
      }).toString()
      
      fetch('/share', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/x-www-form-urlencoded',
        },
        body: body
      })
      .then(response => response.json())
      .then(data => {
        alert('Content shared successfully!');
      })
      .catch(error => {
        console.error('Error sharing content:', error);
      });
    }

    init();
  })();

