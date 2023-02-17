const WebSocketServer = require('websocket').server
const http = require('http');
const fs = require('fs');

const view_html = fs.readFileSync('C:/Users/willi/OneDrive/Documenten/Programmeren/Arduino/informatica_eindproject/local_server/view.html');

var httpServer = http.createServer(function(request, response) {
    response.writeHead(200, {'Content-Type': 'text/html'});
    response.end(view_html);
});
httpServer.listen(80, ()=> console.log('Online!'));


const wsServer = new WebSocketServer({httpServer: httpServer, autoAcceptConnections: false});

wsServer.on('request', (request) => {
        
    // for now, always accept
    const connection = request.accept()
    connection.send("Hello world!");

    console.log('Connection with ' + request.remoteAddress);

    connection.on('message', function(message) {
        console.log('Received message: ' + message.utf8Data);
    });
    
    connection.on('close', function(reasonCode, description) {
        console.log('Peer ' + connection.remoteAddress + ' disconnected: ' + description);
    });
})