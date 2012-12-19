var path = require('path');
var fs = require('fs');
var child_process = require('child_process');

exports.index = function (req, res) {
  res.render('index');
};

exports.upload = function (req, res) {
  var audiofile = req.files.audiofile;
  if (!audiofile) {
    return next();
  }
  
  var id = path.basename(audiofile.path);
  var basepath = 'files/' + id;
  fs.mkdirSync(basepath, 0755);
  var audioData = fs.readFileSync(audiofile.path);
  var audioFilename = basepath + '/' + audiofile.name;
  fs.writeFileSync(audioFilename, audioData);
  
  child_process.exec('bin/extract.sh ' + basepath + ' ' + audiofile.name, function (err, stdout, stderr) {
    if (err) return next(err);
    console.log(stdout, stderr);
    res.redirect('/sheet/' + id);
  });
};

exports.sheet = function (req, res) {
  var id = req.params.id;
  res.render('sheet', {
    id: id
  });
};

exports.sheetPng = function (req, res) {
  var id = req.params.id;
  res.sendfile('files/' + id + '/sheet_1.png');
};

exports.sheetMid = function (req, res) {
  var id = req.params.id;
  res.sendfile('files/' + id + '/notes.mid');
};

exports.sheetOgg = function (req, res) {
  var id = req.params.id;
  res.sendfile('files/' + id + '/notes.ogg');
};

exports.sheetTxt = function (req, res) {
  var id = req.params.id;
  res.sendfile('files/' + id + '/notes.txt');
};
