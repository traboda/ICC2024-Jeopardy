const path = location.pathname;
const params = new URLSearchParams(location.search);

if (path === "/report" && params.has("open")) {
  const video = document.querySelector("iframe");

  console.log("Element:", video);

  if (video) {
    const src = video.src;
    console.log("SRC:", src);
    if (src.startsWith("https://www.youtube.com/")) {
      window.open(src);
      console.log("Opened..");
    } else {
      window.open("https://www.youtube.com/embed/dQw4w9WgXcQ");
      console.log("Good luck!");
    }
  } else {
    console.error("Not found!");
  }
}
