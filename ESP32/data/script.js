// Load beer & food text on page load
window.onload = function () {
  loadData("/beer.txt", "beerBox");
  loadData("/food.txt", "foodBox");
};

function loadData(url, elementId) {
  fetch(url)
    .then(response => response.text())
    .then(text => {
      document.getElementById(elementId).value = text.trim();
    })
    .catch(err => console.error("Load error:", err));
}

// Save both menus via /update route
async function saveMenus() {
  const beerText = document.getElementById("beerBox").value.trim();
  const foodText = document.getElementById("foodBox").value.trim();

  const response = await fetch("/update", {
    method: "POST",
    headers: { "Content-Type": "application/x-www-form-urlencoded" },
    body: `beer=${encodeURIComponent(beerText)}&food=${encodeURIComponent(foodText)}`
  });

  if (response.ok) {
    document.getElementById("statusMsg").textContent = "✅ Menu Updated & Saved!";
  } else {
    document.getElementById("statusMsg").textContent = "❌ Failed to save.";
  }

  setTimeout(() => document.getElementById("statusMsg").textContent = "", 3000);
}

// Attach single save button
document.getElementById("saveAllBtn").addEventListener("click", saveMenus);
