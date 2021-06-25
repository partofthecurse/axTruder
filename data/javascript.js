var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

var lastTemp = 0;
var lastTempSP = 0;
var lastDiameter = 0;
var lastDiameterSP = 0;

function initWebSocket() {
  console.log('Trying to open a WebSocket connection...');
  websocket = new WebSocket(gateway);
  websocket.onopen = onOpen;
  websocket.onclose = onClose;
  websocket.onmessage = onMessage; // <-- add this line
}

function onOpen(event) {
  console.log('Connection opened');
}

function onClose(event) {
  console.log('Connection closed');
  setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
  //Websocket Message receiving and processing for tasks
  console.log('Message Received on Websocket: ');
  //Parsing JSOn Strings
  var Obj = JSON.parse(event.data, function (key, value) {
    console.log('Raw Message');
    console.log(event.data);
    console.log('Processed Message');
    console.log('key: ');
    console.log(key);
    console.log('value: ');

    if (key == "nozzleTemp") {

      console.log(value);
      document.getElementById('nozzle-temperature').innerHTML = value;
      lastTemp = value;
      plottTempChart();
    }
    if (key == "nozzleTempSP") {
      console.log(value);
      document.getElementsByName('tempInput')[0].placeholder = value;
      document.getElementsByName('tempInput')[0].value = value;
      document.getElementById('nozzle-temperature-target').innerHTML = value;
      lastTempSP = value;
      plottTempChart();
    }
    if (key == "nozzleTempOffsetSP") {
      console.log(value);
      document.getElementsByName('tempOffsetInput')[0].placeholder = value;
      document.getElementsByName('tempOffsetInput')[0].value = value;
    }
    if (key == "diameter") {
      console.log(value);
      document.getElementById('diameter').innerHTML = value;
      lastDiameter = value;
      plottDiameterChart();
    }
    if (key == "diameterSP") {
      console.log(value);
      document.getElementsByName('diameterInput')[0].placeholder = value;
      document.getElementsByName('diameterInput')[0].value = value;
      document.getElementById('diameter-target').innerHTML = value;
      lastDiameterSP = value;
      plottDiameterChart();
    }
    if (key == "diameterOffsetSP") {
      console.log(value);
      document.getElementsByName('diameterOffsetInput')[0].placeholder = value;
      document.getElementsByName('diameterOffsetInput')[0].value = value;
    }
    if (key == "axTruderState") {
      console.log(value);
      document.getElementById('axTruderState').innerHTML = value;
      if (value === "Heating") {
        document.getElementById('preheat-button').disabled = true;
        document.getElementById('cooldown-button').disabled = false;
        document.getElementById('auger-forward-button').innerHTML = "Forward";
        document.getElementById('auger-reverse-button').innerHTML = "Reverse";
      }
      if (value === "Cooling") {
        document.getElementById('preheat-button').disabled = false;
        document.getElementById('cooldown-button').disabled = true;
        document.getElementById('auger-forward-button').innerHTML = "Forward";
        document.getElementById('auger-reverse-button').innerHTML = "Reverse";
        document.getElementById('auger-forward-button').disabled = true;
        document.getElementById('auger-reverse-button').disabled = true;
        document.getElementById('auger-extrude-button').disabled = true;
      }
      if (value === "Hot") {
        document.getElementById('auger-forward-button').disabled = false;
        document.getElementById('auger-forward-button').innerHTML = "Forward";
        document.getElementById('auger-reverse-button').disabled = false;
        document.getElementById('auger-reverse-button').innerHTML = "Reverse";
        document.getElementById('auger-forward-button').removeEventListener('click', augerStop);
        document.getElementById('auger-forward-button').addEventListener('click', augerForward);
        document.getElementById('auger-reverse-button').removeEventListener('click', augerStop);
        document.getElementById('auger-reverse-button').addEventListener('click', augerReverse);
      }
      if (value === "Ready") {
        document.getElementById('auger-extrude-button').disabled = false;
        document.getElementById('auger-forward-button').innerHTML = "Forward";
        document.getElementById('auger-forward-button').disabled = false;
        document.getElementById('auger-reverse-button').disabled = false;
        document.getElementById('auger-reverse-button').innerHTML = "Reverse";
        document.getElementById('auger-forward-button').removeEventListener('click', augerStop);
        document.getElementById('auger-forward-button').addEventListener('click', augerForward);
        document.getElementById('auger-reverse-button').removeEventListener('click', augerStop);
        document.getElementById('auger-reverse-button').addEventListener('click', augerReverse);
      }
      if (value === "Extruding") {
        document.getElementById('auger-extrude-button').disabled = true;
        document.getElementById('auger-forward-button').innerHTML = "Stop";
        document.getElementById('auger-forward-button').disabled = false;
        document.getElementById('auger-forward-button').removeEventListener('click', augerForward);
        document.getElementById('auger-forward-button').addEventListener('click', augerStop);
        document.getElementById('auger-reverse-button').innerHTML = "Stop";
        document.getElementById('auger-reverse-button').disabled = false;
        document.getElementById('auger-reverse-button').removeEventListener('click', augerReverse);
        document.getElementById('auger-reverse-button').addEventListener('click', augerStop);
      }
      if (value === "Error") {
        document.getElementById('auger-forward-button').innerHTML = "Forward";
        document.getElementById('auger-reverse-button').innerHTML = "Reverse";
        document.getElementById('auger-forward-button').disabled = false;
        document.getElementById('auger-reverse-button').disabled = false;
        document.getElementById('auger-forward-button').removeEventListener('click', augerStop);
        document.getElementById('auger-forward-button').addEventListener('click', augerForward);
        document.getElementById('auger-reverse-button').removeEventListener('click', augerStop);
        document.getElementById('auger-reverse-button').addEventListener('click', augerReverse);
      }
      //add more states later
    }
    if (key == "augerSpeedSP") {
      console.log(value);
      document.getElementById('auger-speed-slider').value = value;
    }
    if (key == "fanSpeedSP") {
      console.log(value);
      document.getElementById('fan-speed-slider').value = value;
    }
    if (key == "fanOn") {
      console.log(value);
      document.getElementById('fan-speed-slider').value = value;
      document.getElementById('fan-button').innerHTML = "Stop";
    }
    if (key == "fanOff") {
      console.log(value);
      document.getElementById('fan-speed-slider').value = value;
      document.getElementById('fan-button').innerHTML = "Start";
    }
  });
}

window.addEventListener('load', onLoad);

function onLoad(event) {
  console.log('function: onLoad');
  initWebSocket();
  initButtons();
  initPresets();
  initTab();
  initAcordion();
  document.getElementById('auger-forward-button').disabled = true;
  document.getElementById('auger-reverse-button').disabled = true;
  document.getElementById('auger-extrude-button').disabled = true;
}

function initPresets() {
  document.getElementById('material-dropdown').addEventListener('change', setMaterialPreset);
  document.getElementById('diameter-dropdown').addEventListener('change', setDiameterPreset);
  document.getElementById('manual-dropdown').addEventListener('click', setManualTime);
}

function initTab() {
  var tab = document.getElementById('tablist');
  tab.addEventListener('click', clickHandler);
}

function initButtons() {
  console.log('function: initButtons');
  document.getElementById('decrease-temp-button').addEventListener('click', decreaseTemp);
  document.getElementById('increase-temp-button').addEventListener('click', increaseTemp);
  document.getElementById('decrease-diameter-button').addEventListener('click', decreaseDiameter);
  document.getElementById('increase-diameter-button').addEventListener('click', increaseDiameter);

  document.getElementById('decrease-tempOffset-button').addEventListener('click', decreaseTempOffset);
  document.getElementById('increase-tempOffset-button').addEventListener('click', increaseTempOffset);
  document.getElementById('decrease-diameterOffset-button').addEventListener('click', decreaseDiameterOffset);
  document.getElementById('increase-diameterOffset-button').addEventListener('click', increaseDiameterOffset);

  document.getElementById('submit-temp-button').addEventListener('click', submitTemp);
  document.getElementById('submit-tempOffset-button').addEventListener('click', submitTempOffset);
  document.getElementById('submit-diameter-button').addEventListener('click', submitDiameter);
  document.getElementById('submit-diameterOffset-button').addEventListener('click', submitDiameterOffset);

  document.getElementById('preheat-button').addEventListener('click', preheat);
  document.getElementById('cooldown-button').addEventListener('click', cooldown);
  document.getElementById('auger-forward-button').addEventListener('click', augerForward);
  document.getElementById('auger-reverse-button').addEventListener('click', augerReverse);
  document.getElementById('auger-extrude-button').addEventListener('click', augerExtrude);
  document.getElementById('auger-stop-button').addEventListener('click', augerStop);
}

function clickHandler(elem) {
  var target = elem.target;

  var selectedTab = document.querySelector('[aria-selected="true"]');
  selectedTab.setAttribute('aria-selected', false);
  target.setAttribute('aria-selected', true);

  var panels = document.querySelector('[aria-hidden="false"]');
  panels.setAttribute('aria-hidden', true);

  var panelId = target.getAttribute('aria-controls'),
    panel = document.getElementById(panelId);
  panel.setAttribute('aria-hidden', false);
}

function initAcordion() {
  var acc = document.getElementsByClassName("accordion");
  var i;

  for (i = 0; i < acc.length; i++) {
    acc[i].addEventListener("click", function () {
      /* Toggle between adding and removing the "active" class,
      to highlight the button that controls the panel */
      this.classList.toggle("active");

      /* Toggle between hiding and showing the active panel */
      var panel = this.nextElementSibling;
      if (panel.style.display === "block") {
        panel.style.display = "none";
      } else {
        panel.style.display = "block";
      }
    });    
  }
}

//Sending Functions
// Small Buttons on User Interace to increase or decrease the Temperature Setpoint
function decreaseTemp() {
  var value = "0";
  var task = "decrease";
  var action = "nozzleTemp";
  notifyESP(action, task, value);
  console.log('function: decreaseTemp');
}
function increaseTemp() {
  var value = "0";
  var task = "increase";
  var action = "nozzleTemp";
  notifyESP(action, task, value);
  console.log('function: increaseTemp');
}
// Small Buttons on User Interace to increase or decrease the Temperature Offset Setpoint
function decreaseTempOffset() {
  var value = "0";
  var task = "decreaseOffset";
  var action = "nozzleTemp";
  notifyESP(action, task, value);
  console.log('function: decreaseTempOffset');
}
function increaseTempOffset() {
  var value = "0";
  var task = "increaseOffset";
  var action = "nozzleTemp";
  notifyESP(action, task, value);
  console.log('function: increaseTempOffset');
}

// Small Buttons on User Interace to increase or decrease the Diameter Setpoint
function decreaseDiameter() {
  var value = "0";
  var task = "decrease";
  var action = "diameter";
  notifyESP(action, task, value);
  console.log('function: decreaseDiameter');
}
function increaseDiameter() {
  var value = "0";
  var task = "increase";
  var action = "diameter";
  notifyESP(action, task, value);
  console.log('function: increaseDiameter');
}
// Small Buttons on User Interace to increase or decrease the Diameter Setpoint
function decreaseDiameterOffset() {
  var value = "0";
  var task = "decreaseOffset";
  var action = "diameter";
  notifyESP(action, task, value);
  console.log('function: decreaseDiameterOffset');
}
function increaseDiameterOffset() {
  var value = "0";
  var task = "increaseOffset";
  var action = "diameter";
  notifyESP(action, task, value);
  console.log('function: increaseDiameterOffset');
}

//Submit Buttons  
function submitTemp() {
  console.log('function: submitTemp');
  var value = document.getElementsByName("tempInput")[0].value;
  var task = "setValue";
  var action = "nozzleTemp";
  notifyESP(action, task, value);
}
function submitTempOffset() {
  console.log('function: submitTempOffset');
  var value = document.getElementsByName("tempOffsetInput")[0].value;
  var task = "setOffset";
  var action = "nozzleTemp";
  notifyESP(action, task, value);
}
function submitDiameter() {
  console.log('function: submitDiameter');
  var value = document.getElementsByName("diameterInput")[0].value;
  var task = "setValue";
  var action = "diameter";
  notifyESP(action, task, value);
}
function submitDiameterOffset() {
  console.log('function: submitDiameterOffset');
  var value = document.getElementsByName("diameterOffsetInput")[0].value;
  var task = "setOffset";
  var action = "diameter";
  notifyESP(action, task, value);
}

//DropDown Preset Functions
function setMaterialPreset() {
  var e = document.getElementById('material-dropdown');
  var task = e.options[e.selectedIndex].value;
  console.log('function: setMaterialPreset - sending: ');
  var value = "0";
  var action = "nozzleTemp";
  notifyESP(action, task, value);
}
function setDiameterPreset() {
  var e = document.getElementById('diameter-dropdown');
  var task = e.options[e.selectedIndex].value;
  console.log('function: setDiameterPreset - sending: ');
  var value = "0";
  var action = "diameter";
  notifyESP(action, task, value);
}
function setManualTime() {
  var e = document.getElementById('manual-dropdown');
  var value = e.options[e.selectedIndex].value;
  console.log('function: setManualPreset - sending: ');
  var value = "setTimeFrame";
  var action = "auger";
  notifyESP(action, task, value);
}

//PreHeat button (Left Navigation)
function preheat() {
  var value = "0";
  var task = "preheat";
  var action = "auger";
  notifyESP(action, task, value);
  console.log('function: preheat');
}

function cooldown() {
  var value = "0";
  var task = "cooldown";
  var action = "auger";
  notifyESP(action, task, value);
  console.log('function: cooldown');
}

function augerForward() {
  var value = "0";
  var task = "forward";
  var action = "auger";
  notifyESP(action, task, value);
  console.log('function: augerForward');
}

function augerReverse() {
  var value = "0";
  var task = "reverse";
  var action = "auger";
  notifyESP(action, task, value);
  console.log('function: augerReverse');
}

function augerExtrude() {
  var value = "0";
  var task = "extrude";
  var action = "auger";
  notifyESP(action, task, value);
  console.log('function: augerExtrude');
}

function augerStop() {
  var value = "0";
  var task = "stop";
  var action = "auger";
  notifyESP(action, task, value);
  console.log('function: augerStop');
}

function sendAugerSpeed() {
  var value = document.getElementById('auger-speed-slider').value;
  var action = "auger";
  var task = "setSpeed";
  notifyESP(action, task, value);
  console.log('function: sendAugerSpeed');
}

function fanStop() {
  var value = "0";
  var task = "stop";
  var action = "fan";
  notifyESP(action, task, value);
  console.log('function: fanStop');
}

function fanStart() {
  var value = "0";
  var task = "start";
  var action = "fan";
  notifyESP(action, task, value);
  console.log('function: fanStart');
}

function sendFanSpeed() {
  var value = document.getElementById('fan-speed-slider').value;
  var action = "fan";
  var task = "setSpeed";
  notifyESP(action, task, value);
  console.log('function: sendFanSpeed');
}

//Send a JSON String
function notifyESP(action, task, value) {
  var payload = "{'action':'" + action + "','task':'" + task + "','value':'" + value + "'}"
  console.log('function: notifyESP: sending message: ');
  websocket.send(payload);
  console.log(payload);
}

//HighCharts
function plottTempChart() {
  var y = parseInt(lastTempSP);
  var z = parseInt(lastTemp);
  var x = (new Date()).getTime();
  if (chartT.series[0].data.length > 20) {
    chartT.series[0].addPoint([x, y], true, true, true);
    chartT.series[1].addPoint([x, z], true, true, true);//
  } else {
    chartT.series[0].addPoint([x, y], true, false, true);
    chartT.series[1].addPoint([x, z], true, false, true);//
  }
}

function plottDiameterChart() {
  var y = parseInt(lastDiameterSP);
  var z = parseInt(lastDiameter);
  var x = (new Date()).getTime();
  if (chartD.series[0].data.length > 20) {
    chartD.series[0].addPoint([x, y], true, true, true);
    chartD.series[1].addPoint([x, z], true, true, true);//
  } else {
    chartD.series[0].addPoint([x, y], true, false, true);
    chartD.series[1].addPoint([x, z], true, false, true);//
  }
}