var express = require('express')
var app = express()

// Require the filsystem extension for writing the log
var fs = require('fs')

// Require body parser to get the POST parameters correctly
var bodyParser = require("body-parser")
app.use(bodyParser.urlencoded({ extended: false }))

// Read config file
var data = fs.readFileSync('config.json'),
    config;

try {
  config = JSON.parse(data);
} catch (err) {
  console.log('There has been an error parsing the config file.')
  throw err
}

// accept POST request
app.post(config.PATH, function (req, res) {

  // Check for key
  if (req.body.key == config.PRIVATE_KEY) {
    
    temperature = req.body.t
    power = req.body.w
    date = new Date().toISOString().replace(/T/, ' ').replace(/\..+/, '').replace(/-/g, '\/')

    // Format log
    /*
      Matching grok configuration :
      match => { "message" => "(?<timestamp>%{YEAR}/%{MONTHNUM:month}/%{MONTHDAY:day} %{TIME}) %{NUMBER:temperature} %{NUMBER:power}" }
    */
    var log_message = date + " " + temperature + " " + power

    fs.appendFile(config.LOG_FILE, log_message + "\n", function (err) {
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
