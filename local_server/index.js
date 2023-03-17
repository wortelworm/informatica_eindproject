const WebSocketServer = require('websocket').server
const http = require('http');
const fs = require('fs');

function getImage() {
    let output = fs.openSync('./output.txt', 'a+');
    let contents = fs.readFileSync(output).toString();
    fs.closeSync(output);
    return contents;
}

function getTone() {
    let tone_output = fs.openSync('./tone_output.txt', 'a+');
    let contents = fs.readFileSync(tone_output).toString();
    fs.closeSync(tone_output);
    console.log(contents);
    return contents;
}

function writeButtons(buttons) {
    fs.writeFileSync('./input.txt', buttons);
}

var httpServer = http.createServer(function(request, response) {
    if (request.url.startsWith("/image/")) {
        // from stormworks
        response.writeHead(200, {'Content-Type': 'text/html'});
        response.end([...getImage()].map( char => char.charCodeAt(0)).join(' '));
        // response.end(getImage());
        let data = request.url.substring(7) * 1;
        let result = ''
        for (let i = 0; i < 6; i++) {
            if ((data&(1<<i)) == 0) {
                result += '0';
            } else {
                result += '1';
            }
        }

        writeButtons(result);
        return;
    }

    response.writeHead(200, {'Content-Type': 'text/html'});
    response.end(fs.readFileSync('./local_server/view.html'));
});

const wsServer = new WebSocketServer({httpServer: httpServer, autoAcceptConnections: false});
let current_connection = null;

wsServer.on('request', (request) => {
        
    // for now, always accept
    const connection = request.accept()

    console.log('Connection with ' + request.remoteAddress);
    current_connection = connection;

    connection.on('message', function(message) {
        writeButtons(message.utf8Data);
    });
    
    connection.on('close', function(reasonCode, description) {
        console.log('Peer ' + connection.remoteAddress + ' disconnected: ' + description);
        current_connection = null;
    });
});
httpServer.listen(80, ()=> console.log('Online!'));

let imageBuffer;
let prevTone = 0;

setInterval(() => {
    if (current_connection == null) {
        return;
    }

    let newImage = getImage();
    if (newImage != imageBuffer) {
        imageBuffer = newImage;
        current_connection.send(newImage);
    }

    let newTone = getTone();
    if (prevTone != newTone) {
        prevTone = newTone;
        current_connection.send(newTone);
    }

}, 50);