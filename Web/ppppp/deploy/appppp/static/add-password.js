document.addEventListener('DOMContentLoaded', function() {
  document.getElementById('add-button').addEventListener('click', addpassword);
});

async function addpassword() {
  const username = document.getElementById('username').value;
  const password = document.getElementById('password').value;
  const url = document.getElementById('url').value;
  const filename = document.getElementById('filename').value;

  const data = {
    username: username,
    password: password,
    url: url,
    filename: filename
  };

  try {
    const response = await fetch('/add-password', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(data)
    });

    const responseText = await response.text();
    alert(responseText);
  } catch (error) {
    console.error('Login error:', error);
    alert('An error occurred during login. Please try again.');
  }
}
