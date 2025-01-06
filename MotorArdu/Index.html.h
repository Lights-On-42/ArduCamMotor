const char* htmlPage = R"rawliteral(
  
  <script>
  function buttonClick()
  {
    fetch('/buttonstopstream')
    .then(response =>  response.text())
  }

  function test() 
  {
    ws.send("M03 x2000 y2000");
  }
  function stop()
  {
    ws.send("stop");
  }
  function PowerDown()
  {
    ws.send("PowerDown");
  }
  function PowerON()
  {
    ws.send("PowerON");
  }
  function connect() 
  {
    let host = window.location.host;
    
    ws = new WebSocket("ws://" + host + ":81")
    ws.onopen = (event) => {document.getElementById("ConnectButton").textContent="Wir sind connectet"; ws.send("hi");};
    ws.onclose = (event) => {document.getElementById("ConnectButton").textContent="Re connected";};

    ws.onmessage = (event) => 
    {
      const parts = event.data.split(/:(.+)/) 
      console.log(parts);
      //receive(parts[0], parts[1]);
    }
  }

  window.onload=function()
  {
    let host = window.location.host;
    ws = new WebSocket("ws://" + host + ":81")
    ws.onopen = (event) => {document.getElementById("ConnectButton").textContent="Wir sind connectet"; ws.send("hi");};
    ws.onclose = (event) => {document.getElementById("ConnectButton").textContent="re connected";};
  }
  </script>
  
  <html>

  <body>
    <h1>ESP Camera Roboter</h1>

    

    <img style="width:600px;margin:50px;" src='/stream'/> <br />

    <div class="row">
      <div class="columnLateral">
          <div id="joy1Div" style="width:600px;height:600px;margin:50px;border:solid"></div>
          Position X:<input id="joy1PositionX" type="text" /><br />
          Position Y:<input id="joy1PositionY" type="text" /><br />
          X :<input id="joy1X" type="text" /><br />
          Y :<input id="joy1Y" type="text" /><br />
          Speed left: <input id="speedleft" type="text" /><br />
			    Speed right:<input id="speedright" type="text" /><br />
			    Quadrant:<input id="qua" type="text" /><br />
        <br />
        <button onclick = "buttonClick()">Stop Stream</button>
        <button onclick = ='/stream'/>Start Stream</button>
        <button id="ConnectButton" onclick = "connect()">connect</button>
        
        <br />
        <br />

        <button  onclick = "test()">Test Motor</button>
        <button  onclick = "stop()">Stop Motor</button>
        <button  onclick = "PowerDown()">PowerDown</button>
        <button  onclick = "PowerON()">PowerON</button>
        <br />
        <br />
        <!--  <button  onclick = "test()">Test Motor down</button>-->
        <br />
        <!--  <button  onclick = "test()">Test Motor Up</button>-->
        <br />
        <br />
      </div>	
    </div>


  <script type="text/javascript">

    var joy1InputPosX = document.getElementById("joy1PositionX"); 
    var joy1InputPosY = document.getElementById("joy1PositionY");

    var oltX=0;
    var oltY=0;

    var Joy1 = new JoyStick('joy1Div', {}, function(stickData) 
    {
      
      if( stickData.x==0&&
        stickData.y==0&&
          oltX!=0&&
          oltY!=0)
    {
      document.getElementById("speedleft").value= "stopp";
      document.getElementById("speedright").value= "stopp";
      console.log("stop from Joystick");
      ws.send("stop joy");
    }
    else if( stickData.x==0&&
        stickData.y==0&&
          oltX==0&&
          oltY==0)
    {
      //console.log("nix zu tun");
    }
    else
    {
      //darstellen und speichern
      joy1X.value = stickData.x;
      joy1Y.value = stickData.y;


      //robert
      var rightspeed=parseFloat(stickData.y);
      var leftspeed=parseFloat(stickData.y);

      var c = parseFloat(stickData.y)* (parseFloat(stickData.x)/100);

      if(stickData.y>=0&&stickData.x>=0)
      {
        quadrant=1;
        rightspeed=rightspeed-c;
      } 
      if(stickData.y<0&&stickData.x>=0)
      {
        quadrant=2;
        rightspeed=rightspeed-c;
      }
      if(stickData.y<0&&stickData.x<=0)
      {
        quadrant=3;
        leftspeed=leftspeed+c;
      }
      if(stickData.y>=0&&stickData.x<0)
      {
        quadrant=4;
        leftspeed=leftspeed+c;
      }

      if(leftspeed!=0)
      {
        if(leftspeed>0)
        {
          leftspeed = 1000 - (leftspeed * 8);
        }
        else
        {
          leftspeed = -(1000 + (leftspeed * 8));      
        }
      }  

      if(rightspeed!=0)
      {
        if(rightspeed>0)
        {
          rightspeed = 1000 - (rightspeed * 8);
        }
        else
        {
          rightspeed = -(1000 + (rightspeed * 8));  
        } 
      }
      joy1InputPosX.value=c;
      document.getElementById("speedright").value= parseInt(rightspeed);
      document.getElementById("speedleft").value= parseInt(leftspeed);

      ws.send("M03 y"+parseInt(leftspeed)+" a"+parseInt(leftspeed)+" z"+parseInt(rightspeed)+" x"+parseInt(rightspeed));
    }
    


    joy1InputPosX.value = stickData.x;
    joy1InputPosY.value = stickData.y;

    //darstellen und speichern
    oltX = stickData.x;
    oltY = stickData.y;

    });

	</script>
  </body></html>

)rawliteral";