// ThingSpeak Details
const channelId = " 2804055";
const readApiKey = "UXH7HA7UYXRSFZUF";

// DOM Elements
const foodLevelElement = document.getElementById('food-level');
const moistureLevelElement = document.getElementById('moisture-level');
const notificationsElement = document.getElementById('notifications');
const checkFoodButton = document.getElementById('check-food-button');
const foodStatusElement = document.getElementById('food-status');

// Fetch Data from ThingSpeak
async function fetchData() {
  try {
    const response = await fetch(
      `https://api.thingspeak.com/channels/${channelId}/feeds.json?api_key=${readApiKey}&results=1`
    );
    if (!response.ok) throw new Error('Failed to fetch data');
    const data = await response.json();
    const latestFeed = data.feeds[0];
    
    // Update Food and Moisture Levels
    const foodLevel = parseFloat(latestFeed.field1) || 0;
    const moistureLevel = parseFloat(latestFeed.field2) || 0;

    foodLevelElement.textContent = `${foodLevel} %`;
    moistureLevelElement.textContent = `${moistureLevel} %`;

    // Update Notifications
    const foodDispensed = latestFeed.field3 === '1' ? 'Food Dispensed' : '';
    const waterDispensed = latestFeed.field4 === '1' ? 'Water Dispensed' : '';

    notificationsElement.innerHTML = `
      ${foodDispensed ? `<li>${foodDispensed}</li>` : ''}
      ${waterDispensed ? `<li>${waterDispensed}</li>` : ''}
    `;
  } catch (error) {
    console.error(error);
    foodLevelElement.textContent = 'Error loading data';
    moistureLevelElement.textContent = 'Error loading data';
    notificationsElement.innerHTML = '<li>Error fetching notifications</li>';
  }
}

// Check for Empty Food Container
function checkFoodStatus() {
  const foodLevel = parseFloat(foodLevelElement.textContent);
  if (foodLevel === 0) {
    foodStatusElement.textContent = 'Food container is empty!';
  } else {
    foodStatusElement.textContent = 'Food container has enough food.';
  }
}

// Initialize Fetch and Set Interval
fetchData();
setInterval(fetchData, 15000);

// Add Event Listener to Check Food Button
checkFoodButton.addEventListener('click', checkFoodStatus);
