var express = require('express')
  , util = require('util')
  , ejs = require('ejs')
  , io = require('socket.io')
  , mongoose = require('mongoose')
  , nconf = require('nconf')
  , http = require('http')
;

var SerialPort = require('serialport2').SerialPort;
var port = new SerialPort();

// Database related setup
nconf.file({ file: 'config.json' });
var mongos = nconf.get('mongos'),
    replicaset = nconf.get('replicaset'),
    database = nconf.get('database');
var mongooptions = {
        'db': {
            'native_parser': true
        },
        'server': {
            'auto_reconnect': true,
            'poolSize': 10
        },
        'replset': {
            'readPreference': 'nearest',
            'strategy': 'ping',
            'rs_name': replicaset,
            'slaveok': true
        }
    };
var db = mongoose.createConnection(mongos, database, mongooptions);
db.on('error', console.error.bind(console, 'Database connection error:'));
db.once('open', function () {
  console.log("Database connection successful");
});
var Schema = mongoose.Schema
  , ObjectId = Schema.ObjectId;
var Reading = new Schema({
    date : { type: Date, default: Date.now }
  , humidity : Number
  , temperature: Number
});
var ReadingModel = db.model('Reading', Reading);
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

var app = express();
app.use(express.logger());
app.use(express.bodyParser());
app.use(express.cookieParser());
app.use(express.errorHandler());
app.use(express.session({secret: process.env.SESSION_SECRET || 'kdgfcbdgfsgftsrcgsgr'}));
app.use(express.static(__dirname + '/public'));
var server = http.createServer(app)

var sio = io.listen(server);
sio.sockets.on('connection', function (socket) {
    console.log('A socket connected!');
});

function handleReading(data) {
    var humidity = data.humidity;
    var temperature = data.temperature;
    var reading = new ReadingModel(data);
    sio.sockets.emit('humidity', { value: humidity });
    sio.sockets.emit('temperature', { value: temperature });
    reading.save(function (err) {
      if (err) {
        console.log("Can't save: "+err);
      } else {
        // console.log("Saved!");
      }
    });
}

port.on('data', function(data) {
    totaldata += data;
    // console.log(totaldata);
    if (totaldata.charAt(totaldata.length-1) == '\n') {
	try {
	    dataitems = totaldata.split(',');
	    var time = Date.now();
	    var humidity = parseFloat(dataitems[0]);
	    var temperature = parseFloat(dataitems[2]);
	    if (!isNaN(humidity)) {
		var reading = { humidity : humidity, temperature: temperature, date : time};
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

// Start the app
var port = process.env.PORT || 5000;
server.listen(port, function() {
  console.log("Listening on " + port);
});
