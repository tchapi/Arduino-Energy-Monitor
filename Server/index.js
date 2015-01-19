PRIVATE_KEY = "m88JNhTBEZyX";
LOG_FILE = "/var/log/energy/energy.log";

var express = require('express')
var app = express()

// Require the filsystem extension for writing the log
var fs = require('fs')

// Require body parser to get the POST parameters correctly
var bodyParser = require("body-parser")
app.use(bodyParser.urlencoded({ extended: false }))

// accept POST request
app.post('/home/energy', function (req, res) {

  // Check for key
  if (req.body.key == PRIVATE_KEY) {
    
    temperature = req.body.t
    power = req.body.w
    date = new Date().toISOString().replace(/T/, ' ').replace(/\..+/, '').replace(/-/, '\/')

    // Format log
    /*
      Matching grok configuration :
      match => { "message" => "(?<timestamp>%{YEAR}/%{MONTHNUM:month}/%{MONTHDAY:day} %{TIME}) %{NUMBER:temperature} %{NUMBER:power}" }
    */
    var log_message = date + " " + temperature + " " + power

    fs.appendFile(LOG_FILE, log_message + "\n", function (err) {
      if (err) {
        throw err;
        res.json({ code: 0 })
      } else {
        res.json({ code: 1 })
      }
    });


  } else {

    res.json({ code: 0 })

  }

})

// Start application
var server = app.listen(3000, function () {

  var host = server.address().address
  var port = server.address().port

  console.log('Starting data API at http://%s:%s', host, port)

})
