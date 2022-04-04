//导入express框架
var express = require("express");
const bodyParser=require('body-parser')
var app = express();

const WebSocket = require('ws');

const wss = new WebSocket.Server({ port: 8085 });
let num = 0
wss.on('connection', (ws) => {
  ws.on('message', (message) => {
    console.log('服务器接收到消息'+message)
    wss.clients.forEach(e => {
      e.send('l'+message);
    });
  });
  num++
  console.log('连接成功'+num)
  wss.clients.forEach(e => {
    e.send('s'+num);
  });
});

var server = app.listen(3000,function(){
    console.log('runing 3000...');
})
