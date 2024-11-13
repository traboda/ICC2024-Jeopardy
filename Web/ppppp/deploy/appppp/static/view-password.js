async function fetchPasswords() {
  try {
    const response = await fetch('/view-passwords');
    const html = await response.text();
    document.getElementById('passwords-table').innerHTML = html;
  } catch (error) {
    console.error('Error fetching passwords:', error);
    document.getElementById('passwords-table').innerHTML = 'An error occurred while fetching passwords.';
  }
}

document.addEventListener('DOMContentLoaded', fetchPasswords);