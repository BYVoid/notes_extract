var express = require('express');
var routes = require('./routes');
var http = require('http');
var path = require('path');

var app = express();

app.configure(function(){
  app.set('port', process.env.PORT || 3175);
  app.set('views', __dirname + '/views');
  app.set('view engine', 'jade');
  app.use(express.favicon());
  app.use(express.logger('dev'));
  app.use(express.bodyParser());
  app.use(express.methodOverride());
  app.use(app.router);
  app.use(express.static(path.join(__dirname, 'public')));
});

app.configure('development', function(){
  app.use(express.errorHandler());
});

app.get('/', routes.index);
app.post('/upload', routes.upload);
app.get('/sheet/:id', routes.sheet);
app.get('/sheet/:id/png', routes.sheetPng);
app.get('/sheet/:id/mid', routes.sheetMid);
app.get('/sheet/:id/ogg', routes.sheetOgg);
app.get('/sheet/:id/txt', routes.sheetTxt);

http.createServer(app).listen(app.get('port'), function(){
  console.log("Express server listening on port " + app.get('port'));
});
