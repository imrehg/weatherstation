var express = require('express')
  , util = require('util')
  , ejs = require('ejs')
  , io = require('socket.io')
  , mongoose = require('mongoose')
;

var SerialPort = require('serialport2').SerialPort;
var port = new SerialPort();

// Database related setup
var db = 'mongoose://'+process.env.MONGO_URL+'/'+process.env.MONGO_DB; 
mongoose.connect(db);
var Schema = mongoose.Schema
  , ObjectId = Schema.ObjectId;
var Reading = new Schema({
    date : { type: Date, default: Date.now }
  , humidity : Number
});
var ReadingModel =  mongoose.model('Reading', Reading);
// database setup end

var totaldata = '';
var humidity = 0;
port.on('error', function(err) {
  console.log(err);
});

port.open('/dev/ttyUSB0', {
  baudRate: 115200,
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

function handleReading(data) {
    var humidity = data.humidity;
    var reading = new ReadingModel(data);
    sio.sockets.emit('humidity', { value: humidity });
    reading.save()
}

port.on('data', function(data) {
    totaldata += data;
    // console.log(totaldata);
    if (totaldata.charAt(totaldata.length-1) == '\n') {
	try {
	    var time = Date.now();
	    var humidity = parseFloat(totaldata);
	    if (!isNaN(humidity)) {
		var reading = { humidity : humidity, date : time};
		console.log(reading);
		handleReading(reading);
	    }
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
