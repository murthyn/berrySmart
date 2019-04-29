
        function tempGradient(l,h){
            var el = document.getElementById('legend-parent');
            el.innerHTML = '<span id="legend"> </span>';
            var first = document.createElement('span');
            first.innerText = l.toString(10);
            el.prepend(first);

            var legend = document.getElementById('legend');
            var canvas = document.createElement('canvas');
            canvas.setAttribute('width','200');
            canvas.setAttribute('height','20');

            legend.appendChild(canvas);

            var ctx = canvas.getContext("2d");
            var grd = ctx.createLinearGradient(0, 0, 200, 0);
            grd.addColorStop(0, "yellow");
            grd.addColorStop(1, "red");

            ctx.fillStyle = grd;
            ctx.fillRect(0, 0, 200, 20);

            var last = document.createElement('span');
            last.innerText = h.toString(10);
            el.appendChild(last);
        }

        function humidGradient(l,h){
            var el = document.getElementById('legend-parent');
            el.innerHTML = '<span id="legend"> </span>';
            var first = document.createElement('span');
            first.innerText = l.toString(10);
            el.prepend(first);

            var legend = document.getElementById('legend');
            var canvas = document.createElement('canvas');
            canvas.setAttribute('width','200');
            canvas.setAttribute('height','20');

            legend.appendChild(canvas);

            var ctx = canvas.getContext("2d");
            var grd = ctx.createLinearGradient(0, 0, 200, 0);
            grd.addColorStop(0, "rgb(255,255,255");
            grd.addColorStop(1, "rgb(0,128,255)");

            ctx.fillStyle = grd;
            ctx.fillRect(0, 0, 200, 20);

            var last = document.createElement('span');
            last.innerText = h.toString(10);
            el.appendChild(last);
        }

        function lightGradient(l,h){
            var el = document.getElementById('legend-parent');
            el.innerHTML = '<span id="legend"> </span>';
            var first = document.createElement('span');
            first.innerText = l.toString(10);
            el.prepend(first);

            var legend = document.getElementById('legend');
            var canvas = document.createElement('canvas');
            canvas.setAttribute('width','200');
            canvas.setAttribute('height','20');

            legend.appendChild(canvas);

            var ctx = canvas.getContext("2d");
            var grd = ctx.createLinearGradient(0, 0, 200, 0);
            grd.addColorStop(0, "black");
            grd.addColorStop(1, "white");

            ctx.fillStyle = grd;
            ctx.fillRect(0, 0, 200, 20);

            var last = document.createElement('span');
            last.innerText = h.toString(10);
            el.appendChild(last);
        }
    
    function drawTableTemperature() {
        var x = 5;
        var y = 5;
        var t = '<table cellspacing="0" border="1" cellpadding="10" class="grxd">';
        for (var i = 1; i <= (x * y); i++) {
            t += (i == 1 ? '<tr>' : '');
            t += '<td style="cursor:pointer; border-color:rgb(247, 202, 54);" id=cell';   
            t += i.toString(10);
            t += '></td>';
            if (i == (x * y)) {
                t += '</tr>';
            } else {
            t += (i % y === 0 ? '</tr><tr>' : '');
            }
        }
    t += '</table>';
    $("#drawTable").html(t);
    tempGradient(0.2, 0.8);
    for (var i=1; i <= (x*y); i++){
      var v = i/(x*y);   
      drawTempSquare(i, v, 0.2, 0.8);
    }
    }   
   
        function displayfntemp(el){
            el.innerHTML= '<span style="color:black;">'+el.id+'</span>' ; 
        }

        function displayfnhumid(el){
            el.innerHTML= '<span style="color:darkslateblue;">'+el.id+'</span>' ; 
        }

        function displayfnlight(el){
            el.innerHTML= '<span style="color:aqua;">'+el.id+'</span>' ; 
        }


        function drawTempSquare(i,v, thl, thh){
            var element = document.getElementById("cell"+i.toString(10));
            var el = document.createElement('div');
            var square = document.createElement("div");
            square.style.width = "40px";
            square.style.height = "40px";
            square.style.borderRadius="10px";
            var r = Math.floor(255);
            var b = 51;
            var g = Math.floor(51+(1-v)*204);
            square.setAttribute('id',v.toFixed(2).toString(10));
            square.style.backgroundColor =  "rgb("+ r.toString(10)+","+g.toString(10)+","+b.toString(10)+")";
            square.setAttribute('onclick', 'displayfntemp(this)');
            element.appendChild(el);
            el.appendChild(square);
            if (v <= thl || v >= thh){
              var newel = document.createElement('img');
              newel.setAttribute('src', '../images/sadsmiley.png');
              element.appendChild(newel);
            }
        }
    
    function drawTableHumidity() {
        var x = 5;
        var y = 5;
        var t = '<table cellspacing="0" border="1" cellpadding="10" class="grxd">';
        for (var i = 1; i <= (x * y); i++) {
            t += (i == 1 ? '<tr>' : '');
            t += '<td style="cursor:pointer; border-color:skyblue;" id=cell';   
            t += i.toString(10);
            t += '></td>';
            if (i == (x * y)) {
                t += '</tr>';
            } else {
            t += (i % y === 0 ? '</tr><tr>' : '');
            }
        }
    t += '</table>';
    $("#drawTable").html(t);
    humidGradient(0.2, 0.8);
    for (var i=1; i <= (x*y); i++){
      var v = i/(x*y);   
      drawHumidSquare(i, v, 0.2, 0.8);
    }
    }   
    
        function drawHumidSquare(i,v, thl, thh){
            var element = document.getElementById("cell"+i.toString(10));
            var square = document.createElement("div");
            var el = document.createElement('div');
            square.style.width = "40px";
            square.style.height = "40px";
            square.style.borderRadius="10px";
            var r = Math.floor((1-v)*255);
            var g = Math.floor(128+(1-v)*127);
            var b = 255;
            square.setAttribute('id', v.toFixed(2).toString(10));
            square.style.backgroundColor =  "rgb("+ r.toString(10)+","+g.toString(10)+","+b.toString(10)+")";
            square.setAttribute('onclick', 'displayfnhumid(this)');
            element.appendChild(el);
            el.appendChild(square);
            if (v <= thl || v >= thh){
              var newel = document.createElement('img');
              newel.setAttribute('src', '../images/sadsmiley.png');
              element.appendChild(newel);
            }
        }
    
    function drawTableLight() {
        var x = 5;
        var y = 5;
        var t = '<table cellspacing="0" border="1" cellpadding="10" class="grxd">';
        for (var i = 1; i <= (x * y); i++) {
            t += (i == 1 ? '<tr>' : '');
            t += '<td style="cursor:pointer; border-color:white;" id=cell';   
            t += i.toString(10);
            t += '></td>';
            if (i == (x * y)) {
                t += '</tr>';
            } else {
            t += (i % y === 0 ? '</tr><tr>' : '');
            }
        }
    t += '</table>';
    $("#drawTable").html(t);
    lightGradient(0.3, 0.7);
    for (var i=1; i <= (x*y); i++){
      var v = i/(x*y);   
      drawLightSquare(i, v, 0.3, 0.7);
    }
    }   
    
        function drawLightSquare(i,v, thl, thh){
            var element = document.getElementById("cell"+i.toString(10));
            var el = document.createElement('div');
            var square = document.createElement("div");
            square.style.width = "40px";
            square.style.height = "40px";
            square.style.borderRadius="10px";
            var j = Math.floor(v*255);
            square.setAttribute('id',v.toFixed(2).toString(10));
            square.style.backgroundColor =  "rgb("+ j.toString(10)+","+j.toString(10)+","+j.toString(10)+")";
            square.setAttribute('onclick', 'displayfnlight(this)');
            element.appendChild(el);
            el.appendChild(square);
            if (v <= thl || v >= thh){
              var newel = document.createElement('img');
              newel.setAttribute('src', '../images/sadsmiley.png');
              element.appendChild(newel);
            }
        }
    