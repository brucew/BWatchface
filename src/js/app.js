var myAPIKey = '08774fcf1fa9ddc82cf4008cbe477383'; 

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function locationSuccess(pos) {
  // Construct URL
  var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' +
      pos.coords.latitude + '&lon=' + pos.coords.longitude + '&units=imperial&appid=' + myAPIKey;

  // Send request for current conditions to OpenWeatherMap
  xhrRequest(url, 'GET', 
    function(responseText) {
      // responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);

      // Temperature
      var temperature = Math.round(json.main.temp);
      console.log('Temperature is ' + temperature);

      // Conditions
      var conditions = json.weather[0].main;      
      console.log('Conditions are ' + conditions);
      
      // Assemble dictionary using our keys
      var dictionary = {
        'TEMPERATURE': temperature,
        'CONDITIONS': conditions
      };
      
      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log('Current conditions sent to Pebble successfully!');
        },
        function(e) {
          console.log('Error sending current conditions to Pebble!');
        }
      );
      
    }      
  );

  // Construct URL
  url = 'http://api.openweathermap.org/data/2.5/forecast/daily?lat=' +
      pos.coords.latitude + '&lon=' + pos.coords.longitude + '&units=imperial&appid=' + myAPIKey;

  // Send request for forecast to OpenWeatherMap
  xhrRequest(url, 'GET', 
    function(responseText) {
      // responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);
      console.log(json.city.name);
      
      // High temperature
      var high = Math.round(json.list[0].temp.max);
      console.log('High will be ' + high);

      // Low temperature
      var low = Math.round(json.list[0].temp.min);
      console.log('Low will be ' + low);

      // Assemble dictionary using our keys
      var dictionary = {
        'HIGH': high,
        'LOW': low
      };
      
      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log('Forecast sent to Pebble successfully!');
        },
        function(e) {
          console.log('Error sending forecast to Pebble!');
        }
      );
      
    }      
  );

}

function locationError(err) {
  console.log('Error requesting location!');
}

function getWeather() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log('PebbleKit JS ready!');

    // Get the initial weather
    getWeather();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log('AppMessage received!');
    getWeather();
  }                     
);