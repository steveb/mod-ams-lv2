function (event) {
  var icon = event.icon;
  if (icon.length) {
    var instance = icon[0].getAttribute("mod-instance");
    var modPedals = document.getElementsByClassName("mod-pedal");
    for (i = 0; i < modPedals.length; i++) {
      var modPedal = modPedals[i];
      if (modPedal.getAttribute("mod-instance") === instance) {
        if (event.type == 'change') {
          var ledLine = modPedal.getElementsByClassName("led-line")[0];
          if (event.symbol === "waveForm") {
            if (event.value === 2 || event.value === 3) {
              ledLine.style.backgroundPosition = "left center";
            } else {
              ledLine.style.backgroundPosition = "right center";
            }
          }
        }
      }
    }
  }
}
