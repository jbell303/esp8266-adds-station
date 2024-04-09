#pragma once

// load html, css and javascript to progmem
static const char html_1[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
  <head>
  <meta name='viewport' content='width=device-width, initial-scale=1.0'/>
  <meta charset='utf-8'>

<script>
  // set the button text to a pending status
  // read the station id from the form and use it in our request URL
  function fetchWeather() {
    document.getElementById("submit_button").value = "Fetching weather...";
    station_id = document.getElementById("identifier").value;
    ajaxLoad(station_id);
  }

  // initialize ajax request object
  var ajaxRequest = null;
  if (window.XMLHttpRequest) { ajaxRequest = new XMLHttpRequest(); }
                           { ajaxRequest = new ActiveXObject("Microsoft.XMLHTTP"); }

  // send a POST request to the server with station ID
  // parse the response and display the values in HTML
  function ajaxLoad(station_id) {
    if (!ajaxRequest) {
      alert("AJAX is not supported.");
      return;
    }

    ajaxRequest.open("POST", "station_id/", true);
    ajaxRequest.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    ajaxRequest.send("station_id=" + station_id);
    ajaxRequest.onreadystatechange = function () {
      if (ajaxRequest.readyState == 4 && ajaxRequest.status == 200) {
        var response = JSON.parse(ajaxRequest.responseText);
        document.getElementById("identifier").value = response.identifier;
        document.getElementById("flight_category").innerHTML = response.flight_category;
        document.getElementById("ceiling").innerHTML = response.ceiling;
        document.getElementById("visibility").innerHTML = response.visibility;
        document.getElementById("wind_speed").innerHTML = response.wind_speed;
        document.getElementById("submit_button").value = "Fetch Weather";
        document.getElementById("toggle_button").value = (response.isDrinkingWeather ? "Set Flying Weather" : "Set Drinking Weather");
        document.getElementById("toggle_button").style.backgroundColor = (response.isDrinkingWeather ? "#4CAF50" : "#FF0000");
      }
    }
    ajaxRequest.send();
  }

  // manually toggle flying weather and drinking weather lights
  // change the button text based on the desired state
  // send the LED toggle request to the server
  function toggleLed () {
    var button_text = document.getElementById("toggle_button").value;
    if (button_text == "Set Drinking Weather") {
      document.getElementById("toggle_button").value = "Setting Drinking Weather";
      document.getElementById("toggle_button").style.backgroundColor = "#4CAF50";
      ajaxLED('DRINKING');
    } else {
      document.getElementById("toggle_button").value = "Setting Flying Weather";
      document.getElementById("toggle_button").style.backgroundColor = "#FF0000";
      ajaxLED('FLYING');
    }
  }

  function ajaxLED(ajaxURL) {
    if (!ajaxRequest) {
      alert("AJAX is not supported.");
      return;
    }

    ajaxRequest.open("GET", ajaxURL, true);
    ajaxRequest.onreadystatechange = function () {
      if (ajaxRequest.readyState == 4 && ajaxRequest.status == 200) {
        var ajaxResult = ajaxRequest.responseText;
        if (ajaxResult == "Flying Weather") {
          document.getElementById("toggle_button").value = "Set Drinking Weather";
          document.getElementById("toggle_button").style.backgroundColor = "#FF0000";
        } else if (ajaxResult == "Drinking Weather") {
          document.getElementById("toggle_button").value = "Set Flying Weather";
          document.getElementById("toggle_button").style.backgroundColor = "#4CAF50";
        }
      }
    }
    ajaxRequest.send();
  }

  // request the local IP address from the server
  function getIpAddress() {
    if (!ajaxRequest) {
      alert("AJAX is not supported.");
      return;
    }

    // display the local IP address in HTML
    ajaxRequest.open("GET", "ip_address", true);
    ajaxRequest.onreadystatechange = function () {
      if (ajaxRequest.readyState == 4 && ajaxRequest.status == 200) {
        var ajaxResult = ajaxRequest.responseText;
        console.log(ajaxResult);
        document.getElementById("ip_address").innerHTML = ajaxResult;
      }
    }
    ajaxRequest.send();
  }
</script>

  <title>Saints Weather Station</title>
</head>

 <style type="text/css">
    #main {display: table; 
    margin: auto; 
    padding: 0 10px 0 10px; 
    }
    h2 {text-align:center; }
    body {
   font-family: "Helvetica Neue", Helvetica, Arial, sans-serif;
   font-size:140%;
}

.label {
  text-align: right;
}

.data {
  padding-left: 10px;
}
    
input[type=text], select {
   width: 100%;
   padding: 12px 20px;
   margin: 8px 0;
   display: inline-block;
   border: 1px solid #ccc;
   border-radius: 4px;
   box-sizing: border-box;
}

input[type=button] {
  -webkit-appearance: none;
  width: 100%;
  color: white;
  padding: 10px 10px;
  margin: 8px 0;
  border: none;
  border-radius: 4px;
  cursor: pointer;
}

input[type=button] {
  -webkit-appearance: none;
  background-color: #45a049;
}

div {
  border-radius: 5px;
  background-color: #f2f2f2;
  padding: 20px;
}
    
  </style>
</head>
<body onload="getIpAddress()">
  <div id='main'>
    
    <h2>Saints Weather Station</h2>
    <table>
      <tr>
        <td class="label">Flight Category: </td>
        <td class="data" id="flight_category"></td>
      </tr>
      <tr>
        <td class="label">Ceiling: </td>
        <td class="data" id="ceiling"></td>
      </tr>
      <tr>
        <td class="label">Visibility: </td>
        <td class="data" id="visibility"></td>
      <tr>
        <td class="label">Wind Speed: </td>
        <td class="data" id="wind_speed"></td>
      </tr>   
    </table>
      <div>
        <form class="form "action="javascript:fetchWeather()">
          <label for="name">Identifier:</label>
          <input type="text" id="identifier" name="station_id">
          <input type="button" style="background-color:#4169E1" id="submit_button" onclick="fetchWeather()" value="Fetch Weather" />
        </form>
      </div>
     <input class="button" type="button" id="toggle_button" style="background-color:#4CAF50;" onclick="toggleLed()" value="Set Flying Weather"/>
      </div>
      <p style="padding-top:15px;text-align:center">
      <a href="/_ac">
        <img src="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABgAAAAYCAYAAADgdz34AAAC2klEQVRIS61VvWsUQRSfmU2pon9BUIkQUaKFaCBKgooSb2d3NSSFKbQR/KrEIiIKBiGF2CgRxEpjQNHs7mwOUcghwUQ7g58IsbGxEBWsb2f8zR177s3t3S2cA8ftzPu993vzvoaSnMu2vRKlaqgKp74Q/tE8qjQPyHGcrUrRjwlWShmDbFMURd/a6TcQwNiYUmpFCPElUebcuQ2vz6aNATMVReHEPwzfSSntDcNwNo2rI+DcvQzhpAbA40VKyV0p1Q9snzBG1qYVcYufXV1sREraDcxpyHdXgkfpRBj6Uwm2RsC5dxxmZ9pdOY9cKTISRcHTCmGiUCh4fYyplTwG2mAUbtMTBMHXOgK9QfyXEZr+TkgQ1oUwDA40hEgfIAfj+HuQRaBzAs9eKyUZ5Htx+T3ZODKG8DzOJMANhmGomJVMXPll+hx9UUAlzZrJJ4QNCDG3VEfguu7mcpmcB/gkBOtShhQhchAlu5jlLUgc9ENgyP5gf9+y6LTv+58p5zySkgwzLNOIGc8sEoT1Lc53NMlbCQQuvMxeCME1NNPVVkmH/i3IzzXDtCSA0qQQwZWOCJDY50jsQRjJmkslEOxvTcDRO6zPxOh5xZglKkYLhWM9jMVnkIsTyMT6NBj7IbOCEjm6HxNVVTo2WXqEWJZ1T8rytB6GxizyDkPhWVpBqfiXUtbo/HywYJSpA9kMamNNPZ71R9Hcm+TMHHZNGw3EuraXEUldbfvw25UdOjqOt+JhMwJd7+jSTpZaEiIcaCDwPK83jtWnTkwnunFMtxeL/ge9r4XItt1RNNaj/0GAcV2bR3U5sG3nEh6M61US+Qrfd9Bs31GGulI2GOS/8dgcQZV1w+ApjIxB7TDwF9GcNzJzoA+rD0/8HvPnXQJCt2qFCwbBTfRI7UyXumWVt+HJ9NO4XI++bdsb0YyrqXmlh+AWOLHaLqS5CLQR5EggR3YlcVS9gKeH2hnX8r8Kmi1CAsl36QAAAABJRU5ErkJggg==" border="0" title="AutoConnect menu" alt="AutoConnect menu"/>
      </a>
  </p>
<p id="ip_address">192.168.x.x</p>
</html>
)=====";

