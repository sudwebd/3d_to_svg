var express = require('express');
var spawn = require("child_process").spawn;
var nrc = require('node-run-cmd');
var cmd=require('node-cmd');
var app=express();
var filenm;var filewext;
var myPythonScriptPath = 'main.py';
const fileUpload = require('express-fileupload');
app.use(fileUpload());
var PythonShell = require('python-shell');
//var pyshell = new PythonShell(myPythonScriptPath);
var http = require('http').Server(app);
var io = require('socket.io')(http);
app.set('port',(process.env.PORT||8000));
http.listen (app.get('port'),function() {
  console.log("listening to port number "+app.get('port'));
});
app.get('/',function(req,res){
	res.sendFile(__dirname + '/index.html');
});

app.post('/upload', function(req, res) {
  if (!req.files)
    return res.status(400).send('No files were uploaded.');

  // The name of the input field (i.e. "sampleFile") is used to retrieve the uploaded file
  let sampleFile = req.files.sampleFile;
  filenm=sampleFile.name;filewext=filenm.replace(".obj","");
  console.log(filenm);console.log(filewext);
  // Use the mv() method to place the file somewhere on your server
  sampleFile.mv(__dirname+'/'+filenm, function(err) {
    if (err)
      return res.status(500).send(err);

      res.sendFile(__dirname + '/upload.html');
  });
});

app.post('/result',function(req,res){
    console.log(req.body);
    var viewx=req.body['viewx'];
    var viewy=req.body['viewy'];
    var viewz=req.body['viewz'];
    var height=req.body['height'];
    var width=req.body['width'];
    var rotationx=req.body['rotationx'];
    var rotationy=req.body['rotationy'];
    var rotationz=req.body['rotationz'];


//  var arg1=' --vx '+viewx+' --vy '+viewy+' --vz '+viewz+' -H '+height+' -W '+width+' objs/'+filenm+' -o outputs/candy.svg';
    //var process = spawn('python',["/home/shubham/Desktop/svg_visualization/obj-to-svg/main.py", arg1]);
  var options = { cwd: '/home/shubham/Desktop/svg_visualization/obj-to-svg/' };
  var commn='./poly '+filenm+' '+rotationx+' '+rotationy+' '+rotationz;
  var commannds=[commn];
  nrc.run(commannds,options);

  setTimeout(function(){ console.log("Hello"); }, 3000);

  res.sendFile(__dirname + '/'+filewext+'.svg');
});
