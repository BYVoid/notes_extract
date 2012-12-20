var path = require('path');
var fs = require('fs');
var child_process = require('child_process');

exports.index = function (req, res) {
  res.render('index');
};

var getId = function () {
  return new Buffer(new Date().getTime().toString()).toString('base64');
};

var transform = function (id, audioData, next) {
  var filename = 'record.wav';
  var basepath = 'files/' + id;
  fs.mkdirSync(basepath, 0755);
  var audioFilename = basepath + '/' + filename;
  fs.writeFileSync(audioFilename, audioData);
  child_process.exec('bin/extract.sh ' + basepath + ' ' + filename, function (err, stdout, stderr) {
    if (err) return next(err);
    if (stdout) {
      console.log(stdout);
    }
    if (stderr) {
      console.log(stderr);
    }
    next();
  });
};

exports.upload = function (req, res, next) {
  var audiofile = req.files.audiofile;
  if (!audiofile) {
    return next();
  }
  var id = path.basename(audiofile.path);
  var audioData = fs.readFileSync(audiofile.path);
  transform(id, audioData, function (err) {
    if (err) return next(err);
    res.redirect('/sheet/' + id);
  });
};

var latestId;

exports.record = function (req, res, next) {
  var buf;
  req.on('data', function (data) {
    if (!buf) {
      buf = data;
    } else {
      var newbuf = new Buffer(buf.length + data.length);
      buf.copy(newbuf);
      data.copy(newbuf, buf.length);
      buf = newbuf;
    }
  });
  req.on('end', function () {
    var id = getId();
    transform(id, buf, function (err) {
      if (err) return next(err);
      latestId = id;
    });
  });
};

exports.latest = function (req, res, next) {
  res.redirect('/sheet/' + latestId);
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
