window.onload = function() {
  loadData("/beer.txt", "beer");
  loadData("/food.txt", "food");
};

function loadData(url, id) {
  fetch(url)
    .then(response => response.text())
    .then(data => {
      document.getElementById(id).value = data.trim();
    })
    .catch(error => console.error('Error loading data:', error));
}

function saveData(url, id) {
  const data = document.getElementById(id).value;
  fetch(url, {
    method: "POST",
    headers: { "Content-Type": "text/plain" },
    body: data
  })
  .then(response => response.text())
  .then(text => alert("Saved successfully!"))
  .catch(error => console.error('Error saving data:', error));
}

async function loadMenus() {
  try {
    const beerResponse = await fetch('/beer.txt');
    const foodResponse = await fetch('/food.txt');

    if (beerResponse.ok) {
      document.getElementById('beerBox').value = await beerResponse.text();
    } else {
      document.getElementById('beerBox').value = "Failed to load beer menu.";
    }

    if (foodResponse.ok) {
      document.getElementById('foodBox').value = await foodResponse.text();
    } else {
      document.getElementById('foodBox').value = "Failed to load food menu.";
    }
  } catch (error) {
    console.error("Error loading menus:", error);
  }
}

async function saveBeer() {
  const beerText = document.getElementById('beerBox').value.trim();
  const response = await fetch('/saveBeer', {
    method: 'POST',
    headers: { 'Content-Type': 'text/plain' },
    body: beerText
  });

  if (response.ok) {
    document.getElementById('statusMsg').textContent = "✅ Beer menu saved!";
    setTimeout(() => document.getElementById('statusMsg').textContent = '', 3000);
  } else {
    document.getElementById('statusMsg').textContent = "❌ Failed to save beer menu.";
  }
}

async function saveFood() {
  const foodText = document.getElementById('foodBox').value.trim();
  const response = await fetch('/saveFood', {
    method: 'POST',
    headers: { 'Content-Type': 'text/plain' },
    body: foodText
  });

  if (response.ok) {
    document.getElementById('statusMsg').textContent = "✅ Food menu saved!";
    setTimeout(() => document.getElementById('statusMsg').textContent = '', 3000);
  } else {
    document.getElementById('statusMsg').textContent = "❌ Failed to save food menu.";
  }
}

// Attach buttons to functions
document.getElementById('saveBeerBtn').addEventListener('click', saveBeer);
document.getElementById('saveFoodBtn').addEventListener('click', saveFood);

// Load menu content on page load
loadMenus();

