var express = require('express')
  , util = require('util')
  , ejs = require('ejs')
  , io = require('socket.io')
;

var SerialPort = require('serialport2').SerialPort;
var port = new SerialPort();

var totaldata = '';
var humidity = 0;
port.on('error', function(err) {
  console.log(err);
});

port.open('/dev/ttyUSB0', {
  baudRate: 19200,
  dataBits: 8,
  parity: 'none',
  stopBits: 1
}, function(err) {
  port.write("hello world");
  port.close();
});

var app = express.createServer(
    express.logger()
  , express.static(__dirname + '/public')
  , express.bodyParser()
  , express.cookieParser()
  , express.session({secret: process.env.SESSION_SECRET || 'kdgfcbdgfsgftsrcgsgr'})
  , express.errorHandler()
);

var sio = io.listen(app);
sio.sockets.on('connection', function (socket) {
    console.log('A socket connected!');
});

port.on('data', function(data) {
    totaldata += data;
    // console.log(totaldata);
    if (totaldata.charAt(totaldata.length-1) == '\n') {
	try {
	    humidity = parseFloat(totaldata);
	    // console.log(humidity);
	    sio.sockets.emit('humidity', { value: humidity });
	    console.log(humidity);
	    totaldata = '';
	} catch(err) {
	    console.log(err);
	}
    }
});


app.get("/", function (req, res) {
    res.render('dash.ejs',
	       { layout: false
	       });
});

var port = process.env.PORT || 3000;

app.listen(port, function() {
  console.log("Listening on " + port);
});
