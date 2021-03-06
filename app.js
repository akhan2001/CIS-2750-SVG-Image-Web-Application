'use strict'

// C library API
const ffi = require('ffi-napi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }

  let uploadFile = req.files.uploadFile;

  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      console.log('Error in file downloading route: '+err);
      res.send('');
    }
  });
});

//******************** Your code goes here ********************


var library = ffi.Library('./parser/bin/libsvgparser.so', {
  'validImageToJSON': ['string', ['string', 'string']]
});

//Sample endpoint
app.get('/fileInput', function(req , res){
  const fs = require('fs');
  const path = "uploads";
  const files = fs.readdirSync(path);

  let images = [];
  files.forEach(file => {
    //console.log('This is the file: ' + file + "\n");
    var result = JSON.parse(library.validImageToJSON('./uploads/' + file, "parser/bin/testFiles/svg.xsd"));
    //console.log('Should be an object: ' + result + '\n');

    if (result != null) {
      var tempData = [];
      tempData[0] = file;
      var filesize =  Math.round(fs.statSync('./uploads/' + file).size / 1024);
      //var filesize = (file.size);
      //console.log("Filesize: " + filesize + " Data 1:" + tempData[0] + "\n");
      tempData[1] = filesize;
      tempData[2] = result.numRect;
      tempData[3] = result.numCirc;
      tempData[4] = result.numPaths;
      tempData[5] = result.numGroups;

      images.push(tempData);
    }
  });

  //console.log(images);

  res.send({
    data: JSON.stringify(images)
    //data: images
  });
});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);
