document.addEventListener("DOMContentLoaded", function () {
  var activeTab = document.querySelector(".active-tab");
  var activeIndex = Array.prototype.indexOf.call(
    document.querySelectorAll(".tabs li"),
    activeTab
  );
  var contentLis = document.querySelectorAll(".tabs-content li");
  var tabsLis = document.querySelectorAll(".tabs li");

  contentLis[activeIndex].style.display = "block";

  window.submitTx = function (formId) {
    const form = document.getElementById(formId);
    let length = form.len ? form.len.value : -1;
    let url = `http://${location.host}/transmit?code=${form.code.value}&protocol=${form.protocol.value}&repeat=${form.repeat.value}&length=${length}`;
    fetch(url)
      .then((response) => {
        return response.json();
      })
      .then((data) => {
        alert(data.message);
        form.reset();
      })
      .catch((error) => {
        console.error("Error:", error);
      });
  };

  document.querySelector(".tabs").addEventListener("click", function (e) {
    if (e.target && e.target.matches("li")) {
      var current = e.target;
      var currentTab = current.getAttribute("data-tab");
      var index = Array.prototype.indexOf.call(tabsLis, current);

      tabsLis.forEach(function (tab) {
        tab.classList.remove("active-tab");
      });
      current.classList.add("active-tab");

      contentLis.forEach(function (content) {
        content.style.display = "none";
      });
      contentLis[index].style.display = "block";

      switch (currentTab) {
        case "settings": {
          const getSetting = async () => {
            const response = await fetch("/get-settings");
            const settings = await response.json();

            const select = document.getElementById("frequency");
            settings.frequencies.forEach((frequency) => {
              const option = document.createElement("option");
              option.value = frequency;
              option.textContent = frequency + " MHz";
              option.selected = frequency === settings.frequency;
              select.appendChild(option);
            });

            const radioButtons = document.getElementsByName("bandwidth");
            for (const radioButton of radioButtons) {
              if (radioButton.value == settings.bandwidth) {
                radioButton.checked = true;
                break;
              }
            }

            document
              .getElementById("ssid")
              .setAttribute("value", settings.ssid);
            document
              .getElementById("password")
              .setAttribute("value", settings.password);
          };
          getSetting();
          break;
        }
        case "transmit": {
          function populateSelect(formId) {
            const form = document.getElementById(formId);
            const element = form.protocol;
            for (var i = 1; i < 37; i++) {
              const option = document.createElement("option");
              option.value = i;
              option.textContent = i;
              element.appendChild(option);
            }
          }
          populateSelect("binary");
          populateSelect("decimal");
          break;
        }
      }
    }
  });
});
