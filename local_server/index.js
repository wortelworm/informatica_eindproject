const WebSocketServer = require('websocket').server
const http = require('http');
const fs = require('fs');

const view_html = fs.readFileSync('./local_server/view.html');

var httpServer = http.createServer(function(request, response) {
    response.writeHead(200, {'Content-Type': 'text/html'});
    response.end(view_html);
});

const wsServer = new WebSocketServer({httpServer: httpServer, autoAcceptConnections: false});
let current_connection = null;

wsServer.on('request', (request) => {
        
    // for now, always accept
    const connection = request.accept()

    console.log('Connection with ' + request.remoteAddress);
    current_connection = connection;

    connection.on('message', function(message) {
        fs.writeFileSync('./input.txt', message.utf8Data);
    });
    
    connection.on('close', function(reasonCode, description) {
        console.log('Peer ' + connection.remoteAddress + ' disconnected: ' + description);
        current_connection = null;
    });
});
httpServer.listen(80, ()=> console.log('Online!'));

setInterval(() => {
    if (current_connection == null) {
        return;
    }

    let output = fs.openSync('./output.txt', 'a+');
    let contents = fs.readFileSync(output).toString();
    fs.closeSync(output);

    current_connection.send(contents);
}, 50);